#ifndef AABB_H
#define AABB_H

#include "PacketVector.h"
#include "Vector.h"
#include <cfloat>

//debug
#include "Plane.h"
/*
--------*
|       |
|       |
*--------

For a cube 1 unit on each side the left point is (-1,-1,-1) and the right point is (1,1,1)
(this is important! the right point should be always be greater than the left point, otherwise
it will mess up the intersection calculations)
*/
namespace Math
{
	template <class T>
	class Aabb
	{
	public:
		/*
		0000 = MMM = 0x0
		0010 = MMP = 0x2
		0100 = MPM = 0x4
		0110 = MPP = 0x6
		1000 = PMM = 0x8
		1010 = PMP = 0xA
		1100 = PPM = 0xC
		1110 = PPP = 0xE
		*/
		enum RAY_ORIENTATION{ MMM, MMP = 0x2, MPM = 0x4, MPP = 0x6, PMM = 0x8, PMP = 0xA, PPM = 0xC, PPP = 0xE };
		bool Intersects(const Vector& oOrigin, const Vector& Direction, RAY_ORIENTATION eOrientation);

		bool Intersects(const Vector& oOrigin, const Vector& Direction);
		void DoesIntersect(Vector128& oAnswer, const PacketVector& oOrigin, const PacketVector& Direction);

		inline void SetLeftPoint(const Math::Vector& oPoint){ m_oLeftPoint = oPoint; };
		inline void SetRightPoint(const Math::Vector& oPoint){ m_oRightPoint = oPoint; };
		inline Vector& GetLeftPoint(){ return m_oLeftPoint; };
		inline Vector& GetRightPoint(){ return m_oRightPoint; };
		inline void SetData(const T& t){ m_oData = t; };
		inline T&		GetData(){ return m_oData; };
	private:
		void DoesIntersectIndividual(Vector128& oAnswer, const PacketVector& oOrigin,
			const PacketVector& Direction, unsigned int uiMask[]);
		Vector	m_oLeftPoint;
		Vector m_oRightPoint;
		T		m_oData;
	};

