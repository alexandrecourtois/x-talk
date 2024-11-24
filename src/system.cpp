#include "system.h"
#include <iostream>
#include <unistd.h>

size_t getMemoryUsageInMB() {
    std::ifstream statm("/proc/self/statm");
    if (!statm.is_open()) {
        std::cerr << "Failed to open /proc/self/statm" << std::endl;
        return 0;
    }

    size_t memoryPages;
    statm >> memoryPages; // La première valeur est le nombre de pages mémoire utilisées
    statm.close();

    size_t pageSize = sysconf(_SC_PAGESIZE); // Taille d'une page mémoire en octets
    return (memoryPages * pageSize) / 1024 /1024; // Conversion en kilooctets
}