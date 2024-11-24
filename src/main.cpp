#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <stack>
#include <fstream>
#include <queue>
#include <thread>
#include <condition_variable>
#include <chrono>
#include "xprint.h"
#include "globals.h"
#include "eval.h"
#include <SDL2/SDL.h>
#include <stdexcept>
#include "xscan.h"
#include "ptt.h"
#include <termios.h>
#include <unistd.h>
#include "audio.h"
#include "utilities.h"
#include <functional>
#include "system.h"
#include "dataframe.h"
#include "updater.h"
#include "command.h"
#include "init.h"
#include "airport.h"

#define __VERSION__MAJOR    "0"
#define __VERSION__MINOR    "01"
#define __LICENSE           "GPLv3"

#define __LINUX
//#define __WINDOWS

void connect_xp() {
    xprint(M_INIT,"Connecting to X-Plane");
    
    socket.recv(message, zmq::recv_flags::none);
    memcpy(&dataframe, message.data(), sizeof(dataframe));   
 
    xprint(M_DONE, "X-Plane " + std::to_string(dataframe.xp_major / 1000) + "." + std::to_string((dataframe.xp_major / 10) % ((dataframe.xp_major / 1000) * 100)));    
}

void waitfor_aircraft() {
    xprint(M_INIT,"Waiting for aircraft");

    while(dataframe.tailnum[0] == '\0') {
        socket.recv(message, zmq::recv_flags::none);
        memcpy(&dataframe, message.data(), sizeof(dataframe));
    }

    aircraft.tailnum = dataframe.tailnum;
    xprint(M_DONE, aircraft.tailnum);
}

