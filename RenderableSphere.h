#ifndef RENDERABLESPHERE_H
#define RENDERABLESPHERE_H

#include <tuple>
#include <limits>
#include "Sphere.h"
#include "Renderable.h"
#include "Vector.h"

namespace Graphics
{
	class RenderableSphere : public Renderable 
	{
	public:
		RenderableSphere(const Math::Vector& center, float radius) {
			_sphere = Math::Sphere(center, radius);
		}

		Math::Vector GetMin() {
			return Math::Vector(_sphere.GetCenter().GetX() - _sphere.GetRadius(), 
				_sphere.GetCenter().GetY() - _sphere.GetRadius(),
				_sphere.GetCenter().GetZ() - _sphere.GetRadius());
		}

		Math::Vector GetMax() {
			auto center = _sphere.GetCenter();
			auto radius = _sphere.GetRadius();
			return Math::Vector(center.GetX() + radius, center.GetY() + radius, center.GetZ() + radius);
		}

		float GetIntersection(const Math::Vector& origin, const Math::Vector& direction) {
			auto intersect = _sphere.GetIntersection(origin, direction);
			if (std::get<0>(intersect)) {
				return std::get<1>(intersect);
			}
			return std::numeric_limits<float>::max();
		}
	private:
		Math::Sphere _sphere;
	};
}

#endif