#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct Vertex
{
    static vk::VertexInputBindingDescription BindingDescription()
    {
        vk::VertexInputBindingDescription description{0, sizeof(Vertex),
                                                      vk::VertexInputRate::eVertex};
        return description;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> AttributeDescriptions()
    {
        constexpr uint32_t binding = 0;
        constexpr uint32_t location = 0;

        vk::VertexInputAttributeDescription descriptionPosition{
            location, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex, m_position)};

        vk::VertexInputAttributeDescription descriptionColor{
            location + 1, binding, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_color)};

        vk::VertexInputAttributeDescription descriptionTexCoordinate{
            location + 2, binding, vk::Format::eR32G32Sfloat, offsetof(Vertex, m_texCoord)};

        return {descriptionPosition, descriptionColor, descriptionTexCoordinate};
    }

    Vertex(glm::vec2 position, glm::vec3 color, glm::vec2 texCoord)
        : m_position{position}, m_color{color}, m_texCoord{texCoord}
    {
    }

    glm::vec2 &Position()
    {
        return m_position;
    }

    glm::vec3 &Color()
    {
        return m_color;
    }

    glm::vec2 &TexCoord()
    {
        return m_texCoord;
    }

  private:
    glm::vec2 m_position;
    glm::vec3 m_color;
    glm::vec2 m_texCoord;
};

} // namespace vkdeck
