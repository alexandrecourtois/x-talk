#include "xprint.h"
#include <iostream>
#include "globals.h"
#include "utilities.h"

std::string LAST_COLOR(RESET);

void xprint(XType type, std::string msg, std::string ext, log4cpp::Category* logger) {
    std::cout << RESET;
   
    switch(type) {
        case M_CLEAN:
            std::cout << RESET << CLEAN_LINE;
            break;
            
        case M_INVITE:
            std::cout << RESET << "> ";
            //if (logger) logger->info("> ");
            break;
            
        case M_INFO:
            std::cout << YELLOW_BOLD << "I " << RESET << BG_YELLOW << BLACK << msg << RESET << std::endl;
            if (logger) logger->info("I " + msg + "\n");
            break;
            
        case M_HELP:
            std::cout << MAGENTA << "H " << MAGENTA_BOLD << msg << RESET << MAGENTA << ": " << ext << RESET << std::endl;
            if (logger) logger->info("H " + msg + ": " + ext + "\n");
            break;
            
        case M_WARNING:
            std::cout << YELLOW_BOLD << "W " << RESET << msg;
            if (logger) logger->info("W " + msg);
            
            if (!ext.empty()) {
                std::cout << ": " << YELLOW_BOLD << ext << RESET;
                if (logger) logger->info(": " + ext);
            }

            std::cout << std::endl;
            if (logger) logger->info("\n");
            
            LAST_COLOR = RESET;
            break;
        
        case M_INIT:
            std::cout << RED_BOLD << "* " << RESET << msg;
            if (logger) logger->info("* " + msg);
            
            if (!ext.empty()) {
                std::cout << ": " << YELLOW_BOLD << ext << RESET;
                if (logger) logger->info(": " + ext);
            }
                
            LAST_COLOR = RESET;
            std::cout << "..." << std::flush; 
            if (logger) logger->info("...");

            break;
            
        case M_DONE:
            std::cout << GREEN_BOLD << " done" << RESET << ".";
            if (logger) logger->info(" done.");

            if (!msg.empty()) {
                std::cout << CYAN_BOLD << "(" << msg << ")" << RESET;
                if (logger) logger->info("(" + msg + ")");
            }
                
            LAST_COLOR = RESET;
            std::cout << std::endl;
            if (logger) logger->info("\n");
            
            break;
            
        case M_DEFAULT:
            std::cout << msg << std::flush;
            if (logger) logger->info(msg);
            
            break;
            
        case M_REQU:
            if (ext.empty()) {
                std::cout << MAGENTA_BOLD << "? " << WHITE_BOLD << msg << ": " << MAGENTA_BOLD;
                if (logger) logger->info("?" + msg + ": ");
                LAST_COLOR = MAGENTA_BOLD;
            }
            else {
                std::cout << MAGENTA_BOLD << msg << " " << WHITE_BOLD << ext << RESET << std::endl;
                if (logger) logger->info(msg + " " + ext + "\n");
                LAST_COLOR = RESET;
            }
            
            break;
            
        case M_ERROR:
            std::cout << RED_BOLD << "! " << msg << "." << RESET << std::endl;
            if (logger) logger->info("! " + msg + ".\n");
            LAST_COLOR = RESET;
            
            break;

        case M_USER:
            std::cout << BG_CYAN_BOLD << BLACK << ' ' << aircraft.tailnum << ' ' << RESET << CYAN_BOLD << ": " << msg << RESET << std::endl;
            if (logger) logger->info(" " + aircraft.tailnum + " : " + msg + "\n");
            break;

        case M_USER_ALT:
            std::cout << BG_RED_BOLD << BLACK << ' ' << aircraft.tailnum << ' ' << RESET << RED_BOLD << ": " << msg << RESET << std::endl;
            if (logger) logger->info(" " + aircraft.tailnum + " : " + msg + "\n");
            break;

        case M_XTALK:
            std::cout << BG_GREEN_BOLD << BLACK << speaker << RESET << GREEN_BOLD << ": " << msg << RESET << std::endl;
            if (logger) logger->info(speaker + ": " + remove_ansi(msg) + "\n");
            break;

        case M_XTALK_ALT:
            std::cout << BG_RED_BOLD << WHITE_BOLD << speaker << RESET << RED_BOLD << ": " << msg << RESET << std::endl;
            if (logger) logger->info(speaker + ": " + msg);
            break;

        case M_BLINK_BEGIN:
            std::cout << WHITE_BOLD << BG_RED_BOLD << BLINK << msg << RESET;
            break;

        case M_BLINK_END:
            std::cout << BACK_LINE << CLEAN_LINE;
            break;

        case M_ENDL:
            std::cout << std::endl;
            break;

        default:
            break;
    }
    
    std::cout << std::flush;
}

void xprintbox(char type, std::string text, std::string bcolor, std::string tcolor) {
    if (type == DOUBLE) {
        std::cout << bcolor << TL_DOUBLE;
        for (size_t i = 0; i < text.size() + 2; ++i) {
            std::cout << H_DOUBLE;
        }
        std::cout << TR_DOUBLE << std::endl;

        std::cout << V_DOUBLE << " " << tcolor << text << RESET << bcolor << " " << V_DOUBLE << std::endl;

        std::cout << BL_DOUBLE;

        for (size_t i = 0; i < text.size() + 2; ++i) {
            std::cout << H_DOUBLE;
        }

        std::cout << BR_DOUBLE << std::endl;
    }
    
    if (type == SIMPLE) {
        std::cout << bcolor << TL_SIMPLE;
        for (size_t i = 0; i < text.size() + 2; ++i) {
            std::cout << H_SIMPLE;
        }
        std::cout << TR_SIMPLE << std::endl;

        std::cout << V_SIMPLE << " " << tcolor << text << RESET << bcolor << " " << V_SIMPLE << std::endl;

        std::cout << BL_SIMPLE;

        for (size_t i = 0; i < text.size() + 2; ++i) {
            std::cout << H_SIMPLE;
        }

        std::cout << BR_SIMPLE << std::endl;
    }
    
    //std::cout << RESET;
}