#ifndef BASEIMAGEBUILDER_H
#define BASEIMAGEBUILDER_H

#include "iceimagedef.h"

#include "matfunction.h"
#include "paramvalue.h"

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtCore/QPair>

#include <QtGui/QImage>
#include <QtGui/QColor>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning(disable: 4985)
#endif
#include <opencv2/gpu/gpu.hpp>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#include <boost/tr1/memory.hpp>

namespace ICEIMAGE_NS
{

/**
* @class BaseImageBuilder
* ���������� �����������:
*	- ��������� �������������, �������
*	- �������� � � ��������� �� ��������� �����������
*	- ������� � ����������� � ������� ������
*	- ������� ����������� ������ ����������� ����� �����������
*/
class ICEIMAGE_EXPORT BaseImageBuilder : public QThread
{
	Q_OBJECT
public:
	static int cudaDeviceCount;
	static double scaledMul;
	static int maxChannelCount;

	/// ��� ��������������
	enum ScaledType
	{
		SCALE_IGNORE_ASPECT = 0,	///< �������������� �� �������� �� ������ � ������ ��� ���������� ���������
		SCALE_TO_HEIGHT,			///< �������������� �� ������ � ����������� ���������
		SCALE_TO_WIDTH,				///< �������������� �� ������ � ����������� ���������
		SCALE_KEEP_ASPECT			///< �������������� �� �������� �� ������ � ������ � ����������� ���������
	};

	enum ImageType
	{
		SOURCE_IMAGE = 0,
		IMAGE_TYPE_SIZE
	};

	enum ChannelType
	{
		RGB_CHANNEL = 0,
		R_CHANNEL,
		G_CHANNEL,
		B_CHANNEL
	};

	/**
	* �����������
	* @param parent ������������ ������
	*/
	BaseImageBuilder(QObject* parent = 0);

	/**
	* ����������
	*/
	virtual ~BaseImageBuilder();

	static QPair<size_t, size_t> queryMemory();
	static size_t calcGpuMemBlockSize(bool forFreeMem = false, int imageInGpu = 10);

	/**
	* ������������ ������ � �������� ������ �� �������������
	* @param memSize �������� ������ �� ������������� � ������
	*/
	static QString createMemoryString(quint64 memSize);

	template<typename T>
	static cv::Mat convertToBGRAMatImage(cv::Mat image);

	void setSync(bool sync);
	bool isGdalUsed() const;
	void apply(bool makeEmit = false, bool onlyVisibleRect = true);

	/**
	* ���������� ������� ��� ��������� �����������
	* @param brightness ������� ��� ��������� ����������� (�������� �� -100 �� 100)
	*/
	void setBrightness(int brightness);

	/**
	* �������� ������� ��� ��������� �����������
	* @return ������� ��� ��������� �����������
	*/
	int getBrightness() const;

	/**
	* ���������� ������������� ��� ��������� �����������
	* @param contrast ������������� ��� ��������� ����������� (�������� �� -100 �� 100)
	*/
	void setContrast(int contrast);

	/**
	* �������� ������������� ��� ��������� �����������
	* @return ������������� ��� ��������� �����������
	*/
	int getContrast() const;

	void setIntensity(int intensity);
	int getIntensity() const;

	void setRedBalance(int value);
	int getRedBalance() const;

	void setGreenBalance(int value);
	int getGreenBalance() const;

	void setBlueBalance(int value);
	int getBlueBalance() const;

	/**
	* ���������� ������� ����������� � ������� ������
	* @param gray =TRUE - ��������� ����������� � ������� ������
	* @param TODO comment
	*/
	void setGray(bool gray, ChannelType grayChannel = RGB_CHANNEL);

	/**
	* �������� ������� �������� ����������� � ������� ������
	* @return ������� �������� ����������� � ������� ������
	*/
	bool isGray() const;

	// TODO comment
	ChannelType getGrayChannel() const;

	/**
	* ���������� ������� ����������� ������ ����������� ����� �����������
	* @param rotatedAngleDegree ���� �������� ����������� �� ������� ������� � ��������
	* @param keepSizeOnRotated ��������� ������ �����������
	*/
	void setRotatedAngleDegree(double rotatedAngleDegree, bool keepSizeOnRotated = true);

	/**
	* �������� ���� �������� ����������� �� ������� ������� � ��������
	* @return ���� �������� ����������� �� ������� ������� � ��������
	*/
	double getRotatedAngleDegree() const;
	bool isKeepSizeOnRotated() const;

