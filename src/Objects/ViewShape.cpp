#include "stdafx.h"
#include "ViewShape.h"

ViewShape::ViewShape(float angle, float radius) :m_Angle(angle), m_Radius(radius)
{
	update();
}

ViewShape::~ViewShape()
{

}

size_t ViewShape::getPointCount() const
{
	return 30;
}

sf::Vector2f ViewShape::getPoint(size_t index) const
{
	static const float pi = 3.141592654f;
	if (index == 0)
		return{ 0,0 };


	float ang = m_Angle * pi / 180.0f;
	float step = 2 * ang / getPointCount();
	sf::Vector2f pos{ m_Radius*cos(ang - (index - 1)*step),m_Radius*sin(ang - (index - 1)*step) };
	return pos;
}

