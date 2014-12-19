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
		bool CanLoadImage(const std::wstring& sFilename);
		inline void DeleteImage()
		{
			m_oData.Deallocate();
		}
		bool CanSet(Utility::Array2D< Graphics::Color<unsigned char> >& oData);
		bool CanCopy(Utility::Array2D< Graphics::Color<unsigned char> >& oData);

		inline unsigned int GetWidth(){return m_oData.GetWidth();};
		inline unsigned int GetHeight(){return m_oData.GetHeight();};
		inline unsigned int GetBpp(){return 32;};
	private:
		Utility::Array2D< Graphics::Color<unsigned char> > m_oData;
	};

	bool Texture::CanLoadImage(const std::wstring &sFilename)
	{
		return true;	
	}

	//This transfers a pointer to an array from oData to m_oData, oData will now be invalid
	inline bool Texture::CanSet(Utility::Array2D< Graphics::Color<unsigned char> > &oData)
	{
		m_oData.Set(oData);
	}

	//This copies the content of oData to m_oData
	inline bool Texture::CanCopy(Utility::Array2D< Graphics::Color<unsigned char> > &oData)
	{
		m_oData.Copy(oData);
	}
}

#endif