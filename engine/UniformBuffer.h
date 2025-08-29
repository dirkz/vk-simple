#pragma once

#include "stdafx.h"

namespace vkdeck
{

struct UniformBuffer
{
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
