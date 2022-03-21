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
* Обработчик изображения:
*	- изменение контрастности, яркости
*	- масштаба – в процентах от исходного изображения
*	- перевод в изображения в оттенки серого
*	- поворот изображения вокруг центральной точки изображения
*/
class ICEIMAGE_EXPORT BaseImageBuilder : public QThread
{
	Q_OBJECT
public:
	static int cudaDeviceCount;
	static double scaledMul;
	static int maxChannelCount;

	/// Тип масшабирования
	enum ScaledType
	{
		SCALE_IGNORE_ASPECT = 0,	///< Масштабировать до размеров по высоте и ширине без сохранения пропорций
		SCALE_TO_HEIGHT,			///< Масштабировать по высоте с сохранением пропорций
		SCALE_TO_WIDTH,				///< Масштабировать по ширине с сохранением пропорций
		SCALE_KEEP_ASPECT			///< Масштабировать до размеров по высоте и ширине с сохранением пропорций
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
	* Конструктор
	* @param parent родительский объект
	*/
	BaseImageBuilder(QObject* parent = 0);

	/**
	* Деструктор
	*/
	virtual ~BaseImageBuilder();

	static QPair<size_t, size_t> queryMemory();
	static size_t calcGpuMemBlockSize(bool forFreeMem = false, int imageInGpu = 10);

	/**
	* Сформировать строку с размером памяти на видеокарточке
	* @param memSize размером памяти на видеокарточке в байтах
	*/
	static QString createMemoryString(quint64 memSize);

	template<typename T>
	static cv::Mat convertToBGRAMatImage(cv::Mat image);

	void setSync(bool sync);
	bool isGdalUsed() const;
	void apply(bool makeEmit = false, bool onlyVisibleRect = true);

	/**
	* Установить яркость для обработки изображения
	* @param brightness яркость для обработки изображения (значения от -100 до 100)
	*/
	void setBrightness(int brightness);

	/**
	* Получить яркость для обработки изображения
	* @return яркость для обработки изображения
	*/
	int getBrightness() const;

	/**
	* Установить контрастность для обработки изображения
	* @param contrast контрастность для обработки изображения (значения от -100 до 100)
	*/
	void setContrast(int contrast);

	/**
	* Получить контрастность для обработки изображения
	* @return контрастность для обработки изображения
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
	* Установить перевод изображения в оттенки серого
	* @param gray =TRUE - перевести изображение в оттенки серого
	* @param TODO comment
	*/
	void setGray(bool gray, ChannelType grayChannel = RGB_CHANNEL);

	/**
	* Получить признак перевода изображения в оттенки серого
	* @return признак перевода изображения в оттенки серого
	*/
	bool isGray() const;

	// TODO comment
	ChannelType getGrayChannel() const;

	/**
	* Установить поворот изображения вокруг центральной точки изображения
	* @param rotatedAngleDegree угол поворота изображения по часовой стрелке в градусах
	* @param keepSizeOnRotated сохранять размер изображения
	*/
	void setRotatedAngleDegree(double rotatedAngleDegree, bool keepSizeOnRotated = true);

	/**
	* Получить угол поворота изображения по часовой стрелке в градусах
	* @return угол поворота изображения по часовой стрелке в градусах
	*/
	double getRotatedAngleDegree() const;
	bool isKeepSizeOnRotated() const;

	/**
	* Установить коэффициент масштабирования изображения
	* @param xScaleFactor коэффициент масштабирования изображения по оси X
	* @param yScaleFactor коэффициент масштабирования изображения по оси Y
	* @param scaleType тип масшабирования изображения
	*/
	void setScaleFactor(double xScaleFactor, double yScaleFactor, ScaledType scaleType = SCALE_KEEP_ASPECT);
	void setScaleSize(int xSize, int ySize, ScaledType scaleType = SCALE_KEEP_ASPECT);

	/**
	* Получить коэффициент масштабирования изображения по оси X
	* @return коэффициент масштабирования изображения по оси X
	*/
	double getScaleFactorX() const;

	/**
	* Получить коэффициент масштабирования изображения по оси Y
	* @return коэффициент масштабирования изображения по оси Y
	*/
	double getScaleFactorY() const;

	/**
	* Получить тип масштабирования
	* @return тип масштабирования (см enum ScaledType)
	*/
	ScaledType getScaleType() const;

	/**
	* Получить размер отмасштабированного изображения по оси X
	* @return размер отмасштабированного изображения по оси X
	*/
	int getScaledSizeX() const;

	/**
	* Получить размер отмасштабированного изображения по оси Y
	* @return размер отмасштабированного изображения по оси Y
	*/
	int getScaledSizeY() const;

	/**
	* Установить коэффициент изменения размеров изображения
	* @param xResizeFactor коэффициент изменения размеров изображения по оси X
	* @param yResizeFactor коэффициент изменения размеров изображения по оси Y
	* @param scaleType тип изменения размеров изображения
	*/
	void setResizeFactor(double xResizeFactor, double yResizeFactor, ScaledType resizeType = SCALE_KEEP_ASPECT);

