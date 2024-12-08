#include <SDL2/SDL_joystick.h>
#include <tools.h>
#include <inputs.h>
#include <iostream>
#include <ostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <thread>
#include <xprint.h>
#include <utf8.h>
#include <audio.h>
#include <session.h>

void IN::disableInput() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON; // Mode non canonique : saisie immédiate sans Entrée
    t.c_lflag &= ~ECHO;   // Désactiver l'écho : n'affiche pas les caractères
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void IN::enableInput() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON; // Réactiver le mode canonique
    t.c_lflag |= ECHO;   // Réactiver l'écho
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    
    tcflush(STDIN_FILENO, TCIFLUSH);
}

char IN::kbhit(bool in_xscan) {
    int ch;
    bool ret = false;

    // Sauvegarder les paramètres du terminal
    tcgetattr(STDIN_FILENO, &__old_term);
    __new_term = __old_term;

    // Passer en mode non-canonique (entrée immédiate sans avoir besoin de presse "Entrée")
    __new_term.c_lflag &= ~(ICANON | ECHO);  // Désactiver le mode canonique (pas besoin d'Entrée pour valider)
    __new_term.c_cc[VMIN] = 1;        // Lecture d'un caractère à la fois
    __new_term.c_cc[VTIME] = 0;       // Pas de délai
    tcsetattr(STDIN_FILENO, TCSANOW, &__new_term);  // Appliquer les nouveaux paramètres

    // Passer le descripteur de fichier en mode non-bloquant
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); 

    // Lire un caractère en utilisant read() pour éviter que getchar() ne bloque
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        ret = true;
        ungetc(ch, stdin);  // Remettre le caractère dans stdin
    }

    // Restaurer les paramètres du terminal
    if (!in_xscan)
        tcsetattr(STDIN_FILENO, TCSANOW, &__old_term);

    return ret;  // Retourne true si un caractère est disponible
}

bool IN::__isUtf8Character(const std::string& str) {
    try {
        auto it = str.begin();
        while (it != str.end()) {
            char32_t character = utf8::next(it, str.end());
            if (character > 127) { // Non-ASCII
                return true;
            }
        }
    } catch (const utf8::exception&) {
        std::cerr << "Invalid UTF-8 encoding detected!" << std::endl;
    }
    return false;
}

bool IN::__isLastCharacterASCII(const std::string& str) {
    if (str.empty()) {
        return false;  // La chaîne est vide, pas de dernier caractère
    }

    // Commencer à la fin de la chaîne
    size_t i = str.size() - 1;

    unsigned char byte = str[i];

    // Si c'est un caractère ASCII (1 octet)
    if ((byte & 0x80) == 0) {
        return true;
    }
    // Si c'est le début d'un caractère sur 2 octets
    else if ((byte & 0xE0) == 0xC0) {
        if (i == 0 || (str[i - 1] & 0xC0) != 0x80) {
            return false;  // Séquence invalide
        }
        return true;
    }
    // Si c'est le début d'un caractère sur 3 octets
    else if ((byte & 0xF0) == 0xE0) {
        if (i < 2 || (str[i - 1] & 0xC0) != 0x80 || (str[i - 2] & 0xC0) != 0x80) {
            return false;  // Séquence invalide
        }
        return true;
    }
    // Si c'est le début d'un caractère sur 4 octets
    else if ((byte & 0xF8) == 0xF0) {
        if (i < 3 || (str[i - 1] & 0xC0) != 0x80 || (str[i - 2] & 0xC0) != 0x80 || (str[i - 3] & 0xC0) != 0x80) {
            return false;  // Séquence invalide
        }
        return true;
    }

    return false;  // Si c'est un octet invalide
}

