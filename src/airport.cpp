#include "airport.h"
#include <cmath>
#include "eval.h"
#include "globals.h"
#include "dataframe.h"

bool supported_airport() {
    return !(current_airport.ICAO == UNSUPPORTED_AIRPORT);
}

void update_current_airport() {
    {
        LOCK;
        int active_freq;
        
        // si com1 et com2 sont actives alors l'aérodrome retenu sera toujours celui
        // correspondant à la fréquence sélectionné sur com1 (pas de dataref trouvé pour Com1/Com2 Mic)
        if (dataframe.com1_active || (dataframe.com1_active && dataframe.com2_active))
            active_freq = dataframe.com1_freq;
        else
            active_freq = dataframe.com2_freq;
            
        current_airport.ICAO = get_airport_by_frequency(active_freq);
    }
    
    if (current_airport.ICAO != UNSUPPORTED_AIRPORT) {
        current_airport.lat     = airports[current_airport.ICAO]["latitude"].get<float>();
        current_airport.lon     = airports[current_airport.ICAO]["longitude"].get<float>();
        current_airport.alt     = airports[current_airport.ICAO]["altitude"].get<float>();
        current_airport.tower   = airports[current_airport.ICAO]["tower"].get<int>();
        current_airport.atis    = airports[current_airport.ICAO]["atis"].get<int>();
                    
        std::vector<std::string> r = splitString(airports[current_airport.ICAO]["runways"].get<std::string>(), ',');
        std::vector<std::string> h = splitString(airports[current_airport.ICAO]["holdpoints"].get<std::string>(), ',');
        int i = 0;
                    
        for(std::string &str: r) {
            std::string key = str;
                        
            if (str[str.size() - 1] > '9')
                str.pop_back();
                            
            current_airport.runways[key] = std::stoi(str);
            current_airport.holdpoints[key] = h[i++];    
        }
    }
}

std::string get_airport_by_frequency(float frequency) {
    for (auto& [icao, data] : airports.items()) {
        if (data.contains("tower")) {
            float tower_frequency = data["tower"].get<int>();
            if (tower_frequency == frequency)                
                return icao;
        }
    }
    
    return UNSUPPORTED_AIRPORT;  // Aucun aéroport correspondant trouvé
}

// Fonction pour calculer la piste en service
std::string get_active_rwy(int windDirection, const std::map<std::string, int>& runways) {
    std::string bestRunway = "";
    int minDiff = 360;  // Différence initiale maximale (360 degrés)

    // Trouver la piste la plus proche de l'opposée du vent
    for (const auto& runway : runways) {
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
