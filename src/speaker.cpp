#include <msg.h>
#include <speaker.h>
#include <xprint.h>
#include <callbacks.h>
#include <session.h>
#include <audio.h>
#include <inputs.h>
#include <airport.h>
#include <radio.h>
#include <tools.h>
#include <lang.h>

void Speaker::__init_vosk(const char* path) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::INITIALIZING_VOSK), path);

    if (SESSION::vosk_Models.contains(this->__vmodel_name)) {
        this->__vmodel = SESSION::vosk_Models[this->__vmodel_name].first;
        this->__vrec = SESSION::vosk_Recognizers[this->__vmodel_name];
        SESSION::vosk_Models[this->__vmodel_name].second++;
    } else {
        vosk_set_log_level(-1);
        SESSION::vosk_Models[this->__vmodel_name].first = vosk_model_new(path);
        SESSION::vosk_Recognizers[this->__vmodel_name] = vosk_recognizer_new(SESSION::vosk_Models[__vmodel_name].first, 16000.0f);
        SESSION::vosk_Models[this->__vmodel_name].second = 1;
        this->__vmodel = SESSION::vosk_Models[this->__vmodel_name].first;
        this->__vrec = SESSION::vosk_Recognizers[this->__vmodel_name];
    }

    X_OUTPUT::xprint(MSG_STYLE::DONE);
}

void Speaker::__init_resp(const std::string& dialogPath) {
    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LOADING_DIALOG), dialogPath);
    
    this->__dialog = TOOLBOX::loadJSON(dialogPath);
    this->__current_node = &this->__dialog[">ROOT"];

    this->__load_labels(this->__current_node);

    X_OUTPUT::xprint(MSG_STYLE::DONE);
}

void Speaker::__load_labels(nlohmann::json* startNode) {
    for(auto& [key, node]: startNode->items()) {
        if (key[0] ==  '>')
            this->__labels[key] = &node;

        if (node.is_object())
            this->__load_labels(&node);
    }
}

