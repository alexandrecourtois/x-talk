#pragma once

// ******************************************************
// * STD INCLUDES
// ******************************************************

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <stdexcept>
#include <cmath>
#include <string>
#include <optional>
#include <stop_token>
#include <thread>
#include <functional>
#include <compare>
#include <string_view>
#include <random>
#include <regex>
#include <algorithm>
#include <sstream>
#include <memory>
#include <mutex>
#include <filesystem>
#include <ostream>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <thread>
#include <mutex>

#ifdef PLATFORM_LINUX
#include <termios.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <utf8cpp/utf8.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <vosk_api.h>
#include <zmq.hpp>
#endif

#ifdef PLATFORM_WINDOWS
#include "third_party/nlohmann/json.hpp"
#include "third_party/nlohmann/json_fwd.hpp"
#include "third_party/utf8/utf8.h"
#include "third_party/SDL2/SDL.h"
#include "third_party/SDL2/SDL_audio.h"
#include "third_party/SDL2/SDL_mixer.h"
#include "third_party/vosk_api/vosk_api.h"
#include "third_party/cppzmq/zmq.hpp"
#endif