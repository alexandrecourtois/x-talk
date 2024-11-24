#ifndef EVAL_H
#define EVAL_H

#include <string>
#include <set>
#include <nlohmann/json.hpp>
#include <SDL2/SDL_mixer.h>

// Fonction pour charger les réponses depuis un fichier JSON
nlohmann::json load_json(const std::string& filename);

// Fonction pour évaluer une expression avec des opérateurs | et &
bool evaluateExpression(const std::string& expression, const std::set<std::string>& userWords);

// Fonction pour vérifier si au moins un mot de userInput correspond aux mots-clés
bool matchesKeyword(const std::string& keywords, const std::set<std::string>& userWords);

// Fonction pour obtenir la réponse en fonction de l'entrée utilisateur et du nœud actuel
nlohmann::json* getResponse(nlohmann::json* currentNode, const std::set<std::string>& userWords, nlohmann::json& responses);

// Fonction pour séparer les mots de l'entrée utilisateur
std::set<std::string> splitUserInput(const std::string& userInput);

std::vector<std::string> splitString(const std::string& str, char separator);

std::string extractBetweenBraces(const std::string& str);

std::vector<std::string> readPhrase(const std::string &str);

std::string replace_keys(const std::string& input);

#endif