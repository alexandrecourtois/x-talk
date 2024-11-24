#include "eval.h"
#include <fstream>
#include <random>
#include "globals.h"
#include <iostream>
#include <map>
#include <string>
#include "dataframe.h"
#include "airport.h"
#include <functional>
#include <regex>
#include "xprint.h"
#include <cmath>

#define AUDIO       &callbacks_audio
#define TEXT        &callbacks_text

enum value_key {
    key_visibility,
    key_qnh,
    key_windspeed,
    key_winddir,
    key_runway,
    key_holdpoint
};

std::map<std::string, value_key> value_keys;
std::map<std::string, std::function<std::string()>> callbacks_audio;
std::map<std::string, std::function<std::string()>> callbacks_text; 

// Fonction pour charger les réponses depuis un fichier JSON
nlohmann::json load_json(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        //std::cerr << "Erreur: Impossible de lire le fichier " << filename << std::endl;
        exit(1);
    }
    nlohmann::json data;
    file >> data;
    return data;
}

// Fonction pour évaluer une expression avec des opérateurs | et &
bool evaluateExpression(const std::string& expression, const std::set<std::string>& userWords) {
    std::istringstream stream(expression);
    std::string group;
    bool finalResult = true;

    while (std::getline(stream, group, '&')) {
        std::istringstream orStream(group);
        std::string orKeyword;
        bool orMatch = false;

        while (std::getline(orStream, orKeyword, '|')) {
            orKeyword.erase(remove(orKeyword.begin(), orKeyword.end(), '('), orKeyword.end());
            orKeyword.erase(remove(orKeyword.begin(), orKeyword.end(), ')'), orKeyword.end());

            if (userWords.find(orKeyword) != userWords.end()) {
                orMatch = true;
                break;
            }
        }

        finalResult = finalResult && orMatch;
        if (!finalResult) break;
    }
    return finalResult;
}

// Fonction pour vérifier si au moins un mot de userInput correspond aux mots-clés
bool matchesKeyword(const std::string& keywords, const std::set<std::string>& userWords) {
    return evaluateExpression(keywords, userWords);
}

// Fonction pour obtenir la réponse en fonction de l'entrée utilisateur et du nœud actuel
nlohmann::json* getResponse(nlohmann::json* currentNode, const std::set<std::string>& userWords, nlohmann::json& responses) {
    if (currentNode->contains("responses")) {
        for (auto& [key, responseNode] : (*currentNode)["responses"].items()) {
            if (responseNode.contains("keywords") && matchesKeyword(responseNode["keywords"], userWords)) {
                if (responseNode.contains("goto")) {
                    return &responses["root"]["responses"][responseNode["goto"].get<std::string>()];
                }
                return &responseNode;
            }
        }
    }
    return nullptr;
}

// Fonction pour séparer les mots de l'entrée utilisateur
std::set<std::string> splitUserInput(const std::string& userInput) {
    std::set<std::string> words;
    std::istringstream stream(userInput);
    std::string word;

    while (stream >> word) {
        words.insert(word);
    }

    return words;
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
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

std::string extractBetweenBraces(const std::string& str) {
    size_t start = str.find('{');
    size_t end = str.find('}', start);

    // Si une paire d'accolades est trouvée
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return str.substr(start + 1, end - start - 1);  // Extraire la sous-chaîne entre les accolades
    }

    // Retourne une chaîne vide si les accolades ne sont pas présentes ou sont mal formées
    return "";
}

int generateRandomNumber(int max) {
    // Initialise le générateur aléatoire avec une graine basée sur l'horloge
    std::random_device rd; // Source aléatoire
    std::mt19937 gen(rd()); // Générateur Mersenne Twister
    std::uniform_int_distribution<> distrib(0, max); // Distribution uniforme entre 0 et max

    return distrib(gen); // Génère le nombre aléatoire
}

std::string get_visibility_AUDIO() {
    int v;
    
    {
        LOCK;
        v = static_cast<int>(dataframe.visibility);
    }
                
    if (v < 1500)
        return std::to_string(1500);
                    
    if (v < 5000)
        return std::to_string(5000);
                    
    return std::to_string(5001);
}

std::string get_visibility_TEXT() {
    int v;
    
    {
        LOCK;    
        v = static_cast<int>(dataframe.visibility);
    }
                
    if (v < 1500)
        return std::string("< 1500m");
                    
    if (v < 5000)
        return std::string("< 5000m");
                    
    return std::string("> 5000m");
}

std::string get_qnh() {
    LOCK;
    return std::to_string(static_cast<int>(std::round(dataframe.qnh / 100.0f)));
}

