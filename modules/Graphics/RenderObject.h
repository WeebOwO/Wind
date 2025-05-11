#pragma once

#include <glm/glm.hpp>

namespace wind 
{
    struct RenderObject 
    {
        glm::mat4 modelMatrix;
        glm::vec3 color;
        float     scale;
        bool      isVisible;

        RenderObject(const glm::mat4& model, const glm::vec3& col, float scl, bool visible) :
            modelMatrix(model), color(col), scale(scl), isVisible(visible) {}
    };
}