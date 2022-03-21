#include "scaledialog.h"

namespace ICEGUI_NS
{

ScaleDialog::ScaleDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	ui.setupUi(this);
}

ScaleDialog::~ScaleDialog()
{

}

void ScaleDialog::setCurrentPercentType(int percentType)
{
	ui.scaleCB->setCurrentPercentType(percentType);
}

int ScaleDialog::getCurrentPercentType() const
{
	return 	ui.scaleCB->getCurrentPercentType();
}

} // namespace ICEGUI_NS
