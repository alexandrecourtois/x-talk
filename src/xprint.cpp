#include <msg.h>
#include <xprint.h>
#include <session.h>
#include <tools.h>
#include <lang.h>

#define XPRINT      X_OUTPUT::xprint

std::string X_OUTPUT::__LAST_COLOR(RESET);

std::string padRight(const std::string& str, size_t totalLength) {
    if (str.length() >= totalLength)
        return str;
    return str + std::string(totalLength - str.length(), ' ');
}

const std::string& X_OUTPUT::get_LAST_COLOR() {
    return __LAST_COLOR;
}

std::string X_OUTPUT::xprint(MSG_STYLE style, std::string msg, std::string ext) {
    std::ostringstream oss;

    if (style != MSG_STYLE::DONE)
        oss << CLEAN_LINE;
    
    oss << RESET;
   
    switch(style) {
        case MSG_STYLE::CLEAN:
            oss << RESET << CLEAN_LINE;
            break;
            
        case MSG_STYLE::INVITE:
            oss << RESET << "> ";
            //if (logger) logger->info("> ");
            break;
            
        case MSG_STYLE::INFO:
            oss << YELLOW_BOLD << "I " << RESET << BG_YELLOW << BLACK << msg;
            
            if (!ext.empty())
                oss << ": " << ext;
            
            oss << RESET << std::endl;

            break;
            
        case MSG_STYLE::HELP:
            oss << MAGENTA << "H " << MAGENTA_BOLD << msg << RESET << MAGENTA << ": " << ext << RESET << std::endl;
            break;
            
        case MSG_STYLE::WARNING:
            oss << YELLOW_BOLD << "W " << RESET << msg;
            
            if (!ext.empty()) {
                oss << ": " << YELLOW_BOLD << ext << RESET;
            }

            oss << std::endl;
            
            __LAST_COLOR = RESET;
            break;
        
        case MSG_STYLE::INIT:
            oss << RED_BOLD << "* " << RESET << msg;
            
            if (!ext.empty()) {
                oss << ": " << YELLOW_BOLD << ext << RESET;
            }
                
            __LAST_COLOR = RESET;
            oss << "..." << std::flush; 

            break;
            
        case MSG_STYLE::DONE:
            oss << GREEN_BOLD << " " + Lang::getString(T_MSG::DONE) << RESET << ".";

            if (!msg.empty()) {
                oss << CYAN_BOLD << " (" << msg << ")" << RESET;
            }
                
            __LAST_COLOR = RESET;
            oss << std::endl;
            
            break;
            
        case MSG_STYLE::DEFAULT:
            oss << msg << std::flush;
            
            break;
            
        case MSG_STYLE::REQU:
            if (ext.empty()) {
                oss << MAGENTA_BOLD << "? " << WHITE_BOLD << msg << ": " << MAGENTA_BOLD;
                __LAST_COLOR = MAGENTA_BOLD;
            }
            else {
                oss << MAGENTA_BOLD << msg << " " << WHITE_BOLD << ext << RESET << std::endl;
                __LAST_COLOR = RESET;
            }
            
            break;
            
        case MSG_STYLE::M_ERROR:
            oss << RED_BOLD << "! " << msg << "." << RESET << std::endl;
            __LAST_COLOR = RESET;
            
            break;

        case MSG_STYLE::USER:
            oss << BG_CYAN_BOLD << BLACK << ' ' << SESSION::aircraft.tailnum << ' ' << RESET << CYAN_BOLD << ": " << msg << RESET << std::endl;
            break;

        case MSG_STYLE::USER_ALT:
            oss << BG_RED_BOLD << BLACK << ' ' << SESSION::aircraft.tailnum << ' ' << RESET << RED_BOLD << ": " << msg << RESET << std::endl;
            break;

        case MSG_STYLE::XTALK:
            oss << BG_GREEN_BOLD << BLACK << SESSION::call_ID << RESET << GREEN_BOLD << ": " << msg << RESET << std::endl;
            break;

        case MSG_STYLE::XTALK_ALT:
            oss << BG_RED_BOLD << WHITE_BOLD << SESSION::call_ID << RESET << RED_BOLD << ": " << msg << RESET << std::endl;
            break;

        case MSG_STYLE::BLINK_BEGIN:
            oss << /*"> " <<*/ WHITE_BOLD << BG_RED_BOLD << BLINK << msg << RESET;
            break;

        case MSG_STYLE::BLINK_END:
            oss << BACK_LINE << CLEAN_LINE;
            break;

        case MSG_STYLE::ENDL:
            oss << std::endl;
            break;

        case MSG_STYLE::CMD:
            oss << WHITE_BOLD << ": " << msg;
            
            if (!ext.empty())
                oss << GREEN_BOLD << ext;

            oss << RESET << std::endl;
            break;

        case MSG_STYLE::KEYWORDS:
            oss << BLUE_BOLD << "K " << RESET << BG_BLUE_BOLD << BLACK << msg << RESET << std::endl;
            break;

        default:
            break;
    }
    
    std::cout << oss.str() << std::flush;
    return oss.str();
}

void X_OUTPUT::X_OUTPUT::xprintbox(BOX_STYLE style, std::string text, std::string bcolor, std::string tcolor) {
    if (style == BOX_STYLE::DOUBLE) {
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
    
    if (style == BOX_STYLE::SIMPLE) {
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
}