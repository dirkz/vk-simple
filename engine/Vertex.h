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
