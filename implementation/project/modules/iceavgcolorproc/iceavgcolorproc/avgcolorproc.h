#ifndef AVGCOLORPROC_H
#define AVGCOLORPROC_H

#include <cuda.h>
#include <cuda_runtime.h>

#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/stream_accessor.hpp>

namespace ICEAVGCOLORPROC_NS
{

namespace device
{
template <typename T>
void avgColorProc(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& dst,
	float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
	float maxLum, float minLum, cudaStream_t stream);

template <typename T>
void avgLumProc(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& dst,
	float maxLum, float minLum, cudaStream_t stream);
}

template<typename T>
class Proc
{
public:
	static void avgColor(const T& /*src*/, T& /*dst*/, const cv::Scalar& /*colorF*/,
		const cv::Scalar& /*plusF*/, const cv::Scalar& /*minusF*/, float /*maxLum*/,
		float /*minLum*/)
	{

	}

	static void avgLum(const T& /*src*/, T& /*dst*/, float /*maxLum*/, float /*minLum*/)
	{

	}
};

template<>
class Proc<cv::gpu::GpuMat>
{
public:
	static void avgColor(const cv::gpu::GpuMat& src, cv::gpu::GpuMat& dst, const cv::Scalar& colorF,
		const cv::Scalar& plusF, const cv::Scalar& minusF, float maxLum, float minLum)
	{
		typedef void (*func_t)(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& dst,
			float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
			float maxLum, float minLum, cudaStream_t stream);
		static const func_t funcs[6][4] =
		{
			{0 /*device::avgColorProc<uchar>*/ , 0 /*device::avgColorProc<uchar2>*/ , 0 /*device::avgColorProc<uchar3>*/ , device::avgColorProc<uchar4>        },
			{0 /*device::avgColorProc<schar>*/ , 0 /*device::avgColorProc<char2>*/  , 0 /*device::avgColorProc<char3>*/  , 0 /*device::avgColorProc<char4>*/   },
			{0 /*device::avgColorProc<ushort>*/, 0 /*device::avgColorProc<ushort2>*/, 0 /*device::avgColorProc<ushort3>*/, 0 /*device::avgColorProc<ushort4>*/ },
			{0 /*device::avgColorProc<short>*/ , 0 /*device::avgColorProc<short2>*/ , 0 /*device::avgColorProc<short3>*/ , 0 /*device::avgColorProc<short4>*/  },
			{0 /*device::avgColorProc<int>*/   , 0 /*device::avgColorProc<int2>*/   , 0 /*device::avgColorProc<int3>*/   , 0 /*device::avgColorProc<int4>*/    },
			{0 /*device::avgColorProc<float>*/ , 0 /*device::avgColorProc<float2>*/ , 0 /*device::avgColorProc<float3>*/ , 0 /*device::avgColorProc<float4>*/  }
		};

		CV_Assert(src.depth() <= CV_32F && src.channels() <= 4);
		dst.create(src.size(), CV_MAKETYPE(CV_8U, 1));

		const func_t func = funcs[src.depth()][src.channels() - 1];

		if(!func)
		{
			CV_Error(CV_StsUnsupportedFormat, "Unsupported combination of source and destination types");
		}
		float minX = (float)colorF[2] - (float)minusF[2];
		float minY = (float)colorF[1] - (float)minusF[1];
		float minZ = (float)colorF[0] - (float)minusF[0];
		float maxX = (float)colorF[2] + (float)plusF[2];
		float maxY = (float)colorF[1] + (float)plusF[1];
		float maxZ = (float)colorF[0] + (float)plusF[0];

		func(src, dst, minX, minY, minZ, maxX, maxY, maxZ, maxLum, minLum,
			cv::gpu::StreamAccessor::getStream(cv::gpu::Stream::Null()));
	}

