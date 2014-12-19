/*
2007/10/1
Class for loading geometry data (as of 2007/10/1 only supports .obj files)
*/
#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <string>
#include <fstream>
#include "AlignedArray.h"
#include "Vector.h"
#include "Vector2D.h"
#include "PolygonList.h"

namespace Graphics
{
	class MeshLoader
	{
	public:
		//Check file extension and take appropriate action, then pass off to specialized function
		bool CanLoad(const std::wstring& sFilename,RayTracer::PolygonList& oPolyList);
	private:
		//Function for loading .obj files
		bool CanLoadObjFile(RayTracer::PolygonList& oPolyList);
		std::ifstream m_inData;
	};

	bool MeshLoader::CanLoad(const std::wstring& sFilename,RayTracer::PolygonList& oPolyList)
	{
		if(sFilename.find(L".obj")!=std::string::npos)
		{
			m_inData.open(sFilename.c_str());
			return CanLoadObjFile(oPolyList);
		}
		return false;
	}

	bool MeshLoader::CanLoadObjFile(RayTracer::PolygonList& oPolyList)
	{
		using Utility::AlignedArray;
		using Math::Vector;
		using Math::Vector2D;
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

			AlignedArray<unsigned int>	oIndices;
			AlignedArray<Vector>		oVertices;
			AlignedArray<Vector>		oNormals;
			AlignedArray<Vector2D>	oTexCoords;

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

			return oPolyList.CanInitialize(oIndices,oVertices,oNormals,oTexCoords);
		}
		else
			return false;
	}
}

#endif