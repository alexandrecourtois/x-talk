#include "msg.h"
#include "speaker.h"
#include <cstdlib>
#include <pch.h>
#include <xprint.h>
#include <reqrsp.h>
#include <session.h>
#include <zmq_client.h>
#include <zmq_server.h>
#include <audio.h>
#include <session.h>
#include <tools.h>
#include <airport.h>
#include <inputs.h>
#include <interp.h>
#include <updater.h>
#include <lang.h>
#include <en_US.h>
#include <fr_FR.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#define __VERSION__MAJOR    "0"
#define __VERSION__MINOR    "01"
#define __LICENSE           "GPLv3"

//#define TEST_MODE

zmq::context_t __GLOBAL__context(1);

void connect_xp() {
    X_OUTPUT::xprint(MSG_STYLE::INIT,lang(T_MSG::CONNECTING_TO_XPLANE));
    
    Req rq;
    rq.type = Request::GET_DATAFRAME;
    SESSION::client->sendRequest(rq);
 
    X_OUTPUT::xprint(MSG_STYLE::DONE, "X-Plane " + std::to_string(SESSION::dataframe.xp_major / 1000) + "." + std::to_string((SESSION::dataframe.xp_major / 10) % ((SESSION::dataframe.xp_major / 1000) * 100)));    
}

void waitfor_aircraft() {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::WAITING_FOR_AIRCRAFT));

    Req rq;
    rq.type = Request::GET_DATAFRAME;

    SESSION::client->sendRequest(rq);

    SESSION::aircraft.tailnum = SESSION::dataframe.tailnum;
    X_OUTPUT::xprint(MSG_STYLE::DONE, SESSION::aircraft.tailnum);
}

bool process_args(int argc, char** argv) {
    // Parcours des arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Traitement des options longues
        if (arg == "--no-audio-rec") {
            SESSION::no_audio = true;
            SESSION::no_joystick = true;
        } else if (arg == "--no-joystick") {
            SESSION::no_joystick = true;
        } else if (arg == "--no-xplane") {
            SESSION::no_xplane = true;
        } else if (arg == "--xp-address") {
            if (i < argc) {
                if (TOOLBOX::isValidIP(argv[i+1])) {
                    SESSION::ip_address = argv[i+1];
                } else {
                    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, "Invalid IP address. Networking disabled");
                    SESSION::no_xplane = true;
                }
            }
        } else if (arg == "--monitor") {
            SESSION::no_audio = true;
            SESSION::no_joystick = true;
            SESSION::no_xplane = true;
            SESSION::is_monitor = true;
        }
        // Traitement des options courtes avec "-"
        else if (arg[0] == '-' && arg.length() > 1 && arg[1] != '-') {
            // Boucle sur chaque caractère après "-"
            for (size_t j = 1; j < arg.length(); ++j) {
                switch (arg[j]) {
                    case 'a':
                        SESSION::no_audio = true;
                        SESSION::no_joystick = true;
                        break;
                    case 'j':
                        SESSION::no_joystick = true;
                        break;
                    case 'x':
                        SESSION::no_xplane = true;
                        break;
                    default:
                        std::cerr << "Unknown argument : -" << arg[j] << std::endl;
                        return false; // Sortie avec erreur
                }
            }
        }
    }

    return true;
}

void init_netw() {
}

