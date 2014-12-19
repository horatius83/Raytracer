#ifndef ARRAY2D_H
#define ARRAY2D_H

namespace Utility
{
	template <class T>
	class Array2D
	{
	public:
		Array2D() : m_pArray(0), m_uiSize(0), m_uiWidth(0) {};
		Array2D(unsigned int uiWidth, unsigned int uiHeight)
		{
			Allocate(uiWidth, uiHeight);
		}
		Array2D(Array2D<T>& tArray) : m_pArray(0), m_uiSize(0), m_uiWidth(0)
		{
			Allocate(tArray.GetWidth(), tArray.GetHeight());
			for (unsigned int ui = 0; ui < m_uiSize; ui++)
				m_pArray[ui] = tArray[ui];
		}
		~Array2D(){ Deallocate(); };

		inline void Set(Array2D<T>& tArray)
		{
			Deallocate();
			m_pArray = tArray.m_pArray;
			m_uiSize = tArray.m_uiSize;
			m_uiWidth = tArray.m_uiWidth;
			tArray.Dereference();
		}

		inline void Set(T* pArray, unsigned int uiWidth, unsigned int uiSize)
		{
#ifdef _DEBUG
			if (!pArray)
				throw "Array2D::Set(T* pArray,unsigned int uiWidth,unsigned int uiSize): invalid array";

			if (uiSize < 1 || uiWidth < 1)
				throw "Array2D::Set(T* pArray,unsigned int uiWidth,unsigned int uiSize): invalid array size";
#endif
			m_pArray = pArray;
			m_uiWidth = uiWidth;
			m_uiSize = uiSize;
		}

		inline void Copy(Array2D<T>& tArray)
		{
			if (tArray.uiGetSize() != uiGetSize())
			{
				Deallocate();
				Allocate(tArray.GetWidth(), tArray.GetHeight());
				m_uiSize = tArray.uiGetSize();
			}
			for (unsigned int ui = 0; ui < tArray.uiGetSize(); ++ui)
				m_pArray[ui] = tArray[ui];
		}
		inline void Dereference()
		{
			m_pArray = 0;
			m_uiSize = 0;
			m_uiWidth = 0;
		}

		inline void Allocate(unsigned int uiWidth, unsigned int uiHeight)
		{
			if (uiWidth*uiHeight)
			{
				Deallocate();
				m_uiSize = uiWidth*uiHeight;
				m_uiWidth = uiWidth;
				m_pArray = new T[m_uiSize];
			}
		}
		//This is unsafe, but I want this to be as fast as possible
		inline T& operator[](const unsigned int& ui)
		{
			return m_pArray[ui];
		}
		inline unsigned int uiGetSize(){ return m_uiSize; };
		inline unsigned int GetHeight()
		{
			return (m_uiWidth) ? m_uiSize / m_uiWidth : 0;
		};
		inline unsigned int GetWidth(){ return m_uiWidth; };
		inline T*			GetArray(){ return m_pArray; };
		inline void Deallocate()
		{
			if (m_pArray)
			{
				delete[] m_pArray;
				//_mm_free(m_pArray);
				m_pArray = 0;
				m_uiSize = 0;
				m_uiWidth = 0;
			}
		}
	private:
		T*				m_pArray;
		unsigned int	m_uiSize;
		unsigned int	m_uiWidth;
	};
}

#endif