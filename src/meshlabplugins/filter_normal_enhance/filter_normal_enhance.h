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

#include <common/plugins/interfaces/filter_plugin.h>

class FilterNormalEnhancePlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum { 
        FP_NORMAL_SMOOTHING,       // 法向平滑
        FP_NORMAL_CONSISTENCY,     // 法向一致性检查
        FP_NORMAL_RECALCULATION,   // 法向重计算
        FP_NORMAL_ANGLE_THRESHOLD  // 角度阈值调整
    };

	FilterNormalEnhancePlugin();
	
	virtual QString pluginName() const;
	virtual QString filterName(ActionIDType filter) const;
	virtual QString pythonFilterName(ActionIDType filter) const;
	virtual QString filterInfo(ActionIDType filter) const;
    virtual FilterClass getClass(const QAction* a) const;
	virtual RichParameterList initParameterList(const QAction*, const MeshDocument&);
	virtual std::map<std::string, QVariant> applyFilter(
        const QAction* action,
        const RichParameterList& params,
        MeshDocument& md,
        unsigned int& postConditionMask,
        vcg::CallBackPos* cb);
    
    // 新增工具函数声明
    bool applyNormalSmoothing(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb);
    bool applyNormalConsistencyCheck(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb);
    bool applyNormalRecalculation(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb);
    bool applyNormalAngleThreshold(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb);
    
private:
    float computeNeighborAreaWeight(CMeshO::VertexPointer v);
    void findAdjacentFaces(CMeshO::VertexPointer v, std::vector<CMeshO::FacePointer> &adjacentFaces);
};

#endif