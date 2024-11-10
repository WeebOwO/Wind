#pragma once

#include <string>

namespace wind::rg
{
    struct VirutalResource
    {
        VirutalResource(const std::string& name) : name(name) {};
        virtual ~VirutalResource() noexcept = default;

        virtual bool IsImported() const { return false; }

        std::string name;
    };

    template<typename RESOURCE>
    class Resource : public VirutalResource
    {
    public:
        using Descriptor = typename RESOURCE::Descriptor;

        Resource() noexcept = default;
        Resource(const std::string& name, const Descriptor& desc) : VirutalResource(name), desc(desc) {}

        ~Resource() noexcept override = default;

        RESOURCE   resource {};
        Descriptor desc {};
        bool       detached = false;
    };

    template<typename RESOURCE>
    class ImportedResource : public Resource<RESOURCE>
    {
    public:
        using Descriptor = typename RESOURCE::Descriptor;

        ImportedResource(const std::string& name, const Descriptor& desc, const RESOURCE& resource) :
            Resource<RESOURCE>(name, desc)
        {
            this->resource = resource;
        }

        bool IsImported() const override { return true; }
    };
} // namespace wind::rg