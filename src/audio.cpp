#include "audio.h"
#include "xprint.h"
#include "xscan.h"
#include <string>
#include "utilities.h"
#include "globals.h"
#include <filesystem>
#include <map>
#include <iostream>


int sampleRate = 16000;

void audioCallback(void* userdata, Uint8* stream, int len) {
    if (recording) {
        audioData.insert(audioData.end(), stream, stream + len);
    }
}

void select_audio_input() {
        // Initialiser SDL avec le sous-système audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        xprint(M_ERROR, "Error while initializing SDL_AUDIO");
    }
    
    // Obtenir le nombre de périphériques d'entrée audio disponibles
    int numAudioDevices = SDL_GetNumAudioDevices(SDL_TRUE);  // SDL_TRUE pour les périphériques d'entrée
    if (numAudioDevices < 0) {
        xprint(M_ERROR, "Error while enumerating audio devices.");
    }

    for (int i = 0; i < numAudioDevices; ++i) {
        const char* deviceName = SDL_GetAudioDeviceName(i, SDL_TRUE);  // SDL_TRUE pour l'entrée
        if (deviceName) {
            xprint(M_REQU, std::to_string(i), deviceName);
        } else {
            xprint(M_REQU, std::to_string(i), "Unamed audio device");
        }
    }
        
        if (numAudioDevices) {
            int audioIndex = select_by_id(numAudioDevices);
            const char* selectedDeviceName = SDL_GetAudioDeviceName(audioIndex, SDL_TRUE);
            
            if (!selectedDeviceName) {
                xprint(M_ERROR, "Invalid audio device.");
                return;
            }

            // Spécifications du format audio souhaité
            SDL_AudioSpec desiredSpec;
            SDL_zero(desiredSpec);             // Initialiser à zéro
            desiredSpec.freq = sampleRate;           // Fréquence d'échantillonnage
            desiredSpec.format = AUDIO_S16SYS;     // Format de l'échantillon (32 bits float)
            desiredSpec.channels = 1;           // Nombre de canaux (stéréo)
            desiredSpec.samples = 4096;         // Taille de la mémoire tampon
            desiredSpec.callback = audioCallback;
            
            // Structure pour stocker les spécifications obtenues
            SDL_AudioSpec obtainedSpec;

            // Ouvrir le périphérique audio
            audioDeviceID = SDL_OpenAudioDevice(selectedDeviceName, SDL_TRUE, &desiredSpec, &obtainedSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

            if (audioDeviceID == 0) {
                xprint(M_ERROR, "Audio device error: " + std::string(SDL_GetError()));
            }
    }
}

void start_recording(){
    audioData.clear();
    recording = true;
    SDL_PauseAudioDevice(audioDeviceID, 0);
}

void stop_recording() {
    SDL_PauseAudioDevice(audioDeviceID, 1);
    recording = false;
    size_t silenceDuration = static_cast<size_t>(sampleRate * 2);
    audioData.insert(audioData.end(), silenceDuration, 0);
}

void displayProgressBar(int current, int total) {
    int barWidth = 40;
    float progress = (float)current / total;
    int pos = barWidth * progress;
    
    std::cout << '\r';
    xprint(M_INIT, "Loading audio files", "", nullptr);
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
}

void load_files_WAV(const std::string& directoryPath, std::map<std::string, Mix_Chunk*>& sounds) {
    // Étape 1 : Compter le nombre total de fichiers .wav
    int totalFiles = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
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
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            std::string filename = entry.path().stem().string() + entry.path().extension().string();
            Mix_Chunk* chunk = Mix_LoadWAV(entry.path().string().c_str());
            if (chunk != nullptr) {
                sounds[filename] = chunk;
            } else {
                std::cerr << "Erreur de chargement pour " << entry.path() << ": " << Mix_GetError() << std::endl;
            }
            
            // Mise à jour de la barre de progression
            loadedFiles++;
            displayProgressBar(loadedFiles, totalFiles);
        }
    }
    
    // Terminer la barre de progression à 100 %
    displayProgressBar(totalFiles, totalFiles);
    std::cout << CLEAN_LINE;
    xprint(M_INIT, "Loading audio files");
    xprint(M_DONE, std::to_string(totalFiles) + " files");
}

void play_tailnum() {
    std::vector<std::string> tailnum_files;
    
    for(unsigned int i = 0; i < aircraft.tailnum.size(); ++i) {
        if ((aircraft.tailnum[i] >= 'A') && (aircraft.tailnum[i] <= 'Z')) {
            tailnum_files.push_back(std::string(1, std::tolower(aircraft.tailnum[i])) + ".wav");
        }
        
        if (i == 0)
            i = aircraft.tailnum.size() - 3;
    }
    
    play_files_WAV(tailnum_files);
}

void play_files_WAV(const std::vector<std::string>& fileNames) {
    for (const auto& fileName : fileNames) {
        auto it = WAV_files.find(fileName);
        if (it != WAV_files.end()) {
            Mix_Chunk* sound = it->second;
            // Jouer le fichier WAV
            if (Mix_PlayChannel(-1, sound, 0) == -1) {
                std::cerr << "Erreur de lecture de " << fileName << ": " << Mix_GetError() << std::endl;
            } else {
                //std::cout << "Lecture de " << fileName << std::endl;
                // Attendre que le fichier WAV se termine (optionnel, selon l'effet désiré)
                while (Mix_Playing(-1) > 0) {
                    SDL_Delay(100);  // Petite pause pour ne pas monopoliser le CPU
                }
            }
        } else {
            //std::cerr << "Fichier non trouvé dans la map: " << fileName << std::endl;
            xprint(M_WARNING, "Missing file", fileName);
        }
    }
}