	static void avgLum(const cv::gpu::GpuMat& src, cv::gpu::GpuMat& dst, float maxLum,
		float minLum)
	{
		typedef void (*func_t)(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& dst,
			float maxLum, float minLum, cudaStream_t stream);
		static const func_t funcs[6][4] =
		{
			{0 /*device::avgLumProc<uchar>*/ , 0 /*device::avgLumProc<uchar2>*/ , 0 /*device::avgLumProc<uchar3>*/ , device::avgLumProc<uchar4>        },
			{0 /*device::avgLumProc<schar>*/ , 0 /*device::avgLumProc<char2>*/  , 0 /*device::avgLumProc<char3>*/  , 0 /*device::avgLumProc<char4>*/   },
			{0 /*device::avgLumProc<ushort>*/, 0 /*device::avgLumProc<ushort2>*/, 0 /*device::avgLumProc<ushort3>*/, 0 /*device::avgLumProc<ushort4>*/ },
			{0 /*device::avgLumProc<short>*/ , 0 /*device::avgLumProc<short2>*/ , 0 /*device::avgLumProc<short3>*/ , 0 /*device::avgLumProc<short4>*/  },
			{0 /*device::avgLumProc<int>*/   , 0 /*device::avgLumProc<int2>*/   , 0 /*device::avgLumProc<int3>*/   , 0 /*device::avgLumProc<int4>*/    },
			{0 /*device::avgLumProc<float>*/ , 0 /*device::avgLumProc<float2>*/ , 0 /*device::avgLumProc<float3>*/ , 0 /*device::avgLumProc<float4>*/  }
		};

		CV_Assert(src.depth() <= CV_32F && src.channels() <= 4);
		dst.create(src.size(), CV_MAKETYPE(CV_8U, 1));

		const func_t func = funcs[src.depth()][src.channels() - 1];

		if(!func)
		{
			CV_Error(CV_StsUnsupportedFormat, "Unsupported combination of source and destination types");
		}
		func(src, dst, maxLum, minLum, cv::gpu::StreamAccessor::getStream(cv::gpu::Stream::Null()));
	}
};

template<>
class Proc<cv::Mat>
{
public:
	static void avgColor(const cv::Mat& src, cv::Mat& dst, const cv::Scalar& colorF,
		const cv::Scalar& plusF, const cv::Scalar& minusF, float maxLum, float minLum)
	{
		dst = cv::Mat(src.rows, src.cols, CV_8UC1, cv::Scalar(0));

		Q_ASSERT(src.type() == CV_8UC4);
		cv::MatConstIterator_<cv::Vec4b> iter = src.begin<cv::Vec4b>();
		cv::MatConstIterator_<cv::Vec4b> iterEnd = src.end<cv::Vec4b>();
		cv::MatIterator_<uchar> iterMask = dst.begin<uchar>();
		cv::MatIterator_<uchar> iterMaskEnd = dst.end<uchar>();
		for(; iter != iterEnd && iterMask != iterMaskEnd; iter++, iterMask++)
		{
			const cv::Vec4b& pixel = *iter;
			double lum = pixel[0] + pixel[1] + pixel[2];
			cv::Scalar pixelRelColor = cv::Scalar(0, 0, 0);
			if(lum > maxLum || lum < minLum)
			{
				continue;
			}
			if(lum == 0.0)
			{
				pixelRelColor = cv::Scalar(1.0/3.0, 1.0/3.0, 1.0/3.0);
			}
			else
			{
				pixelRelColor[0] = (pixel[0] == 0.0) ? 0.001 : pixel[0] / lum;
				pixelRelColor[1] = (pixel[1] == 0.0) ? 0.001 : pixel[1] / lum;
				pixelRelColor[2] = (pixel[2] == 0.0) ? 0.001 : pixel[2] / lum;
			}
			if(pixelRelColor[2] <= colorF[0] + plusF[0] && pixelRelColor[2] >= colorF[0] - minusF[0] &&
				pixelRelColor[1] <= colorF[1] + plusF[1] && pixelRelColor[1] >= colorF[1] - minusF[1] &&
				pixelRelColor[0] <= colorF[2] + plusF[2] && pixelRelColor[0] >= colorF[2] - minusF[2])
			{
				*iterMask = 255;
			}
		}
	}

	static void avgLum(const cv::Mat& src, cv::Mat& dst, float maxLum, float minLum)
	{
		dst = cv::Mat(src.rows, src.cols, CV_8UC1, cv::Scalar(0));

		Q_ASSERT(src.type() == CV_8UC4);
		cv::MatConstIterator_<cv::Vec4b> iter = src.begin<cv::Vec4b>();
		cv::MatConstIterator_<cv::Vec4b> iterEnd = src.end<cv::Vec4b>();
		cv::MatIterator_<uchar> iterMask = dst.begin<uchar>();
		cv::MatIterator_<uchar> iterMaskEnd = dst.end<uchar>();
		for(; iter != iterEnd && iterMask != iterMaskEnd; iter++, iterMask++)
		{
			const cv::Vec4b& pixel = *iter;
			double lum = pixel[0] + pixel[1] + pixel[2];
			cv::Scalar pixelColor = cv::Scalar(0, 0, 0);
			if(lum <= maxLum && lum >= minLum)
			{
				*iterMask = 255;
			}
		}
	}
};

} // namespace ICEAVGCOLORPROC_NS

#endif // AVGCOLORPROCPLUGIN_H
