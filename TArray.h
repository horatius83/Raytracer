//Created 07172006 
//Modified 12272006: Added to namespace "NUtility"
#ifndef TARRAY_H
#define TARRAY_H

namespace NUtility
{
	template <class T>
	class TArray
	{
	public:
		TArray() : m_pArray(0),m_uiSize(0) {};
		TArray(unsigned int uiSize) : m_pArray(0), m_uiSize(0)
		{
			if(uiSize)
				Allocate(uiSize);
		}
		TArray(TArray<T>& tArray) : m_pArray(0), m_uiSize(0)
		{
			Allocate(uiSize);
			for(unsigned int ui=0;ui<m_uiSize;ui++)
				m_pArray[ui] = tArray[ui];
		}
		~TArray(){Deallocate();};

		inline void Set(TArray<T>& tArray)
		{
			Deallocate();
			m_pArray = tArray.m_pArray;
			m_uiSize = tArray.m_uiSize;
			tArray.Dereference();
		}

		inline void Copy(TArray<T>& tArray)
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
				//m_pArray = (T*)_mm_malloc(uiSize,sizeof(T));
				m_pArray = new T[uiSize];
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
			if(m_pArray)
			{
				delete [] m_pArray;
				//_mm_free(m_pArray);
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