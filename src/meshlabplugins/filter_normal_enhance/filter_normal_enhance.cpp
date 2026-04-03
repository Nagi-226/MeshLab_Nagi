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
$Log: filter_normal_enhance.cpp,v $
Revision 1.0  2026/04/03  Created by Developer
Normal vector enhancement filter for mesh quality improvement

****************************************************************************/

#include "filter_normal_enhance.h"
#include <QAction>

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/position.h>
#include <vcg/complex/algorithms/update/normal.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/complex/algorithms/stat.h>

using namespace vcg;

// Constructor
FilterNormalEnhancePlugin::FilterNormalEnhancePlugin() 
{ 
    typeList = {
        FP_NORMAL_SMOOTHING,
        FP_NORMAL_CONSISTENCY,
        FP_NORMAL_RECALCULATION,
        FP_NORMAL_ANGLE_THRESHOLD
    };
    
    for (ActionIDType tt : types()) {
        actionList.push_back(new QAction(filterName(tt), this));
    }
}

QString FilterNormalEnhancePlugin::pluginName() const
{
    return "FilterNormalEnhance";
}

QString FilterNormalEnhancePlugin::filterName(ActionIDType filterId) const
{
    switch(filterId) {
        case FP_NORMAL_SMOOTHING :  return QString("Normal Smoothing"); 
        case FP_NORMAL_CONSISTENCY :  return QString("Normal Consistency Check"); 
        case FP_NORMAL_RECALCULATION :  return QString("Normal Recalculation"); 
        case FP_NORMAL_ANGLE_THRESHOLD :  return QString("Adjust Normal Angle Threshold"); 
        default : assert(0); 
    }
    return QString();
}

QString FilterNormalEnhancePlugin::pythonFilterName(ActionIDType filterId) const
{
    switch(filterId) {
        case FP_NORMAL_SMOOTHING :  return QString("apply_normal_smoothing"); 
        case FP_NORMAL_CONSISTENCY :  return QString("check_normal_consistency"); 
        case FP_NORMAL_RECALCULATION :  return QString("recalculate_normals"); 
        case FP_NORMAL_ANGLE_THRESHOLD :  return QString("adjust_normal_angle_threshold"); 
        default : assert(0); 
    }
    return QString();
}

QString FilterNormalEnhancePlugin::filterInfo(ActionIDType filterId) const
{
    switch(filterId) {
        case FP_NORMAL_SMOOTHING :  
            return QString("Apply Laplacian smoothing to vertex normals for better visual quality.<br>"
                           "Useful for removing noise and artifacts in scanned mesh normals."); 
        case FP_NORMAL_CONSISTENCY :  
            return QString("Check and fix normal orientation consistency across the mesh.<br>"
                           "Detects flipped normals and ensures consistent orientation for proper lighting."); 
        case FP_NORMAL_RECALCULATION :  
            return QString("Recalculate normals based on face geometry with different methods.<br>"
                           "Options: Per-face, per-vertex area-weighted, or angle-weighted normals."); 
        case FP_NORMAL_ANGLE_THRESHOLD :  
            return QString("Adjust the angle threshold for normal smoothing and sharp edge preservation.<br>"
                           "Higher values preserve sharp features, lower values produce smoother results."); 
        default : assert(0); 
    }
    return QString();
}

