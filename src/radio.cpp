#include <radio.h>
#include <cmath>
#include <SDL2/SDL_mixer.h>
#include <session.h>
#include <tools.h>
#include <airport.h>
#include <xprint.h>

int RADIO::__old_com1_freq;
int RADIO::__old_com2_freq;
int RADIO::__old_com1_status;
int RADIO::__old_com2_status;

double RADIO::__white_rabbit_obj(double factor) {
    return std::log(2.0f * factor) / std::exp(1.4f) + 0.84f;
}

double RADIO::getRange() {
    //double distance_km;
    double altitude_m;
    float visibility_m;
    double range_km;

    SESSION::dataframe.lock();
        //distance_km = haversine(SESSION::dataframe.latitude, SESSION::dataframe.longitude, current_airport.lat, current_airport.lon);
        altitude_m = SESSION::dataframe.altitude * 0.3048;
        visibility_m = SESSION::dataframe.visibility;
    SESSION::dataframe.unlock();

    //double visibility_factor = std::log(std::log10(0.09f + visibility_m / BASE_VISIBILITY) + 2.68);
    //double visibility_factor = std::log(2.0f * visibility_m / BASE_VISIBILITY) / std::exp(2.0f) + 0.9f;

    range_km = 3.57 * sqrt(altitude_m) * __white_rabbit_obj(visibility_m / BASE_VISIBILITY);

    if (range_km < VHF_MAX_RANGE) {
        return range_km;
    }

    return VHF_MAX_RANGE;
}

int RADIO::getNoiseVolume() {
    double range_km = getRange();
    double distance_km = getDistanceToAirport();

    double delta = range_km - distance_km;

    //double volume = MIX_MAX_VOLUME - delta * MIX_MAX_VOLUME / range_km;
    double volume = MIX_MAX_VOLUME * __white_rabbit_obj(1 - delta / range_km);//cos(std::numbers::pi * delta / (2.0f * range_km));
    
    if (volume > MIX_MAX_VOLUME)
        return MIX_MAX_VOLUME;

    return std::floor(volume);
}

int RADIO::getSpeakVolume(int noiseVolume) {
    return MIX_MAX_VOLUME - noiseVolume;
}

bool RADIO::isWithinRange() {
    return getDistanceToAirport() < getRange();
}

double RADIO::getDistanceToAirport() {
    double result;
    SESSION::dataframe.lock();
    result = TOOLBOX::haversine(SESSION::dataframe.latitude, SESSION::dataframe.longitude, AIRPORTS::getCurrentAirport().lat, AIRPORTS::getCurrentAirport().lon);
    SESSION::dataframe.unlock();

    return result;
}

void RADIO::updateStatus() {
    SESSION::dataframe.lock();

        if ((__old_com1_freq != SESSION::dataframe.com1_freq) && SESSION::dataframe.com1_active) {
            std::string airport = AIRPORTS::getAirportByFrequency(SESSION::dataframe.com1_freq);
            
            if (airport != UNSUPPORTED_AIRPORT) {
                OUT::xprint(OUT::MSG_STYLE::CLEAN);
                OUT::xprint(OUT::MSG_STYLE::INFO, "Airport frequency selected on COM1: " + airport);
                OUT::xprint(OUT::MSG_STYLE::INVITE);
            }

            __old_com1_freq = SESSION::dataframe.com1_freq;
        }
        
        if ((__old_com2_freq != SESSION::dataframe.com2_freq) && SESSION::dataframe.com2_active) {
            std::string airport = AIRPORTS::getAirportByFrequency(SESSION::dataframe.com2_freq);
            
            if (airport != UNSUPPORTED_AIRPORT) {
                OUT::xprint(OUT::MSG_STYLE::CLEAN);
                OUT::xprint(OUT::MSG_STYLE::INFO, "Airport frequency selected on COM2: " + airport);
                OUT::xprint(OUT::MSG_STYLE::INVITE);
            }

            __old_com2_freq = SESSION::dataframe.com2_freq;
        }

        if (__old_com1_status != SESSION::dataframe.com1_active && SESSION::dataframe.com1_active && !SESSION::dataframe.com2_active) {
            AIRPORTS::updateCurrentAirport();
        }
        
        if (__old_com2_status != SESSION::dataframe.com2_active && SESSION::dataframe.com2_active && !SESSION::dataframe.com1_active) {
            AIRPORTS::updateCurrentAirport();
        }
        
        __old_com1_status = SESSION::dataframe.com1_active;
        __old_com2_status = SESSION::dataframe.com2_active;

    SESSION::dataframe.unlock();
}