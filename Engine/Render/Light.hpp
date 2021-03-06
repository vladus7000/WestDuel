#pragma once

#include <Engine/Camera.hpp>

class Light
{
public:
	enum class Type
	{
		Directional,
		Point,
		Spot
	};

	Light() {}
	
	void updateMatrices()
	{
		m_camera.setPosition(m_position);
		m_camera.setDirection(m_direction);
		m_camera.updateView();
		if (perspective)
		{
			m_camera.setProjection(60.0f, m_aspect, 0.1f, 1000.0f);
		}
		else
		{
			m_camera.setOrtho(-800.0f, 800.f, -800.0f, 800.0f, 0.1f, 1000.0f);
		}
	}

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_attenuation;
	glm::vec3 m_intensity;
	float m_radius = 0.0f;
	Type m_type = Type::Directional;
	float m_cutoff;
	bool perspective = true;
	float m_aspect = 600.0f / 800.0f;
	bool enabled = true;
	Camera m_camera;
};