	/**
	* ���������� ����������� ��������������� �����������
	* @param xScaleFactor ����������� ��������������� ����������� �� ��� X
	* @param yScaleFactor ����������� ��������������� ����������� �� ��� Y
	* @param scaleType ��� �������������� �����������
	*/
	void setScaleFactor(double xScaleFactor, double yScaleFactor, ScaledType scaleType = SCALE_KEEP_ASPECT);
	void setScaleSize(int xSize, int ySize, ScaledType scaleType = SCALE_KEEP_ASPECT);

	/**
	* �������� ����������� ��������������� ����������� �� ��� X
	* @return ����������� ��������������� ����������� �� ��� X
	*/
	double getScaleFactorX() const;

	/**
	* �������� ����������� ��������������� ����������� �� ��� Y
	* @return ����������� ��������������� ����������� �� ��� Y
	*/
	double getScaleFactorY() const;

	/**
	* �������� ��� ���������������
	* @return ��� ��������������� (�� enum ScaledType)
	*/
	ScaledType getScaleType() const;

	/**
	* �������� ������ ������������������� ����������� �� ��� X
	* @return ������ ������������������� ����������� �� ��� X
	*/
	int getScaledSizeX() const;

	/**
	* �������� ������ ������������������� ����������� �� ��� Y
	* @return ������ ������������������� ����������� �� ��� Y
	*/
	int getScaledSizeY() const;

	/**
	* ���������� ����������� ��������� �������� �����������
	* @param xResizeFactor ����������� ��������� �������� ����������� �� ��� X
	* @param yResizeFactor ����������� ��������� �������� ����������� �� ��� Y
	* @param scaleType ��� ��������� �������� �����������
	*/
	void setResizeFactor(double xResizeFactor, double yResizeFactor, ScaledType resizeType = SCALE_KEEP_ASPECT);

	/**
	* ���������� ������������� ������� ������� ������ ����������������
	* @param rect ������� ��� ���������������
	*             ���� rect = QRect() ������������� ��� �����������
	*/
	// TODO ��� ��������������� ��� ���� ��������� ���� � �� �� �����������. 1-�� ��� - �����
	// ���������� �-�� ��������������� setScaleFactor, 2-�� ��� ����� ���������� ��� �-��.
	// ��� ���� ����������
	void setScaleRegionRect(const QRect& rect = QRect());
	const QRect& getScaleRegionRect() const;

	virtual int getImageSizeWidth() const;
	virtual int getImageSizeHeight() const;

	QRect adjustedRect(const QRect& rect);

	template<typename T>
	T getMatImage(int imageType);

protected:
	cv::Mat getWorkImageMat(int imageType) const;

	/**
	* ��������� � ��������� ����������� ������������� ��������� ��������� �����������
	*/
	virtual void run();

	virtual void applyGpu(bool onlyVisibleRect = true) = 0;
	virtual void applyCpu(bool onlyVisibleRect = true) = 0;

	template<typename T>
	T resize(const T& srcImage);

	//TODO refac
	template<typename T>
	QRect calcRectForScreen(const T& srcImage, double xFactor, double yFactor, int type);
	QRect calcRectForScreen(double xFactor, double yFactor, int type,
		const QPoint& leftTopPointForScale, const QSize& screenSize);
public:
	template<typename T>
	static QSize calcSizeForScaled(const T& srcImage, double xFactor, double yFactor,
		ScaledType type);

	/**
	* ��������� ����������� � ������� ������
	* @param srcImage ����������� ��� ���������
	* @param gray ������� �������� ����������� � ������� ������
	//TODO comment
	* @return ������������ �����������
	*/
	template<typename T>
	static T applyGray(const T& srcImage, bool gray, ChannelType grayChannel = RGB_CHANNEL);

	static void calcLutMat(uchar* lut, int brightness, int contrast, int intensity);
	static int normTo100(int value);

	/**
	* ��������� � ����������� ������������� � �������
	* @param srcImage ����������� ��� ���������
	* @param brightness ������� ��� ��������� ����������� (�������� �� -100 �� 100)
	* @param contrast ������������� ��� ��������� ����������� (�������� �� -100 �� 100)
	* TODO
	* @return ������������ �����������
	*/
	template<typename T>
	static T applyBrightnessContrastIntensity(const T& srcImage, int brightness,
		int contrast, int intensity, int redBalance, int greenBalance, int blueBalance);

