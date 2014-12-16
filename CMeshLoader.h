/*
2007/10/1
Class for loading geometry data (as of 2007/10/1 only supports .obj files)
*/
#ifndef CMESHLOADER_H
#define CMESHLOADER_H

#include <string>
#include <fstream>
#include "TAlignedArray.h"
#include "CVector.h"
#include "CVector2D.h"
#include "CPolygonList.h"

namespace NGraphics
{
	class CMeshLoader
	{
	public:
		//Check file extension and take appropriate action, then pass off to specialized function
		bool bCanLoad(const std::wstring& sFilename,NRayTracer::CPolygonList& oPolyList);
	private:
		//Function for loading .obj files
		bool bCanLoadObjFile(NRayTracer::CPolygonList& oPolyList);
		std::ifstream m_inData;
	};

	bool CMeshLoader::bCanLoad(const std::wstring& sFilename,NRayTracer::CPolygonList& oPolyList)
	{
		if(sFilename.find(L".obj")!=std::string::npos)
		{
			m_inData.open(sFilename.c_str());
			return bCanLoadObjFile(oPolyList);
		}
		return false;
	}

	bool CMeshLoader::bCanLoadObjFile(NRayTracer::CPolygonList& oPolyList)
	{
		using NUtility::TAlignedArray;
		using NMath::CVector;
		using NMath::CVector2D;
		using std::string;
		//std::ifstream inData;
		
		//char c;
		string sToken;
		unsigned int uiVertices=0;
		unsigned int uiFaces=0;

		if(m_inData.is_open())
		{
			while(!m_inData.eof())
			{
				
				m_inData>>sToken;
				if(sToken=="v")
				{
					++uiVertices;
					m_inData.ignore(512,'\n');
				}
				else
				{
					if(sToken=="f")
					{
						++uiFaces;
						//m_inData.ignore(512,'\n');
					}
				}
			}

			TAlignedArray<unsigned int>	oIndices;
			TAlignedArray<CVector>		oVertices;
			TAlignedArray<CVector>		oNormals;
			TAlignedArray<CVector2D>	oTexCoords;

			if(!uiVertices)
				return false;
	
			oVertices.Allocate(uiVertices);
			oNormals.Allocate(uiVertices);
			oTexCoords.Allocate(uiVertices);
			oIndices.Allocate(uiFaces*3);

			m_inData.clear();
			m_inData.seekg(0,std::ios::beg);
			//m_inData.clear();

			m_inData>>sToken;
			//For now, ignore comments, ID, and materials information, search for the first vertex
			while(!m_inData.eof() && sToken!="v")
			{
				m_inData.ignore(512,'\n');
				m_inData>>sToken;
			}
			
			unsigned int uiIndex=0;
			float fx,fy,fz;
			//Load the vertexes
			while(!m_inData.eof() && sToken=="v")
			{
				m_inData>>fx;
				m_inData>>fy;
				m_inData>>fz;
				oVertices[uiIndex++].Set(fx,fy,fz);
				m_inData>>sToken;
			}

			uiIndex=0;
			//Load the texture coordinates
			while(!m_inData.eof() && sToken=="vt")
			{
				m_inData>>fx;
				m_inData>>fy;
				oTexCoords[uiIndex++].Set(fx,fy);
				m_inData>>sToken;
			}

			uiIndex=0;
			//Load the normals
			while(!m_inData.eof() && sToken=="vn")
			{
				m_inData>>fx;
				m_inData>>fy;
				m_inData>>fz;
				oNormals[uiIndex++].Set(fx,fy,fz);
				m_inData>>sToken;
			}

			uiIndex=0;
			//Load the faces (currently assumes the same index value for all three arrays)
			while(!m_inData.eof() && sToken=="f")
			{
				m_inData>>sToken;
				string::size_type i = sToken.find("/",0);
				sToken = sToken.substr(0,i);
				unsigned int ui = (unsigned int)atoi(sToken.c_str())-1;
				oIndices[uiIndex++]=ui;
				m_inData>>sToken;

				i = sToken.find("/",0);
				sToken = sToken.substr(0,i);
				ui = (unsigned int)atoi(sToken.c_str())-1;
				oIndices[uiIndex++]=ui;
				m_inData>>sToken;

				i = sToken.find("/",0);
				sToken = sToken.substr(0,i);
				ui = (unsigned int)atoi(sToken.c_str())-1;
				oIndices[uiIndex++]=ui;
				m_inData>>sToken;
			}
			m_inData.close();

			return oPolyList.bCanInitialize(oIndices,oVertices,oNormals,oTexCoords);
		}
		else
			return false;
	}
}

#endif