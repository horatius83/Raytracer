#ifndef TCOLOR_H
#define TCOLOR_H

namespace NGraphics
{
	template <class T>
	class TColor
	{
	public:
		TColor() : m_fRed(0.0f),m_fGreen(0.0f),m_fBlue(0.0f),m_fAlpha(0.0f) {};
		TColor(const TColor<T>& tColor)
		{
			Set(tColor);
		};
		TColor(const T& red,const T& green,const T& blue)
		{
			Set(red,green,blue);
		}
		TColor(const T& red,const T& green,const T& blue,const T& alpha)
		{
			Set(red,green,blue,alpha);
		}
		inline T& GetRed()
		{
			return m_fRed;
		}
		inline T& GetGreen()
		{
			return m_fGreen;
		};
		inline T& GetBlue()
		{
			return m_fBlue;
		}
		inline T& GetAlpha()
		{
			return m_fAlpha;
		}
	
		inline void SetRed(const T& t)
		{
			m_fRed = t;
		}
		inline void SetGreen(const T& t)
		{
			m_fGreen = t;
		}
		inline void SetBlue(const T& t)
		{
			m_fBlue = t;
		}
		inline void SetAlpha(const T& t)
		{
			m_fAlpha = t;
		}
		inline void Set(const TColor<T>& tColor)
		{
			m_fRed = tColor.m_fRed;
			m_fGreen = tColor.m_fGreen;
			m_fBlue = tColor.m_fBlue;
			m_fAlpha = tColor.m_fAlpha;
		}
		inline void Set(const T& red,const T& green,const T& blue)
		{
			SetRed(red);
			SetGreen(green);
			SetBlue(blue);
		}
		inline void Set(const T& tRed,const T& tGreen,const T& tBlue,const T& tAlpha)
		{
			SetRed(tRed);
			SetGreen(tGreen);
			SetBlue(tBlue);
			SetAlpha(tAlpha);
		}
		inline void Mul(const T& tScalar,const TColor<T>& tColor)
		{
			m_fRed = tColor.m_fRed * tScalar;
			m_fGreen = tColor.m_fGreen * tScalar;
			m_fBlue = tColor.m_fBlue * tScalar;
			/*SetRed(tColor.m_fRed*tScalar);
			SetGreen(tColor.m_fGreen*tScalar);
			SetBlue(tColor.m_fBlue*tScalar);*/
		}
	private:
		float m_fRed;
		float m_fGreen;
		float m_fBlue;
		float m_fAlpha;
	};
}

#endif