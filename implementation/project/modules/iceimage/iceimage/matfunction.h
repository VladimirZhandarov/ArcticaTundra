#ifndef MATFUNCTION_H
#define MATFUNCTION_H

#include "iceimagedef.h"

#include <QtCore/QMutex>

#include <vector>

#include <cuda.h>
#include <cuda_runtime.h>

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning(disable: 4985)
#endif
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/stream_accessor.hpp>
#ifdef _MSC_VER
#pragma warning (pop)
#endif

namespace ICEIMAGE_NS
{

template<typename T>
class MatFunction
{
public:
	static void split(const T& /*src*/, std::vector<T>& /*dst*/)
	{
		
	}

	static void LUT(const T& /*src*/, const cv::Mat& /*lut*/, T& /*dst*/)
	{

	}

	static void merge(const std::vector<T> /*src*/, T& /*dst*/)
	{

	}

	static void cvtColor(const T& /*src*/, T& /*dst*/, int /*code*/, int /*dcn = 0*/)
	{

	}

	static void resize(const T& /*src*/, T& /*dst*/, cv::Size /*dsize*/, double /*fx = 0*/,
		double /*fy = 0*/, int /*interpolation = cv::INTER_NEAREST*/)
	{

	}

	static void rotate(const T& /*src*/, T& /*dst*/, cv::Size /*dsize*/, double /*angle*/,
		double /*xShift = 0*/, double /*yShift = 0*/, int /*interpolation = cv::INTER_NEAREST*/)
	{

	}

	static void overlayImage(const T& /*background*/, const T& /*foreground*/, T& /*output*/)
	{

	}

	static void setAlpha(T& /*image*/, int /*alpha*/, const T& /*mask*/)
	{

	}

	static cv::Mat toMat(const T& /*arg*/)
	{
		return cv::Mat();
	}

	static T fromMat(const cv::Mat& /*arg*/)
	{
		return T();
	}
};

namespace device
{
	template <typename T>
	void LUT(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& lut,
		const cv::gpu::PtrStepSzb& dst, cudaStream_t stream);
}

template<>
class MatFunction<cv::gpu::GpuMat>
{
public:
	static void split(const cv::gpu::GpuMat& src, std::vector<cv::gpu::GpuMat>& dst)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::split(src, dst);
	}

	static void LUT(const cv::gpu::GpuMat& src, const cv::Mat& lut, cv::gpu::GpuMat& dst)
	{
		typedef void (*func_t)(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& lut,
			const cv::gpu::PtrStepSzb& dst, cudaStream_t stream);
		static const func_t funcs[6][4] =
		{
			{device::LUT<uchar>       , 0 /*device::LUT<uchar2>*/ , 0 /*device::LUT<uchar3>*/ , 0 /*device::LUT<uchar4>*/  },
			{0 /*device::LUT<schar>*/ , 0 /*device::LUT<char2>*/  , 0 /*device::LUT<char3>*/  , 0 /*device::LUT<char4>*/   },
			{0 /*device::LUT<ushort>*/, 0 /*device::LUT<ushort2>*/, 0 /*device::LUT<ushort3>*/, 0 /*device::LUT<ushort4>*/ },
			{0 /*device::LUT<short>*/ , 0 /*device::LUT<short2>*/ , 0 /*device::LUT<short3>*/ , 0 /*device::LUT<short4>*/  },
			{0 /*device::LUT<int>*/   , 0 /*device::LUT<int2>*/   , 0 /*device::LUT<int3>*/   , 0 /*device::LUT<int4>*/    },
			{0 /*device::LUT<float>*/ , 0 /*device::LUT<float2>*/ , 0 /*device::LUT<float3>*/ , 0 /*device::LUT<float4>*/  }
		};

		CV_Assert(src.depth() <= CV_8UC1 && src.channels() == 1);
		CV_Assert(lut.rows == 1 && lut.cols == 256 && lut.depth() == CV_8UC1 && lut.channels() == 1);
		dst.create(src.size(), src.type());

		const func_t func = funcs[src.depth()][src.channels() - 1];

		if(!func)
		{
			CV_Error(CV_StsUnsupportedFormat, "Unsupported combination of source and destination types");
		}
		cv::gpu::GpuMat lutGpu;
		lutGpu.upload(lut);
		func(src, lutGpu, dst, cv::gpu::StreamAccessor::getStream(cv::gpu::Stream::Null()));
	}