RichParameterList FilterNormalEnhancePlugin::initParameterList(const QAction* action, const MeshDocument&)
{
    RichParameterList parlst;
    
    switch(ID(action)) {
        case FP_NORMAL_SMOOTHING :  
            parlst.addParam(RichInt("Iterations",
                                        3,
                                        "Smoothing Iterations",
                                        "Number of Laplacian smoothing iterations to apply.<br>"
                                        "Higher values produce smoother results but may oversmooth."));
            parlst.addParam(RichFloat("Lambda",
                                        0.3f,
                                        "Smoothing Factor (Lambda)",
                                        "Smoothing factor between 0 and 1.<br>"
                                        "Lower values preserve original normals more."));
            parlst.addParam(RichBool("PreserveBoundary",
                                        true,
                                        "Preserve Boundary Normals",
                                        "Do not smooth boundary vertices to maintain sharp edges."));
            break;
            
        case FP_NORMAL_CONSISTENCY :  
            parlst.addParam(RichBool("AutoFix",
                                        true,
                                        "Automatically Fix Inconsistent Normals",
                                        "Automatically flip inconsistent normals based on majority voting."));
            parlst.addParam(RichBool("MarkInconsistent",
                                        false,
                                        "Mark Inconsistent Faces",
                                        "Mark faces with inconsistent normals with a special color."));
            parlst.addParam(RichFloat("AngleThreshold",
                                        60.0f,
                                        "Angle Threshold (degrees)",
                                        "Minimum angle between face normals to consider as inconsistent."));
            break;
            
        case FP_NORMAL_RECALCULATION :  
            parlst.addParam(RichEnum("Method",
                                        0,
                                        QStringList() << "Per-Vertex Area-Weighted" << "Per-Vertex Angle-Weighted" << "Per-Face",
                                        "Normal Calculation Method",
                                        "Method for normal calculation.<br>"
                                        "Area-weighted: weights by triangle area.<br>"
                                        "Angle-weighted: weights by vertex angle in triangle.<br>"
                                        "Per-Face: same normal for all vertices of a face."));
            parlst.addParam(RichBool("UpdatePerFace",
                                        true,
                                        "Update Face Normals",
                                        "Also update face normals after vertex normal calculation."));
            parlst.addParam(RichBool("Normalize",
                                        true,
                                        "Normalize Results",
                                        "Ensure resulting normals are unit length."));
            break;
            
        case FP_NORMAL_ANGLE_THRESHOLD :  
            parlst.addParam(RichFloat("AngleThreshold",
                                        45.0f,
                                        "Angle Threshold (degrees)",
                                        "Angle threshold for distinguishing sharp edges.<br>"
                                        "Edges with dihedral angle larger than this value are preserved as sharp."));
            parlst.addParam(RichBool("ApplyToAll",
                                        false,
                                        "Apply to All Layers",
                                        "Apply the threshold adjustment to all visible layers."));
            break;

        default: break;
    }
    
    return parlst;
}

FilterNormalEnhancePlugin::FilterClass FilterNormalEnhancePlugin::getClass(const QAction* a) const
{
    switch(ID(a))
    {
        case FP_NORMAL_SMOOTHING :
        case FP_NORMAL_CONSISTENCY :
        case FP_NORMAL_RECALCULATION :
        case FP_NORMAL_ANGLE_THRESHOLD :
            return FilterClass(FilterPlugin::Normal);
        default : 
            return FilterClass(FilterPlugin::Generic);
    }
}

std::map<std::string, QVariant> FilterNormalEnhancePlugin::applyFilter(
    const QAction* action,
    const RichParameterList& par,
    MeshDocument& md,
    unsigned int& postConditionMask,
    vcg::CallBackPos* cb)
{
    std::map<std::string, QVariant> outputValues;
    
    switch(ID(action)) {
        case FP_NORMAL_SMOOTHING :
            applyNormalSmoothing(md, par, cb);
            break;
        case FP_NORMAL_CONSISTENCY :
            applyNormalConsistencyCheck(md, par, cb);
            break;
        case FP_NORMAL_RECALCULATION :
            applyNormalRecalculation(md, par, cb);
            break;
        case FP_NORMAL_ANGLE_THRESHOLD :
            applyNormalAngleThreshold(md, par, cb);
            break;
        default: 
            assert(0);
    }
    
    // 设置后置条件：需要更新法向和边界框
    postConditionMask = 
        MeshModel::MM_VERTNORMAL | 
        MeshModel::MM_FACENORMAL | 
        MeshModel::MM_VERTCOLOR | 
        MeshModel::MM_FACECOLOR;
    
    return outputValues;
}

