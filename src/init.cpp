#include "init.h"
#include "xprint.h"
#include "globals.h"
#include "eval.h"
#include <fstream>
#include <iostream>
#include "audio.h"
#include "utilities.h"

void init_logfile() {
    try {
    // Création des layouts
        log4cpp::PatternLayout *file_layout = new log4cpp::PatternLayout();
        file_layout->setConversionPattern("%m");

        // Création des appenders
        logfile = new log4cpp::FileAppender("logfile", "xtalk.log", false);
        logfile->setLayout(file_layout);

        // Initialisation de la catégorie racine
        xlog = &log4cpp::Category::getRoot();
        xlog->setPriority(log4cpp::Priority::DEBUG); // Niveau minimal de priorité
        xlog->addAppender(logfile);
    } catch (const log4cpp::ConfigureFailure &e) {
        std::cerr << "Erreur lors de l'initialisation de log4cpp : " << e.what() << std::endl;
    }
}

void init_vosk() {
    xprint(M_INIT, "Initializing Vosk", vmodel_name);
    
    vosk_set_log_level(-1);
    vmodel = vosk_model_new(vmodel_name);
    vrec = vosk_recognizer_new(vmodel, 16000.0);
    
    xprint(M_DONE);
}

void init_json() {
    xprint(M_INIT,"Loading JSON dialog tree", json_file);
    
    responses = load_json("responses.json");
    currentNode = &responses["root"];
    
    xprint(M_DONE);    
}

void init_airports() {
    xprint(M_INIT, "Loading JSON airports", airports_file);
    
    airports = load_json("airports.json");
    
    xprint(M_DONE, std::to_string(airports.size()) + " airports found");
}

void init_netw() {
        xprint(M_INIT,"Initializing network");
        
        socket.connect("tcp://" + ip_address + ":5555");
        socket.send(zmq::message_t(), zmq::send_flags::none);
        
        xprint(M_DONE);    
}

void load_phrase_file() {
    xprint(M_INIT, "Loading phrase file", "phrases.cfg");
    
    std::ifstream file("phrases.cfg");

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
        phrases[key] = value;
    }
    
    xprint(M_DONE);
}

void load_audio_data() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Erreur SDL_Mixer: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }
        
    load_files_WAV("audio/VOIX_AFIS/", WAV_files);    
}

void list_joysticks() {
    SDL_Init(SDL_INIT_JOYSTICK);
    
    xprint(M_INIT, "Listing available devices");
    
    int numJoysticks = SDL_NumJoysticks();

    if (numJoysticks == 0) {
        printf("No device found.\n");
    }
    
    std::cout << std::endl;
    
    for (int i = 0; i < numJoysticks; ++i) {
        const char *joystickName = SDL_JoystickNameForIndex(i);
        if (joystickName) {
            //printf("%d : %s\n", i, joystickName);
            xprint(M_REQU, std::to_string(i), joystickName);
        } else {
            printf("%d : Unknown name\n", i);
        }
    }
    
    int joystickIndex;
    bool selected = false;
    
    if (numJoysticks) {
        joystickIndex = select_by_id(numJoysticks);

        // Ouvrir le joystick sélectionné
        joystick = SDL_JoystickOpen(joystickIndex);
        if (joystick == NULL) {
            fprintf(stderr, "Impossible d'ouvrir le joystick %d : %s\n", joystickIndex, SDL_GetError());
        }

        xprint(M_INIT, "Selecting device", SDL_JoystickName(joystick));
        xprint(M_DONE);

        // Boucle principale pour détecter les événements de bouton
        SDL_Event event;
        xprint(M_REQU, "Press any button on the device");
        selected = false;
        
        while (!selected) {
            // Vérifier les événements
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_JOYBUTTONUP) {
                    ptt = event.jbutton.button;
                    selected = true;
                    xprint(M_DONE);
                }
            }
            
            // Pause pour éviter une boucle excessive
            SDL_Delay(10);
        }
    }
}