int main(int argc, char **argv) {
    #ifdef __LINUX
    system("clear");
    #endif
    
    #ifdef __WINDOWS
    system("cls");
    #endif

     xprintbox(DOUBLE,
              "xtalk -- version " + std::string(__VERSION__MAJOR) + "." + std::string(__VERSION__MINOR) + " -- " + std::string(__LICENSE),
              WHITE,
              CYAN_BOLD);

    init_logfile();

    // Parcours des arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Traitement des options longues
        if (arg == "--no-audio-rec") {
            noaudio = true;
        } else if (arg == "--no-joystick") {
            nojoystick = true;
        } else if (arg == "--no-xplane") {
            noxplane = true;
        } else if (arg == "--xp-address") {
            if (i < argc) {
                if (is_valid_IP(argv[i+1])) {
                    ip_address = argv[i+1];
                } else {
                    xprint(M_ERROR, "Invalid IP address. Networking disabled");
                    noxplane = true;
                }
            }
        } 
        // Traitement des options courtes avec "-"
        else if (arg[0] == '-' && arg.length() > 1 && arg[1] != '-') {
            // Boucle sur chaque caractère après "-"
            for (size_t j = 1; j < arg.length(); ++j) {
                switch (arg[j]) {
                    case 'a':
                        noaudio = true;
                        nojoystick = true;
                        break;
                    case 'j':
                        nojoystick = true;
                        break;
                    case 'x':
                        noxplane = true;
                        break;
                    default:
                        std::cerr << "Unknown argument : -" << arg[j] << std::endl;
                        return 1; // Sortie avec erreur
                }
            }
        }
    }
    
    init_commands();
    
    disableInput();
    //std::cout << WHITE_BOLD << "xtalk -- version " << __VERSION__MAJOR << "." << __VERSION__MINOR << " -- " << __LICENSE << "\n" << RESET;

    std::string input;
    
    if (!noaudio) {
        select_audio_input();
        init_vosk();
        load_audio_data();
    }
    else
        xprint(M_WARNING, "Disable", "AUDIO");
    
    init_json();
    init_airports();
    load_phrase_file();
    
    if (!nojoystick)
        list_joysticks();
    else
        xprint(M_WARNING, "Disable", "JOYSTICK");
    
    if (!noxplane) {
        init_netw();
        connect_xp();
        waitfor_aircraft();
        std::thread netupdater(update_dataframe);
        netupdater.detach();
    }
    else
        xprint(M_WARNING, "Disable", "XPLANE");
        
    xprint(M_INFO, "Type \"cmd_help\" to list debug commands.");
    run_command("cmd_getmem");
    
    while (input != "q") {
        std::cout << "> " << std::flush;
        xscan(input, update);

        if (input == "q" || input == "quit") {
            std::cout << BG_GREEN_BOLD << BLACK << speaker << RESET << GREEN_BOLD << ": Au revoir !" << RESET << std::endl;
            break;
        }
        
        if (std::equal(prefix_cmd.begin(), prefix_cmd.end(), input.begin())) {
            run_command(input);
        }
        else {
            if (input.empty() && !noaudio) {
                start_recording();
                
                if (!ptt_pushed()) {
                    std::cout << BACK_LINE << "> " << std::flush;
                }
                else
                    disableInput();
                    
                //std::cout << WHITE_BOLD << BG_RED_BOLD << BLINK << "Recording... Press Enter to stop." << RESET << std::flush;
                xprint(M_BLINK_BEGIN, "Recording... Press Enter to stop.");
                
                if (ptt_pushed()) {
                    while(ptt_pushed());
                    enableInput();
                    std::cout << std::endl;
                }
                else
                    xscan();
                
                xprint(M_BLINK_END);
                xprint(M_INVITE);
                
                stop_recording();
       
                std::cout << "Analyzing... " << std::flush;
                
                vosk_recognizer_accept_waveform(vrec, reinterpret_cast<const char*>(audioData.data()), audioData.size());
                
                const char* vres = vosk_recognizer_result(vrec);
                std::string userInput(nlohmann::json::parse(vres)["text"]);
                
                std::cout << CLEAN_LINE << "> " << std::endl;
                input = userInput;
            }

            if (!input.empty()) {
                std::cout << BACK_LINE << CLEAN_LINE;

                if (supported_airport())
                    xprint(M_USER, input);
                else
                    xprint(M_USER_ALT, input);

                if (supported_airport()) {
                    std::set<std::string> userWords = splitUserInput(input);
                    nlohmann::json* nextNode = getResponse(currentNode, userWords, responses);

                    if (nextNode) {
                        currentNode = nextNode;
                        
                        if (currentNode->contains("phrase")) {
                            xprint(M_XTALK, replace_keys(REMOVE_QUOTES((*currentNode)["phrase"])));
                        }
                        
                        if (currentNode->contains("audio") && !noaudio) {
                            //std::cout << WHITE_BOLD << BG_RED_BOLD << BLINK << "Speaking" << RESET << std::endl << std::flush;
                            xprint(M_BLINK_BEGIN, "Speaking");
                            
                            std::string value = REMOVE_QUOTES((*currentNode)["audio"]);
                            std::vector<std::string> keys = splitString(value, ',');
                            
                            play_tailnum();
                            
                            for(unsigned int i = 0; i < keys.size(); ++i)
                                play_files_WAV(readPhrase(keys[i]));

                            xprint(M_ENDL);
                            xprint(M_BLINK_END);
                        }
                    }
                    else {
                        xprint(M_XTALK_ALT, responses["default"]["phrase"]);
                    }
                }
            }
            else {
                std::cout << BACK_LINE << std::flush;
            }
        }
    }
    
    quit = true;

    //std::this_thread::sleep_for(std::chrono::seconds(5));

    if (!noaudio) {
        vosk_recognizer_free(vrec);
        vosk_model_free(vmodel);
    }
    
    if (!noxplane) {
        //socket.disconnect("tcp://127.0.0.1:5555");
        socket.close();
        context.close();
    }
    
    SDL_Quit();
    enableInput();

    return EXIT_SUCCESS;
}