void init_main() {
    if (SESSION::no_xplane)
        X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::FAKE_SERVER_ENABLED));
    
    if (!SESSION::no_audio) {
        AUDIO::selectDevice();
        AUDIO::init("audio/VOIX_AFIS/", "phrases.cfg");
    }
    else
        X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::DISABLE), "AUDIO");
    
    if (!SESSION::is_monitor) {
        AIRPORTS::loadAiports("airports.json");
        SESSION::agent = Speaker(SESSION::call_ID, "vosk-model-small-fr-0.22", "responses.json");
    }
    
    if (!SESSION::no_joystick)
        X_INPUT::selectJoystick();
    else
        X_OUTPUT::xprint(MSG_STYLE::WARNING, lang(T_MSG::DISABLE), "JOYSTICK");
    
    if (SESSION::no_xplane) {
        SESSION::server_fake = new ZMQ_Server(__GLOBAL__context, INPROC_Server("fake_server"), SESSION::dataframe_fake);
        SESSION::server_fake->start();
    }

    //init_netw();
    if (!SESSION::no_xplane)
        SESSION::client = new ZMQ_Client(__GLOBAL__context, TCP_Server(SESSION::ip_address + ":5555"));
    else
        SESSION::client = new ZMQ_Client(__GLOBAL__context, INPROC_Server("fake_server"));

    connect_xp();
    waitfor_aircraft();
    SESSION::client->start();

    if (SESSION::is_monitor) {
        SESSION::call_ID = "MONITOR";
        SESSION::client_monitor = new ZMQ_Client(__GLOBAL__context, TCP_Server("127.0.0.1:5556"));
        SESSION::client_monitor->start();
    } else {
        SESSION::server_monitor = new ZMQ_Server(__GLOBAL__context, TCP_Server("127.0.0.1:5556"));
        SESSION::server_monitor->start();
    }
}

#ifndef TEST_MODE
int main(int argc, char **argv) {
    #ifdef PLATFORM_LINUX
    system("clear");
    #endif
    
    #ifdef PLATFORM_WINDOWS
    SetConsoleOutputCP(CP_UTF8);
    system("cls");
    #endif

    X_OUTPUT::xprintbox(X_OUTPUT::BOX_STYLE::DOUBLE,
              "xtalk -- version " + std::string(__VERSION__MAJOR) + "." + std::string(__VERSION__MINOR) + " -- " + std::string(__LICENSE),
              WHITE,
              CYAN_BOLD);

    //Lang::setLang<en_US>();
    Lang::setLang<fr_FR>();
    
    if (!process_args(argc, argv))
        return EXIT_FAILURE;
    
    X_INPUT::disableInput();

    init_main();
 
    std::string input;

    X_OUTPUT::xprint(MSG_STYLE::INFO, lang(T_MSG::GET_HELP));
    CMD::run("cmd_getmem");

    while (true) {
        if (SESSION::show_readback && SESSION::agent.isWaitingForReadback())
            X_OUTPUT::xprint(MSG_STYLE::WARNING, "Waiting for readback");

        if (SESSION::show_keywords)
            X_OUTPUT::xprint(MSG_STYLE::KEYWORDS, SESSION::agent.getKeywords());

        X_OUTPUT::xprint(MSG_STYLE::INVITE);

        if (!SESSION::is_monitor)
            X_INPUT::xscan(input, update);
        else
            X_INPUT::xscan(input);

        if (input == "q" || input == "quit") {
            std::cout << BG_GREEN_BOLD << BLACK << SESSION::call_ID << RESET << GREEN_BOLD << ": " << Lang::getString(T_MSG::GOODBYE) << RESET << std::endl;
            break;
        }

        if (std::equal(SESSION::cmd_prefix.begin(), SESSION::cmd_prefix.end(), input.begin()) || SESSION::is_monitor) {
            CMD::run(input);
        }
        else if (!SESSION::is_monitor)
            SESSION::agent.tell(input);
    }

    delete SESSION::client;
    
    if (SESSION::no_xplane)
        delete SESSION::server_fake;

    if (SESSION::is_monitor)
        delete SESSION::client_monitor;
    else
        delete SESSION::server_monitor;

    SDL_Quit();
    X_INPUT::enableInput();

    return EXIT_SUCCESS;
}
#else
int main(int argc, char **argv) {
    std::string str1;
    std::string str2;

    while(true) {
        std::cin >> str1;
        std::cin >> str2;

        std::cout << "d.......:" << TOOLBOX::getLevenshteinDistance(str1, str2) << std::endl;
        std::cout << "d_alt...:" << TOOLBOX::getLevenshteinDistance_alt(str1, str2) << std::endl;
    }

    return EXIT_SUCCESS;
}
#endif

