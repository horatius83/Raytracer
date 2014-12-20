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
		bool CanLoad(const std::wstring& sFilename,RayTracer::PolygonList& polyList);
	private:
		//Function for loading .obj files
		bool CanLoadObjFile(RayTracer::PolygonList& polyList);
		std::ifstream m_inData;
	};

	bool MeshLoader::CanLoad(const std::wstring& sFilename,RayTracer::PolygonList& polyList)
	{
		if(sFilename.find(L".obj")!=std::string::npos)
		{
			m_inData.open(sFilename.c_str());
			return CanLoadObjFile(polyList);
		}
		return false;
	}

	bool MeshLoader::CanLoadObjFile(RayTracer::PolygonList& polyList)
	{
		using Utility::AlignedArray;
		using Math::Vector;
		using Math::Vector2D;
		using std::string;
		
		string token;
		unsigned int vertices=0;
		unsigned int faces=0;

		if(m_inData.is_open())
		{
			while(!m_inData.eof())
			{
				
				m_inData>>token;
				if(token=="v")
				{
					++vertices;
					m_inData.ignore(512,'\n');
				}
				else
				{
					if(token=="f")
					{
						++faces;
					}
				}
			}

			AlignedArray<unsigned int>	indices;
			AlignedArray<Vector>		alignedVertices;
			AlignedArray<Vector>		normals;
			AlignedArray<Vector2D>	texCoords;

			if(!vertices)
				return false;
	
			alignedVertices.Allocate(vertices);
			normals.Allocate(vertices);
			texCoords.Allocate(vertices);
			indices.Allocate(faces*3);

			m_inData.clear();
			m_inData.seekg(0,std::ios::beg);

			m_inData>>token;

			//For now, ignore comments, ID, and materials information, search for the first vertex
			while(!m_inData.eof() && token!="v")
			{
				m_inData.ignore(512,'\n');
				m_inData>>token;
			}
			
			unsigned int uiIndex=0;
			float fx,fy,fz;

			//Load the vertexes
			while(!m_inData.eof() && token=="v")
			{
				m_inData>>fx;
				m_inData>>fy;
				m_inData>>fz;
				alignedVertices[uiIndex++].Set(fx,fy,fz);
				m_inData>>token;
			}

			uiIndex=0;

			//Load the texture coordinates
			while(!m_inData.eof() && token=="vt")
			{
				m_inData>>fx;
				m_inData>>fy;
				texCoords[uiIndex++].Set(fx,fy);
				m_inData>>token;
			}

			uiIndex=0;

			//Load the normals
			while(!m_inData.eof() && token=="vn")
			{
				m_inData>>fx;
				m_inData>>fy;
				m_inData>>fz;
				normals[uiIndex++].Set(fx,fy,fz);
				m_inData>>token;
			}

			uiIndex=0;

			//Load the faces (currently assumes the same index value for all three arrays)
			while(!m_inData.eof() && token=="f")
			{
				m_inData>>token;
				string::size_type i = token.find("/",0);
				token = token.substr(0,i);
				unsigned int ui = (unsigned int)atoi(token.c_str())-1;
				indices[uiIndex++]=ui;
				m_inData>>token;

				i = token.find("/",0);
				token = token.substr(0,i);
				ui = (unsigned int)atoi(token.c_str())-1;
				indices[uiIndex++]=ui;
				m_inData>>token;

				i = token.find("/",0);
				token = token.substr(0,i);
				ui = (unsigned int)atoi(token.c_str())-1;
				indices[uiIndex++]=ui;
				m_inData>>token;
			}
			m_inData.close();

			return polyList.CanInitialize(indices,alignedVertices,normals,texCoords);
		}
		else
			return false;
	}
}

#endif