	/**
	* ��������� � ����������� �������
	* @param srcImage ����������� ��� ���������
	* @param angleDegrees ���� �������� ����������� �� ������� ������� � ��������
	* @param keepSizeOnRotated ��������� ������� ���� ��� ��������
	* @return ���������� �����������
	*/
	template<typename T>
	static T applyRotateCenter(const T& srcImage, double angleDegrees, bool keepSizeOnRotated);

	/**
	* ��������� � ����������� ���������������
	* @param srcImage ����������� ��� ���������
	* @param xySize ������ �����������
	* TODO comment
	* @return ������������������ �����������
	*/
	template<typename T>
	static T applyScaleSize(const T& srcImage, const QSize& xySize, int interpolation = cv::INTER_NEAREST);

	/**
	* ��������� � ����������� ��������� ������������� �������
	* @param srcImage ����������� ��� ���������
	* @param rect ��������� ���������� ����� �����������
	* @return ���������� ����� �����������
	*/
	template<typename T>
	static T applySelectRect(const T& srcImage, const QRect& rect);

	/**
	* �������������� ����������� � ������ QImage
	* @param iplImage �����������
	* @return ����������� � ������� QImage
	*/
	static QImage toQImage(const cv::Mat& mat);
	static cv::Mat fromQImage(const QImage& image);

	/**
	* �������� ��������� ��������� ��������� ����������� � ������� QImage
	* @param imageType ��� ����������� (��� ��������� �����������)
	* @param rect ��������� ���������� ����� �����������.
	*             ���� rect = QRect(), �� ��� �������� ����� �����������
	* @param shallowCopy =TRUE - "�������������" �����
	*                    =FALSE - ������ �����
	* @return ��������� ��������� ��������� ����������� � ������� QImage
	*/
	QImage toQImage(int imageType, const QRect& rect = QRect(), bool shallowCopy = false);

	/**
	* �������� �������� ��������� ��������� ��������� ����������� � ������� QImage
	* @param rect ��������� ���������� ����� �����������.
	*             ���� rect = QRect(), �� ��� �������� ����� �����������
	* @return �������� ��������� ��������� ��������� ����������� � ������� QImage
	*/
	QImage getScaledImage(const QRect& rect = QRect());

	virtual QColor getPointColor(int imageType, const QPoint& pos);

	void startCalc();

signals:
	/**
	* ����������� ���������� � ������ ��� �����������
	*/
	void imageReady();
	void errorProcessed(QString errorMsg);

protected slots:
	void onFinished();

private:
	/**
	* ���������������� ����������
	*/
	void init();

	/**
	* �������� ����������
	*/
	void clear();

protected:
	QVector<cv::Mat> m_matImageVector;
	QVector<cv::Mat> m_matRectImageVector;

	bool m_usedParam;				///< ������� ���� ��� ��������� ��� ������������
	ParamValue<int> m_brightness;	///< ������� ��� ��������� �����������
	ParamValue<int> m_contrast;		///< ������������� ��� ��������� �����������
	ParamValue<int> m_intensity;	///< ������������� ��� ��������� �����������
	ParamValue<int> m_redBalance;
	ParamValue<int> m_greenBalance;
	ParamValue<int> m_blueBalance;
	ParamValue<bool> m_gray;					///< ������� �������� ����������� � ������� ������
	ParamValue<ChannelType> m_grayChannel;		///< ����� ������ �� �������� ����� ������������� ����������� � ������� ������
	ParamValue<double> m_rotatedAngleDegree;	///< ���� �������� ����������� �� ������� ������� � ��������
	ParamValue<bool> m_keepSizeOnRotated;
	ParamValue<double> m_xScaleFactor;			///< ����������� ��������������� �� ��� X
	ParamValue<double> m_yScaleFactor;			///< ����������� ��������������� �� ��� Y
	ParamValue<ScaledType> m_scaleType;			///< ��� ��������������
	
	ParamValue<QPoint> m_leftTopPointForScale;
	QPoint m_leftTopPointInScaled;
	QRect m_scaleRegionRect;

	double m_xResizeFactor;
	double m_yResizeFactor;
	ScaledType m_resizeType;
	QSize m_screenSize;
	QSize m_sourceImageSize;

	QMutex m_mutex;
	QMutex m_imageMutex;
	QMutex m_initMutex;
	bool m_sync;	///< ���������� ���������� ���������� ���������
	bool m_gdalUsed;	///TODO rename???
};

} // namespace ICEIMAGE_NS

#include "baseimagebuildertempl.h"

#endif // BASEIMAGEBUILDER_H
