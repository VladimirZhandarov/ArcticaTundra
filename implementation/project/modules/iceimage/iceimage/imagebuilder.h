#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include "baseimagebuilder.h"

#include <QtCore/QVariant>
#include <QtCore/QModelIndex>

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
* Обработчик изображения:
*	- изменение контрастности, яркости
*	- масштаба – в процентах от исходного изображения
*	- перевод в изображения в оттенки серого
*	- поворот изображения вокруг центральной точки изображения
*/
class ICEIMAGE_EXPORT ImageBuilder : public BaseImageBuilder
{
	Q_OBJECT
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
	* Конструктор
	* @param parent родительский объект
	*/
	ImageBuilder(QObject* parent = 0);

	/**
	* Конструктор с загрузкой изображения из файла
	* @param filename имя файла с изображением
	* @param parent родительский объект
	*/
	ImageBuilder(const QString& filename, QObject* parent = 0);

	ImageBuilder(const QImage& image, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView,
		QObject* parent = 0);

	/**
	* Деструктор
	*/
	virtual ~ImageBuilder();

	/**
	* Загрузить изображение из файла
	* @param filename имя файла с изображением
	* @return =TRUE - файл загружен успешно
	*/
	virtual bool load(const QString& filename, bool onlyNoGdalImage = false);

	bool load(const QImage& image);

	void setBackgroundImageMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& backgroundImageMaskView);

	/**
	* Сохранить изображение в файла
	* @param filename имя файла
	* @return =TRUE - файл сохранен успешно
	*/
	virtual bool save(const QString& filename, int quality);

	virtual bool createCutDownImage(const QString& settingsFilename);
	virtual void addCutDownImage(const cv::Mat& matImage, const QRect& rect);
	virtual void flushCutDownImage();

	const QString& getFilename() const;

	virtual int getImageSizeWidth() const;
	virtual int getImageSizeHeight() const;

	template<typename T>
	void addMask(const T& maskImage, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView =
		std::tr1::shared_ptr<ICEODB_NS::MaskView>(), int skippedPointCount = 0);
	virtual void addMaskView(const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView); //TODO refac

	virtual void flushMask(int index);

	QString getMaskName(int index) const;
	void showMask(int index, bool show);
	virtual int getMaskListSize() const;

	//TODO refac
	std::tr1::shared_ptr<ICEODB_NS::MaskView> getColorMaskView(int index) const;

	void setContours(int index, const std::vector<std::vector<cv::Point> >& contours,
		const cv::Mat& matImage, const QString& filterName, const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView =
		std::tr1::shared_ptr<ICEODB_NS::MaskView>(), const std::tr1::shared_ptr<ICEODB_NS::LineView>& lineView =
		std::tr1::shared_ptr<ICEODB_NS::LineView>(), int minSizeValue = 0, int minPerimeterValue = 0,
		double minKFormValue = 0.0, double divK = 1.0, int unit = 0);
	
	void setCurrentContoursIndex(int index);
	int getCurrentContoursIndex() const;
	void setCurrentContourColor(const QColor& color);
	QString getContoursName(int index) const;
	void showContours(int index, bool show);
	int getContourItemModelListSize() const;
	int getNotNullContourItemModelListSize() const;
	ContourItemModel* getContourItemModel(int index) const;
	ContourItemModel* createEmptyContourItemModel(int matImageCols, int matImageRows);

	void addLines(const std::vector<cv::Vec4i>& lines,
		const std::tr1::shared_ptr<ICEODB_NS::MaskView>& maskView = std::tr1::shared_ptr<ICEODB_NS::MaskView>());

	void setCurrentLinesIndex(int index);
	void setCurrentLineColor(const QColor& color);
	QString getLinesName(int index) const;
	void showLines(int index, bool show);
	int getLineItemModelListSize() const;
	LineItemModel* getLineItemModel(int index) const;

	virtual cv::Mat getMatMaskImage(int index, const QRect& rect = QRect());

	void setCursorPosition(const QPoint& pos);
	const QPoint& getCursorPosition() const;
	bool setCurrentContourByPos(const QPoint& pos);

	QImage getUserCreatedObjectQImage();
	void setUserCreatedObject(const QVariant& selectedArea = QVariant());
	const QVariant& getUserCreatedObject() const;

	QRect getUserCreatedObjectRect() const;
	bool isClosedUserCreatedObject() const;

	virtual QColor getPointColor(const QPoint& pos);

	void copyData(const QSharedPointer<ICEIMAGE_NS::ImageBuilder>& prevImageBuilder);
	int getUsedFilterCount() const;

	void setAddedTextListToBottomRight(const QStringList& addedTextList);

	virtual cv::Mat createMatCutUpImage(int imageType, const QRect& rect = QRect());

protected:
	std::vector<cv::Point> getUserCreatedObjectContour(const QRect& rect = QRect()) const;
	virtual void applyGpu(bool onlyVisibleRect = true);
	virtual void applyCpu(bool onlyVisibleRect = true);

	template<typename T>
	void applyData(bool onlyVisibleRect = true);

protected slots:
	void onCurrentContourDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	void onCurrentLineDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

private:
	/**
	* Инициализировать переменные
	*/
	void init();

protected:
	/**
	* Очистить переменные
	*/
	void clear();

	QString m_filename;

	QList<cv::Mat> m_matColorMaskImageList;
	QList<cv::Mat> m_matMaskImageList;

	std::tr1::shared_ptr<ICEODB_NS::MaskView> m_backgroundImageMaskView;

	QList<std::tr1::shared_ptr<ICEODB_NS::MaskView> > m_colorMaskViewList;

	QList<bool> m_colorMaskShowList;
	QList<ContourItemModel*> m_contourItemModelList;

	int m_currentContoursIndex;
	QColor m_currentContourColor;

	QList<LineItemModel*> m_lineItemModelList;
	int m_currentLinesIndex;
	QColor m_currentLineColor;

	QPoint m_cursorPosition;
	QVariant m_selectedArea;
	QSize m_resultImageSize;

	QStringList m_addedTextList;
};

} // namespace ICEIMAGE_NS

#include "imagebuildertempl.h"

#endif // IMAGEBUILDER_H
