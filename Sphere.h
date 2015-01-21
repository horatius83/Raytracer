#ifndef SPHERE_H
#define SPHERE_H

#include <tuple>
#include "Vector.h"

namespace Math
{
	class Sphere
	{
	public:
		Sphere() {
			_center.Set(0, 0, 0);
			_radius = 0;
		}
		Sphere(const Vector& center, float radius) {
			_center = center;
			_radius = radius;
		}

		const Vector& GetCenter() { return _center; }
		const float GetRadius() { return _radius; }

		std::tuple<bool,float> GetIntersection(const Vector& origin, const Vector& direction) {
			// If the dot of the ray to the center with the direction is <= 0 then it does not intersect
			auto rayToOrigin = _center.Sub(origin);
			auto dot = direction.Dot(rayToOrigin);
			if (dot > 0) {
				auto a = direction.Dot(rayToOrigin);
				auto c = rayToOrigin.Dot(rayToOrigin);
				auto x = a;
				auto y = a * a - c + _radius * _radius;
				auto pos = x + y;
				auto neg = x - y;
				return std::tuple<bool, float>(true, fminf(pos, neg));
			}
			return std::tuple<bool, float>(false, 0);
		}
	private:
		Vector _center;
		float _radius;
	};
}

#endif