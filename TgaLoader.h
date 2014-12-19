#ifndef TGALOADER_H
#define TGALOADER_H

#include <string>
#include <fstream>
#include "HtmlError.h"
#include "Array2D.h"
#include "Color.h"

namespace Graphics
{
	struct TgaHeader
	{
		unsigned	m_ucIdFieldSize		: 8;	//[0] size of ID field that follows header (0 usually)
		unsigned	m_ucColorMapType	: 8;	//[1] type of color map 0=none, 1=palette
		unsigned	m_ucImageType		: 8;	//[2] type of image 0=none, 1=indexed, 2=rgb, 3=grey,+8=rle packed
		unsigned	m_usColorMapStart	: 16;	//[3] start of palette
		unsigned	m_usColorMapLength	: 16;	//[5] length of palette
		unsigned	m_ucColorMapBits	: 8;	//[7] number of bits per palette entry 15,16,24,32
		unsigned	m_usStartX			: 16;	//[8] image x origin
		unsigned	m_usStartY			: 16;	//[10] image y origin
		unsigned	m_usWidth			: 16;	//[12] image width in pixels
		unsigned	m_usHeight			: 16;	//[14] image height in pixels
		unsigned	m_ucBits			: 8;	//[16] image bits per pixel 8,16,24,32
		unsigned	m_ucDescriptor		: 8;	//[17] image descriptor bits (vh flip bits)
	};

	enum TGA_ENCODING{
		TGA_UNSUPPORTED=0,
		TGA_RAW_INDEXED=1,
		TGA_RAW_RGB=2,
		TGA_RAW_GREYSCALE=3,
		TGA_RLE_INDEXED=9,
		TGA_RLE_RGB=10,
		TGA_RLE_GREYSCALE=11};
	
	class TgaLoader
	{
	public:
		~TgaLoader()
		{
			m_oData.Deallocate();
		};
		bool CanLoadImage(const std::wstring& sFilename);
		inline void DeleteImage()
		{
			m_oData.Deallocate();
		}
		inline unsigned int GetWidth(){return m_oData.GetWidth();};
		inline unsigned int GetHeight(){return m_oData.GetHeight()};
		inline unsigned int GetBpp(){return 32;};
	private:
		TGA_ENCODING ReadHeader(const Utility::Array<unsigned char>& oData,TgaHeader& stHeader); 
		bool CanLoadRawIndexed(const Utility::Array<unsigned char>& oData,const TgaHeader& stHeader);
		bool CanLoadRawRgb(const Utility::Array<unsigned char>& oData,const TgaHeader& stHeader);
		bool CanLoadIndexedRleData(const Utility::Array<unsigned char>& oData,const TgaHeader& stHeader);

		Utility::Array2D< Graphics::Color<unsigned char> >	m_oData;
	};

	TGA_ENCODING TgaLoader::ReadHeader(const Utility::Array<unsigned char> &oData,TgaHeader& stHeader)
	{
		if(oData.uiGetLength())
		{
			memcpy(&stHeader,&oData[0],sizeof(TgaHeader));
				
			if(stHeader.m_ucColorMapBits>1)
			{
				err(L"File format not supported",EL_ERROR);
				return TGA_UNSUPPORTED;
			}

			//char cEncoding=oData[2];
			if(stHeader.m_ucImageType>11)
			{
				err(L"Encoding not supported",EL_ERROR);
				return TGA_UNSUPPORTED;
			}
			
			//Check for VGA palette (256 entries @ 24bits per entry)
			if(stHeader.m_ucColorMapType==1)
			{
				if(stHeader.m_usColorMapStart||stHeader.m_usColorMapLength!=256||stHeader.m_ucColorMapBits!=24)
				{
					err(L"Unsupported palette type",EL_ERROR);
					return TGA_UNSUPPORTED;
				}
			}
			
			if(!stHeader.m_usWidth || !stHeader.m_usHeight)
			{
				err(L"Invalid dimensions",EL_ERROR);
				return TGA_UNSUPPORTED;
			}

			//Check flip / interleave byte
			if(stHeader.m_ucDescriptor>32)
			{
				err(L"Interleaved data not supported",EL_ERROR);
				return TGA_UNSUPPORTED;
			}

			return TGA_ENCODING(cEncoding); 
		}
		else
		{
			err(L"No data to read",EL_ERROR);
			return TGA_UNSUPPORTED;
		}
	};

