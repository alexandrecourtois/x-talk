#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <string>
#include <map>
#include <SDL2/SDL_mixer.h>
#include <vector>

void select_audio_input();
void start_recording();
void stop_recording();
void load_files_WAV(const std::string& directoryPath, std::map<std::string, Mix_Chunk*> &sounds);
void play_files_WAV(const std::vector<std::string>& fileNames);
void play_tailnum();

#endif // AUDIO_H