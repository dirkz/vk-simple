#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct UniformObject
{
    UniformObject(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection)
        : m_model{model}, m_view{view}, m_projection{projection} {};

    glm::mat4 &Model()
    {
        return m_model;
    }

    glm::mat4 &View()
    {
        return m_view;
    }

    glm::mat4 &Projection()
    {
        return m_projection;
    }

  private:
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_projection;
};

} // namespace vkdeck
