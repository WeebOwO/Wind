#pragma once

#include <string>

namespace wind::rg
{
    struct VirutalResource
    {
        VirutalResource(const std::string& name) : name(name) {};
        virtual ~VirutalResource() = default;

        std::string name;
    };

    template<typename RESOURCE>
    class Resource : public VirutalResource
    {
    public:
        using Descriptor = typename RESOURCE::Descriptor;

        Resource() noexcept = default;
        Resource(const std::string& name, const Descriptor& desc) : VirutalResource(name), desc(desc) {}

        RESOURCE   resource {};
        Descriptor desc {};
        bool       detached = false;
    };
} // namespace wind::rg