#pragma once

namespace wind
{
    class Asset
    {
    public:
        virtual void Serialize() {}
        virtual void Deserialize() {}
    };
} // namespace wind