	bool TgaLoader::CanLoadRawIndexed(const Utility::Array<unsigned char>& oData,const TgaHeader& stHeader)
	{
		using namespace Utility;
		using namespace Graphics;
		//Allocate Palette
		Array< TColor<unsigned char> > oPalette(stHeader.m_usColorMapLength);
		if(!oPalette.uiGetLength())
		{
			err(L"Could not allocate temporary palette",EL_ERROR);
			return false;
		}

		//Copy Palette Data
		unsigned int uiPaletteIndex=0;
		unsigned int uiStartingIndex=stHeader.m_usColorMapStart;
		unsigned int uiEndingIndex=stHeader.m_usColorMapStart+stHeader.m_usColorMapLength;

		for(unsigned int uiIndex=uiStartingIndex;uiIndex<uiEndingIndex;uiIndex+=3)
		{
			oPalette[uiPaletteIndex].SetBlue(oData[uiIndex]);
			oPalette[uiPaletteIndex].SetGreen(oData[uiIndex+1]);
			oPalette[uiPaletteIndex].SetRed(oData[uiIndex+2]);
		}
		//find beginning of indexed data
		//allocate texture space
		m_oData.Allocate(stHeader.m_usWidth,stHeader.m_usHeight);
		//copy color data from temp array to main texture data
		uiStartingIndex=stHeader.m_usColorMapStart+stHeader.m_usColorMapLength+1;
		uiEndingIndex=stHeader.m_usWidth*stHeader.m_usHeight;

		for(unsigned int ui=uiStartingIndex;ui<uiEndingIndex;++ui)
			m_oData[uiIndex]=oPalette[oData[ui]];
	}

	bool TgaLoader::CanLoadRawRgb(const Utility::Array<unsigned char>& oData,const TgaHeader& stHeader)
	{
		unsigned int uiStartingIndex=stHeader.m_ucIdFieldSize+18;
		unsigned int uiEndingIndex=stHeader.m_ucIdFieldSize+18+stHeader.m_usWidth*stHeader.m_usHeight;
		switch(stHeader.m_ucBits)
		{
		case 24:
			{
				//Allocate texture data
				m_oData.Allocate(stHeader.m_usWidth,stHeader.m_usHeight);
				for(unsigned int ui=uiStartingIndex,unsigned int uiTextureIndex=0;
					ui<uiEndingIndex;
					++uiTextureIndex,ui+=3)
				{
					m_oData[uiTextureIndex].SetBlue(oData[ui]);
					m_oData[uiTextureIndex].SetGreen(oData[ui+1]);
					m_oData[uiTextureIndex].SetRed(oData[ui+2]);
					++uiTextureIndex;
				}
				return true;
			}break;
		case 32:
			{
				//Allocate texture data
				m_oData.Allocate(stHeader.m_usWidth,stHeader.m_usHeight);
				for(unsigned int ui=uiStartingIndex,unsigned int uiTextureIndex=0;
					ui<uiEndingIndex;
					++uiTextureIndex,ui+=4)
				{
					m_oData[uiTextureIndex].SetBlue(oData[ui]);
					m_oData[uiTextureIndex].SetGreen(oData[ui+1]);
					m_oData[uiTextureIndex].SetRed(oData[ui+2]);
					m_oData[uiTextureIndex].SetAlpha(oData[ui+3]);
				}
				return true;
			}break;
		default:	err(L"Format not supported (Please use 24bit or 32bit per pixel)",EL_ERROR);	break;
		}
		return false;
	}

	bool TgaLoader::CanLoadIndexedRleData(const Utility::Array<unsigned char>& oData,const TgaHeader& stHeader)
	{
	}

