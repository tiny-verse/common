#pragma once

#if (defined _WIN32)
#include <Windows.h>
#undef min
#undef max
#endif

#include <cassert>
#include <iostream>

namespace tiny {

    /** Initializes the terminal on Windows.

        While on Linux color escape sequences work by default, this may not be the case on Windows so this simple initializer sets the console mode to process output terminal sequences which allows us to do all kinds of pretty things.

        Note that this only works on recent Windows 10 versions and will not compile on older systems, which frankly is not a problem in 2021.
     */
    inline void initializeTerminal() {
#if (defined _WIN32)
        HANDLE cout = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD cmode;
        GetConsoleMode(cout, & cmode);
        cmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(cout, cmode);
#endif
    }

    enum class color {
        reset,
        black,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        gray,
        lightRed,
        lightGreen,
        lightYellow,
        lightBlue,
        lightMagenta,
        lightCyan,
        lightGray,
    };

    inline std::ostream & operator << (std::ostream & s, color c) {
        switch (c) {
        case color::black:
            s << "\033[30m";
            break;
        case color::red:
            s << "\033[31m";
            break;
        case color::green:
            s << "\033[32m";
            break;
        case color::yellow:
            s << "\033[33m";
            break;
        case color::blue:
            s << "\033[34m";
            break;
        case color::magenta:
            s << "\033[35m";
            break;
        case color::cyan:
            s << "\033[36m";
            break;
        case color::lightGray:
            s << "\033[37m";
            break;
        case color::reset:
            s << "\033[39m";
            break;
        case color::gray:
            s << "\033[90m";
            break;
        case color::lightRed:
            s << "\033[91m";
            break;
        case color::lightGreen:
            s << "\033[92m";
            break;
        case color::lightYellow:
            s << "\033[93m";
            break;
        case color::lightBlue:
            s << "\033[94m";
            break;
        case color::lightMagenta:
            s << "\033[95m";
            break;
        case color::lightCyan:
            s << "\033[96m";
            break;
        case color::white:
            s << "\033[97m";
            break;
        }
        return s;
    }

    inline char const * bg(color c) {
        switch (c) {
        case color::reset:
            return "\033[49m";
        case color::black:
            return "\033[40m";
        case color::red:
            return "\033[41m";
        case color::green:
            return "\033[42m";
        case color::yellow:
            return "\033[43m";
        case color::blue:
            return "\033[44m";
        case color::magenta:
            return "\033[45m";
        case color::cyan:
            return "\033[46m";
        case color::white:
            return "\033[47m";
        case color::gray:
            return "\033[100m";
        case color::lightRed:
            return "\033[101m";
        case color::lightGreen:
            return "\033[102m";
        case color::lightYellow:
            return "\033[103m";
        case color::lightBlue:
            return "\033[104m";
        case color::lightMagenta:
            return "\033[105m";
        case color::lightCyan:
            return "\033[106m";
        case color::lightGray:
            return "\033[107m";
        default:
            assert(false && "Color not supported");
            return "";
        }
    }

} // namespace tiny