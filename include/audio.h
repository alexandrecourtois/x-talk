#pragma once

#include <pch.h>

class AUDIO {
    private:
        static std::map<std::string, Mix_Chunk*>    __WAV_files;
        static std::vector<unsigned char>           __audio_stream;
        static bool                                 __is_recording;
        static SDL_AudioDeviceID                    __device_id;
        static int                                  __sample_rate;
        static std::map<std::string, std::string>   __phrases;

        AUDIO() = default;

        static void __init_phrs(const std::string& phrasePath);
        static void __load_audio(const std::string& audioPath);
        static void __play(const std::vector<std::string>& files, int channel = CHANNEL_RANDOM);
        static int  __play(const std::string& file, int channel = CHANNEL_RANDOM, bool loop = false);
        static void __play_tailnum();
        static void __play_radiostart();
        static void __play_radiostop();
        static void __play_radiocom(const std::vector<std::string>& files);
        static void __audio_callback(void* userdata, Uint8* stream, int len);
        
        static std::vector<std::string> __phrase_to_waves(const std::string& phrase);

    public:
        enum class Type {
            TAILNUM,
            RADIOCOM,
            RADIOSTART,
            RADIOSTOP,
        };

        static constexpr int CHANNEL_RANDOM         = -1;
        static constexpr int CHANNEL_RADIO_REC      =  0;
        static constexpr int CHANNEL_RADIO_COM      =  1;
        static constexpr int CHANNEL_RADIO_NOISE    =  2;

        static void init(const std::string& audioPath, const std::string& phrasePath);
        static void selectDevice();

        static void startRecording();
        static void stopRecording();

        static void play(Type type, const std::vector<std::string>& files = std::vector<std::string>());

        static bool isRecording();

        static const std::vector<unsigned char>& getAudioStream();
};