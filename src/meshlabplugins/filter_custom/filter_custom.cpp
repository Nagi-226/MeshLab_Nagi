#include "filter_custom.h"

#include <QMessageBox>
#include <QString>

#include <cassert>

#include <vcg/complex/algorithms/update/bounding.h>

FilterCustomPlugin::FilterCustomPlugin()
{
	typeList = { COMPUTE_BOUNDING_BOX_REPORT };
	for (ActionIDType tt : types())
		actionList.push_back(new QAction(filterName(tt), this));
}

QString FilterCustomPlugin::pluginName() const
{
	return "FilterCustom";
}

QString FilterCustomPlugin::filterName(ActionIDType filter) const
{
	switch (filter) {
	case COMPUTE_BOUNDING_BOX_REPORT:
		return "Compute Bounding Box Report";
	default:
		return QString();
	}
}

QString FilterCustomPlugin::pythonFilterName(ActionIDType f) const
{
	switch (f) {
	case COMPUTE_BOUNDING_BOX_REPORT:
		return "compute_bounding_box_report";
	default:
		return QString();
	}
}

QString FilterCustomPlugin::filterInfo(ActionIDType filter) const
{
	switch (filter) {
	case COMPUTE_BOUNDING_BOX_REPORT:
		return "Compute mesh bounding box dimensions, estimated AABB volume and face count, then show them in a popup.";
	default:
		return QString();
	}
}

FilterPlugin::FilterClass FilterCustomPlugin::getClass(const QAction*) const
{
	return FilterPlugin::Measure;
}

FilterPlugin::FilterArity FilterCustomPlugin::filterArity(const QAction*) const
{
	return SINGLE_MESH;
}

int FilterCustomPlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_NONE;
}

int FilterCustomPlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_NONE;
}

RichParameterList FilterCustomPlugin::initParameterList(const QAction*, const MeshModel&)
{
	return RichParameterList();
}

std::map<std::string, QVariant> FilterCustomPlugin::applyFilter(
		const QAction* action,
		const RichParameterList&,
		MeshDocument& md,
		unsigned int&,
		vcg::CallBackPos*)
{
	if (ID(action) != COMPUTE_BOUNDING_BOX_REPORT) {
		wrongActionCalled(action);
		return {};
	}

	CMeshO& m = md.mm()->cm;
	vcg::tri::UpdateBounding<CMeshO>::Box(m);

	const vcg::Point3m dim = m.bbox.Dim();
	const double sx = static_cast<double>(dim[0]);
	const double sy = static_cast<double>(dim[1]);
	const double sz = static_cast<double>(dim[2]);
	const double aabbVolume = sx * sy * sz;
	const int faceCount = m.fn;

	QString report = QString("Bounding Box Size\n")
				 .append(QString("X: %1\nY: %2\nZ: %3\n\n")
						 .arg(sx, 0, 'f', 6)
						 .arg(sy, 0, 'f', 6)
						 .arg(sz, 0, 'f', 6))
				 .append(QString("AABB Volume (estimate): %1\n")
						 .arg(aabbVolume, 0, 'f', 6))
				 .append(QString("Triangle Faces: %1").arg(faceCount));

	QMessageBox::information(nullptr, "Compute Bounding Box Report", report);

	return {
		{"bbox_size_x", sx},
		{"bbox_size_y", sy},
		{"bbox_size_z", sz},
		{"aabb_volume", aabbVolume},
		{"triangle_faces", faceCount}
	};
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterCustomPlugin)

