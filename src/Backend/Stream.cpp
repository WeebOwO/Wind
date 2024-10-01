#pragma once

#include "Backend/Stream.h"

namespace wind
{
    CommandStream& CommandStream::operator<<(Command* command)
    {
        m_commands.push_back(command);
        return *this;
    }
    
} // namespace wind