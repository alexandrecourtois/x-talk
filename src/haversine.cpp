#include "haversine.h"

double to_rad(double degree) {
    return degree * M_PI / 180.0;
}

// Fonction pour calculer la distance entre deux points en latitude et longitude
double haversine(double lat1, double lon1, double lat2, double lon2) {
    // Convertir les coordonnées en radians
    lat1 = to_rad(lat1);
    lon1 = to_rad(lon1);
    lat2 = to_rad(lat2);
    lon2 = to_rad(lon2);

    // Différences de coordonnées
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    // Formule de Haversine
    double a = std::sin(dlat / 2) * std::sin(dlat / 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::sin(dlon / 2) * std::sin(dlon / 2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));

    // Calcul de la distance
    return R * c; // Distance en kilomètres
}