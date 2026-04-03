/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
  History
$Log: filter_normal_enhance.h,v $
Revision 1.0  2026/04/03  Created by Developer
Normal vector enhancement filter for mesh quality improvement

****************************************************************************/

#ifndef FILTER_NORMAL_ENHANCE_H
#define FILTER_NORMAL_ENHANCE_H

#include <QObject>

#include <common/interfaces.h>

class FilterNormalEnhancePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { 
        FP_NORMAL_SMOOTHING,       // 法向平滑
        FP_NORMAL_CONSISTENCY,     // 法向一致性检查
        FP_NORMAL_RECALCULATION,   // 法向重计算
        FP_NORMAL_ANGLE_THRESHOLD  // 角度阈值调整
    };

	FilterNormalEnhancePlugin();
	
	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;
    virtual FilterClass getClass(QAction *);
	virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter, MeshDocument &m, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
    
    // 新增工具函数声明
    bool applyNormalSmoothing(MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
    bool applyNormalConsistencyCheck(MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
    bool applyNormalRecalculation(MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
    bool applyNormalAngleThreshold(MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
    
private:
    float computeNeighborAreaWeight(CMeshO::VertexPointer v);
    void findAdjacentFaces(CMeshO::VertexPointer v, std::vector<CMeshO::FacePointer> &adjacentFaces);
};

#endif