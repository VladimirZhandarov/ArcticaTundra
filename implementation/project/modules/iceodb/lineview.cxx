#include "lineview.hxx"

namespace ICEODB_NS
{

LineView::LineView() : MaskView()
{
	lineWidth_ = 1;
}

LineView::LineView(const QString& name, const QColor& color, bool useImage,
	int alphaImageChannel, const QStringList& filterParamNameList, int lineWidth) :
	MaskView(name, color, useImage, alphaImageChannel, filterParamNameList)
{
	setLineWidth(lineWidth);
}

LineView::~LineView()
{

}

void LineView::setLineWidth(int lineWidth)
{
	lineWidth_ = lineWidth;
}

int LineView::getLineWidth() const
{
	return lineWidth_;
}

} // namespace ICEODB_NS
