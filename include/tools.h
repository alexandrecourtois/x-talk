#ifndef TOOLS_H
#define TOOLS_H

#include <optional>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class TOOLBOX {
    private:
        TOOLBOX() = default;

    public:
        static std::string removeQuotes(std::string str);
        static std::string removeAnsiSq(const std::string& str);
        static std::string toString(double value, int precision);
        static std::optional<int> toInt(const std::string& str);
        static int selectById(int count_id);
        static int countLines(const std::string& str);
        static int generateRandomNumber(int max);
        static double round(double value, int decimal);
        static bool isValidIP(const std::string& str);
        static void displayProgressBar(const std::string& msg, int current, int total);
        static std::vector<std::string> splitString(const std::string& str, char delimiter);
        static std::string extractBetweenBraces(const std::string& str);
        static std::string removeSubstring(const std::string& str, const std::string& substring);
        static double haversine(double lat1, double lon1, double lat2, double lon2);
        static double toRad(double degree);
        static nlohmann::json loadJSON(const std::string& filename);
};

#endif // TOOLS_H