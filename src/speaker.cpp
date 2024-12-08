#include "vosk_api.h"
#include <speaker.h>
#include <vector>
#include <xprint.h>
#include <callbacks.h>
#include <airport.h>
#include <audio.h>
#include <tools.h>
#include <session.h>
#include <inputs.h>
#include <regex>
#include <fstream>
#include <radio.h>

nlohmann::json Speaker::__load_json(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        //std::cerr << "Erreur: Impossible de lire le fichier " << filename << std::endl;
        exit(1);
    }
    nlohmann::json data;
    file >> data;
    return data;
}

void Speaker::__init_vosk(const char* path) {
    OUT::xprint(OUT::MSG_STYLE::INIT, "Initializing Vosk", path);

    vosk_set_log_level(-1);
    this->__vmodel  = vosk_model_new(path);
    this->__vrec    = vosk_recognizer_new(this->__vmodel, 16000.f);

    OUT::xprint(OUT::MSG_STYLE::DONE);
}

void Speaker::__init_resp(const std::string& dialogPath) {
    OUT::xprint(OUT::MSG_STYLE::INIT,"Loading JSON dialog tree", dialogPath);
    
    this->__dialog = this->__load_json(dialogPath);
    this->__current_node = &this->__dialog["root"];
    
    OUT::xprint(OUT::MSG_STYLE::DONE);
}

nlohmann::json* Speaker::__get_response(const std::set<std::string>& userWords) {
    if (this->__current_node->contains("responses")) {
        for (auto& [key, responseNode] : (*this->__current_node)["responses"].items()) {
            if (responseNode.contains("keywords") && this->__matches_keyword(responseNode["keywords"], userWords)) {
                if (responseNode.contains("goto")) {
                    return &this->__dialog["root"]["responses"][responseNode["goto"].get<std::string>()];
                }
                return &responseNode;
            }
        }
    }

    return nullptr;
}

bool Speaker::__evaluate_expression(const std::string& expression, const std::set<std::string>& userWords) {
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
bool Speaker::__matches_keyword(const std::string& keywords, const std::set<std::string>& userWords) {
    return this->__evaluate_expression(keywords, userWords);
}

std::string Speaker::__replace_keys(const std::string& input) {
    std::string result = input;
    std::regex pattern(R"(\{([A-Z]+)\})"); // Cherche des mots majuscules entre {}
    
    auto it = std::sregex_iterator(result.begin(), result.end(), pattern);
    auto end = std::sregex_iterator();

    while (it != end) {
        std::smatch match = *it;
        std::string key = match.str(1); // Récupérer la clé sans les {}
        if (CALLBACKS::hasCallback(CALLBACKS::TEXT, key)) {
            std::string value = std::string(RESET) + std::string(WHITE_BOLD) + CALLBACKS::getValue(CALLBACKS::TEXT, key) + std::string(RESET) + std::string(GREEN_BOLD); // Appeler la fonction associée
            result.replace(match.position(), match.length(), value);
            // Recalculer l'itérateur car la chaîne a changé
            it = std::sregex_iterator(result.begin(), result.end(), pattern);
        } else {
            ++it; // Passer à la prochaine correspondance si clé inconnue
        }
    }

    return result;
}

std::set<std::string> Speaker::__split_user_input(const std::string& userInput) {
    std::set<std::string> words;
    std::istringstream stream(userInput);
    std::string word;

    while (stream >> word) {
        words.insert(word);
    }

    return words;
}

Speaker::Speaker(const std::string& callID, const std::string& modelPath, const std::string& dialogPath): __call_id(callID) {
    if (!SESSION::no_audio)
        this->__init_vosk(modelPath.c_str());
    
    this->__init_resp(dialogPath);
}

Speaker::~Speaker() {
    if (!SESSION::no_audio) {
        vosk_recognizer_free(this->__vrec);
        vosk_model_free(this->__vmodel);
    }
}

void Speaker::tell(std::string& input) {
    if (input.empty() && !SESSION::no_audio) {
        AUDIO::play(AUDIO::Type::RADIOSTART);
        AUDIO::startRecording();
        
        if (!IN::ptt_pushed()) {
            std::cout << BACK_LINE << "> " << std::flush;
        }
        else
            IN::disableInput();
            
        OUT::xprint(OUT::MSG_STYLE::BLINK_BEGIN, "Recording... Press Enter to stop.");
        
        if (IN::ptt_pushed()) {
            while(IN::ptt_pushed());
            IN::enableInput();
            std::cout << std::endl;
        }
        else
            IN::xscan();
        
        OUT::xprint(OUT::MSG_STYLE::BLINK_END);
        OUT::xprint(OUT::MSG_STYLE::INVITE);
        
        AUDIO::stopRecording();
        AUDIO::play(AUDIO::Type::RADIOSTOP);

        std::cout << "Analyzing... " << std::flush;
        
        vosk_recognizer_accept_waveform(this->__vrec, reinterpret_cast<const char*>(AUDIO::getAudioStream().data()), AUDIO::getAudioStream().size());
        
        const char* vres = vosk_recognizer_result(this->__vrec);
        std::string userInput(nlohmann::json::parse(vres)["text"]);
        
        std::cout << CLEAN_LINE << "> " << std::endl;
        input = userInput;
    }

    if (!input.empty()) {
        std::cout << BACK_LINE << CLEAN_LINE;

        if (AIRPORTS::isSupportedAirport())
            OUT::xprint(OUT::MSG_STYLE::USER, input);
        else
            OUT::xprint(OUT::MSG_STYLE::USER_ALT, input);

        if (AIRPORTS::isSupportedAirport() && RADIO::isWithinRange()) {
            std::set<std::string> userWords = this->__split_user_input(input);
            nlohmann::json* nextNode = this->__get_response(userWords);

            if (nextNode) {
                this->__current_node = nextNode;
                
                if (this->__current_node->contains("phrase")) {
                    OUT::xprint(OUT::MSG_STYLE::XTALK, this->__replace_keys(TOOLBOX::removeQuotes((*this->__current_node)["phrase"])));
                }
                
                if (this->__current_node->contains("audio") && !SESSION::no_audio) {
                    OUT::xprint(OUT::MSG_STYLE::BLINK_BEGIN, "Speaking");
                    
                    std::string value = TOOLBOX::removeQuotes((*this->__current_node)["audio"]);
                    std::vector<std::string> keys = TOOLBOX::splitString(value, ',');
                    
                    AUDIO::play(AUDIO::Type::RADIOCOM, keys);

                    OUT::xprint(OUT::MSG_STYLE::ENDL);
                    OUT::xprint(OUT::MSG_STYLE::BLINK_END);
                }
            }
            else {
                OUT::xprint(OUT::MSG_STYLE::XTALK_ALT, this->__dialog["default"]["phrase"]);
            }
        }
    }
    else {
        std::cout << BACK_LINE << std::flush;
    }
}