	static void merge(const std::vector<cv::gpu::GpuMat> src, cv::gpu::GpuMat& dst)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::merge(src, dst);
	}

	static void cvtColor(const cv::gpu::GpuMat& src, cv::gpu::GpuMat& dst, int code,
		int dcn = 0)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::cvtColor(src, dst, code, dcn);
	}

	static void resize(const cv::gpu::GpuMat& src, cv::gpu::GpuMat& dst, cv::Size dsize,
		double fx = 0, double fy = 0, int interpolation = cv::INTER_NEAREST)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::resize(src, dst, dsize, fx, fy, interpolation);
	}

	static void rotate(const cv::gpu::GpuMat& src, cv::gpu::GpuMat& dst, cv::Size dsize,
		double angle, double xShift = 0, double yShift = 0, int interpolation = cv::INTER_NEAREST)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::rotate(src, dst, dsize, angle, xShift, yShift, interpolation);
	}

	static void overlayImage(const cv::gpu::GpuMat& background, const cv::gpu::GpuMat& foreground,
		cv::gpu::GpuMat& output)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::alphaComp(foreground, background, output, cv::gpu::ALPHA_OVER);
	}

	static void setAlpha(cv::gpu::GpuMat& image, int alpha, const cv::gpu::GpuMat& mask)
	{
		std::vector<cv::gpu::GpuMat> imageVector;
		split(image, imageVector);
		if(imageVector.size() == 4)
		{
			cv::gpu::GpuMat alphaImage = cv::gpu::GpuMat(mask.size(), mask.type(), cv::Scalar(alpha));
			cv::gpu::bitwise_and(alphaImage, mask, imageVector[3]);
			merge(imageVector, image);
		}
	}

	static cv::Mat toMat(const cv::gpu::GpuMat& arg)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::Mat result;
		arg.download(result);
		return result;
	}

	static cv::gpu::GpuMat fromMat(const cv::Mat& arg)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::gpu::GpuMat result;
		result.upload(arg);
		return result;
	}
};

template<>
class MatFunction<cv::Mat>
{
public:
	static void split(const cv::Mat& src, std::vector<cv::Mat>& dst)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::split(src, dst);
	}

	static void LUT(const cv::Mat& src, const cv::Mat& lut, cv::Mat& dst)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::LUT(src, lut, dst);
	}

	static void merge(const std::vector<cv::Mat> src, cv::Mat& dst)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::merge(src, dst);
	}

	static void cvtColor(const cv::Mat& src, cv::Mat& dst, int code, int dcn = 0)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::cvtColor(src, dst, code, dcn);
	}

	static void resize(const cv::Mat& src, cv::Mat& dst, cv::Size dsize,
		double fx = 0, double fy = 0, int interpolation = cv::INTER_NEAREST)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		cv::resize(src, dst, dsize, fx, fy, interpolation);
	}

	static void rotate(const cv::Mat& src, cv::Mat& dst, cv::Size dsize, double angle,
		double xShift = 0, double yShift = 0, int interpolation = cv::INTER_NEAREST)
	{
		Q_UNUSED(src);
		Q_UNUSED(dst);
		Q_UNUSED(dsize);
		Q_UNUSED(angle);
		Q_UNUSED(xShift);
		Q_UNUSED(yShift);
		Q_UNUSED(interpolation);
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		//TODO rotate function
//		cv::rotate(src, dst, dsize, angle, xShift, yShift, interpolation);
	}


	static void overlayImage(const cv::Mat& background, const cv::Mat& foreground, cv::Mat& output)
	{
		background.copyTo(output);
		for(int y=0; y<background.rows; y++)
		{
			if(y >= foreground.rows)
			{
				break;
			}
			for(int x=0; x<background.cols; x++)
			{
				if(x >= foreground.cols)
				{
					break;
				}
				double opacity = ((double)foreground.data[y * foreground.step + x * foreground.channels() + 3]) / 255.;

				for(int c=0; opacity>0 && c<output.channels(); c++)
				{
					unsigned char foregroundPx = foreground.data[y * foreground.step + x * foreground.channels() + c];
					unsigned char backgroundPx = background.data[y * background.step + x * background.channels() + c];
					output.data[y * output.step + output.channels() * x + c] = backgroundPx * (1.-opacity) + foregroundPx * opacity;
				}
			}
		}
	}

	static void setAlpha(cv::Mat& image, int alpha, const cv::Mat& mask)
	{
		std::vector<cv::Mat> imageVector;
		split(image, imageVector);
		if(imageVector.size() == 4)
		{
			cv::Mat alphaImage = cv::Mat(mask.size(), mask.type(), cv::Scalar(alpha));
			cv::bitwise_and(alphaImage, mask, imageVector[3]);
			merge(imageVector, image);
		}
	}

	static cv::Mat toMat(const cv::Mat& arg)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		return arg;
	}

	static cv::Mat fromMat(const cv::Mat& arg)
	{
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		return arg;
	}
};

} // namespace ICEIMAGE_NS

#endif //MATFUNCTION_H
