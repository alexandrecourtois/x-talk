#include <tools.h>
#include <stdexcept>
#include <xprint.h>
#include <inputs.h>
#include <regex>
#include <sstream>
#include <algorithm>
#include <regex>
#include <random>
#include <cmath>
#include <fstream>

std::string TOOLBOX::removeQuotes(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
    return str;
}

std::optional<int> TOOLBOX::toInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

int TOOLBOX::selectById(int count_id) {
    bool selected = false;
    int index;
    
    while(!selected) {
        OUT::xprint(OUT::MSG_STYLE::REQU, "Select one device by ID");
        
        std::string indexStr;
        std::optional<int> result;
                
        do {
            IN::xscan(indexStr);
            result = toInt(indexStr);
        } while(!result);
                
        index = *result;

        if (index < 0 || index >= count_id) {
            OUT::xprint(OUT::MSG_STYLE::ERROR, "Invalid ID");
        }
        else
        {
            selected = true;
        }
    }
    
    return index;
}

bool TOOLBOX::isValidIP(const std::string& ip) {
    // Expression régulière pour une adresse IPv4
    const std::regex ipRegex(
        R"(^((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])\.){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])$)"
    );

    // Vérifie si l'adresse IP correspond au motif
    return std::regex_match(ip, ipRegex);
}

std::string TOOLBOX::removeAnsiSq(const std::string& input) {
    // Regex pour détecter les séquences ANSI
    std::regex ansi_escape_regex("\033\\[[0-9;]*[mK]");
    
    // Remplacer toutes les séquences ANSI par une chaîne vide
    std::string result = std::regex_replace(input, ansi_escape_regex, "");
    
    // Retourner la chaîne nettoyée
    return result;
}

double TOOLBOX::round(double value, int decimalPlaces) {
    double factor = std::pow(10, decimalPlaces);
    return std::round(value * factor) / factor;
}

std::string TOOLBOX::toString(double value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

int TOOLBOX::countLines(const std::string &str) {
    return std::count(str.begin(), str.end(), '\n');
}

void TOOLBOX::displayProgressBar(const std::string& msg, int current, int total) {
    int barWidth = 40;
    float progress = (float)current / total;
    int pos = barWidth * progress;
    
    std::cout << '\r';
    OUT::xprint(OUT::MSG_STYLE::INIT, msg, "", nullptr);
    std::cout << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %";
    std::cout.flush();
}

int TOOLBOX::generateRandomNumber(int max) {
    // Initialise le générateur aléatoire avec une graine basée sur l'horloge
    std::random_device rd; // Source aléatoire
    std::mt19937 gen(rd()); // Générateur Mersenne Twister
    std::uniform_int_distribution<> distrib(0, max); // Distribution uniforme entre 0 et max

    return distrib(gen); // Génère le nombre aléatoire
}

std::vector<std::string> TOOLBOX::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::string current;

    for (char c : str) {
        if (c == delimiter) {
            if (!current.empty()) {
                // Supprime les espaces de la sous-chaîne
                current.erase(std::remove_if(current.begin(), current.end(), ::isspace), current.end());
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    // Ajouter le dernier segment, s'il existe
    if (!current.empty()) {
        current.erase(std::remove_if(current.begin(), current.end(), ::isspace), current.end());
        result.push_back(current);
    }

    return result;
}

std::string TOOLBOX::extractBetweenBraces(const std::string& str) {
    size_t start = str.find('{');
    size_t end = str.find('}', start);

    // Si une paire d'accolades est trouvée
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return str.substr(start + 1, end - start - 1);  // Extraire la sous-chaîne entre les accolades
    }

    // Retourne une chaîne vide si les accolades ne sont pas présentes ou sont mal formées
    return "";
}

std::string TOOLBOX::removeSubstring(const std::string& str, const std::string& substring) {
    std::string result = str; // Créer une copie de la chaîne d'entrée
    size_t pos = 0;

    // Rechercher et supprimer toutes les occurrences de la sous-chaîne
    while ((pos = result.find(substring, pos)) != std::string::npos) {
        result.erase(pos, substring.length());
    }

    return result; // Retourner la chaîne modifiée
}

double TOOLBOX::toRad(double degree) {
    return degree * M_PI / 180.0;
}

// Fonction pour calculer la distance entre deux points en latitude et longitude
double TOOLBOX::haversine(double lat1, double lon1, double lat2, double lon2) {
    // Convertir les coordonnées en radians
    lat1 = toRad(lat1);
    lon1 = toRad(lon1);
    lat2 = toRad(lat2);
    lon2 = toRad(lon2);

    // Différences de coordonnées
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    // Formule de Haversine
    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    // Calcul de la distance
    return 6371.0f * c; // Distance en kilomètres
}

nlohmann::json TOOLBOX::loadJSON(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        //std::cerr << "Erreur: Impossible de lire le fichier " << filename << std::endl;
        exit(1);
    }
    nlohmann::json data;
    file >> data;
    return data;
}