nlohmann::json* Speaker::__get_response(const std::set<std::string>& userWords) {
    if (this->__current_node->contains("responses")) {
        for (auto& [key, responseNode] : (*this->__current_node)["responses"].items()) {
            if (responseNode.contains("keywords") && this->__matches_keyword(responseNode["keywords"], userWords)) {
                    if (responseNode.contains("goto"))
                        return this->__goto(responseNode["goto"].get<std::string>());

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
            std::erase(orKeyword, '(');
            std::erase(orKeyword, ')');

            if (userWords.contains(orKeyword)) {
                orMatch = true;
                break;
            } else {
                for(auto const& s: userWords) {
                    int ld = TOOLBOX::getLevenshteinDistance_alt(s, orKeyword);
                    
                    if (ld >= 0 && ld <= SESSION::levenshtein_threshold) {
                        orMatch = true;
                        break;
                    }
                }
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

Speaker::Speaker(const std::string& callID, const std::string& modelPath, const std::string& dialogPath): __call_id(callID), __vmodel_name(modelPath) {
    if (!SESSION::no_audio) {
        this->__init_vosk(modelPath.c_str());
    }
    
    this->__init_resp(dialogPath);
}

Speaker::~Speaker() {
    if (!SESSION::no_audio) {
        if (SESSION::vosk_Models[this->__vmodel_name].second == 1) {
            vosk_recognizer_free(this->__vrec);
            vosk_model_free(this->__vmodel);
            SESSION::vosk_Models.erase(this->__vmodel_name);
            SESSION::vosk_Recognizers.erase(this->__vmodel_name);
        } else {
            SESSION::vosk_Models[this->__vmodel_name].second--;
        }
    }
}

Speaker& Speaker::operator=(const Speaker& orig) {
    this->__call_id = orig.__call_id;
    this->__vmodel_name = orig.__vmodel_name;
    this->__vmodel = orig.__vmodel;
    this->__vrec = orig.__vrec;
    this->__dialog = orig.__dialog;
    this->__current_node = &this->__dialog[">ROOT"];
    SESSION::vosk_Models[this->__vmodel_name].second++;
    this->__labels[">ROOT"] = this->__current_node;
    this->__load_labels(this->__current_node);

    return *this;
}

nlohmann::json* Speaker::__goto(const std::string& label) {
    if (this->__labels.contains(label)) {
        return this->__labels[label];
    }

    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, label + ": unknown label");
    return nullptr;
}

void Speaker::__speak(nlohmann::json* node) {
    X_OUTPUT::xprint(MSG_STYLE::BLINK_BEGIN, "Speaking");
    
    std::string value = TOOLBOX::removeQuotes((*node)["audio"]);
    std::vector<std::string> keys = TOOLBOX::splitString(value, ',');
    
    AUDIO::play(AUDIO::Type::RADIOCOM, keys);

    X_OUTPUT::xprint(MSG_STYLE::ENDL);
    X_OUTPUT::xprint(MSG_STYLE::BLINK_END);    
}

void Speaker::tell(std::string& input) {
    if (input.empty() && !SESSION::no_audio) {
        AUDIO::play(AUDIO::Type::RADIOSTART);
        AUDIO::startRecording();
        
        if (!X_INPUT::ptt_pushed()) {
            std::cout << BACK_LINE << "> " << std::flush;
        }
        else
            X_INPUT::disableInput();
            
        X_OUTPUT::xprint(MSG_STYLE::BLINK_BEGIN, lang(T_MSG::RECORDING));
        
        if (X_INPUT::ptt_pushed()) {
            while(X_INPUT::ptt_pushed());
            X_INPUT::enableInput();
            std::cout << std::endl;
        }
        else
            X_INPUT::xscan();
        
        X_OUTPUT::xprint(MSG_STYLE::BLINK_END);
        X_OUTPUT::xprint(MSG_STYLE::INVITE);
        
        AUDIO::stopRecording();
        AUDIO::play(AUDIO::Type::RADIOSTOP);

        std::cout << lang(T_MSG::ANALYZING) << "... " << std::flush;
        
        vosk_recognizer_accept_waveform(this->__vrec, reinterpret_cast<const char*>(AUDIO::getAudioStream().data()), AUDIO::getAudioStream().size());
        
        const char* vres = vosk_recognizer_result(this->__vrec);
        std::string userInput(nlohmann::json::parse(vres)["text"]);
        
        std::cout << CLEAN_LINE << "> " << std::endl;
        input = userInput;
    }

    if (!input.empty()) {
        std::cout << BACK_LINE << CLEAN_LINE;

        if (AIRPORTS::isSupportedAirport())
            X_OUTPUT::xprint(MSG_STYLE::USER, input);
        else
            X_OUTPUT::xprint(MSG_STYLE::USER_ALT, input);

        if (AIRPORTS::isSupportedAirport() && RADIO::isWithinRange()) {
            if (this->__readback_keywords.empty()) {
                std::set<std::string> userWords = this->__split_user_input(input);
                nlohmann::json* nextNode = this->__get_response(userWords);

                if (nextNode) {
                    this->__current_node = nextNode;
                    
                    if (this->__current_node->contains("phrase")) {
                        X_OUTPUT::xprint(MSG_STYLE::XTALK, this->__replace_keys(TOOLBOX::removeQuotes((*this->__current_node)["phrase"])));
                    }
                    
                    if (this->__current_node->contains("audio") && !SESSION::no_audio) {
                        this->__speak(this->__current_node);
                    }

                    if (this->__current_node->contains("goto")) {
                        this->__current_node = this->__goto((*this->__current_node)["goto"]);
                        //std::cout << (*this->__current_node).dump(4) << std::endl;
                    }

                    if (this->__current_node->contains("readback")) {
                        this->__readback_keywords = TOOLBOX::removeQuotes((*this->__current_node)["readback"]);
                    }
                }
                else {
                    X_OUTPUT::xprint(MSG_STYLE::XTALK_ALT, this->__dialog["default"]["phrase"]);

                    if (!SESSION::no_audio)
                        this->__speak(&this->__dialog["default"]);
                }
            } else if (this->__matches_keyword(this->__readback_keywords, this->__split_user_input(input))) {
                this->__readback_keywords.clear();
            }
        }
    }
    else {
        std::cout << BACK_LINE << std::flush;
    }
}

std::string Speaker::getKeywords() {
    if (this->isWaitingForReadback())
        return this->__readback_keywords;
    else
        if (this->__current_node->contains("responses")) {
            std::string keywords;

            for (auto& [key, responseNode] : (*this->__current_node)["responses"].items()) {
                if (responseNode.contains("keywords")) {
                    keywords.insert(0, std::string(responseNode["keywords"]) + ",");
                }
            }

            keywords.pop_back();
            return keywords;
        }

    return std::string();
}

bool Speaker::isWaitingForReadback() {
    return this->__readback_keywords.size();
}