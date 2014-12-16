#ifndef TALIGNEDARRAY_H
#define TALIGNEDARRAY_H

#include <xmmintrin.h>

namespace NUtility
{
	template <class T>
	class TAlignedArray
	{
	public:
		TAlignedArray() : m_pArray(0),m_uiSize(0) {};
		TAlignedArray(unsigned int uiSize) : m_pArray(0), m_uiSize(0)
		{
			if(uiSize)
				Allocate(uiSize);
		}
		TAlignedArray(TAlignedArray<T>& tArray) : m_pArray(0), m_uiSize(0)
		{
			Allocate(uiSize);
			for(unsigned int ui=0;ui<m_uiSize;ui++)
				m_pArray[ui] = tArray[ui];
		}
		~TAlignedArray(){Deallocate();};

		inline void Set(TAlignedArray<T>& tArray)
		{
			Deallocate();
			m_pArray = tArray.m_pArray;
			m_uiSize = tArray.m_uiSize;
			tArray.Dereference();
		}

		inline void Copy(TAlignedArray<T>& tArray)
		{
			if(tArray.uiGetSize()!=uiGetSize())
			{
				Deallocate();
				Allocate(tArray.uiGetSize());
			}
			for(unsigned int ui=0;ui<tArray.uiGetSize();++ui)
				m_pArray[ui]=tArray[ui];
		}

		inline void Dereference()
		{
			m_pArray=0;
			m_uiSize=0;
		}
		inline void Allocate(unsigned int uiSize)
		{
			if(uiSize)
			{
				Deallocate();
				m_uiSize = uiSize;
				m_pArray = (T*)_aligned_malloc(uiSize*sizeof(T),16);
			}
		}
		//This is unsafe, but I want this to be as fast as possible
		inline T& operator[](const unsigned int& ui)
		{
			return m_pArray[ui];
		}
		inline unsigned int uiGetSize()	const {return m_uiSize;};
		inline T*			pGetArray(){return m_pArray;};
		inline void Deallocate()
		{
			if(m_uiSize)
			{
				_aligned_free(m_pArray);
				m_pArray = 0;
				m_uiSize = 0;
			}
		}
	private:
		T*				m_pArray;
		unsigned int	m_uiSize;
	};
}

#endif