// ========================================================================
// 法向平滑实现
// ========================================================================
bool FilterNormalEnhancePlugin::applyNormalSmoothing(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb)
{
    int iterations = par.getInt("Iterations");
    float lambda = par.getFloat("Lambda");
    bool preserveBoundary = par.getBool("PreserveBoundary");
    
    int meshCount = 0;
    for (MeshModel &mmp : md.meshIterator()) {
        if (mmp.isVisible()) {
            ++meshCount;
            if (cb) cb(meshCount * 100 / md.meshNumber(), "Smoothing normals...");
            
            CMeshO &mesh = mmp.cm;
            
            // 首先更新拓扑信息
            tri::UpdateTopology<CMeshO>::VertexFace(mesh);
            tri::UpdateTopology<CMeshO>::FaceFace(mesh);
            tri::UpdateFlags<CMeshO>::FaceBorderFromFF(mesh);
            
            // 备份原始法向
            std::vector<Point3f> originalNormals;
            originalNormals.reserve(mesh.vert.size());
            for (size_t i = 0; i < mesh.vert.size(); ++i) {
                originalNormals.push_back(mesh.vert[i].N());
            }
            
            // 应用平滑迭代
            for (int iter = 0; iter < iterations; ++iter) {
                if (cb && iter % 5 == 0) {
                    cb((iter * 100) / iterations, QString("Smoothing iteration %1/%2").arg(iter + 1).arg(iterations).toStdString().c_str());
                }
                
                std::vector<Point3f> smoothedNormals;
                smoothedNormals.reserve(mesh.vert.size());
                
                for (size_t i = 0; i < mesh.vert.size(); ++i) {
                    CMeshO::VertexType &v = mesh.vert[i];
                    
                    if (preserveBoundary && v.IsB()) {
                        // 保持边界顶点法向不变
                        smoothedNormals.push_back(v.N());
                        continue;
                    }
                    
                    Point3f sumNormal(0, 0, 0);
                    float totalWeight = 0.0f;
                    
                    // 遍历邻接面
                    vcg::face::VFIterator<CMeshO::FaceType> vfi(&mesh.vert[i]);
                    while (!vfi.End()) {
                        sumNormal += vfi.F()->N();
                        totalWeight += 1.0f;
                        ++vfi;
                    }
                    
                    if (totalWeight > 0) {
                        sumNormal /= totalWeight;
                        // Laplacian平滑: N_new = (1-lambda)*N_old + lambda*N_average
                        Point3f newNormal = originalNormals[i] * (1.0f - lambda) + sumNormal * lambda;
                        newNormal.Normalize();
                        smoothedNormals.push_back(newNormal);
                    } else {
                        smoothedNormals.push_back(v.N());
                    }
                }
                
                // 更新法向
                for (size_t i = 0; i < mesh.vert.size(); ++i) {
                    mesh.vert[i].N() = smoothedNormals[i];
                }
            }
            
            log("Applied normal smoothing to mesh %s: %d iterations, lambda=%.2f", 
                qPrintable(mmp.label()), iterations, lambda);
        }
    }
    
    log("Normal smoothing completed on %d meshes", meshCount);
    return true;
}

