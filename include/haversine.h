#ifndef HAVERSINE_H
#define HAVERSINE_H

#include <cmath>

// Rayon moyen de la Terre en kilomètres
#define R       6371.0

// Fonction pour convertir les degrés en radians
double to_rad(double degree);

// Fonction pour calculer la distance entre deux points en latitude et longitude
double haversine(double lat1, double lon1, double lat2, double lon2);

#endif // HAVERSINE_H