	template <class T>
	inline void Aabb<T>::DoesIntersect(Vector128& oAnswer, const PacketVector& oOrigin,
		const PacketVector& Direction)
	{
		//Determine if all four vectors have the same orientation
		unsigned int uiMask[3];
		uiMask[0] = _mm_movemask_ps(_mm_cmpge_ps(Direction.m_sseX, _mm_set_ps1(0.0f)));
		uiMask[1] = _mm_movemask_ps(_mm_cmpge_ps(Direction.m_sseY, _mm_set_ps1(0.0f)));
		uiMask[2] = _mm_movemask_ps(_mm_cmpge_ps(Direction.m_sseZ, _mm_set_ps1(0.0f)));
		RAY_ORIENTATION eOrientation = MMM;
		unsigned int uiOrientation = 0;
		//oAnswer.m_sseData = _mm_set_ps1(0.0f);
		oAnswer.m_sseData = _mm_set_ps1(FLT_MAX);

		//Are all four X's in the positive or negative direction
		switch (uiMask[0])
		{
		case 15:	uiOrientation += 0x8;	break;
		case 0:							break;
		default:	DoesIntersectIndividual(oAnswer, oOrigin, Direction, uiMask);	return;
		}

		//All are four Y's in the positive or negative direction
		switch (uiMask[1])
		{
		case 15:	uiOrientation += 0x4;	break;
		case 0:							break;
		default:	DoesIntersectIndividual(oAnswer, oOrigin, Direction, uiMask);	return;
		}

		//Are all four Z's in the positive or negative direction
		switch (uiMask[2])
		{
		case 15:	uiOrientation += 0x2;	break;
		case 0:							break;
		default:	DoesIntersectIndividual(oAnswer, oOrigin, Direction, uiMask);	return;
		}

		eOrientation = RAY_ORIENTATION(uiOrientation);

		Math::PacketVector left, right;
		left.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.GetX()), oOrigin.m_sseX);
		left.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.GetY()), oOrigin.m_sseY);
		left.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oLeftPoint.fGetZ()), oOrigin.m_sseZ);
		right.m_sseX = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.GetX()), oOrigin.m_sseX);
		right.m_sseY = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.GetY()), oOrigin.m_sseY);
		right.m_sseZ = _mm_sub_ps(_mm_set_ps1(m_oRightPoint.fGetZ()), oOrigin.m_sseZ);

		__m128 c2[3], d2[3], cmp, sub, zero;
		zero = _mm_set_ps1(0.0f);

		switch (eOrientation)
		{
		case MMM:
		{
			//See if origin is less then m_oLeftPoint, if so then the ray misses
			Math::Vector128 ltX, ltY, ltZ;
			ltX.m_sseData = _mm_cmplt_ps(oOrigin.m_sseX, _mm_set_ps1(m_oLeftPoint.GetX()));
			ltY.m_sseData = _mm_cmplt_ps(oOrigin.m_sseY, _mm_set_ps1(m_oLeftPoint.GetY()));
			ltZ.m_sseData = _mm_cmplt_ps(oOrigin.m_sseZ, _mm_set_ps1(m_oLeftPoint.fGetZ()));

			if (_mm_movemask_ps(ltX.m_sseData) == 0xF || _mm_movemask_ps(ltY.m_sseData) == 0xF ||
				_mm_movemask_ps(ltZ.m_sseData) == 0xF)
				return;

			c2[0] = _mm_mul_ps(Direction.m_sseX, left.m_sseX);
			c2[1] = _mm_mul_ps(Direction.m_sseX, left.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, left.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, right.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, right.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, right.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmplt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xE)
					return;
			}

			c2[0] = _mm_mul_ps(Direction.m_sseY, left.m_sseX);
			c2[1] = _mm_mul_ps(Direction.m_sseZ, left.m_sseX);
			c2[2] = _mm_mul_ps(Direction.m_sseZ, left.m_sseY);
			d2[0] = _mm_mul_ps(Direction.m_sseX, right.m_sseY);
			d2[1] = _mm_mul_ps(Direction.m_sseX, right.m_sseZ);
			d2[2] = _mm_mul_ps(Direction.m_sseY, right.m_sseZ);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmple_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xE)
					return;
			}
		}break;
		case MMP:
		{
			cmp = _mm_cmplt_ps(oOrigin.m_sseX, _mm_set_ps1(m_oLeftPoint.GetX()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmplt_ps(oOrigin.m_sseY, _mm_set_ps1(m_oLeftPoint.GetY()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmpgt_ps(oOrigin.m_sseZ, _mm_set_ps1(m_oRightPoint.fGetZ()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;

			c2[0] = _mm_mul_ps(Direction.m_sseX, left.m_sseX);
			c2[1] = _mm_mul_ps(Direction.m_sseX, left.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, left.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, right.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, left.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, left.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmplt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xE)
					return;
			}

			c2[0] = _mm_mul_ps(Direction.m_sseX, right.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseX, right.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, right.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, left.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseY, right.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, right.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmpgt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xE)
					return;
			}
		}break;
		case MPM:
		{
			return;
		}break;
		case MPP:
		{

			cmp = _mm_cmplt_ps(oOrigin.m_sseX, _mm_set_ps1(m_oLeftPoint.GetX()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmpgt_ps(oOrigin.m_sseY, _mm_set_ps1(m_oRightPoint.GetY()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmpgt_ps(oOrigin.m_sseZ, _mm_set_ps1(m_oRightPoint.fGetZ()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;

			c2[0] = _mm_mul_ps(Direction.m_sseX, left.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseX, left.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, right.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, left.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, left.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, left.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmplt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xF)
					return;
			}

			c2[0] = _mm_mul_ps(Direction.m_sseX, right.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseX, right.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, left.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, right.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, right.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, right.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmpgt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xF)
					return;
			}
		}break;
		case PMM:
		{
			return;
		}break;
		case PMP:
		{
			cmp = _mm_cmpgt_ps(oOrigin.m_sseX, _mm_set_ps1(m_oRightPoint.GetX()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmplt_ps(oOrigin.m_sseY, _mm_set_ps1(m_oLeftPoint.GetY()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmpgt_ps(oOrigin.m_sseZ, _mm_set_ps1(m_oRightPoint.fGetZ()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;

			c2[0] = _mm_mul_ps(Direction.m_sseX, right.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseX, right.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, left.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, right.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, left.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, left.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmplt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xF)
					return;
			}

			c2[0] = _mm_mul_ps(Direction.m_sseX, left.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseX, left.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, right.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, left.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, right.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, right.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmpgt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xF)
					return;
			}
		}break;
		case PPM:
		{
			return;
		}break;
		case PPP:
		{
			cmp = _mm_cmpgt_ps(oOrigin.m_sseX, _mm_set_ps1(m_oRightPoint.GetX()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmpgt_ps(oOrigin.m_sseY, _mm_set_ps1(m_oRightPoint.GetY()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;
			cmp = _mm_cmpgt_ps(oOrigin.m_sseZ, _mm_set_ps1(m_oRightPoint.fGetZ()));
			if (_mm_movemask_ps(cmp) == 0xF)
				return;

			c2[0] = _mm_mul_ps(Direction.m_sseX, left.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseY, left.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseX, left.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, right.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, right.m_sseY);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, right.m_sseX);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmpgt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xF)
					return;
			}
			c2[0] = _mm_mul_ps(Direction.m_sseX, right.m_sseY);
			c2[1] = _mm_mul_ps(Direction.m_sseX, right.m_sseZ);
			c2[2] = _mm_mul_ps(Direction.m_sseY, right.m_sseZ);
			d2[0] = _mm_mul_ps(Direction.m_sseY, left.m_sseX);
			d2[1] = _mm_mul_ps(Direction.m_sseZ, left.m_sseX);
			d2[2] = _mm_mul_ps(Direction.m_sseZ, left.m_sseY);

			for (int i = 0; i < 3; ++i)
			{
				sub = _mm_sub_ps(c2[i], d2[i]);
				cmp = _mm_cmplt_ps(sub, zero);
				if (_mm_movemask_ps(cmp) == 0xF)
					return;
			}
		}break;
		default:
		{
		};
		}
		oAnswer.m_sseData = _mm_set_ps1(1.0f);
	}

	template <class T>
	inline void Aabb<T>::DoesIntersectIndividual(Vector128& oAnswer, const PacketVector& oOrigin,
		const PacketVector& Direction, unsigned int uiMask[])
	{
		RAY_ORIENTATION eOrientation[4];
		eOrientation[0] = RAY_ORIENTATION((uiMask[0] & 0x8) + ((uiMask[1] & 0x8) >> 1) +
			((uiMask[2] & 0x8) >> 2));
		eOrientation[1] = RAY_ORIENTATION((uiMask[0] & 0x4) + ((uiMask[1] & 0x4) >> 1) +
			((uiMask[2] & 0x4) >> 2));
		eOrientation[2] = RAY_ORIENTATION((uiMask[0] & 0x2) + ((uiMask[1] & 0x2) >> 1) +
			((uiMask[2] & 0x2) >> 2));
		eOrientation[3] = RAY_ORIENTATION((uiMask[0] & 0x1) + ((uiMask[1] & 0x1) >> 1) +
			((uiMask[3] & 0x1) >> 2));

		Vector vecOrigins[4], vecDirections[4];
		for (unsigned int ui = 0; ui < 4; ++ui)
		{
			vecOrigins[ui].Set(oOrigin.m_fX[ui], oOrigin.m_fY[ui], oOrigin.m_fZ[ui]);
			vecDirections[ui].Set(Direction.m_fX[ui], Direction.m_fY[ui], Direction.m_fZ[ui]);
			oAnswer.m_fData[ui] = float(Intersects(vecOrigins[ui], vecDirections[ui], eOrientation[ui]));
		}
	}

	/*
	Plucker Coordinates for a line L passing through points A and B:
	L0 = AxBy - BxAy
	L1 = AxBz - BxAz
	L2 = Ax - Bx
	L3 = AyBz - ByAz
	L4 = Az - Bz
	L5 = By - Ay

	So a Ray R would be expressed as Origin O + Direction D <Di, Dj, Dk>:
	R0 = OxDj - DiOy
	R1 = OxDk - DiOz
	R2 = -Di
	R3 = OyDk - DjOz
	R4 = -Dk
	R5 = Dj

	The surface normal N or the plane formed of the points A,B, and Origin O is:
	N = (A-O)X(B-O)

	Thus the function side(R,L) = -D.((A-O)X(B-O))
	(... some ugly math later...)
	side(R,L) = -DiL3 + DjL1 - DkL0 + R1L5 + R0L4 + R3L2
	side(O,D,A,B) = -Di(AyBZ-ByAz) + Dj(AxBz-BxAz) - Dk(AxBy-BxAy) + (OxDk-DiOz)(By-Ay) +
	(OxDj-DiOy)(Az-Bz) + (OyDk-DjOz)(Ax-Bx)
	*/
	template <class T>
	inline bool Aabb<T>::Intersects(const Math::Vector& oOrigin, const Math::Vector& Direction)
	{
		Math::Vector128 signs;
		//Compare the direction of the ray, see which are in the positive direction
		signs.m_sseData = _mm_cmp_ge(Direction.GetVector(), _mm_set_ps1(0.0f));
		//Since the ray is in 3-space, set the fourth value to 0
		signs.m_fData[3] = 0.0f;
		//The ray orientation can be determined using the move_mask command on the bit-mask created
		//by determining the orientation of the ray:
		//signs before move-mask: 0xFFFFFFFF||0x00000000||0xFFFFFFFF||0x00000000
		//signs after move-mask:  0x00000000||0x00000000||0x00000000||0x00000006
		return Intersects(oOrigin, Direction, RAY_ORIENTATION(_mm_movemask_ps(signs.m_sseData)));
	}

	template <class T>
	inline bool Aabb<T>::Intersects(const Vector& oOrigin, const Vector& Direction, RAY_ORIENTATION eOrientation)
	{
		switch (eOrientation)
		{
		case MMM:
		{
			Math::Vector128 ans;
			//If the origin is less than the left point of the box, it can't possibly intersect
			ans.m_sseData = _mm_cmplt_ps(oOrigin.GetVector(), m_oLeftPoint.GetVector());
			ans.m_fData[4] = 0x0;
			if (_mm_movemask_ps(ans.m_sseData))
				return false;

			Vector a, b;
			a.Sub(m_oLeftPoint, oOrigin);
			b.Sub(m_oRightPoint, oOrigin);

			if (Direction.GetX()*a.GetY() - Direction.GetY()*b.GetX() < 0)
				return false;
			if (Direction.GetX()*a.fGetZ() - Direction.fGetZ()*b.GetX() < 0)
				return false;
			if (Direction.GetY()*a.fGetZ() - Direction.fGetZ()*b.GetY() < 0)
				return false;

			if (Direction.GetY()*a.GetX() - Direction.GetX()*b.GetY() <= 0)
				return false;
			if (Direction.fGetZ()*a.GetX() - Direction.GetX()*b.fGetZ() <= 0)
				return false;
			if (Direction.fGetZ()*a.GetY() - Direction.GetY()*b.fGetZ() <= 0)
				return false;
		}break;
		case MMP:
		{
			/*
			if ((r->x < b->x0) || (r->y < b->y0) || (r->z > b->z1))
			return false;
			*/
			if (oOrigin.GetX() < m_oLeftPoint.GetX())
				return false;
			if (oOrigin.GetY() < m_oLeftPoint.GetY())
				return false;
			if (oOrigin.fGetZ() > m_oRightPoint.fGetZ())
				return false;
			/*
			float xa = b->x0 - r->x;
			float ya = b->y0 - r->y;
			float za = b->z0 - r->z;
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;
			*/
			Math::Vector128 a, b;
			a.m_sseData = _mm_sub_ps(m_oLeftPoint.GetVector(), oOrigin.GetVector());
			b.m_sseData = _mm_sub_ps(m_oRightPoint.GetVector(), oOrigin.GetVector());
			/*
				   c    d      e     f
				   if(	(r->i * ya - r->j * xb < 0) ||
				   (r->i * za - r->k * xa < 0) ||
				   (r->j * za - r->k * ya < 0) ||
				   */
			Math::Vector128 c, d, e, f;

			c.m_sseData = _mm_set_ps1(Direction.GetX());
			c.m_fData[2] = Direction.GetY();
			d.m_sseData = _mm_set_ps1(a.m_fData[2]);
			d.m_fData[0] = a.m_fData[1];
			e.m_sseData = _mm_set_ps1(Direction.fGetZ());
			e.m_fData[0] = Direction.GetY();
			f.m_fData[0] = b.m_fData[0];
			f.m_fData[1] = a.m_fData[0];
			f.m_fData[2] = a.m_fData[1];

			c.m_sseData = _mm_mul_ps(c.m_sseData, d.m_sseData);
			d.m_sseData = _mm_mul_ps(e.m_sseData, f.m_sseData);
			e.m_sseData = _mm_sub_ps(c.m_sseData, d.m_sseData);
			f.m_sseData = _mm_cmplt_ps(e.m_sseData, _mm_set_ps1(0.0f));
			f.m_fData[3] = 0.0f;
			if (_mm_movemask_ps(f.m_sseData))
				return false;
			/*
				  c     d     e     f
				  (r->i * yb - r->j * xa > 0) ||
				  (r->i * zb - r->k * xb > 0) ||
				  (r->j * zb - r->k * yb > 0))
				  return false;

				  return true;*/
			c.m_sseData = _mm_set_ps1(Direction.GetX());
			c.m_fData[2] = Direction.GetY();
			d.m_sseData = _mm_set_ps1(b.m_fData[2]);
			d.m_fData[0] = b.m_fData[1];
			e.m_sseData = _mm_set_ps1(Direction.fGetZ());
			e.m_fData[0] = Direction.GetY();
			f.m_fData[0] = a.m_fData[0];
			f.m_fData[1] = b.m_fData[0];
			f.m_fData[2] = b.m_fData[1];
			c.m_sseData = _mm_mul_ps(c.m_sseData, d.m_sseData);
			d.m_sseData = _mm_mul_ps(e.m_sseData, f.m_sseData);
			e.m_sseData = _mm_sub_ps(c.m_sseData, d.m_sseData);
			f.m_sseData = _mm_cmpgt_ps(e.m_sseData, _mm_set_ps1(0.0f));
			f.m_fData[3] = 0.0f;
			if (_mm_movemask_ps(f.m_sseData))
				return false;
		}break;
		case MPM:
		{
			/*
			if ((r->x < b->x0) || (r->y > b->y1) || (r->z < b->z0))
			return false;
			*/
			if (oOrigin.GetX() < m_oLeftPoint.GetX())
				return false;
			if (oOrigin.GetY() > m_oRightPoint.GetY())
				return false;
			if (oOrigin.fGetZ() < m_oLeftPoint.fGetZ())
				return false;
			/*
		float xa = b->x0 - r->x;
		float ya = b->y0 - r->y;
		float za = b->z0 - r->z;
		float xb = b->x1 - r->x;
		float yb = b->y1 - r->y;
		float zb = b->z1 - r->z;
		*/
			Math::Vector128 a, b;
			a.m_sseData = _mm_sub_ps(m_oLeftPoint.GetVector(), oOrigin.GetVector());
			b.m_sseData = _mm_sub_ps(m_oRightPoint.GetVector(), oOrigin.GetVector());
			/*
			   c     d     e     f
			   if(	(r->i * ya - r->j * xa < 0) ||
			   (r->i * za - r->k * xb < 0) ||
			   (r->j * zb - r->k * yb < 0) ||
			   */
			Math::Vector128 c, d, e, f;
			c.m_sseData = _mm_set_ps1(Direction.GetX());
			c.m_fData[2] = Direction.GetY();
			d.m_fData[0] = a.m_fData[1];
			d.m_fData[1] = a.m_fData[2];
			d.m_fData[2] = b.m_fData[2];
			e.m_sseData = _mm_set_ps1(Direction.fGetZ());
			e.m_fData[0] = Direction.GetY();
			f.m_fData[0] = a.m_fData[0];
			f.m_fData[1] = b.m_fData[0];
			f.m_fData[2] = b.m_fData[1];
			c.m_sseData = _mm_mul_ps(c.m_sseData, d.m_sseData);
			d.m_sseData = _mm_mul_ps(e.m_sseData, f.m_sseData);
			e.m_sseData = _mm_sub_ps(c.m_sseData, d.m_sseData);
			f.m_sseData = _mm_cmplt_ps(e.m_sseData, _mm_set_ps1(0.0f));
			f.m_fData[3] = 0.0f;
			if (_mm_movemask_ps(f.m_sseData))
				return false;
			/*
			  c     d     e      f
			  (r->i * yb - r->j * xb > 0) ||
			  (r->i * zb - r->k * xa > 0) ||
			  (r->j * za - r->k * ya > 0))
			  return false;

			  return true;*/
			c.m_sseData = _mm_set_ps1(Direction.GetX());
			c.m_fData[2] = Direction.GetY();
			d.m_fData[0] = b.m_fData[1];
			d.m_fData[1] = b.m_fData[2];
			d.m_fData[2] = a.m_fData[2];
			e.m_sseData = _mm_set_ps1(Direction.fGetZ());
			e.m_fData[0] = Direction.GetY();
			f.m_fData[0] = b.m_fData[0];
			f.m_fData[1] = a.m_fData[0];
			f.m_fData[2] = a.m_fData[1];
			c.m_sseData = _mm_mul_ps(c.m_sseData, e.m_sseData);
			d.m_sseData = _mm_mul_ps(e.m_sseData, f.m_sseData);
			e.m_sseData = _mm_sub_ps(c.m_sseData, d.m_sseData);
			f.m_sseData = _mm_cmpgt_ps(e.m_sseData, _mm_set_ps1(0.0f));
			f.m_fData[3] = 0.0f;
			if (_mm_movemask_ps(f.m_sseData))
				return false;
		}break;
		case MPP:
		{
			/*
			if ((r->x < b->x0) || (r->y > b->y1) || (r->z > b->z1))
			return false;*/
			if (oOrigin.GetX() < m_oLeftPoint.GetX())
				return false;
			if (oOrigin.GetY() > m_oRightPoint.GetY())
				return false;
			if (oOrigin.fGetZ() > m_oRightPoint.fGetZ())
				return false;
			/*
						float xa = b->x0 - r->x;
						float ya = b->y0 - r->y;
						float za = b->z0 - r->z;
						float xb = b->x1 - r->x;
						float yb = b->y1 - r->y;
						float zb = b->z1 - r->z;
						*/
			Vector a, b;
			a.Sub(m_oLeftPoint, oOrigin);
			b.Sub(m_oRightPoint, oOrigin);
			/*
			  c     d     e     f
			  if(	(r->i * ya - r->j * xa < 0) ||
			  (r->i * za - r->k * xa < 0) ||
			  (r->j * zb - r->k * ya < 0) ||

			  (r->i * yb - r->j * xb > 0) ||
			  (r->i * zb - r->k * xb > 0) ||
			  (r->j * za - r->k * yb > 0))
			  return false;
			  return true;*/
			if (Direction.GetX()*a.GetY() - Direction.GetY()*a.GetX() < 0)
				return false;
			if (Direction.GetX()*a.fGetZ() - Direction.fGetZ()*a.GetX() < 0)
				return false;
			if (Direction.GetY()*b.fGetZ() - Direction.fGetZ()*a.GetY() < 0)
				return false;

			if (Direction.GetX()*b.GetY() - Direction.GetY()*b.GetX() > 0)
				return false;
			if (Direction.GetX()*b.fGetZ() - Direction.fGetZ()*b.GetX() > 0)
				return false;
			if (Direction.GetY()*a.fGetZ() - Direction.fGetZ()*b.GetY() > 0)
				return false;
		}break;
		case PMM:
		{
			/*
			if ((r->x > b->x1) || (r->y < b->y0) || (r->z < b->z0))
			return false;
			*/
			if (oOrigin.GetX() > m_oRightPoint.GetX())
				return false;
			if (oOrigin.GetY() < m_oLeftPoint.GetY())
				return false;
			if (oOrigin.fGetZ() < m_oLeftPoint.fGetZ())
				return false;
			/*
		float xa = b->x0 - r->x;
		float ya = b->y0 - r->y;
		float za = b->z0 - r->z;
		float xb = b->x1 - r->x;
		float yb = b->y1 - r->y;
		float zb = b->z1 - r->z;
		*/
			Vector a;
			a.Sub(m_oLeftPoint, oOrigin);
			Vector b;
			b.Sub(m_oRightPoint, oOrigin);
			/*
		if(	(r->i * yb - r->j * xb < 0) ||
		(r->i * ya - r->j * xa > 0) ||
		(r->i * za - r->k * xa > 0) ||

		(r->i * zb - r->k * xb < 0) ||
		(r->j * za - r->k * yb < 0) ||
		(r->j * zb - r->k * ya > 0))
		return false;

		return true;*/

			if (Direction.GetX()*b.GetY() - Direction.GetY()*b.GetX() < 0)
				return false;
			if (Direction.GetX()*a.GetY() - Direction.GetY()*a.GetX() > 0)
				return false;
			if (Direction.GetX()*a.fGetZ() - Direction.fGetZ()*a.GetX() > 0)
				return false;

			if (Direction.GetX()*b.fGetZ() - Direction.fGetZ()*b.GetX() < 0)
				return false;
			if (Direction.GetY()*a.fGetZ() - Direction.fGetZ()*b.GetY() < 0)
				return false;
			if (Direction.GetY()*b.fGetZ() - Direction.fGetZ()*a.GetY() > 0)
				return false;
		}break;
		case PMP:
		{
			if (oOrigin.GetX() > m_oRightPoint.GetX())
				return false;
			if (oOrigin.GetY() < m_oLeftPoint.GetY())
				return false;
			if (oOrigin.fGetZ() > m_oRightPoint.fGetZ())
				return false;

			Vector a;
			a.Sub(m_oLeftPoint, oOrigin);
			Vector b;
			b.Sub(m_oRightPoint, oOrigin);

			if (Direction.GetX()*b.GetY() - Direction.GetY()*b.GetX() < 0)
				return false;
			if (Direction.GetX()*a.GetY() - Direction.GetY()*a.GetX() > 0)
				return false;
			if (Direction.GetX()*a.fGetZ() - Direction.fGetZ()*b.GetX() > 0)
				return false;

			if (Direction.GetX()*b.fGetZ() - Direction.fGetZ()*a.GetX() < 0)
				return false;
			if (Direction.GetY()*a.fGetZ() - Direction.fGetZ()*a.GetY() < 0)
				return false;
			if (Direction.GetY()*b.fGetZ() - Direction.fGetZ()*b.GetY() > 0)
				return false;
		}break;
		case PPM:
		{
			/*
			if ((r->x > b->x1) || (r->y > b->y1) || (r->z < b->z0))
			return false;

			float xa = b->x0 - r->x;
			float ya = b->y0 - r->y;
			float za = b->z0 - r->z;
			float xb = b->x1 - r->x;
			float yb = b->y1 - r->y;
			float zb = b->z1 - r->z;

			if(	(r->i * yb - r->j * xa < 0) ||
			(r->i * ya - r->j * xb > 0) ||
			(r->i * za - r->k * xa > 0) ||

			(r->i * zb - r->k * xb < 0) ||
			(r->j * zb - r->k * yb < 0) ||
			(r->j * za - r->k * ya > 0))
			return false;*/
			if (oOrigin.GetX() > m_oRightPoint.GetX())
				return false;
			if (oOrigin.GetY() > m_oRightPoint.GetY())
				return false;
			if (oOrigin.fGetZ() < m_oLeftPoint.fGetZ())
				return false;

			Vector a;
			a.Sub(m_oLeftPoint, oOrigin);
			Vector b;
			b.Sub(m_oRightPoint, oOrigin);

			if (Direction.GetX()*b.GetY() - Direction.GetY()*a.GetX() < 0)
				return false;
			if (Direction.GetX()*a.GetY() - Direction.GetY()*b.GetX() > 0)
				return false;
			if (Direction.GetX()*a.fGetZ() - Direction.fGetZ()*a.GetX() > 0)
				return false;

			if (Direction.GetX()*b.fGetZ() - Direction.fGetZ()*b.GetX() < 0)
				return false;
			if (Direction.GetY()*b.fGetZ() - Direction.fGetZ()*b.GetY() < 0)
				return false;
			if (Direction.GetY()*a.fGetZ() - Direction.fGetZ()*a.GetY() > 0)
				return false;
		}break;
		case PPP:
		{
			//if(oOrigin>m_oRightPoint)
			Math::Vector128 ans;
			ans.m_sseData = _mm_cmpgt_ps(oOrigin.GetVector(), m_oRightPoint.GetVector());
			if (_mm_movemask_ps(ans.m_sseData))
				return false;

			Vector a;
			a.Sub(m_oLeftPoint, oOrigin);
			Vector b;
			b.Sub(m_oRightPoint, oOrigin);

			if (Direction.GetX()*a.GetY() - Direction.GetY()*b.GetX() > 0)
				return false;
			if (Direction.GetY()*a.fGetZ() - Direction.fGetZ()*b.GetY() > 0)
				return false;
			if (Direction.GetX()*a.fGetZ() - Direction.fGetZ()*b.GetX() > 0)
				return false;

			if (Direction.GetX()*b.GetY() - Direction.GetY()*a.GetX() < 0)
				return false;
			if (Direction.GetX()*b.fGetZ() - Direction.fGetZ()*a.GetX() < 0)
				return false;
			if (Direction.GetY()*b.fGetZ() - Direction.fGetZ()*a.GetY() < 0)
				return false;

		}break;
		default:
		{
			return false;
		}break;
		}
		return true;
	}
}

#endif