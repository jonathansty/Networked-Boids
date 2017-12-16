#pragma once
#include "../stdafx.h"

namespace Math
{
	static const double PI = 3.14159265359;

	static std::random_device _rdev;
	static std::mt19937 _mt(_rdev());

#pragma region random Generating
	template<typename T>
	T Random(T min, T max);

	template<>
	inline int Random(int min, int max)
	{
		assert(min < max && "Stupid, fix your random min max");
		std::uniform_int_distribution<int> uniDist{ min,max };
		return uniDist(_mt);
	}
	template<>
	inline double Random(double min, double max)
	{
		assert(min < max && "Stupid, fix your random min max");
		std::uniform_real_distribution<double> d{ min,max };
		return d(_mt);
	}
	template<>
	inline float Random(float min, float max)
	{
		assert(min < max && "Stupid, fix your random min max");
		std::uniform_real_distribution<float> d{ min,max };
		return d(_mt);
	}
#pragma endregion

	template<typename T>
	float dot(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	template<typename T>
	float length(const sf::Vector2<T>& v1)
	{
		return sqrt(v1.x*v1.x + v1.y*v1.y);
	}

	/* 2D normalize */
	template<typename T>
	sf::Vector2<T> Normalize(const sf::Vector2<T> v1)
	{
		/* Optimize this get rid of sqrt*/
		float l = sqrt(dot(v1, v1));
		if (l != 0)
			return{ v1.x / l,v1.y / l };
		else
			return{ 0,0 };
	}

	/* 2D cross */
	template<typename T>
	float Cross(const sf::Vector2<T>& v1, const sf::Vector2<T>& v2)
	{
		return v1.x*v2.y - v2.x*v1.y;
	}

	/* Force inline to disable linker errors */
	inline float AngleBetween(const sf::Vector2f& v1, const sf::Vector2f& v2)
	{
		float dp = (dot(v1, v2));
		float cross = Cross(v1, v2);
		float angle = atan2(cross, dp);
		float a = (float)(angle * 180.0f / PI);

		return a;
	}
}
