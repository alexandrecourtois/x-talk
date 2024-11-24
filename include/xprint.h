#ifndef XPRINTF_H
#define XPRINTF_H

#include <sstream>
#include "globals.h"


#define BLINK       "\033[5m"
#define BACK_LINE   "\033[A"
#define CLEAN_LINE  "\033[2K\r"

#define RESET       "\033[0m"     // Réinitialiser la couleur
#define BLACK       "\033[30m"    // Noir
#define RED         "\033[31m"    // Rouge
#define GREEN       "\033[32m"    // Vert
#define YELLOW      "\033[33m"    // Jaune
#define BLUE        "\033[34m"    // Bleu
#define MAGENTA     "\033[35m"    // Magenta
#define CYAN        "\033[36m"    // Cyan
#define WHITE       "\033[37m"    // Blanc

// Couleurs vives
#define BLACK_BOLD  "\033[1;30m"   // Noir vif
#define RED_BOLD    "\033[1;31m"   // Rouge vif
#define GREEN_BOLD  "\033[1;32m"   // Vert vif
#define YELLOW_BOLD "\033[1;33m"   // Jaune vif
#define BLUE_BOLD   "\033[1;34m"   // Bleu vif
#define MAGENTA_BOLD "\033[1;35m"  // Magenta vif
#define CYAN_BOLD   "\033[1;36m"   // Cyan vif
#define WHITE_BOLD  "\033[1;37m"   // Blanc vif

// Couleurs de fond
#define BG_BLACK       "\033[40m"   // Fond noir
#define BG_RED         "\033[41m"   // Fond rouge
#define BG_GREEN       "\033[42m"   // Fond vert
#define BG_YELLOW      "\033[43m"   // Fond jaune
#define BG_BLUE        "\033[44m"   // Fond bleu
#define BG_MAGENTA     "\033[45m"   // Fond magenta
#define BG_CYAN        "\033[46m"   // Fond cyan
#define BG_WHITE       "\033[47m"   // Fond blanc

// Couleurs de fond vives
#define BG_BLACK_BOLD  "\033[100m"  // Fond noir vif
#define BG_RED_BOLD    "\033[101m"  // Fond rouge vif
#define BG_GREEN_BOLD  "\033[102m"  // Fond vert vif
#define BG_YELLOW_BOLD "\033[103m"  // Fond jaune vif
#define BG_BLUE_BOLD   "\033[104m"  // Fond bleu vif
#define BG_MAGENTA_BOLD "\033[105m" // Fond magenta vif
#define BG_CYAN_BOLD   "\033[106m"  // Fond cyan vif
#define BG_WHITE_BOLD  "\033[107m"  // Fond blanc vif

// Simple line border characters
#define TL_SIMPLE "┌"  // Top Left Corner
#define TR_SIMPLE "┐"  // Top Right Corner
#define BL_SIMPLE "└"  // Bottom Left Corner
#define BR_SIMPLE "┘"  // Bottom Right Corner
#define H_SIMPLE  "─"  // Horizontal Line
#define V_SIMPLE  "│"  // Vertical Line

// Double line border characters
#define TL_DOUBLE "╔"  // Top Left Corner
#define TR_DOUBLE "╗"  // Top Right Corner
#define BL_DOUBLE "╚"  // Bottom Left Corner
#define BR_DOUBLE "╝"  // Bottom Right Corner
#define H_DOUBLE  "═"  // Horizontal Line
#define V_DOUBLE  "║"  // Vertical Line

enum XType {
    M_DEFAULT,
    M_INIT,
    M_DONE,
    M_REQU,
    M_ERROR,
    M_WARNING,
    M_HELP,
    M_INFO,
    M_INVITE,
    M_CLEAN,
    M_USER,
    M_USER_ALT,
    M_XTALK,
    M_XTALK_ALT,
    M_BLINK_BEGIN,
    M_BLINK_END,
    M_ENDL
};

#define SIMPLE      5
#define DOUBLE      6

extern std::string LAST_COLOR;

template <typename T> std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void xprint(XType type = M_DEFAULT, std::string msg = "", std::string ext = "", log4cpp::Category* logger = xlog);
void xprintbox(char type, std::string text, std::string bcolor = WHITE, std::string tcolor = WHITE);

#endif