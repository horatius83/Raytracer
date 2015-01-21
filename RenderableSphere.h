#ifndef RENDERABLESPHERE_H
#define RENDERABLESPHERE_H

#include "Sphere.h"
#include "Renderable.h"
#include "Vector.h"

namespace Graphics
{
	class RenderableSphere : public Renderable 
	{
	public:
		RenderableSphere(const Vector& center, float radius) {
			_sphere = Math::Sphere(center, radius);
		}

		public Math::Vector GetMin() {
			return Math::Vector(center.GetX() - radius, center.GetY() - radius, center.GetZ() - radius);
		}

		return Math::Vector GetMax() {
			return Math::Vector(center.GetX() + radius, center.GetY() + radius, center.GetZ() + radius);
		}

		public float GetIntersection(const Math::Vector& origin, const Math::Vector& direction) {
			auto intersect = _sphere.GetIntersection(origin, direction);
			return FLOAT_MAX;
		}
	private:
		Math::Sphere _sphere;
	};
}

#endif