#pragma once

#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include <Backend/Device.h>

namespace wind
{
    struct GLTFScene
    {
        GLTFScene(const std::string& path) : path(path) {}
        ~GLTFScene();

        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 normal;
            glm::vec2 uv;
            glm::vec3 color;
            glm::vec4 tangent;
        };

        struct VertexBuffer
        {
            uint32_t        vertexCount;
            DeviceBufferRef buffer;
        };

        struct IndexBuffer
        {
            uint32_t        indexCount;
            DeviceBufferRef buffer;
        };

        struct Node;

        struct Primitive
        {
            uint32_t firstIndex;
            uint32_t indexCount;
            int      materialIndex;
        };

        struct Mesh
        {
            std::vector<Primitive> primitives;
        };

        struct Node
        {
            Node*              parent;
            std::vector<Node*> children;
            Mesh               mesh;
            glm::mat4          transform;
            std::string        name;
            bool               isVisiable = true;
            ~Node()
            {
                for (auto& child : children)
                {
                    delete child;
                }
            }
        };

        struct Material
        {
            glm::vec4         baseColorFactor = glm::vec4(1.0f);
            uint32_t          baseColorTextureIndex;
            uint32_t          normalTextureIndex;
            std::string       alphaMode = "OPAQUE";
            float             alphaCutOff;
            bool              doubleSided = false;
            vk::DescriptorSet descriptorSet;
            vk::Pipeline      pipeline;
        };

        void LoadImages(tinygltf::Model& input);
        void LoadTextures(tinygltf::Model& input);
        void LoadMaterials(tinygltf::Model& input);
        void LoadNode(const tinygltf::Node&           inputNode,
                      const tinygltf::Model&          input,
                      GLTFScene::Node*                parent,
                      std::vector<uint32_t>&          indexBuffer,
                      std::vector<GLTFScene::Vertex>& vertexBuffer);

        std::vector<GPUTexture> textures;
        std::vector<uint32_t>   textureIndices;
        std::vector<Material>   materials;
        std::vector<Node*>      nodes;
        std::string             path;
    };
} // namespace wind