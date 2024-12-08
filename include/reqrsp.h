#ifndef REQRSP_H
#define REQRSP_H

#include "enums.h"
#include <lockable.h>

#define REQ_GET_DATAFRAME       int(0)
#define REQ_UPD_AIRPORT         int(1)

#define RSP_ACK                 int(0)

class Req: public Lockable {
    public:
        Request type;
        double argD_1;
        double argD_2;
        double argD_3;
};

class Rsp_Dataframe: public Lockable {
    public:
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
        float infreq_visibility;
        float infreq_qnh;
        float infreq_windspeed;
        float infreq_winddir;
        
        auto operator <=>(const Rsp_Dataframe&) const = default;
};

extern Rsp_Dataframe dataframe;

template<typename T = int> class Rsp: public Lockable {
    public:
        T value;
};

#endif // REQRSP_H