#ifndef SCENE_H
#define SCENE_H

#include "PacketVector.h"
#include "Renderable.h"

namespace Raytracer {
	class Scene
	{
	public:
		virtual void GetIntersection(Math::PacketVector& returnValue, const Math::PacketVector& origin, const Math::PacketVector& ray);
		virtual void AddSceneObject(const Graphics::Renderable& obj);
	};
}


#endif