	/**
	* Установить прямоугольную область которая должна масштабироваться
	* @param rect область для масштабирования
	*             Если rect = QRect() мастабируется все изображение
	*/
	// TODO при масштабировании два раза создается одно и то же изображение. 1-ый раз - когда
	// вызывается ф-ия масштабирования setScaleFactor, 2-ой раз когда вызывается эта ф-ия.
	// Это надо исправлять
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
	* Применить к исходному изображению установленные параметры обработки изображения
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
	* Перевести изображение в оттенки серого
	* @param srcImage изображение для обработки
	* @param gray признак перевода изображения в оттенки серого
	//TODO comment
	* @return обработанное изображение
	*/
	template<typename T>
	static T applyGray(const T& srcImage, bool gray, ChannelType grayChannel = RGB_CHANNEL);

	static void calcLutMat(uchar* lut, int brightness, int contrast, int intensity);
	static int normTo100(int value);

	/**
	* Применить к изображению контрастность и яркость
	* @param srcImage изображение для обработки
	* @param brightness яркость для обработки изображения (значения от -100 до 100)
	* @param contrast контрастность для обработки изображения (значения от -100 до 100)
	* TODO
	* @return обработанное изображение
	*/
	template<typename T>
	static T applyBrightnessContrastIntensity(const T& srcImage, int brightness,
		int contrast, int intensity, int redBalance, int greenBalance, int blueBalance);

	/**
	* Применить к изображению поворот
	* @param srcImage изображение для обработки
	* @param angleDegrees угол поворота изображения по часовой стрелке в градусах
	* @param keepSizeOnRotated сохранять размеры окна при повороте
	* @return повернутое изображение
	*/
	template<typename T>
	static T applyRotateCenter(const T& srcImage, double angleDegrees, bool keepSizeOnRotated);

	/**
	* Применить к изображению масштабирование
	* @param srcImage изображение для обработки
	* @param xySize размер изображения
	* TODO comment
	* @return отмасштабированное изображение
	*/
	template<typename T>
	static T applyScaleSize(const T& srcImage, const QSize& xySize, int interpolation = cv::INTER_NEAREST);

	/**
	* Применить к изображению выделения прямоугольной области
	* @param srcImage изображение для обработки
	* @param rect параметры выделяемой часть изображения
	* @return выделенная часть изображения
	*/
	template<typename T>
	static T applySelectRect(const T& srcImage, const QRect& rect);

	/**
	* Конвертировать изображение в формат QImage
	* @param iplImage изображение
	* @return изображение в формате QImage
	*/
	static QImage toQImage(const cv::Mat& mat);
	static cv::Mat fromQImage(const QImage& image);

	/**
	* Получить результат обработки исходного изображения в формате QImage
	* @param imageType тип изображения (шаг обработки изображения)
	* @param rect параметры выделяемой часть изображения.
	*             Если rect = QRect(), то нет выдления части изображения
	* @param shallowCopy =TRUE - "поверхностная" копия
	*                    =FALSE - полная копия
	* @return результат обработки исходного изображения в формате QImage
	*/
	QImage toQImage(int imageType, const QRect& rect = QRect(), bool shallowCopy = false);

	/**
	* Получить конечный результат обработки исходного изображения в формате QImage
	* @param rect параметры выделяемой часть изображения.
	*             Если rect = QRect(), то нет выдления части изображения
	* @return конечный результат обработки исходного изображения в формате QImage
	*/
	QImage getScaledImage(const QRect& rect = QRect());

	virtual QColor getPointColor(int imageType, const QPoint& pos);

	void startCalc();

signals:
	/**
	* Изображение изменилось и готово для отображения
	*/
	void imageReady();
	void errorProcessed(QString errorMsg);

protected slots:
	void onFinished();

private:
	/**
	* Инициализировать переменные
	*/
	void init();

	/**
	* Очистить переменные
	*/
	void clear();

protected:
	QVector<cv::Mat> m_matImageVector;
	QVector<cv::Mat> m_matRectImageVector;

	bool m_usedParam;				///< Признак того что параметры уже использованы
	ParamValue<int> m_brightness;	///< Яркость для обработки изображения
	ParamValue<int> m_contrast;		///< Контрастность для обработки изображения
	ParamValue<int> m_intensity;	///< Интенсивность для обработки изображения
	ParamValue<int> m_redBalance;
	ParamValue<int> m_greenBalance;
	ParamValue<int> m_blueBalance;
	ParamValue<bool> m_gray;					///< Признак перевода изображения в оттенки серого
	ParamValue<ChannelType> m_grayChannel;		///< Номер канала по которому будет приобразовано изображение в оттенки серого
	ParamValue<double> m_rotatedAngleDegree;	///< Угол поворота изображения по часовой стрелке в градусах
	ParamValue<bool> m_keepSizeOnRotated;
	ParamValue<double> m_xScaleFactor;			///< Коэффициент масштабирования по оси X
	ParamValue<double> m_yScaleFactor;			///< Коэффициент масштабирования по оси Y
	ParamValue<ScaledType> m_scaleType;			///< Тип масшабирования
	
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
	bool m_sync;	///< Синхронное применение параметров обработки
	bool m_gdalUsed;	///TODO rename???
};

} // namespace ICEIMAGE_NS

#include "baseimagebuildertempl.h"

#endif // BASEIMAGEBUILDER_H
