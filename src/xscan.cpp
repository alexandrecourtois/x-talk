#include "xscan.h"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ptt.h"
#include <chrono>
#include <thread>
#include "xprint.h"
#include <utf8.h>
#include "globals.h"

void disableInput() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON; // Mode non canonique : saisie immédiate sans Entrée
    t.c_lflag &= ~ECHO;   // Désactiver l'écho : n'affiche pas les caractères
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enableInput() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON; // Réactiver le mode canonique
    t.c_lflag |= ECHO;   // Réactiver l'écho
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    
    tcflush(STDIN_FILENO, TCIFLUSH);
}

struct termios oldt, newt;

char kbhit() {
    //struct termios oldt, newt;
    int ch;
    bool ret = false;

    // Sauvegarder les paramètres du terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Passer en mode non-canonique (entrée immédiate sans avoir besoin de presse "Entrée")
    newt.c_lflag &= ~(ICANON | ECHO);  // Désactiver le mode canonique (pas besoin d'Entrée pour valider)
    newt.c_cc[VMIN] = 1;        // Lecture d'un caractère à la fois
    newt.c_cc[VTIME] = 0;       // Pas de délai
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);  // Appliquer les nouveaux paramètres

    // Passer le descripteur de fichier en mode non-bloquant
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); 

    // Lire un caractère en utilisant read() pour éviter que getchar() ne bloque
    if (read(STDIN_FILENO, &ch, 1) == 1) {
        ret = true;
        ungetc(ch, stdin);  // Remettre le caractère dans stdin
    }

    // Restaurer les paramètres du terminal
    //tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ret;  // Retourne true si un caractère est disponible
}

bool isUtf8Character(const std::string& str) {
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

bool isLastCharacterASCII(const std::string& str) {
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

void xscan(std::string &str, void (*callback)(), int delay) {
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
        
        if (kbhit()) {  // Si un caractère est disponible
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
                        if (!isLastCharacterASCII(input))
                            input.pop_back();
                            
                        input.pop_back();  // Supprimer le dernier caractère
                    //} while (!input.empty() && !isStartOfUtf8Char(input.back()));
                    
                    std::cout << RESET << "\b " << LAST_COLOR << "\b" << std::flush;  // Effacer le dernier caractère affiché
                }
            } else if (ch == '\n') {  // Si la touche Entrée est appuyée
                std::cout << std::endl;
                break;  // Quitter la boucle
            } else {
                if (!recording) {
                    input.push_back(ch);  // Ajouter le caractère à l'entrée
                    std::cout << ch << std::flush;  // Afficher le caractère saisi
                }
            }
            
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
        
        usleep(1000);
    }
    
    disableInput();
    
    str = input;
}

std::string garbage;

void xscan() {
    xscan(garbage);
}