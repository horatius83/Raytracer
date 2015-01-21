#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Vector.h"

namespace Graphics 
{
	class Renderable 
	{
	public:
		virtual Math::Vector GetMin();
		
		virtual Math::Vector GetMax();

		virtual float GetIntersection(const Math::Vector& origin, const Math::Vector& direction);
	};
}



#endif