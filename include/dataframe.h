#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <compare>

extern struct Location {
    double latitude;
    double longitude;
    double altitude;

    auto operator <=>(const Location&) const = default;
} selected_airport_location;

extern Location old_airport_location;

#endif // DATAFRAME_H