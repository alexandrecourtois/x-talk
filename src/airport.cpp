#include "airport.h"
#include <cmath>
#include "enums.h"
#include <callbacks.h>
#include "reqrsp.h"
#include "session.h"
#include <zmq_client.h>
#include <tools.h>
#include <xprint.h>

void AIRPORTS::loadAiports(const std::string& filename) {
    OUT::xprint(OUT::MSG_STYLE::INIT, "Loading JSON airports", filename);
    
    __airports = TOOLBOX::loadJSON(filename);
    
    OUT::xprint(OUT::MSG_STYLE::DONE, std::to_string(__airports.size()) + "airports found");
}

bool AIRPORTS::isSupportedAirport() {
    return !(__current_airport.ICAO == UNSUPPORTED_AIRPORT);
}

void AIRPORTS::updateCurrentAirport() {
      SESSION::dataframe.lock();
            int active_freq;
            
            // si com1 et com2 sont actives alors l'aérodrome retenu sera toujours celui
            // correspondant à la fréquence sélectionné sur com1 (pas de dataref trouvé pour Com1/Com2 Mic)
            if (SESSION::dataframe.com1_active || (SESSION::dataframe.com1_active && SESSION::dataframe.com2_active))
                active_freq = SESSION::dataframe.com1_freq;
            else
                active_freq = SESSION::dataframe.com2_freq;
                
            __current_airport.ICAO = getAirportByFrequency(active_freq);
        SESSION::dataframe.unlock();

    
    if (__current_airport.ICAO != UNSUPPORTED_AIRPORT) {
        __current_airport.lat     = __airports[__current_airport.ICAO]["latitude"].get<float>();
        __current_airport.lon     = __airports[__current_airport.ICAO]["longitude"].get<float>();
        __current_airport.alt     = __airports[__current_airport.ICAO]["altitude"].get<float>();
        __current_airport.tower   = __airports[__current_airport.ICAO]["tower"].get<int>();
        __current_airport.atis    = __airports[__current_airport.ICAO]["atis"].get<int>();
                    
        std::vector<std::string> r = TOOLBOX::splitString(__airports[__current_airport.ICAO]["runways"].get<std::string>(), ',');
        std::vector<std::string> h = TOOLBOX::splitString(__airports[__current_airport.ICAO]["holdpoints"].get<std::string>(), ',');
        int i = 0;
                    
        for(std::string &str: r) {
            std::string key = str;
                        
            if (str[str.size() - 1] > '9')
                str.pop_back();
                            
            __current_airport.runways[key] = std::stoi(str);
            __current_airport.holdpoints[key] = h[i++];    
        }

        Req airport_update_rq;
        airport_update_rq.type = Request::UPD_AIRPORT;
        airport_update_rq.argD_1 = __current_airport.lat;
        airport_update_rq.argD_2 = __current_airport.lon;
        airport_update_rq.argD_3 = __current_airport.alt;
        SESSION::client->sendRequest(airport_update_rq);
    }
}

std::string AIRPORTS::getAirportByFrequency(float frequency) {
    for (auto& [icao, data] : __airports.items()) {
        if (data.contains("tower")) {
            float tower_frequency = data["tower"].get<int>();
            if (tower_frequency == frequency)                
                return icao;
        }
    }
    
    return UNSUPPORTED_AIRPORT;  // Aucun aéroport correspondant trouvé
}

// Fonction pour calculer la piste en service
std::string AIRPORTS::getActiveRunway(int windDirection) {
    std::string bestRunway = "";
    int minDiff = 360;  // Différence initiale maximale (360 degrés)

    // Trouver la piste la plus proche de l'opposée du vent
    for (const auto& runway : __current_airport.runways) {
        // Calculer la direction idéale de la piste (opposée au vent)
        int oppositeWind = (windDirection + 180) % 360;

        // Calculer la différence entre la direction de la piste et l'opposée du vent
        int diff = std::abs(runway.second - oppositeWind);

        // Ajuster la différence pour tenir compte de la circularité (360°)
        if (diff > 180) {
            diff = 360 - diff;
        }

        // Si la différence est la plus petite, choisir cette piste
        if (diff < minDiff) {
            minDiff = diff;
            bestRunway = runway.first;  // Récupérer le nom de la piste
        }
    }

    return bestRunway;
}

AIRPORTS::Airport& AIRPORTS::getCurrentAirport() {
    return __current_airport;
}

nlohmann::json AIRPORTS::__airports;
AIRPORTS::Airport AIRPORTS::__current_airport;