#pragma once

#include <tuple>

// learn from google filament
namespace wind
{
    template<typename Allocator, typename... Types>
    class StructureOfArraysBase
    {
    public:
        using SOA       = StructureOfArraysBase<Allocator, Types...>;
        using Structure = std::tuple<Types...>;

        template<size_t N>
        using TypeAt = typename std::tuple_element<N, Structure>::type;

        

    private:
        static constexpr const size_t kArrayCount = sizeof...(Types);
    };
} // namespace wind