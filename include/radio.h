#ifndef RADIO_H
#define RADIO_H

#include <session.h>

class RADIO {
    private:
        static int __old_com1_freq;
        static int __old_com2_freq;
        static int __old_com1_status;
        static int __old_com2_status;

        RADIO() = default;

        static double __white_rabbit_obj(double factor);

    public:
        static constexpr double VHF_MAX_RANGE   =   200.0f;
        static constexpr double BASE_VISIBILITY = 64000.0f;

        static double getRange();
        static int getNoiseVolume();
        static int getSpeakVolume(int noiseVolume);
        static bool isWithinRange();
        static double getDistanceToAirport();
        static void updateStatus();
};

#endif  // RADIO_H