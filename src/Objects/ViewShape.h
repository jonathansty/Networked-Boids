#pragma once
#include <SFML/Graphics/Shape.hpp>

class ViewShape : public sf::Shape
{
public:
	explicit ViewShape(float angle, float radius);
	~ViewShape();
	virtual size_t getPointCount() const;
	virtual sf::Vector2f getPoint(size_t index)const;

private:
	float m_Angle;
	float m_Radius;
};

