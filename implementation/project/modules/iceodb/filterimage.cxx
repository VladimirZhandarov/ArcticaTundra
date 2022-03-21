#include "filterimage.hxx"

namespace ICEODB_NS
{

FilterImage::FilterImage() : FilterBaseType()
{
	m_typeName = typeid(FilterImage).name();
}

FilterImage::FilterImage(const QString& filename, const QByteArray& image, int width,
	int height, int widthStep, int format, bool makeSmaller) : FilterBaseType(),
	id_(0), filename_(filename.toStdString()), image_(image), width_(width), height_(height),
	widthStep_(widthStep), format_(format), makeSmaller_(makeSmaller)
{
	m_typeName = typeid(FilterImage).name();
}

FilterImage::~FilterImage()
{

}

void FilterImage::setId(unsigned int id)
{
	id_ = id;
}

unsigned int FilterImage::getId() const
{
	return id_;
}

QString FilterImage::getFilename() const
{
	return QString::fromStdString(filename_);
}

const QByteArray& FilterImage::getImage() const
{
	return image_;
}

int FilterImage::getWidth() const
{
	return width_;
}

int FilterImage::getHeight() const
{
	return height_;
}

int FilterImage::getWidthStep() const
{
	return widthStep_;
}

int FilterImage::getFormat() const
{
	return format_;
}

bool FilterImage::isMakeSmaller() const
{
	return makeSmaller_;
}

} // namespace ICEODB_NS
