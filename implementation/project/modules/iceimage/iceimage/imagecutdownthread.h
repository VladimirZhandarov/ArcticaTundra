#ifndef IMAGECUTDOWNTHREAD_H
#define IMAGECUTDOWNTHREAD_H

#include "iceimagedef.h"

#include <QtCore/QThread>

#include <QtCore/QFileInfo>

#include "gdalimagebuilder.h"
#include "global.h"

namespace ICEIMAGE_NS
{

class ICEIMAGE_EXPORT ImageCutDownThread : public QThread
{
	Q_OBJECT
public:
	ImageCutDownThread(QSharedPointer<GdalImageBuilder> imageBuilder,
		const QFileInfo& fileInfo, QObject* parent = 0);
	~ImageCutDownThread();

	bool isNoError() const;
	UNIT getUnit() const;
	double getDivK() const;

public slots:
	void stop();

protected:
	virtual void run();

signals:
	void maximumCalculated(int maximum);
	void valueChanged(int value);

private:
	void loadFromCsvFile(const QFileInfo& csvFileInfo,
		ICEIMAGE_NS::ContourItemModel* contourItemModel);

	void loadFromHtmlFile(const QFileInfo& htmlFileInfo,
		QList<ICEIMAGE_NS::ContourItemModel*>& contourItemModelList,
		const QSize& imageSize, bool firstRead);

	QFileInfo m_fileInfo;
	bool m_errorFlag;
	bool m_stopFlag;
	QSharedPointer<GdalImageBuilder> m_imageBuilder;
	UNIT m_unit;
	double m_divK;
};

} // namespace ICEIMAGE_NS

#endif // IMAGECUTDOWNTHREAD_H
