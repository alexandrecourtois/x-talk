#include "audio.h"
#include <callbacks.h>
#include <xprint.h>
#include <inputs.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <cmath>
#include <string>
#include <tools.h>
#include "globals.h"
#include <filesystem>
#include <map>
#include <iostream>
#include <fstream>
#include <session.h>
#include <radio.h>

std::map<std::string, Mix_Chunk*>   AUDIO::__WAV_files;
std::vector<unsigned char>          AUDIO::__audio_stream;
bool                                AUDIO::__is_recording = false;
SDL_AudioDeviceID                   AUDIO::__device_id;
int                                 AUDIO::__sample_rate = 16000;
std::map<std::string, std::string>  AUDIO::__phrases;

void AUDIO::__init_phrs(const std::string& phrasePath) {
    OUT::xprint(OUT::MSG_STYLE::INIT, "Loading phrase file", "phrases.cfg");
    
    std::ifstream file(phrasePath);

    if (!file.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier.");
    }

    std::string line;
    while (std::getline(file, line)) {
        // Supprimer les espaces en début et fin de ligne
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Ignorer les lignes vides ou les commentaires
        if (line.empty() || line[0] == '#') continue;

        // Trouver la position du caractère '='
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error("Ligne mal formatée : " + line);
        }

        // Séparer la clé (à gauche de '=') et la valeur (à droite de '=')
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Supprimer les espaces autour de la clé et de la valeur
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Ajouter dans la map
        __phrases[key] = value;
    }
    
    OUT::xprint(OUT::MSG_STYLE::DONE);
}

void AUDIO::__load_audio(const std::string& audioPath) {
    // Étape 1 : Compter le nombre total de fichiers .wav
    int totalFiles = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(audioPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            totalFiles++;
        }
    }

    // Vérifie s'il y a des fichiers à charger
    if (totalFiles == 0) {
        std::cout << "Aucun fichier .wav trouvé dans le répertoire." << std::endl;
        return;
    }

    // Étape 2 : Charger les fichiers et afficher la barre de progression
    int loadedFiles = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(audioPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            std::string filename = entry.path().stem().string() + entry.path().extension().string();
            Mix_Chunk* chunk = Mix_LoadWAV(entry.path().string().c_str());
            if (chunk != nullptr) {
                __WAV_files[filename] = chunk;
            } else {
                std::cerr << "Erreur de chargement pour " << entry.path() << ": " << Mix_GetError() << std::endl;
            }
            
            // Mise à jour de la barre de progression
            loadedFiles++;
            TOOLBOX::displayProgressBar("Loading audio files...", loadedFiles, totalFiles);
        }
    }
    
    // Terminer la barre de progression à 100 %
    TOOLBOX::displayProgressBar("Loading audio files...", totalFiles, totalFiles);
    std::cout << CLEAN_LINE;
    OUT::xprint(OUT::MSG_STYLE::INIT, "Loading audio files");
    OUT::xprint(OUT::MSG_STYLE::DONE, std::to_string(totalFiles) + " files");
}

void AUDIO::__play_tailnum() {
    std::vector<std::string> tailnum_files;
    
    for(unsigned int i = 0; i < SESSION::aircraft.tailnum.size(); ++i) {
        if ((SESSION::aircraft.tailnum[i] >= 'A') && (SESSION::aircraft.tailnum[i] <= 'Z')) {
            tailnum_files.push_back(std::string(1, std::tolower(SESSION::aircraft.tailnum[i])) + ".wav");
        }
        
        if (i == 0)
            i = SESSION::aircraft.tailnum.size() - 3;
    }
    
    __play(tailnum_files, CHANNEL_RADIO_COM);
}

int AUDIO::__play(const std::string &filename, int channel, bool loop) {
    if (__WAV_files.find(filename) != __WAV_files.end()) {
        Mix_Chunk* sound = __WAV_files[filename];

        if (int ch = Mix_PlayChannel(channel, sound, loop ? -1 : 0) == -1) {
            OUT::xprint(OUT::MSG_STYLE::ERROR, "Unable to play " + filename);
        } else {
            if (!loop) 
                while(int p = Mix_Playing(channel) > 0) {
                    SDL_Delay(100);
                }

            return ch;
        }
    }

    return -1;
}

void AUDIO::__play(const std::vector<std::string>& fileNames, int channel) {
    for (/*const auto&*/ std::string fileName : fileNames) {
        auto it = __WAV_files.find(fileName);
        if (it != __WAV_files.end()) {
            __play(fileName, channel);
        } else {
            //std::cerr << "Fichier non trouvé dans la map: " << fileName << std::endl;
            OUT::xprint(OUT::MSG_STYLE::WARNING, "Missing file", fileName);
        }
    }
}

void AUDIO::__play_radiocom(const std::vector<std::string> &filenames) {
    int noise_volume = RADIO::getNoiseVolume();

    Mix_Volume(CHANNEL_RADIO_NOISE, noise_volume);
    Mix_Volume(CHANNEL_RADIO_COM, RADIO::getSpeakVolume(noise_volume));
    __play("bruit.wav", CHANNEL_RADIO_NOISE, true);
    __play_tailnum();

    for(int i = 0; i < filenames.size(); ++i)
        __play(__phrase_to_waves(filenames[i]), CHANNEL_RADIO_COM);

    __play("fin.wav", CHANNEL_RADIO_COM);
    Mix_HaltChannel(CHANNEL_RADIO_NOISE);
}

