#ifndef BIHTREE_H
#define BIHTREE_H

#include <cfloat>
#include <vector>
#include <algorithm>
#include "NaiveSearch.h"
#include "Aabb.h"

namespace RayTracer
{
	enum BihAxis{XAxis,YAxis,ZAxis,Leaf};

	struct VectorIndex
	{
		float x;
		float y;
		float z;
		unsigned int index;
	};

	class VectorIndexFunctionObject
	{
	public:
		VectorIndexFunctionObject() : m_axis(Leaf) {};
		inline bool operator()(const VectorIndex& a,const VectorIndex& b)
		{
			switch(m_axis)
			{
			case XAxis: return a.x < b.x;	break;
			case YAxis: return a.y < b.y;	break;
			case ZAxis: return a.z < b.z;	break;
			default:
				{
					throw "VectorIndexFunctionObject::operator() : invalid argument \"Leaf\"";
					return false;
				}break;
			}
		}
		BihAxis m_axis;
	};

	class BihTree : public NaiveSearch
	{
	public:
		void GetPrimaryIntersection(Vector128& oResults,PolyIndices& oPolyIndices,
			PacketVector& oOrigin,PacketVector& oDirection);
		void GetShadowIntersection(Vector128& oResults,PacketVector& oOrigin,PacketVector& oDirection);
		bool bCanInitialize();
		void BuildTree();
	private:
		void BuildTree(unsigned int uiLeft,unsigned int uiRight,float fSplitPlane,BihAxis eAxis,unsigned int& uiIndex);

		struct SBihNode
		{
			unsigned	uiAxis	: 4;
			unsigned	uiIndex	: 28;
			union
			{
				int		iItems;
				float	fClipping[2];
			};
		};
		//Bounding box for the scene
		Math::Aabb<BihAxis>					m_oScene;
		//Left-most vertices of all polygons
		std::vector<VectorIndex>				m_oVectors;
		//need to use bounding boxes for all polygons
	};

	void BihTree::BuildTree()
	{
		unsigned int uiIndex = 0;
		switch(m_oScene.oGetData())
		{
		case XAxis:	BuildTree(0,(unsigned int)m_oVectors.size(),m_oScene.oGetRightPoint().fGetX()-m_oScene.oGetLeftPoint().fGetX(),XAxis,uiIndex); break;
		case YAxis: BuildTree(0,(unsigned int)m_oVectors.size(),m_oScene.oGetRightPoint().fGetY()-m_oScene.oGetLeftPoint().fGetY(),YAxis,uiIndex); break;
		case ZAxis: BuildTree(0,(unsigned int)m_oVectors.size(),m_oScene.oGetRightPoint().fGetZ()-m_oScene.oGetLeftPoint().fGetZ(),ZAxis,uiIndex); break;
		default:	throw "BihTree::BuildTree() : Invalid argument \"Leaf\""; break;
		}
	}

	void BihTree::BuildTree(unsigned int uiLeft,unsigned int uiRight,float fSplitPlane,BihAxis eAxis,unsigned int& uiIndex)
	{
		if(uiLeft!=uiRight-1)
		{
			VectorIndexFunctionObject functor;
			functor.m_axis = eAxis;

			//divide the scene by the longest side
			switch(m_oScene.oGetData())
			{
			case XAxis:
				{
					//determine length of scene and split-plane (length/2)
					std::vector<VectorIndex>::iterator begin = m_oVectors.begin();
					begin+=uiLeft;
					std::vector<VectorIndex>::iterator end = m_oVectors.begin();
					end+=uiRight;
					std::sort(begin,end,functor);
					float length = m_oVectors[uiRight-1].x - m_oVectors[uiLeft].x;
					length*=0.5f;
				}break;
			case YAxis:
				{
				}break;
			case ZAxis:
				{
				}break;
			default:
				{
				}break;
			}
		}
		//arrange into left, right or both
		//fit to geometry
		//create two nodes after this one
	}

	void BihTree::GetPrimaryIntersection(Vector128& oResults,RayTracer::PolyIndices& oPolyIndices,
		PacketVector& oOrigin,PacketVector& oDirection)
	{
		oResults.m_sseData = _mm_set_ps1(FLT_MAX);

		//does it intersect the bounding cube
		Math::Vector128 intsScene;

		m_oScene.DoesIntersect(intsScene,oOrigin,oDirection);
		if(_mm_movemask_ps(_mm_cmplt_ps(intsScene.m_sseData,_mm_set_ps1(FLT_MAX))))
		{
			Math::Vector128 polyInt;
			
			polyInt.m_sseData = _mm_set_ps1(FLT_MAX);
			//Math::Vector128 intsBox;

			for(unsigned int ui=0;ui<NaiveSearch::m_oPolyList.uiGetSize();++ui)
			{
				NaiveSearch::m_oPolyList.GetIntersection(polyInt,ui,oOrigin,oDirection);
				oResults.m_sseData = _mm_min_ps(oResults.m_sseData,polyInt.m_sseData);
			}
		}

	}

