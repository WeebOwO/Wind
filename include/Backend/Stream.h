#pragma once

#include <vector>

#include "Command.h"
#include "Resource.h"

#include "Core/NonCopy.h"

namespace wind
{
    class Device;

    class CommandStream final : public Resource
    {
    public:
        NON_COPYABLE(CommandStream);

        CommandStream(Device* device) : Resource(device, Tag::CommandStream) {};
        ~CommandStream();

        CommandStream& operator<<(Command* command);

        void Execute();
        
    private:
        std::vector<Command*> m_commands;
    };
} // namespace wind