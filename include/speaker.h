#ifndef SPEAKER_H
#define SPEAKER_H

#include <nlohmann/json_fwd.hpp>
#include <vector>
#include <vosk_api.h>
#include <nlohmann/json.hpp>
#include <set>

class Speaker {
    private:
        std::string                         __call_id;
        VoskModel*                          __vmodel;
        VoskRecognizer*                     __vrec;
        nlohmann::json                      __dialog;
        nlohmann::json*                     __current_node;

        nlohmann::json  __load_json(const std::string& filename);
        void            __init_vosk(const char* path);
        void            __init_resp(const std::string& dialogPath);

        nlohmann::json*         __get_response(const std::set<std::string>& userWords);
        std::set<std::string>   __split_user_input(const std::string& userInput);
        bool                    __evaluate_expression(const std::string& expression, const std::set<std::string>& userWords);
        bool                    __matches_keyword(const std::string& keywords, const std::set<std::string>& userWords);
        std::string             __replace_keys(const std::string& input);
        
    public:
        Speaker() = default;
        Speaker(const std::string& callID, const std::string& modelPath, const std::string& dialogPath);
        ~Speaker();

        void tell(std::string& input);
};

#endif  // SPEAKER_H