void IN::xscan(std::string &str, void (*callback)(), int delay) {
    std::string input;
    char ch;
    auto last_time = std::chrono::steady_clock::now();
    auto interval = std::chrono::milliseconds(delay);
    
    enableInput();

    while (!ptt_pushed()) {
        if (callback) {
            auto now = std::chrono::steady_clock::now();
            
            if (now - last_time >= interval) {
                callback();
                last_time = now;
            }
        }
        
        if (kbhit(true)) {  // Si un caractère est disponible
            ch = getchar();  // Lire le caractère
            
            // Détection et suppression des séquences de flèches
            if (ch == '\033') {  // Première séquence pour une touche fléchée (ESC)
                getchar();       // Ignorer le caractère '['
                getchar();       // Ignorer 'A', 'B', 'C', ou 'D'
                continue;        // Ignorer cette séquence de touche fléchée
            }

            // Gestion du backspace
            if (ch == 127 || ch == '\b') {  // Backspace peut être '\b' ou 127
                if (!input.empty()) {
                    //containsUtf8(input);
                    //do {
                        if (!__isLastCharacterASCII(input))
                            input.pop_back();
                            
                        input.pop_back();  // Supprimer le dernier caractère
                    //} while (!input.empty() && !isStartOfUtf8Char(input.back()));
                    
                    std::cout << RESET << "\b " << OUT::__LAST_COLOR << "\b" << std::flush;  // Effacer le dernier caractère affiché
                }
            } else if (ch == '\n') {  // Si la touche Entrée est appuyée
                std::cout << std::endl;
                break;  // Quitter la boucle
            } else {
                if (!AUDIO::isRecording()) {
                    input.push_back(ch);  // Ajouter le caractère à l'entrée
                    std::cout << ch << std::flush;  // Afficher le caractère saisi
                }
            }
            
            tcsetattr(STDIN_FILENO, TCSANOW, &__old_term);
        }
        
        usleep(1000);
    }
    
    disableInput();
    
    str = input;
}

void IN::xscan_anykey() {
    while(!kbhit()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        tcsetattr(STDIN_FILENO, TCSANOW, &__old_term);
    }
}

void IN::xscan() {
    xscan(__garbage);
}

bool IN::ptt_changed() {
    SDL_Event event;
    
    while(SDL_PollEvent(&event)) { };
    
    int ptt_state = SDL_JoystickGetButton(__joystick, __ptt_button);
    
    if (ptt_state != __prev_ptt_state) {
        std::cout << std::endl << std::flush;
        __prev_ptt_state = ptt_state;
        return true;
    }
    
    return false;
}

bool IN::ptt_pushed() {
    if (!SESSION::no_joystick) {
        SDL_Event event;
    
        while(SDL_PollEvent(&event)) { };
    
        return SDL_JoystickGetButton(__joystick, __ptt_button);
    }
    
    return false;
}

void IN::selectJoystick() {
    SDL_Init(SDL_INIT_JOYSTICK);
    
    OUT::xprint(OUT::MSG_STYLE::INIT, "Listing available devices");
    
    int numJoysticks = SDL_NumJoysticks();

    if (numJoysticks == 0) {
        printf("No device found.\n");
    }
    
    std::cout << std::endl;
    
    for (int i = 0; i < numJoysticks; ++i) {
        const char *joystickName = SDL_JoystickNameForIndex(i);
        if (joystickName) {
            OUT::xprint(OUT::MSG_STYLE::REQU, std::to_string(i), joystickName);
        } else {
            printf("%d : Unknown name\n", i);
        }
    }
    
    int joystickIndex;
    bool selected = false;
    
    if (numJoysticks) {
        joystickIndex = TOOLBOX::selectById(numJoysticks);

        // Ouvrir le joystick sélectionné
        __joystick = SDL_JoystickOpen(joystickIndex);
        if (__joystick == NULL) {
            fprintf(stderr, "Impossible d'ouvrir le joystick %d : %s\n", joystickIndex, SDL_GetError());
        }

        OUT::xprint(OUT::MSG_STYLE::INIT, "Selecting device", SDL_JoystickName(__joystick));
        OUT::xprint(OUT::MSG_STYLE::DONE);

        // Boucle principale pour détecter les événements de bouton
        SDL_Event event;
        OUT::xprint(OUT::MSG_STYLE::REQU, "Press any button on the device");
        selected = false;
        
        while (!selected) {
            // Vérifier les événements
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_JOYBUTTONUP) {
                    __ptt_button = event.jbutton.button;
                    selected = true;
                    OUT::xprint(OUT::MSG_STYLE::DONE);
                }
            }
            
            // Pause pour éviter une boucle excessive
            SDL_Delay(10);
        }
    }
}

struct termios  IN::__old_term;
struct termios  IN::__new_term;
int             IN::__prev_ptt_state = 0;
std::string     IN::__garbage;
int             IN::__ptt_button;
SDL_Joystick*   IN::__joystick;
