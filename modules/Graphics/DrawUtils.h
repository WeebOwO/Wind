#pragma once

#include <glm/glm.hpp>
#include "Backend/Guard.h"

namespace wind::DrawUtils
{
    void DrawCube(vk::CommandBuffer cmdBuffer, const glm::mat4& modelMatrix, const glm::vec3& color);
}