#pragma once

#include "vosk_api.h"
#include <nlohmann/json_fwd.hpp>
#include <pch.h>

class Speaker {
    private:
        std::string                             __call_id;
        std::string                             __vmodel_name;   
        VoskModel*                              __vmodel;
        VoskRecognizer*                         __vrec;     
        nlohmann::json                          __dialog;
        nlohmann::json*                         __current_node;
        std::string                             __readback_keywords;
        std::map<std::string, nlohmann::json*>  __labels;

        void            __init_vosk(const char* path);
        void            __init_resp(const std::string& dialogPath);

        nlohmann::json*         __get_response(const std::set<std::string>& userWords);
        std::set<std::string>   __split_user_input(const std::string& userInput);
        bool                    __evaluate_expression(const std::string& expression, const std::set<std::string>& userWords);
        bool                    __matches_keyword(const std::string& keywords, const std::set<std::string>& userWords);
        std::string             __replace_keys(const std::string& input);
        void                    __load_labels(nlohmann::json* startNode);
        nlohmann::json*         __goto(const std::string& label);
        void                    __speak(nlohmann::json* node);
        
    public:
        Speaker() = default;
        Speaker(const std::string& callID, const std::string& modelPath, const std::string& dialogPath);
        ~Speaker();

        Speaker& operator=(const Speaker& orig);

        void tell(std::string& input);
        std::string getKeywords();
        bool isWaitingForReadback();
};