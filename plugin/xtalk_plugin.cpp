//#define XPLM200 = 1;
#include <zmq.h>
#define XPLM400 = 1;

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>
#include <XPLMWeather.h>
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <XPLMProcessing.h>
#include <cstring>
#include <reqrsp.h>
#include <zmq_server.h>
#include <enums.h>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

log4cpp::Appender *fileAppender = nullptr;
log4cpp::Appender *consoleAppender = nullptr;
log4cpp::Category *root = nullptr;

XPLMDataRef dref_tailnum    = nullptr;
XPLMDataRef dref_visibility = nullptr;
XPLMDataRef dref_qnh        = nullptr;
XPLMDataRef dref_windspeed  = nullptr;
XPLMDataRef dref_winddir    = nullptr;
XPLMDataRef dref_latitude   = nullptr;
XPLMDataRef dref_longitude  = nullptr;
XPLMDataRef dref_com1_freq  = nullptr;
XPLMDataRef dref_com2_freq  = nullptr;
XPLMDataRef dref_com1_vol   = nullptr;
XPLMDataRef dref_com2_vol   = nullptr;
XPLMDataRef dref_com1_on    = nullptr;
XPLMDataRef dref_com2_on    = nullptr;
XPLMDataRef dref_altitude   = nullptr;

std::jthread zmq_thread;
ZMQ_Server* pserver_ptr = nullptr;


// Fonction de rappel qui sera appelée périodiquement
float callback_dataframe(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void* inRefcon) {
    XPLMWeatherInfo_t weather;   
    Req pserver_req = pserver_ptr->getLastRequest();
    XPLMGetWeatherAtLocation(pserver_req.argD_1, pserver_req.argD_2, pserver_req.argD_3, &weather);

    __GLOBAL__dataframe.lock();
    __GLOBAL__dataframe.infreq_visibility    = weather.visibility;
    __GLOBAL__dataframe.infreq_qnh           = weather.pressure_alt;
    __GLOBAL__dataframe.infreq_windspeed     = weather.wind_spd_alt;
    __GLOBAL__dataframe.infreq_winddir       = weather.wind_dir_alt;
    XPLMGetDatab(dref_tailnum, __GLOBAL__dataframe.tailnum, 0, sizeof(__GLOBAL__dataframe.tailnum));
    __GLOBAL__dataframe.visibility    = XPLMGetDataf(dref_visibility) * 1609.34f;
    __GLOBAL__dataframe.qnh           = XPLMGetDataf(dref_qnh);
    __GLOBAL__dataframe.windspeed     = XPLMGetDataf(dref_windspeed);
    __GLOBAL__dataframe.winddir       = XPLMGetDataf(dref_winddir);
    __GLOBAL__dataframe.latitude      = XPLMGetDataf(dref_latitude);
    __GLOBAL__dataframe.longitude     = XPLMGetDataf(dref_longitude);
    __GLOBAL__dataframe.com1_freq     = XPLMGetDatai(dref_com1_freq);
    __GLOBAL__dataframe.com2_freq     = XPLMGetDatai(dref_com2_freq);
    __GLOBAL__dataframe.com1_vol      = XPLMGetDataf(dref_com1_vol);
    __GLOBAL__dataframe.com2_vol      = XPLMGetDataf(dref_com2_vol);
    __GLOBAL__dataframe.com1_active   = XPLMGetDatai(dref_com1_on);
    __GLOBAL__dataframe.com2_active   = XPLMGetDatai(dref_com2_on);
    __GLOBAL__dataframe.altitude      = XPLMGetDatad(dref_altitude);
    __GLOBAL__dataframe.unlock(); 

    return 1.0; // Retourne 2.0 pour rappeler cette fonction toutes les 2 secondes
}

void zmq_server_thread() {
    zmq::context_t context(1);
    ZMQ_Server pserver(context, TCP_Server("*:5555"));
    pserver_ptr = &pserver;
    pserver.start();
}

// Fonction de démarrage du plugin
PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc) {
    strcpy(outName, "xtalk_plugin");
    strcpy(outSig, "xplane.plugin.xtalk_plugin");
    strcpy(outDesc, "xtalk plugin");

     try {
        // Création des layouts
        log4cpp::PatternLayout *fileLayout = new log4cpp::PatternLayout();
        fileLayout->setConversionPattern("%d [%p] %c: %m%n");

        log4cpp::PatternLayout *consoleLayout = new log4cpp::PatternLayout();
        consoleLayout->setConversionPattern("%d [%p]: %m%n");

        // Création des appenders
        fileAppender = new log4cpp::FileAppender("fileAppender", "application.log");
        fileAppender->setLayout(fileLayout);

        consoleAppender = new log4cpp::OstreamAppender("consoleAppender", &std::cout);
        consoleAppender->setLayout(consoleLayout);

        // Initialisation de la catégorie racine
        root = &log4cpp::Category::getRoot();
        root->setPriority(log4cpp::Priority::DEBUG); // Niveau minimal de priorité
        root->addAppender(fileAppender);
        root->addAppender(consoleAppender);

        // Message initial
        root->info("Logger initialisé avec succès.");
    } catch (const log4cpp::ConfigureFailure &e) {
        std::cerr << "Erreur lors de l'initialisation de log4cpp : " << e.what() << std::endl;
    }

    XPLMGetVersions(&__GLOBAL__dataframe.xp_major, &__GLOBAL__dataframe.xp_minor, NULL);

    dref_tailnum    = XPLMFindDataRef("sim/aircraft/view/acf_tailnum");
    dref_visibility = XPLMFindDataRef("sim/weather/aircraft/visibility_reported_sm");
    dref_qnh        = XPLMFindDataRef("sim/weather/aircraft/barometer_current_pas");
    dref_windspeed  = XPLMFindDataRef("sim/weather/aircraft/wind_speed_kts");
    dref_winddir    = XPLMFindDataRef("sim/weather/aircraft/wind_direction_degt");
    dref_latitude   = XPLMFindDataRef("sim/flightmodel/position/latitude");
    dref_longitude  = XPLMFindDataRef("sim/flightmodel/position/longitude");
    dref_com1_freq  = XPLMFindDataRef("sim/cockpit2/radios/actuators/com1_frequency_hz_833");
    dref_com2_freq  = XPLMFindDataRef("sim/cockpit2/radios/actuators/com2_frequency_hz_833");
    dref_com1_vol   = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_volume_com1");
    dref_com2_vol   = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_volume_com2");
    dref_com1_on    = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_selection_com1");
    dref_com2_on    = XPLMFindDataRef("sim/cockpit2/radios/actuators/audio_selection_com2");
    dref_altitude   = XPLMFindDataRef("sim/flightmodel2/position/pressure_altitude");


    zmq_thread = std::jthread(zmq_server_thread);
    XPLMRegisterFlightLoopCallback(callback_dataframe, 1.0, NULL);

    return 1;
}

PLUGIN_API void XPluginStop() {
    XPLMUnregisterFlightLoopCallback(callback_dataframe, 0);

    if (zmq_thread.joinable())
        zmq_thread.join();

    root->info("Fermeture du logger.");
    log4cpp::Category::shutdown();
}

PLUGIN_API int XPluginEnable() {
    return 1;
}

PLUGIN_API void XPluginDisable() {}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam) {}
