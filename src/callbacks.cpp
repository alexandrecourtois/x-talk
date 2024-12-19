#include <callbacks.h>
#include <session.h>
#include <airport.h>

std::string CALLBACKS::__get_visibility_AUDIO() {
    int v;
    
    SESSION::dataframe.lock();
        v = static_cast<int>(SESSION::dataframe.infreq_visibility);
    SESSION::dataframe.unlock();
                
    if (v < 1500)
        return std::to_string(1500);
                    
    if (v < 5000)
        return std::to_string(5000);
                    
    return std::to_string(5001);
}

std::string CALLBACKS::__get_visibility_TEXT() {
    int v;
    
    SESSION::dataframe.lock();
        v = static_cast<int>(SESSION::dataframe.infreq_visibility);
    SESSION::dataframe.unlock();
                
    if (v < 1500)
        return std::string("< 1500m");
                    
    if (v < 5000)
        return std::string("< 5000m");
                    
    return std::string("> 5000m");
}

std::string CALLBACKS::__get_qnh() {
    std::string str;
    SESSION::dataframe.lock();
        str = std::to_string(static_cast<int>(std::round(SESSION::dataframe.qnh / 100.0f)));
    SESSION::dataframe.unlock();

    return str;
}

std::string CALLBACKS::__get_windspeed() {
    std::string str;
    SESSION::dataframe.lock();
        str = std::to_string(static_cast<int>(SESSION::dataframe.infreq_windspeed));
    SESSION::dataframe.unlock();

    return str;
}

std::string CALLBACKS::__get_winddir() {
    std::string str;
    SESSION::dataframe.lock();
        str = std::to_string(static_cast<int>(SESSION::dataframe.infreq_winddir));
    SESSION::dataframe.unlock();

    return str;
}

std::string CALLBACKS::__get_runway() {
    std::string str;
    SESSION::dataframe.lock();
        AIRPORTS::getCurrentAirport().active_rwy = AIRPORTS::getActiveRunway(SESSION::dataframe.infreq_winddir);
    SESSION::dataframe.unlock();

    return AIRPORTS::getCurrentAirport().active_rwy;
}

std::string CALLBACKS::__get_holdpoint() {
    std::string str;
    SESSION::dataframe.lock();
        str = AIRPORTS::getCurrentAirport().holdpoints[AIRPORTS::getActiveRunway(SESSION::dataframe.infreq_winddir)];
    SESSION::dataframe.unlock();

    return str;
}

std::string CALLBACKS::__get_tailnum() {
    return SESSION::aircraft.tailnum;
}

void CALLBACKS::__init() {
    __callbacks_audio["VISIBILITY"]   = __get_visibility_AUDIO;
    __callbacks_audio["QNH"]          = __get_qnh;
    __callbacks_audio["WINDSPEED"]    = __get_windspeed;
    __callbacks_audio["WINDDIR"]      = __get_winddir;
    __callbacks_audio["RUNWAY"]       = __get_runway;
    __callbacks_audio["HOLDPOINT"]    = __get_holdpoint;
    
    __callbacks_text = __callbacks_audio;
    __callbacks_text["VISIBILITY"]    = __get_visibility_TEXT;
    __callbacks_text["TAILNUM"]       = __get_tailnum;
}

std::string CALLBACKS::getValue(int type, const std::string &str) {
    std::map<std::string, std::function<std::string()>>* callbacks;

    switch (type) {
        case AUDIO:
            callbacks = &__callbacks_audio;
            break;

        case TEXT:
            callbacks = &__callbacks_text;
            break;

        default:
            return std::string();        
    }

    if (__callbacks_audio.empty())
        __init();

    AIRPORTS::updateCurrentAirport();
        
    return (*callbacks)[str]();
}

bool CALLBACKS::hasCallback(int type, const std::string& key) {
    if (__callbacks_audio.empty())
        __init();

    switch(type) {
        case AUDIO:
            return __callbacks_audio.contains(key);

        case TEXT:
            return __callbacks_text.contains(key);

        default:
            break;
    }

    return false;
}

std::map<std::string, std::function<std::string()>> CALLBACKS::__callbacks_audio;
std::map<std::string, std::function<std::string()>> CALLBACKS::__callbacks_text;