void AUDIO::__play_radiostart() {
    __play("debut.wav", CHANNEL_RADIO_REC);
    __play("fond.wav", CHANNEL_RADIO_REC, true);
}

void AUDIO::__play_radiostop() {
    Mix_HaltChannel(CHANNEL_RADIO_REC);
    __play("fin.wav", CHANNEL_RADIO_REC);
}

std::vector<std::string> AUDIO::__phrase_to_waves(const std::string& str) {
    std::vector<std::string> subphrases = TOOLBOX::splitString(__phrases[str], '|');
    std::string phrase;
    std::vector<std::string> words;
    std::vector<std::string> waves;
    
    if (subphrases.size() > 1) {
        int retained_index = TOOLBOX::generateRandomNumber(subphrases.size() - 1);
        phrase = subphrases[retained_index];
    }
    else
        phrase = subphrases[0];
    
    words = TOOLBOX::splitString(phrase, '&');
    
    for(unsigned int i = 0; i < words.size(); ++i) {
        if (words[i][0] == '[' && words[i][words[0].size() - 1] == ']') {
            std::vector<std::string> tmp_waves = __phrase_to_waves(words[i]);
            waves.insert(waves.end(), tmp_waves.begin(), tmp_waves.end());
        }
        else
        {
            std::string value = TOOLBOX::extractBetweenBraces(words[i]);
            
            if (!value.empty()) {
                std::vector<std::string> filename = TOOLBOX::splitString(words[i], '.');
                words[i] = TOOLBOX::removeSubstring(filename[0], "{" + value + "}") + CALLBACKS::getValue(CALLBACKS::AUDIO, value) + "." + filename[1];
            }
            
            waves.push_back(words[i]);
        }
    }
    
    return waves;
}

void AUDIO::__audio_callback(void* userdata, Uint8* stream, int len) {
    if (isRecording()) {
        __audio_stream.insert(__audio_stream.end(), stream, stream + len);
    }
}

void AUDIO::init(const std::string& audioPath, const std::string& phrasePath) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Erreur SDL_Mixer: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    __load_audio(audioPath);
    __init_phrs(phrasePath);
}

void AUDIO::selectDevice() {
    // Initialiser SDL avec le sous-système audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        OUT::xprint(OUT::MSG_STYLE::ERROR, "Error while initializing SDL_AUDIO");
    }
    
    // Obtenir le nombre de périphériques d'entrée audio disponibles
    int numAudioDevices = SDL_GetNumAudioDevices(SDL_TRUE);  // SDL_TRUE pour les périphériques d'entrée
    if (numAudioDevices < 0) {
        OUT::xprint(OUT::MSG_STYLE::ERROR, "Error while enumerating audio devices.");
    }

    for (int i = 0; i < numAudioDevices; ++i) {
        const char* deviceName = SDL_GetAudioDeviceName(i, SDL_TRUE);  // SDL_TRUE pour l'entrée
        if (deviceName) {
            OUT::xprint(OUT::MSG_STYLE::REQU, std::to_string(i), deviceName);
        } else {
            OUT::xprint(OUT::MSG_STYLE::REQU, std::to_string(i), "Unamed audio device");
        }
    }
        
        if (numAudioDevices) {
            int audioIndex = TOOLBOX::selectById(numAudioDevices);
            const char* selectedDeviceName = SDL_GetAudioDeviceName(audioIndex, SDL_TRUE);
            
            if (!selectedDeviceName) {
                OUT::xprint(OUT::MSG_STYLE::ERROR, "Invalid audio device.");
                return;
            }

            // Spécifications du format audio souhaité
            SDL_AudioSpec desiredSpec;
            SDL_zero(desiredSpec);             // Initialiser à zéro
            desiredSpec.freq = __sample_rate;           // Fréquence d'échantillonnage
            desiredSpec.format = AUDIO_S16SYS;     // Format de l'échantillon (32 bits float)
            desiredSpec.channels = 1;           // Nombre de canaux (stéréo)
            desiredSpec.samples = 4096;         // Taille de la mémoire tampon
            desiredSpec.callback = __audio_callback;
            
            // Structure pour stocker les spécifications obtenues
            SDL_AudioSpec obtainedSpec;

            // Ouvrir le périphérique audio
            __device_id = SDL_OpenAudioDevice(selectedDeviceName, SDL_TRUE, &desiredSpec, &obtainedSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

            if (__device_id == 0) {
                OUT::xprint(OUT::MSG_STYLE::ERROR, "Audio device error: " + std::string(SDL_GetError()));
            }
    }
}

void AUDIO::startRecording() {
    __audio_stream.clear();
    __is_recording = true;
    SDL_PauseAudioDevice(__device_id, 0);
}

void AUDIO::stopRecording() {
    SDL_PauseAudioDevice(__device_id, 1);
    __is_recording = false;
    auto silence = static_cast<size_t>(__sample_rate * 2);
    __audio_stream.insert(__audio_stream.end(), silence, 0);
}

void AUDIO::play(Type type, const std::vector<std::string>& files) {
    switch(type) {
        case Type::RADIOCOM:
            __play_radiocom(files);
            break;

        case Type::RADIOSTART:
            __play_radiostart();
            break;

        case Type::RADIOSTOP:
            __play_radiostop();
            break;

        case Type::TAILNUM:
            __play_tailnum();
            break;

        default:
            break;
    }
}

bool AUDIO::isRecording() {
    return __is_recording;
}

const std::vector<unsigned char>& AUDIO::getAudioStream() {
    return __audio_stream;
}