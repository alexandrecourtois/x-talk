#ifndef REQRSP_H
#define REQRSP_H

#include <compare>

#define REQ_GET_DATAFRAME       int(0)
#define REQ_UPD_AIRPORT         int(1)

#define RSP_ACK                 int(0)

struct Req {
    unsigned int type;
    double argD_1 = 0.0f;
    double argD_2 = 0.0f;
    double argD_3 = 0.0f;
};

struct Rsp_Dataframe {
    int xp_major;               // XP release number
    int xp_minor;               // ...
    
    char tailnum[256];          // Aircraft tail number
    
    float visibility;           // Visibility in m around aircraft
    float qnh;                  // QNH around aircraft
    float windspeed;            // Wind speed around aircraft
    float winddir;              // Wind direction around aircraft
    float latitude;             // ...
    float longitude;            // Position of aircraft (XYZ)
    double altitude;            // ...
    int com1_freq;              // COM1 radio frequency
    int com2_freq;              // COM2 ...
    float com1_vol;             // COM1 radio volume
    float com2_vol;             // COM2 ...
    int com1_active;            // COM1 radio status
    int com2_active;            // COM2 ...
    
    auto operator <=>(const Rsp_Dataframe&) const = default;
};

extern Rsp_Dataframe dataframe;

template<typename T = int> struct Rsp {
    T value;
};

#endif // REQRSP_H