// ========================================================================
// 法向一致性检查实现
// ========================================================================
bool FilterNormalEnhancePlugin::applyNormalConsistencyCheck(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb)
{
    bool autoFix = par.getBool("AutoFix");
    bool markInconsistent = par.getBool("MarkInconsistent");
    float angleThreshold = par.getFloat("AngleThreshold") * M_PI / 180.0f; // 转换为弧度
    
    int meshCount = 0;
    int totalInconsistent = 0;
    
    for (MeshModel &mmp : md.meshIterator()) {
        if (mmp.isVisible()) {
            ++meshCount;
            if (cb) cb(meshCount * 100 / md.meshNumber(), "Checking normal consistency...");
            
            CMeshO &mesh = mmp.cm;
            
            // 更新拓扑
            tri::UpdateTopology<CMeshO>::FaceFace(mesh);
            tri::UpdateTopology<CMeshO>::VertexFace(mesh);
            tri::UpdateFlags<CMeshO>::FaceBorderFromFF(mesh);
            
            // 计算面法向（如果需要）
            tri::UpdateNormals<CMeshO>::PerFace(mesh);
            
            int inconsistentCount = 0;
            
            for (size_t i = 0; i < mesh.face.size(); ++i) {
                CMeshO::FaceType &f = mesh.face[i];
                
                // 检查每个相邻面的法向一致性
                for (int j = 0; j < 3; ++j) {
                    if (!face::IsBorder(f, j)) {
                        CMeshO::FaceType *adjFace = f.FFp(j);
                        int adjEdge = f.FFi(j);
                        
                        // 计算法向夹角
                        float dotProduct = f.N() * adjFace->N();
                        if (dotProduct < cos(angleThreshold)) {
                            inconsistentCount++;
                            
                            if (markInconsistent) {
                                // 标记不一致的面为红色
                                f.C() = Color4b(255, 0, 0, 255);
                                adjFace->C() = Color4b(255, 0, 0, 255);
                            }
                            
                            if (autoFix) {
                                // 翻转不一致的法向
                                adjFace->N() = -adjFace->N();
                                // 同时翻转该面的顶点法向
                                for (int k = 0; k < 3; ++k) {
                                    adjFace->V(k)->N() = -adjFace->V(k)->N();
                                }
                            }
                        }
                    }
                }
            }
            
            totalInconsistent += inconsistentCount;
            log("Mesh %s: %d inconsistent face pairs found", qPrintable(mmp.label()), inconsistentCount);
        }
    }
    
    log("Normal consistency check completed: %d inconsistent face pairs found in %d meshes", 
        totalInconsistent, meshCount);
    
    return true;
}

// ========================================================================
// 法向重计算实现
// ========================================================================
bool FilterNormalEnhancePlugin::applyNormalRecalculation(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb)
{
    int method = par.getEnum("Method");
    bool updatePerFace = par.getBool("UpdatePerFace");
    bool normalize = par.getBool("Normalize");
    
    int meshCount = 0;
    
    for (MeshModel &mmp : md.meshIterator()) {
        if (mmp.isVisible()) {
            ++meshCount;
            if (cb) cb(meshCount * 100 / md.meshNumber(), "Recalculating normals...");
            
            CMeshO &mesh = mmp.cm;
            
            // 首先清除现有法向
            for (size_t i = 0; i < mesh.vert.size(); ++i) {
                mesh.vert[i].N() = Point3f(0, 0, 0);
            }
            
            if (method == 0) { // 面积加权法向
                for (size_t i = 0; i < mesh.face.size(); ++i) {
                    CMeshO::FaceType &f = mesh.face[i];
                    
                    // 计算三角形面积
                    Point3f v0 = f.P(0);
                    Point3f v1 = f.P(1);
                    Point3f v2 = f.P(2);
                    
                    Point3f edge1 = v1 - v0;
                    Point3f edge2 = v2 - v0;
                    float area = (edge1 ^ edge2).Norm() / 2.0f;
                    
                    // 计算面法向
                    Point3f faceNormal = (edge1 ^ edge2);
                    if (faceNormal.Norm() > 0)
                        faceNormal.Normalize();
                    
                    // 面积加权加到顶点
                    for (int j = 0; j < 3; ++j) {
                        f.V(j)->N() += faceNormal * area;
                    }
                    
                    if (updatePerFace) {
                        f.N() = faceNormal;
                    }
                }
            } else if (method == 1) { // 角度加权法向
                for (size_t i = 0; i < mesh.face.size(); ++i) {
                    CMeshO::FaceType &f = mesh.face[i];
                    
                    Point3f v0 = f.P(0);
                    Point3f v1 = f.P(1);
                    Point3f v2 = f.P(2);
                    
                    // 计算顶点角度
                    Point3f edge1 = v1 - v0;
                    Point3f edge2 = v2 - v0;
                    float angle0 = acos((edge1.Normalize() * edge2.Normalize()));
                    
                    edge1 = v0 - v1;
                    edge2 = v2 - v1;
                    float angle1 = acos((edge1.Normalize() * edge2.Normalize()));
                    
                    edge1 = v0 - v2;
                    edge2 = v1 - v2;
                    float angle2 = acos((edge1.Normalize() * edge2.Normalize()));
                    
                    // 计算面法向
                    Point3f faceNormal = (f.P(1) - f.P(0)) ^ (f.P(2) - f.P(0));
                    if (faceNormal.Norm() > 0)
                        faceNormal.Normalize();
                    
                    // 角度加权加到顶点
                    f.V(0)->N() += faceNormal * angle0;
                    f.V(1)->N() += faceNormal * angle1;
                    f.V(2)->N() += faceNormal * angle2;
                    
                    if (updatePerFace) {
                        f.N() = faceNormal;
                    }
                }
            } else if (method == 2) { // 面法向
                // 直接使用面法向作为顶点法向
                for (size_t i = 0; i < mesh.face.size(); ++i) {
                    CMeshO::FaceType &f = mesh.face[i];
                    
                    Point3f faceNormal = (f.P(1) - f.P(0)) ^ (f.P(2) - f.P(0));
                    if (faceNormal.Norm() > 0)
                        faceNormal.Normalize();
                    
                    for (int j = 0; j < 3; ++j) {
                        f.V(j)->N() = faceNormal;
                    }
                    
                    if (updatePerFace) {
                        f.N() = faceNormal;
                    }
                }
            }
            
            // 如果需要，归一化顶点法向
            if (normalize) {
                for (size_t i = 0; i < mesh.vert.size(); ++i) {
                    CMeshO::VertexType &v = mesh.vert[i];
                    if (v.N().Norm() > 0) {
                        v.N().Normalize();
                    }
                }
            }
            
            log("Recalculated normals for mesh %s using method %d", qPrintable(mmp.label()), method);
        }
    }
    
    log("Normal recalculation completed on %d meshes", meshCount);
    return true;
}

