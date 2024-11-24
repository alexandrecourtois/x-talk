#include "ptt.h"
#include "globals.h"
#include <SDL2/SDL.h>
#include <iostream>

bool ptt_changed() {
    SDL_Event event;
    
    while(SDL_PollEvent(&event)) { };
    
    int ptt_state = SDL_JoystickGetButton(joystick, ptt);
    
    if (ptt_state != prev_ptt_state) {
        std::cout << std::endl << std::flush;
        prev_ptt_state = ptt_state;
        return true;
    }
    
    return false;
}

bool ptt_pushed() {
    if (!nojoystick) {
        SDL_Event event;
    
        while(SDL_PollEvent(&event)) { };
    
        return SDL_JoystickGetButton(joystick, ptt);
    }
    
    return false;
}

int prev_ptt_state = 0;