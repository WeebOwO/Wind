#pragma once

#include "Core/NonCopy.h"

namespace wind
{
    template<typename T>
    class Singleton
    {
    public:
        Singleton()          = default;
        virtual ~Singleton() = default;

        static T& GetInstance()
        {
            static T instance;
            return instance;
        }

    private:
        NON_COPYABLE(Singleton);
    };
} // namespace wind