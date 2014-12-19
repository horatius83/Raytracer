#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <fstream>
#include "HtmlError.h"
#include "TArray.h"
#include "Array2D.h"
#include "Color.h"

namespace Graphics
{	
	class Texture
	{
	public:
		Texture(){};
		~Texture()
		{
			m_oData.Deallocate();
		};
		bool bCanLoadImage(const std::wstring& sFilename);
		inline void DeleteImage()
		{
			m_oData.Deallocate();
		}
		bool bCanSet(Utility::Array2D< Graphics::Color<unsigned char> >& oData);
		bool bCanCopy(Utility::Array2D< Graphics::Color<unsigned char> >& oData);

		inline unsigned int uiGetWidth(){return m_oData.uiGetWidth();};
		inline unsigned int uiGetHeight(){return m_oData.uiGetHeight();};
		inline unsigned int uiGetBpp(){return 32;};
	private:
		Utility::Array2D< Graphics::Color<unsigned char> > m_oData;
	};

	bool Texture::bCanLoadImage(const std::wstring &sFilename)
	{
		return true;	
	}

	//This transfers a pointer to an array from oData to m_oData, oData will now be invalid
	inline bool Texture::bCanSet(Utility::Array2D< Graphics::Color<unsigned char> > &oData)
	{
		m_oData.Set(oData);
	}

	//This copies the content of oData to m_oData
	inline bool Texture::bCanCopy(Utility::Array2D< Graphics::Color<unsigned char> > &oData)
	{
		m_oData.Copy(oData);
	}
}

#endif