	bool TgaLoader::CanLoadImage(const std::wstring &sFilename)
	{
		using namespace std;	
		using namespace Utility;
		ifstream inData;
		unsigned int uiFileSize=0;
		m_oData.Deallocate();

		//open file
		inData.open(sFilename.data(),ios::binary);
		if(inData.good())
		{
			//get file size
			inData.seekg(0,ios_base::end);
			uiFileSize = inData.tellg();
			inData.seekg(0,ios_base::beg);
			Array<unsigned char>	oTempData(uiFileSize);
			
			if(oTempData.uiGetSize())
			{
				inData.read((char*)oTempData.GetArray(),uiFileSize);
				inData.close();
				TgaHeader stHeader;
				TGA_ENCODING eEncoding = ReadHeader(oTempData,stHeader);
				unsigned int uiLength = stHeader.m_usWidth*stHeader.m_usHeight*(stHeader.m_ucBits/8);
				//Check filesize versus header information
				//if(uiLength+18+oData[0]+768>uiFileSize)
				if(uiLength+18+stHeader.m_ucIdFieldSize+768>uiFileSize)
				{
					err(L"Filesize mismatch",EL_ERROR);
					return false;
				}

				switch(eEncoding)
				{
				case TGA_RAW_INDEXED:
					{
						if(stHeader.m_ucColorMapType!=1)
						{
							err(L"Indexed Color/RGB Color Mismatch: Should be Indexed",EL_ERROR);
							return false;
						}
						//Load Image Data
						if(CanLoadRawIndexed(oTempData,stHeader))
							break;
						else
						{
							err(L"Could not load raw indexed data",EL_ERROR);
							return false;
						}
					}break;
				case TGA_RAW_RGB:
					{
						if(stHeader.m_ucColorMapType!=0)
						{
							err(L"Indexed Color/RGB Color Mismatch: Should be RGB",EL_ERROR);
							return false;
						}
						//Load Data
						if(CanLoadRawRgb(oTempData,stHeader))
							break;
						else
						{
							err(L"Could not load raw RGB data",EL_ERROR);
							return false;
						}
					}break;
				}
				case TGA_RLE_INDEXED:
					{
						if(stHeader.m_ucColorMapType!=1)
						{
							err(L"Indexed Color/RGB Color Mismatch: Should be Indexed",EL_ERROR);
							return false;
						}
						if(bLoadIndexedRleData(oData,stHeader))
							break;
						else
						{
							err(L"Could not load Indexed RLE Data",EL_ERROR);
							return false;
						}
					}break;
					/*
 
     case 9: // RLE Indexed
      {
       // Double check image type field
        if(pData[1]!=1)
         return IMG_ERR_BAD_FORMAT;
 
       // Load image data
       iRet=LoadTgaRLEData();
        if(iRet!=IMG_OK)
         return iRet;
 
       // Load palette
       iRet=LoadTgaPalette();
        if(iRet!=IMG_OK)
         return iRet;
 
       break;
      }
 
     case 10: // RLE RGB
      {
       // Double check image type field
        if(pData[1]!=0)
         return IMG_ERR_BAD_FORMAT;
 
       // Load image data
       iRet=LoadTgaRLEData();
        if(iRet!=IMG_OK)
         return iRet;
 
       BGRtoRGB(); // Convert to RGB
       break;
      }
 
     default:
      return IMG_ERR_UNSUPPORTED;
    }
 
  // Check flip bit
   if((pData[17] & 0x20)==0) 
     FlipImg();
 
  // Release file memory
  delete [] pData;
  pData=NULL;
 
  return IMG_OK;
 }*/
			}
			else
			{
				err(L"Could not allocate temp data",EL_ERROR);
				return false;
			}
		}
		else
		{
			wstring sError = wstring(L"Could not open file: ") + sFilename;
			err(sError,EL_ERROR);
			return false;
		}		
	}
/*
// TGA Loader - 16/11/04 Codehead
 
#include <iostream>
#include <fstream>
#include <memory.h>
 
#define IMG_OK              0x1
#define IMG_ERR_NO_FILE     0x2
#define IMG_ERR_MEM_FAIL    0x4
#define IMG_ERR_BAD_FORMAT  0x8
#define IMG_ERR_UNSUPPORTED 0x40
 
class TGAImg
 {
  public:
   TGAImg();
   ~TGAImg();
   int Load(char* szFilename);
   int GetBPP();
   int GetWidth();
   int GetHeight();
   unsigned char* GetImg();       // Return a pointer to image data
   unsigned char* GetPalette();   // Return a pointer to VGA palette
 
  private:
   short int iWidth,iHeight,iBPP;
   unsigned long lImageSize;
   char bEnc;
   unsigned char *pImage, *pPalette, *pData;
   
   // Internal workers
   int ReadHeader();
   int LoadRawData();
   int LoadTgaRLEData();
   int LoadTgaPalette();
   void BGRtoRGB();
   void FlipImg();
 };
 
 
TGAImg::TGAImg()
 { 
  pImage=pPalette=pData=NULL;
  iWidth=iHeight=iBPP=bEnc=0;
  lImageSize=0;
 }
 
 
TGAImg::~TGAImg()
 {
  if(pImage)
   {
    delete [] pImage;
    pImage=NULL;
   }
 
  if(pPalette)
   {
    delete [] pPalette;
    pPalette=NULL;
   }
 
  if(pData)
   {
    delete [] pData;
    pData=NULL;
   }
 }
 
 
int TGAImg::Load(char* szFilename)
 {
  using namespace std;
  ifstream fIn;
  unsigned long ulSize;
  int iRet;
 
  // Clear out any existing image and palette
   if(pImage)
    {
     delete [] pImage;
     pImage=NULL;
    }
 
   if(pPalette)
    {
     delete [] pPalette;
     pPalette=NULL;
    }
 
  // Open the specified file
  fIn.open(szFilename,ios::binary);
    
   if(fIn==NULL)
    return IMG_ERR_NO_FILE;
 
  // Get file size
  fIn.seekg(0,ios_base::end);
  ulSize=fIn.tellg();
  fIn.seekg(0,ios_base::beg);
 
  // Allocate some space
  // Check and clear pDat, just in case
   if(pData)
    delete [] pData; 
 
  pData=new unsigned char[ulSize];
 
   if(pData==NULL)
    {
     fIn.close();
     return IMG_ERR_MEM_FAIL;
    }
 
  // Read the file into memory
  fIn.read((char*)pData,ulSize);
 
  fIn.close();
 
  // Process the header
  iRet=ReadHeader();
 
   if(iRet!=IMG_OK)
    return iRet;
 
   switch(bEnc)
    {
     case 1: // Raw Indexed
      {
       // Check filesize against header values
        if((lImageSize+18+pData[0]+768)>ulSize)
         return IMG_ERR_BAD_FORMAT;
 
       // Double check image type field
        if(pData[1]!=1)
         return IMG_ERR_BAD_FORMAT;
 
       // Load image data
       iRet=LoadRawData();
        if(iRet!=IMG_OK)
         return iRet;
 
       // Load palette
       iRet=LoadTgaPalette();
        if(iRet!=IMG_OK)
         return iRet;
 
       break;
      }
 
     case 2: // Raw RGB
      {
       // Check filesize against header values
        if((lImageSize+18+pData[0])>ulSize)
         return IMG_ERR_BAD_FORMAT;
 
       // Double check image type field
        if(pData[1]!=0)
         return IMG_ERR_BAD_FORMAT;
 
       // Load image data
       iRet=LoadRawData();
        if(iRet!=IMG_OK)
         return iRet;
 
       BGRtoRGB(); // Convert to RGB
       break;
      }
 
     case 9: // RLE Indexed
      {
       // Double check image type field
        if(pData[1]!=1)
         return IMG_ERR_BAD_FORMAT;
 
       // Load image data
       iRet=LoadTgaRLEData();
        if(iRet!=IMG_OK)
         return iRet;
 
       // Load palette
       iRet=LoadTgaPalette();
        if(iRet!=IMG_OK)
         return iRet;
 
       break;
      }
 
     case 10: // RLE RGB
      {
       // Double check image type field
        if(pData[1]!=0)
         return IMG_ERR_BAD_FORMAT;
 
       // Load image data
       iRet=LoadTgaRLEData();
        if(iRet!=IMG_OK)
         return iRet;
 
       BGRtoRGB(); // Convert to RGB
       break;
      }
 
     default:
      return IMG_ERR_UNSUPPORTED;
    }
 
  // Check flip bit
   if((pData[17] & 0x20)==0) 
     FlipImg();
 
  // Release file memory
  delete [] pData;
  pData=NULL;
 
  return IMG_OK;
 }
 
 
int TGAImg::ReadHeader() // Examine the header and populate our class attributes
 {
  short ColMapStart,ColMapLen;
  short x1,y1,x2,y2;
 
   if(pData==NULL)
    return IMG_ERR_NO_FILE;
 
   if(pData[1]>1)    // 0 (RGB) and 1 (Indexed) are the only types we know about
    return IMG_ERR_UNSUPPORTED;
 
   bEnc=pData[2];     // Encoding flag  1 = Raw indexed image
                      //                2 = Raw RGB
                      //                3 = Raw greyscale
                      //                9 = RLE indexed
                      //               10 = RLE RGB
                      //               11 = RLE greyscale
                      //               32 & 33 Other compression, indexed
 
    if(bEnc>11)       // We don't want 32 or 33
     return IMG_ERR_UNSUPPORTED;
 
 
  // Get palette info
  memcpy(&ColMapStart,&pData[3],2);
  memcpy(&ColMapLen,&pData[5],2);
 
  // Reject indexed images if not a VGA palette (256 entries with 24 bits per entry)
   if(pData[1]==1) // Indexed
    {
     if(ColMapStart!=0 || ColMapLen!=256 || pData[7]!=24)
      return IMG_ERR_UNSUPPORTED;
    }
 
  // Get image window and produce width & height values
  memcpy(&x1,&pData[8],2);
  memcpy(&y1,&pData[10],2);
  memcpy(&x2,&pData[12],2);
  memcpy(&y2,&pData[14],2);
 
  iWidth=(x2-x1);
  iHeight=(y2-y1);
 
   if(iWidth<1 || iHeight<1)
    return IMG_ERR_BAD_FORMAT;
 
  // Bits per Pixel
  iBPP=pData[16];
 
  // Check flip / interleave byte
   if(pData[17]>32) // Interleaved data
    return IMG_ERR_UNSUPPORTED;
 
  // Calculate image size
  lImageSize=(iWidth * iHeight * (iBPP/8));
 
  return IMG_OK;
 }
 
 
int TGAImg::LoadRawData() // Load uncompressed image data
 {
  short iOffset;
 
   if(pImage) // Clear old data if present
    delete [] pImage;
 
  pImage=new unsigned char[lImageSize];
 
   if(pImage==NULL)
    return IMG_ERR_MEM_FAIL;
 
  iOffset=pData[0]+18; // Add header to ident field size
 
   if(pData[1]==1) // Indexed images
    iOffset+=768;  // Add palette offset
 
   memcpy(pImage,&pData[iOffset],lImageSize);
 
  return IMG_OK;
 }
 
 
int TGAImg::LoadTgaRLEData() // Load RLE compressed image data
 {
  short iOffset,iPixelSize;
  unsigned char *pCur;
  unsigned long Index=0;
  unsigned char bLength,bLoop;
 
  // Calculate offset to image data
  iOffset=pData[0]+18;
 
  // Add palette offset for indexed images
   if(pData[1]==1)
    iOffset+=768; 
 
  // Get pixel size in bytes
  iPixelSize=iBPP/8;
 
  // Set our pointer to the beginning of the image data
  pCur=&pData[iOffset];
 
  // Allocate space for the image data
   if(pImage!=NULL)
    delete [] pImage;
 
  pImage=new unsigned char[lImageSize];
 
   if(pImage==NULL)
    return IMG_ERR_MEM_FAIL;
 
  // Decode
   while(Index<lImageSize) 
    {
      if(*pCur & 0x80) // Run length chunk (High bit = 1)
       {
        bLength=*pCur-127; // Get run length
        pCur++;            // Move to pixel data  
 
        // Repeat the next pixel bLength times
         for(bLoop=0;bLoop!=bLength;++bLoop,Index+=iPixelSize)
          memcpy(&pImage[Index],pCur,iPixelSize);
  
        pCur+=iPixelSize; // Move to the next descriptor chunk
       }
      else // Raw chunk
       {
        bLength=*pCur+1; // Get run length
        pCur++;          // Move to pixel data
 
        // Write the next bLength pixels directly
         for(bLoop=0;bLoop!=bLength;++bLoop,Index+=iPixelSize,pCur+=iPixelSize)
          memcpy(&pImage[Index],pCur,iPixelSize);
       }
    }
 
  return IMG_OK;
 }
 
 
int TGAImg::LoadTgaPalette() // Load a 256 color palette
 {
  unsigned char bTemp;
  short iIndex,iPalPtr;
  
   // Delete old palette if present
   if(pPalette)
    {
     delete [] pPalette;
     pPalette=NULL;
    }
 
  // Create space for new palette
  pPalette=new unsigned char[768];
 
   if(pPalette==NULL)
    return IMG_ERR_MEM_FAIL;
 
  // VGA palette is the 768 bytes following the header
  memcpy(pPalette,&pData[pData[0]+18],768);
 
  // Palette entries are BGR ordered so we have to convert to RGB
   for(iIndex=0,iPalPtr=0;iIndex!=256;++iIndex,iPalPtr+=3)
    {
     bTemp=pPalette[iPalPtr];               // Get Blue value
     pPalette[iPalPtr]=pPalette[iPalPtr+2]; // Copy Red to Blue
     pPalette[iPalPtr+2]=bTemp;             // Replace Blue at the end
    }
 
  return IMG_OK;
 }
 
 
void TGAImg::BGRtoRGB() // Convert BGR to RGB (or back again)
 {
  unsigned long Index,nPixels;
  unsigned char *bCur;
  unsigned char bTemp;
  short iPixelSize;
 
  // Set ptr to start of image
  bCur=pImage;
 
  // Calc number of pixels
  nPixels=iWidth*iHeight;
 
  // Get pixel size in bytes
  iPixelSize=iBPP/8;
 
   for(Index=0;Index!=nPixels;Index++)  // For each pixel
    {
     bTemp=*bCur;      // Get Blue value
     *bCur=*(bCur+2);  // Swap red value into first position
     *(bCur+2)=bTemp;  // Write back blue to last position
 
     bCur+=iPixelSize; // Jump to next pixel
    }
 
 }
 
 
void TGAImg::FlipImg() // Flips the image vertically (Why store images upside down?)
 {
  unsigned char bTemp;
  unsigned char *pLine1, *pLine2;
  int iLineLen,iIndex;
 
  iLineLen=iWidth*(iBPP/8);
  pLine1=pImage;
  pLine2=&pImage[iLineLen * (iHeight - 1)];
 
   for( ;pLine1<pLine2;pLine2-=(iLineLen*2))
    {
     for(iIndex=0;iIndex!=iLineLen;pLine1++,pLine2++,iIndex++)
      {
       bTemp=*pLine1;
       *pLine1=*pLine2;
       *pLine2=bTemp;       
      }
    } 
 
 }
 
 
int TGAImg::GetBPP() 
 {
  return iBPP;
 }
 
 
int TGAImg::GetWidth()
 {
  return iWidth;
 }
 
 
int TGAImg::GetHeight()
 {
  return iHeight;
 }
 
 
unsigned char* TGAImg::GetImg()
 {
  return pImage;
 }
 
 
unsigned char* TGAImg::GetPalette()
 {
  return pPalette;
 }
*/
}

#endif