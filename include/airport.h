#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>
#include <map>
#include <nlohmann/json.hpp>

#define UNSUPPORTED_AIRPORT     std::string("UNSUPPORTED_AIRPORT")

std::string get_airport_by_frequency(float frequency);
std::string get_active_rwy(int windDirection, const std::map<std::string, int>& runways);
void update_current_airport();
bool supported_airport();

class AIRPORTS {
    public:
        struct Airport {
            std::string ICAO;
            float lat;
            float lon;
            float alt;
            int tower;
            int atis;
            std::map<std::string, int> runways;
            std::map<std::string, std::string> holdpoints;
            std::string active_rwy;
        };

    private:
        static nlohmann::json __airports;
        static Airport __current_airport;

        AIRPORTS() = default;
    
    public:
        static void loadAiports(const std::string& filename);
        static std::string getAirportByFrequency(float frequency);
        static std::string getActiveRunway(int windDirection);
        static void updateCurrentAirport();
        static bool isSupportedAirport();
        static Airport& getCurrentAirport();
};

#endif  // AIRPORT_H