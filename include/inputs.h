#pragma once

#include <pch.h>

class X_INPUT {
    private:
//#ifdef PLATFORM_LINUX
        static struct termios   __old_term;
        static struct termios   __new_term;
//#endif // PLATFORM_LINUX
        static int              __prev_ptt_state;
        static std::string      __garbage;
        static int              __ptt_button;
        static SDL_Joystick*    __joystick;


        X_INPUT() = default;

        static bool __isUtf8Character(const std::string& str);
        static bool __isLastCharacterASCII(const std::string& str);

    public:
        // --- KEYBOARD ---
        static char kbhit(bool in_xscan = false);
        static void xscan(std::string &str, void (*callback)() = nullptr, int delay = 100);
        static void xscan();
        static void xscan_anykey();
        static void enableInput();
        static void disableInput();

        // --- PUSH-TO-TALK ---
        static bool ptt_changed();
        static bool ptt_pushed();

        // --- JOYSTICK ---
        static void selectJoystick();
};