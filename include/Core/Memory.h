#pragma once

namespace wind
{
    template<typename T>
    T* WIND_NEW(size_t count)
    {
        return new T[count];
    }

    template<typename T>
    void WIND_DELETE(T* ptr)
    {
        delete[] ptr;
    }

} // namespace wind