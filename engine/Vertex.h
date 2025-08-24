#pragma once

#include "stdafx.h"

namespace vksimple
{

struct Vertex
{
    static vk::VertexInputBindingDescription BindingDescription()
    {
        vk::VertexInputBindingDescription description{0, sizeof(Vertex),
                                                      vk::VertexInputRate::eVertex};
        return description;
    }

    static std::array<vk::VertexInputAttributeDescription, 2> AttributeDescriptions()
    {
        constexpr uint32_t binding = 0;
        constexpr uint32_t location = 0;
        vk::VertexInputAttributeDescription description1{
            binding, location, vk::Format::eR32G32Sfloat, offsetof(Vertex, m_position)};
        vk::VertexInputAttributeDescription description2{
            binding, location + 1, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_color)};

        return {description1, description2};
    }

    Vertex(glm::vec2 position, glm::vec3 color) : m_position{position}, m_color{color}
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

  private:
    glm::vec2 m_position;
    glm::vec3 m_color;
};

} // namespace vksimple
