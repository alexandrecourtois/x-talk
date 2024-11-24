#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <atomic>
#include <tuple>

extern struct Dataframe {
    int xp_major;
    int xp_minor;
    
    char tailnum[256];
    
    float visibility;
    float qnh;
    float windspeed;
    float winddir;
    float latitude;
    float longitude;
    int com1_freq;
    int com2_freq;
    float com1_vol;
    float com2_vol;
    int com1_active;
    int com2_active;

    auto operator <=>(const Dataframe&) const = default;
} dataframe;

extern struct Location {
    double latitude;
    double longitude;
    double altitude;

    auto operator <=>(const Location&) const = default;
} selected_airport_location;

extern Location old_airport_location;

#endif // DATAFRAME_H