	bool BihTree::bCanInitialize()
	{
		//if(NaiveSearch::m_oPolyList.bCanInitialize(oIndices,oVertices,oNormals,oTexCoords))
		//{
		Math::Vector128 min,max,vecA,vecB,vecC,polyMax,polyMin;
		min.m_sseData = _mm_set_ps1(0.0f);
		max.m_sseData = _mm_set_ps1(0.0f);
		m_oVectors.resize(NaiveSearch::m_oPolyList.uiGetSize());
		//Masks for A>(B>C) (B>C)>A and A>C
		//__m128 sseMask,sseVar;

		for(unsigned int ui=0;ui<NaiveSearch::m_oPolyList.uiGetSize();++ui)
		{
			vecA.m_sseData = m_oPolyList.oGetVertex(ui,0).sseGetVector();
			vecB.m_sseData = m_oPolyList.oGetVertex(ui,1).sseGetVector();
			vecC.m_sseData = m_oPolyList.oGetVertex(ui,2).sseGetVector();

			//Calculate bounding box for the polygon
			//Greatest vertex
			polyMax.m_sseData = _mm_max_ps(vecA.m_sseData,vecB.m_sseData);
			polyMax.m_sseData = _mm_max_ps(polyMax.m_sseData,vecC.m_sseData);
			/*
			sseMask = _mm_cmpgt_ps(vecA.m_sseData,vecB.m_sseData);
			sseVar = _mm_add_ps(
				_mm_and_ps(vecA.m_sseData,sseMask),
				_mm_andnot_ps(sseMask,vecB.m_sseData));
			sseMask = _mm_cmpgt_ps(sseVar,vecC.m_sseData);
			polyMax.m_sseData = _mm_add_ps(
				_mm_and_ps(sseVar,sseMask),
				_mm_andnot_ps(sseMask,vecC.m_sseData));*/

			//Smallest vertex
			polyMin.m_sseData = _mm_min_ps(vecA.m_sseData,vecB.m_sseData);
			polyMin.m_sseData = _mm_min_ps(polyMin.m_sseData,vecC.m_sseData);
			
			m_oVectors[ui].x = polyMin.m_fData[0];
			m_oVectors[ui].y = polyMin.m_fData[1];
			m_oVectors[ui].z = polyMin.m_fData[3];
			m_oVectors[ui].index = ui;
			/*
			sseMask = _mm_cmplt_ps(vecA.m_sseData,vecB.m_sseData);
			sseVar = _mm_add_ps(
				_mm_and_ps(vecA.m_sseData,sseMask),
				_mm_andnot_ps(sseMask,vecB.m_sseData));
			sseMask = _mm_cmplt_ps(sseVar,vecC.m_sseData);
			polyMin.m_sseData = _mm_add_ps(
				_mm_and_ps(sseVar,sseMask),
				_mm_andnot_ps(sseMask,vecC.m_sseData));*/

		
			//Add to the cumulitive max
			max.m_sseData = _mm_max_ps(max.m_sseData,polyMax.m_sseData);
			min.m_sseData = _mm_min_ps(min.m_sseData,polyMin.m_sseData);
			/*
			sseMask = _mm_cmpgt_ps(max.m_sseData,polyMax.m_sseData);
			max.m_sseData = _mm_add_ps(
				_mm_and_ps(max.m_sseData,sseMask),
				_mm_andnot_ps(sseMask,polyMax.m_sseData));

			sseMask = _mm_cmplt_ps(min.m_sseData,polyMin.m_sseData);
			min.m_sseData = _mm_add_ps(
				_mm_and_ps(min.m_sseData,sseMask),
				_mm_andnot_ps(sseMask,polyMin.m_sseData));*/
		}
			
		m_oScene.SetLeftPoint(Vector(min.m_sseData));
		m_oScene.SetRightPoint(Vector(max.m_sseData));
		
		//Determine the first splitting plane of the global scene 
		Math::Vector128 oAxis;
	
		oAxis.m_sseData = _mm_sub_ps(m_oScene.oGetRightPoint().sseGetVector(),m_oScene.oGetLeftPoint().sseGetVector());

		if((oAxis.m_fData[0]>=oAxis.m_fData[1]) && (oAxis.m_fData[1]>=oAxis.m_fData[2]))
			m_oScene.SetData(XAxis);
		else
		{
			if(((oAxis.m_fData[1]>=oAxis.m_fData[0]) && (oAxis.m_fData[1]>=oAxis.m_fData[2])))
				m_oScene.SetData(YAxis);
			else
			{
				if(((oAxis.m_fData[2]>=oAxis.m_fData[1]) && (oAxis.m_fData[2]>=oAxis.m_fData[0])))
					m_oScene.SetData(ZAxis);
			}
		}

		return true;
	}
}

#endif