#ifndef CTEXTURE_H
#define CTEXTURE_H

#include <string>
#include <fstream>
#include "CHtmlError.h"
#include "TArray.h"
#include "TArray2D.h"
#include "TColor.h"

namespace NGraphics
{	
	class CTexture
	{
	public:
		CTexture(){};
		~CTexture()
		{
			m_oData.Deallocate();
		};
		bool bCanLoadImage(const std::wstring& sFilename);
		inline void DeleteImage()
		{
			m_oData.Deallocate();
		}
		bool bCanSet(NUtility::TArray2D< NGraphics::TColor<unsigned char> >& oData);
		bool bCanCopy(NUtility::TArray2D< NGraphics::TColor<unsigned char> >& oData);

		inline unsigned int uiGetWidth(){return m_oData.uiGetWidth();};
		inline unsigned int uiGetHeight(){return m_oData.uiGetHeight();};
		inline unsigned int uiGetBpp(){return 32;};
	private:
		NUtility::TArray2D< NGraphics::TColor<unsigned char> > m_oData;
	};

	bool CTexture::bCanLoadImage(const std::wstring &sFilename)
	{
		return true;	
	}

	//This transfers a pointer to an array from oData to m_oData, oData will now be invalid
	inline bool CTexture::bCanSet(NUtility::TArray2D< NGraphics::TColor<unsigned char> > &oData)
	{
		m_oData.Set(oData);
	}

	//This copies the content of oData to m_oData
	inline bool CTexture::bCanCopy(NUtility::TArray2D< NGraphics::TColor<unsigned char> > &oData)
	{
		m_oData.Copy(oData);
	}
}

#endif