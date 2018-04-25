#pragma once

#include <cstdint>
#include <string>

namespace om
{
struct engine
{
    struct params
    {
        struct window_mode
        {
            uint32_t width  = 1024;
            uint32_t height = 768;
        };
        window_mode wnd_mode;
        std::string title;
    };

    virtual void initialize(params) = 0;

    virtual ~engine();
};
} // end namespace om
