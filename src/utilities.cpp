#include "utilities.h"
#include <stdexcept>
#include "xprint.h"
#include "xscan.h"
#include <regex>

std::optional<int> to_int(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

int select_by_id(int count_id) {
    bool selected = false;
    int index;
    
    while(!selected) {
        xprint(M_REQU, "Select one device by ID");
        
        std::string indexStr;
        std::optional<int> result;
                
        do {
            xscan(indexStr);
            result = to_int(indexStr);
        } while(!result);
                
        index = *result;

        if (index < 0 || index >= count_id) {
            xprint(M_ERROR, "Invalid ID");
        }
        else
        {
            selected = true;
        }
    }
    
    return index;
}

bool is_valid_IP(const std::string& ip) {
    // Expression régulière pour une adresse IPv4
    const std::regex ipRegex(
        R"(^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$)"
    );

    // Vérifie si l'adresse IP correspond au motif
    return std::regex_match(ip, ipRegex);
}

#include <regex>

std::string remove_ansi(const std::string& input) {
    // Regex pour détecter les séquences ANSI
    std::regex ansi_escape_regex("\033\\[[0-9;]*[mK]");
    
    // Remplacer toutes les séquences ANSI par une chaîne vide
    std::string result = std::regex_replace(input, ansi_escape_regex, "");
    
    // Retourner la chaîne nettoyée
    return result;
}