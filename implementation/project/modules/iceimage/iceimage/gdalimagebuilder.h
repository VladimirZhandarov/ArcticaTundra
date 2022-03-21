#ifndef GDALIMAGEBUILDER_H
#define GDALIMAGEBUILDER_H

#include "imagebuilder.h"

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>
#include <QtCore/QProcess>

class GDALDataset;

class QProgressDialog;

namespace ICEODB_NS
{
	class MaskView;
	class LineView;
}

namespace ICEIMAGE_NS
{
	class ContourItemModel;
	class LineItemModel;

/**
* @class ImageBuilder
* ���������� �����������:
*	- ��������� �������������, �������
*	- �������� � � ��������� �� ��������� �����������
*	- ������� � ����������� � ������� ������
*	- ������� ����������� ������ ����������� ����� �����������
*/
class ICEIMAGE_EXPORT GdalImageBuilder : public ImageBuilder
{
	Q_OBJECT
	static QString homePath;
	static QString tempPath;
	static quint64 maxImageSize;
	static QWidget* mainWindow;

public:
	enum ImageType
	{
		COLORED_SOURCE_IMAGE = BaseImageBuilder::IMAGE_TYPE_SIZE,
		GRAY_IMAGE,
		CONTRAST_BRIGHTNESS_IMAGE,
		RESULT_IMAGE_NO_MASK,
		RESULT_IMAGE_AND_MASK,
		SCALED_RESULT_IMAGE,
		IMAGE_TYPE_SIZE
	};

	/**
	* �����������
	* @param parent ������������ ������
	*/
	GdalImageBuilder(QObject* parent = 0);

	/**
	* ����������� � ��������� ����������� �� �����
	* @param filename ��� ����� � ������������
	* @param parent ������������ ������
	*/
	GdalImageBuilder(const QString& filename, QObject* parent = 0);

	/**
	* ����������
	*/
	virtual ~GdalImageBuilder();

	static void setHomePath(const QString& arg);
	static void setTempPath(const QString& arg);
	static bool setMaxImageSize(quint64 arg);
	static quint64 getMaxImageSize();
	static void setMainWindow(QWidget* arg);

	/**
	* ��������� ����������� �� �����
	* @param filename ��� ����� � ������������
	* @return =TRUE - ���� �������� �������
	*/
	virtual bool load(const QString& filename, bool onlyNoGdalImage = false);

	/**
	* ��������� ����������� � �����
	* @param filename ��� �����
	* @return =TRUE - ���� �������� �������
	*/
	virtual bool save(const QString& filename, int quality);

	virtual bool createCutDownImage(const QString& settingsFilename);
	virtual void addCutDownImage(const cv::Mat& matImage, const QRect& rect);
	virtual void flushCutDownImage();

	GDALDataset* getWorkImageGdal(int imageType) const;

	virtual int getImageSizeWidth() const;
	virtual int getImageSizeHeight() const;

	template<typename T>
	void addMask(const T& maskImage, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView =
		std::tr1::shared_ptr<ICEODB_NS::MaskView>(), int skippedPointCount = 0);
	virtual void addMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView); //TODO refac

	virtual void flushMask(int index);

	virtual int getMaskListSize() const;

	virtual cv::Mat getMatMaskImage(int index, const QRect& rect = QRect());

	virtual QColor getPointColor(const QPoint& pos);

	int getUsedFilterCount() const;

protected:
	std::vector<cv::Point> getUserCreatedObjectContour(const QRect& rect = QRect()) const;
	virtual void applyGpu(bool onlyVisibleRect = true);
	virtual void applyCpu(bool onlyVisibleRect = true);

	cv::Mat createMatImageFromGdal(GDALDataset* dataset, const QRect& rect = QRect(),
		const QSize& resultSize = QSize());

public:
	virtual cv::Mat createMatCutUpImage(int imageType, const QRect& rect = QRect());

	cv::Mat createMatImageFromGdal(int imageType, const QRect& rect = QRect(),
		const QSize& resultSize = QSize(), int* usedIndex = 0);

protected:
	template<typename T>
	void applyData(bool onlyVisibleRect = true);

protected slots:
	void onCurrentContourDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void onCurrentLineDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

	void translateProcess_error(QProcess::ProcessError error);
	void translateProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);
	void translateProcess_readyReadStandardError();
	void translateProcess_readyReadStandardOutput();
	void translateProcess_started();
	void translateProcess_stateChanged(QProcess::ProcessState newState);

signals:
	void translateProcessFinished(const QString& cancelButtonText);
	void translateProcessStartTextReaded(const QString& text);
	void translateProcessValueChanged(int progress);

private:
	/**
	* ���������������� ����������
	*/
	void init();

	/**
	* �������� ����������
	*/
	void clear();

	QVector<GDALDataset*> m_gdalImageVector;
	QList<GDALDataset*> m_gdalMaskImageList;
	QList<GDALDataset*> m_gdalColorMaskImageList;

	QSize m_tileSize;
	QString m_translatedFilename;

	QProcess* m_translateProcess;
	QString m_fileSizeText;
	QProgressDialog* m_translateProgressDialog;
};

} // namespace ICEIMAGE_NS

#include "gdalimagebuildertempl.h"

#endif // GDALIMAGEBUILDER_H
