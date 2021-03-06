#ifndef VECTOR3_H
#define VECTOR3_H

namespace Math
{
	template <class T>
	class Vector3
	{
	public:
		Vector3() : m_fX(0),m_fY(0),m_fZ(0),m_fJunk(0) {};
		Vector3(T tx,T ty,T tz,T tJunk=0) : m_fX(tx),m_fY(ty),m_fZ(tz), m_fJunk(tJunk) {};
		Vector3(const Vector3<T>& tVec) {Set(tVec);};
	
		inline T& GetX(){return m_tData[0];};
		inline T& GetY(){return m_tData[1];};
		inline T& GetZ(){return m_tData[2];};
		inline T& GetJunk() const {return m_tData[3];};
	
		inline void SetX(const T& tX){m_tData[0]=tX;};
		inline void SetY(const T& tY){m_tData[1]=tY;};
		inline void SetZ(const T& tZ){m_tData[2]=tZ;};
		inline void SetJunk(const T& tJunk){m_tData[3]=tJunk;};
		inline void Set(const T& tX,const T& tY,const T& tZ,const T& tJunk)
		{
			m_tData[0] = tX;
			m_tData[1] = tY;
			m_tData[2] = tZ;
			m_tData[3] = tJunk;
		}
		inline void Set(const Math::Vector3<T>& tVector)
		{
			m_tData[0] = tVector.GetX();
			m_tData[1] = tVector.GetY();
			m_tData[2] = tVector.GetZ();
			m_tData[3] = tVector.GetJunk();
		}

		inline T GetDotProduct(const Vector3<T>& tVector) const
		{
			return return m_tData[0]*tVector.tGetX()+m_tData[1]*tVector.tGetY()+
				m_tData[2]*tVector.tGetZ();
		}
		inline T GetMagnitudeSquared() const
		{
			return m_tData[0]*m_tData[0]+m_tData[1]*m_tData[1]+m_tData[2]*m_tData[2];
		}
		inline T GetDistanceSquared(const Vector3<T>& tVector) const
		{
			Vector3<T> tVector = tVector - *this;
			return tVector.tGetDotProduct(tVector);
		}
		inline void SetAsCrossProduct(const Vector3<T>& tVectorA,const Vector3<T>& tVectorB)
		{
			m_tData[0]=tVectorA.tGetY()*tVectorB.tGetZ()-tVectorA.tGetZ()*tVectorB.tGetY();
			m_tData[1]=tVectorA.tGetZ()*tVectorB.tGetX()-tVectorA.tGetX()*tVectorB.tGetZ();
			m_tData[2]=tVectorA.tGetX()*tVectorB.tGetY()-tVectorA.tGetY()*tVectorB.tGetX();
			m_TData[3]=0;
		}
		
		friend Vector3<T> operator *(const Vector3<T>& tVectorA,const Vector3<T>& tVectorB)
		{
			Vector3<T> tVector;
			tVector.SetX(tVectorA.tGetX()*tVectorB.tGetX());
			tVector.SetY(tVectorA.tGetY()*tVectorB.tGetY());
			tVector.SetZ(tVectorA.tGetZ()*tVectorB.tGetZ());
			return tVector;
		}
		friend Vector3<T> operator *(const T& t,const Vector3<T>& tVector)
		{
			Vector3<T> tVector(tVector);
			tVector.SetX(tVector.tGetX()*t);
			tVector.SetY(tVector.tGetY()*t);
			tVector.SetZ(tVector.tGetZ()*t);
			return tVector;
		}
		friend Vector3<T> operator *(const Vector3<T>& tVector,const T& t)
		{
			return t*tVector;
		}
		friend Vector3<T> operator +(const Vector3<T>& tVectorA,const Vector3<T>& tVectorB)
		{
			Vector3<T> tVector;
			tVector.SetX(tVectorA.tGetX()+tVectorB.tGetX());
			tVector.SetY(tVectorA.tGetY()+tVectorB.tGetY());
			tVector.SetZ(tVectorA.tGetZ()+tVectorB.tGetZ());
			return tVector;
		}
		friend Vector3<T> operator -(const Vector3<T>& tVectorA,const Vector3<T>& tVectorB)
		{
			Vector3<T> tVector;
			tVector.SetX(tVectorA.tGetX()-tVectorB.tGetX());
			tVector.SetY(tVectorA.tGetY()-tVectorB.tGetY());
			tVector.SetZ(tVectorA.tGetZ()-tVectorB.tGetZ());
			return tVector;
		}
	
		void operator +=(const Math::Vector3<T>& tVector)
		{
			m_tData[0]+=tVector.tGetX();
			m_tData[1]+=tVector.tGetY();
			m_tData[2]+=tVector.tGetZ();
		}
		void operator -=(const Math::Vector3<T>& tVector)
		{
			m_tData[0]-=tVector.tGetX();
			m_tData[1]-=tVector.tGetY();
			m_tData[2]-=tVector.tGetZ();
		}
		void operator *=(const T& t)
		{
			m_tData[0]*=t;
			m_tData[1]*=t;
			m_tData[2]*=t;
		}
		void operator *=(const Math::Vector3<T>& tVector)
		{
			m_tData[0]*=tVector.tGetX();
			m_tData[1]*=tVector.tGetY();
			m_tData[2]*=tVector.tGetZ();
		}
		void operator !=(const Math::Vector3<T>& tVector)
		{
			if(m_tData[0]!=tVector.tGetX())
				return true;
			if(m_tData[1]!=tVector.tGetY())
				return true;
			if(m_tData[2]!=tVector.tGetZ())
				return true;
			return false;
		}
		void operator ==(const Math::Vector3<T>& tVector)
		{
			if(m_tData[0]==tVector.tGetX())
			{
				if(m_tData[1]==tVector.tGetY())
				{
					if(m_tData[2]==tVector.tGetZ())
						return true;
				}
			}
			return false;
		}
	private:
		union
		{
			T	m_tData[4];
			struct
			{
				float m_fX;
				float m_fY;
				float m_fZ;
				float m_fJunk;
			};
		};
	};	
}
#endif