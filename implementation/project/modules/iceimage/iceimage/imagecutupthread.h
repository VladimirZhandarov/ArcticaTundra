#ifndef IMAGECUTUPTHREAD_H
#define IMAGECUTUPTHREAD_H

#include "iceimagedef.h"

#include <QtCore/QThread>

#include <QtCore/QSizeF>

#include "gdalimagebuilder.h"

namespace ICEIMAGE_NS
{

class ICEIMAGE_EXPORT ImageCutUpThread : public QThread
{
	Q_OBJECT
public:
	ImageCutUpThread(QSharedPointer<GdalImageBuilder> imageBuilder, const QString& outDir,
		const QSizeF& tileSize, int maxSteps, int jpegQuality, QObject* parent = 0);
	~ImageCutUpThread();

	bool isNoError() const;
	void stop();

protected:
	virtual void run();

signals:
	void maximumCalculated(int maximum);
	void valueChanged(int value);

private:
	QSharedPointer<GdalImageBuilder> m_imageBuilder;
	QString m_outDir;
	QSizeF m_tileSize;
	int m_maxSteps;
	int m_jpegQuality;
	bool m_errorFlag;
	bool m_stopFlag;
};

} // namespace ICEIMAGE_NS

#endif // IMAGECUTUPTHREAD_H
