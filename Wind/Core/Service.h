#pragma once

namespace wind
{
    template <typename T>
    struct Service
    {      
        virtual void Init() {}
        virtual void Quit() {}
    };
} // namespace wind