#pragma once

#include <atomic>
#include <memory>

namespace crychic
{
    template<typename T>
    using Ref = std::shared_ptr<T>;
    template<typename T>
    using Scope = std::unique_ptr<T>;
} // namespace crychic