std::string get_windspeed() {
    LOCK;
    return std::to_string(static_cast<int>(dataframe.windspeed));
}

std::string get_winddir() {
    LOCK;
    return std::to_string(static_cast<int>(dataframe.winddir));
}

std::string get_runway() {
    LOCK;
    current_airport.active_rwy = get_active_rwy(dataframe.winddir, current_airport.runways);
    return current_airport.active_rwy;
}

std::string get_holdpoint() {
    LOCK;
    return current_airport.holdpoints[get_active_rwy(dataframe.winddir, current_airport.runways)];
}

std::string get_tailnum() {
    return aircraft.tailnum;
}

void init_value_keys() {
/*    value_keys["VISIBILITY"]  = key_visibility;
    value_keys["QNH"]         = key_qnh;
    value_keys["WINDSPEED"]   = key_windspeed;
    value_keys["WINDDIR"]     = key_winddir;
    value_keys["RUNWAY"]      = key_runway;
    value_keys["HOLDPOINT"]   = key_holdpoint;*/
    
    callbacks_audio["VISIBILITY"]   = get_visibility_AUDIO;
    callbacks_audio["QNH"]          = get_qnh;
    callbacks_audio["WINDSPEED"]    = get_windspeed;
    callbacks_audio["WINDDIR"]      = get_winddir;
    callbacks_audio["RUNWAY"]       = get_runway;
    callbacks_audio["HOLDPOINT"]    = get_holdpoint;
    
    callbacks_text = callbacks_audio;
    callbacks_text["VISIBILITY"]    = get_visibility_TEXT;
    callbacks_text["TAILNUM"]       = get_tailnum;
}

std::string getValue(std::map<std::string, std::function<std::string()>>* callbacks, const std::string &str) {
    if (!noxplane) {
        if (callbacks_audio.empty())
            init_value_keys();

        update_current_airport();
            
        return (*callbacks)[str]();
    }
    
    return "any";
}

std::string replace_keys(const std::string& input) {
    std::string result = input;
    std::regex pattern(R"(\{([A-Z]+)\})"); // Cherche des mots majuscules entre {}
    
    if (callbacks_text.empty())
        init_value_keys();

    auto it = std::sregex_iterator(result.begin(), result.end(), pattern);
    auto end = std::sregex_iterator();

    while (it != end) {
        std::smatch match = *it;
        std::string key = match.str(1); // Récupérer la clé sans les {}
        if (callbacks_text.find(key) != callbacks_text.end()) {
            std::string value = std::string(RESET) + std::string(WHITE_BOLD) + getValue(TEXT, key) + std::string(RESET) + std::string(GREEN_BOLD); // Appeler la fonction associée
            result.replace(match.position(), match.length(), value);
            // Recalculer l'itérateur car la chaîne a changé
            it = std::sregex_iterator(result.begin(), result.end(), pattern);
        } else {
            ++it; // Passer à la prochaine correspondance si clé inconnue
        }
    }

    return result;
}

// Fonction pour supprimer toutes les occurrences de 'substring' dans 'str'
std::string removeSubstring(const std::string& str, const std::string& substring) {
    std::string result = str; // Créer une copie de la chaîne d'entrée
    size_t pos = 0;

    // Rechercher et supprimer toutes les occurrences de la sous-chaîne
    while ((pos = result.find(substring, pos)) != std::string::npos) {
        result.erase(pos, substring.length());
    }

    return result; // Retourner la chaîne modifiée
}

std::vector<std::string> readPhrase(const std::string &str) {
    std::vector<std::string> subphrases = splitString(phrases[str], '|');
    std::string phrase;
    std::vector<std::string> words;
    std::vector<std::string> waves;
    
    if (subphrases.size() > 1) {
        int retained_index = generateRandomNumber(subphrases.size() - 1);
        phrase = subphrases[retained_index];
    }
    else
        phrase = subphrases[0];
    
    words = splitString(phrase, '&');
    
    for(unsigned int i = 0; i < words.size(); ++i) {
        if (words[i][0] == '[' && words[i][words[0].size() - 1] == ']') {
            std::vector<std::string> tmp_waves = readPhrase(words[i]);
            waves.insert(waves.end(), tmp_waves.begin(), tmp_waves.end());
        }
        else
        {
            std::string value = extractBetweenBraces(words[i]);
            
            if (!value.empty()) {
                std::vector<std::string> filename = splitString(words[i], '.');
                words[i] = removeSubstring(filename[0], "{" + value + "}") + getValue(AUDIO, value) + "." + filename[1];
            }
            
            waves.push_back(words[i]);
            //std::cout << words[i] << std::endl;
        }
    }
    
    return waves;
}