#ifndef FILTER_CUSTOM_H
#define FILTER_CUSTOM_H

#include <common/plugins/interfaces/filter_plugin.h>

class FilterCustomPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum { COMPUTE_BOUNDING_BOX_REPORT };

	FilterCustomPlugin();

	QString pluginName() const;

	QString filterName(ActionIDType filter) const;
	QString pythonFilterName(ActionIDType f) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	int getPreConditions(const QAction*) const;
	int postCondition(const QAction*) const;
	RichParameterList initParameterList(const QAction*, const MeshModel&);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList& parameters,
			MeshDocument& md,
			unsigned int& postConditionMask,
			vcg::CallBackPos* cb);
};

#endif
