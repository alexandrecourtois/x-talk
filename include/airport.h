#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>
#include <map>

#define UNSUPPORTED_AIRPORT     std::string("UNSUPPORTED_AIRPORT")

std::string get_airport_by_frequency(float frequency);
std::string get_active_rwy(int windDirection, const std::map<std::string, int>& runways);
void update_current_airport();
bool supported_airport();

#endif  // AIRPORT_H