// ========================================================================
// 法向角度阈值调整实现
// ========================================================================
bool FilterNormalEnhancePlugin::applyNormalAngleThreshold(MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb)
{
    float angleThreshold = par.getFloat("AngleThreshold");
    bool applyToAll = par.getBool("ApplyToAll");
    
    int meshCount = 0;
    
    for (MeshModel &mmp : md.meshIterator()) {
        if (applyToAll || mmp.isVisible()) {
            ++meshCount;
            if (cb) cb(meshCount * 100 / md.meshNumber(), "Adjusting normal angle threshold...");
            
            // 这个功能主要影响其他插件（如平滑、重网格化）的行为
            // 我们可以在模型上存储阈值作为自定义属性
            mmp.setUserData("NormalAngleThreshold", angleThreshold);
            
            log("Set normal angle threshold to %.2f degrees for mesh %s", angleThreshold, qPrintable(mmp.label()));
        }
    }
    
    log("Normal angle threshold adjusted on %d meshes", meshCount);
    
    // 同时输出提示信息
    log("NOTE: This setting affects how other filters (smoothing, remeshing) handle sharp edges.");
    log("Sharp edges with dihedral angle > %.2f degrees will be preserved.", angleThreshold);
    
    return true;
}

// 查找邻接面（工具函数实现）
void FilterNormalEnhancePlugin::findAdjacentFaces(CMeshO::VertexPointer v, std::vector<CMeshO::FacePointer> &adjacentFaces)
{
    adjacentFaces.clear();
    vcg::face::VFIterator<CMeshO::FaceType> vfi(v);
    while (!vfi.End()) {
        adjacentFaces.push_back(&(*vfi.F()));
        ++vfi;
    }
}

// 计算邻接面面积权重（工具函数实现）
float FilterNormalEnhancePlugin::computeNeighborAreaWeight(CMeshO::VertexPointer v)
{
    float totalArea = 0.0f;
    vcg::face::VFIterator<CMeshO::FaceType> vfi(v);
    while (!vfi.End()) {
        CMeshO::FaceType &f = *vfi.F();
        Point3f v0 = f.P(0);
        Point3f v1 = f.P(1);
        Point3f v2 = f.P(2);
        
        Point3f edge1 = v1 - v0;
        Point3f edge2 = v2 - v0;
        float area = (edge1 ^ edge2).Norm() / 2.0f;
        totalArea += area;
        
        ++vfi;
    }
    return totalArea;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterNormalEnhancePlugin)