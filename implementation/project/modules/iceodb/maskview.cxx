#include "maskview.hxx"

namespace ICEODB_NS
{

MaskView::MaskView() : FilterBaseType()
{
	redChannel_ = 0;
	greenChannel_ = 0;
	blueChannel_ = 0;
	alphaChannel_ = 0;
	useImage_ = true;
	alphaChannel_ = 0;
	skippedPointCount_ = 0;
}

MaskView::MaskView(const QString& name, const QColor& color, bool useImage,
	int alphaImageChannel, const QStringList& filterParamNameList, int skippedPointCount) :
	FilterBaseType()
{
	name_ = name.toStdString();
	setColor(color);
	setUseImage(useImage);
	setAlphaImageChannel(alphaImageChannel);
	setFilterParamNameList(filterParamNameList);
	setSkippedPointCount(skippedPointCount);
}

MaskView::~MaskView()
{

}

void MaskView::setName(const QString& name)
{
	name_ = name.toStdString();
}

QString MaskView::getName() const
{
	return QString::fromStdString(name_);
}

void MaskView::setColor(const QColor& color)
{
	redChannel_ = color.red();
	greenChannel_ = color.green();
	blueChannel_ = color.blue();
	alphaChannel_ = color.alpha();
}

QColor MaskView::getColor() const
{
	return QColor(redChannel_, greenChannel_, blueChannel_, alphaChannel_);
}

void MaskView::setUseImage(bool useImage)
{
	useImage_ = useImage;
}

bool MaskView::isUseImage() const
{
	return useImage_;
}

void MaskView::setAlphaImageChannel(int alphaImageChannel)
{
	alphaImageChannel_ = alphaImageChannel;
}

int MaskView::getAlphaImageChannel() const
{
	return alphaImageChannel_;
}

void MaskView::setFilterParamNameList(const QStringList& filterParamNameList)
{
	if(!filterParamNameVector_.empty())
	{
		return;
	}
	for(int i=0; i<filterParamNameList.size(); i++)
	{
		filterParamNameVector_.push_back(filterParamNameList[i].toStdString());
	}
}

QStringList MaskView::getFilterParamNameList() const
{
	QStringList result;
	for(int i=0; i<filterParamNameVector_.size(); i++)
	{
		result.append(QString::fromStdString(filterParamNameVector_[i]));
	}
	return result;
}

void MaskView::setSkippedPointCount(int skippedPointCount)
{
	skippedPointCount_ = skippedPointCount;
}

int MaskView::getSkippedPointCount() const
{
	return skippedPointCount_;
}

bool MaskView::equal(MaskView* arg) const
{
	if(!arg)
	{
		return false;
	}
	return (QString::fromStdString(name_) == arg->getName() && QColor(redChannel_,
		greenChannel_, blueChannel_, alphaChannel_) == arg->getColor() &&
		useImage_ == arg->isUseImage() && alphaImageChannel_ == arg->getAlphaImageChannel());
}

} // namespace ICEODB_NS
