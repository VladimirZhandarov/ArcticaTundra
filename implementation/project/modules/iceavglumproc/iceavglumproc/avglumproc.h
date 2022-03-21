#ifndef AVGLUMPROC_H
#define AVGLUMPROC_H

#include <cuda.h>
#include <cuda_runtime.h>

#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/stream_accessor.hpp>

namespace ICEAVGLUMPROC_NS
{

namespace device
{
template <typename T>
void avgLumProc(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& dst,
	int channelTypeValue, float maxLum, float minLum, cudaStream_t stream);
}

template<typename T>
class Proc
{
public:
	static void avgLum(const T& /*src*/, T& /*dst*/, int /*channelTypeValue*/, float /*maxLum*/,
		float /*minLum*/)
	{

	}
};

template<>
class Proc<cv::gpu::GpuMat>
{
public:
	static void avgLum(const cv::gpu::GpuMat& src, cv::gpu::GpuMat& dst, int channelTypeValue,
		float maxLum, float minLum)
	{
		typedef void (*func_t)(const cv::gpu::PtrStepSzb& src, const cv::gpu::PtrStepSzb& dst,
			int channelTypeValue, float maxLum, float minLum, cudaStream_t stream);
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
		func(src, dst, channelTypeValue, maxLum, minLum, cv::gpu::StreamAccessor::getStream(cv::gpu::Stream::Null()));
	}
};

template<>
class Proc<cv::Mat>
{
public:
	static void avgLum(const cv::Mat& src, cv::Mat& dst, int channelTypeValue, float maxLum,
		float minLum)
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
			if(channelTypeValue == Param::B_TYPE)
			{
				lum = pixelColor[0];
			}
			else if(channelTypeValue == Param::G_TYPE)
			{
				lum = pixelColor[1];
			}
			else if(channelTypeValue == Param::R_TYPE)
			{
				lum = pixelColor[2];
			}
			if(lum <= maxLum && lum >= minLum)
			{
				*iterMask = 255;
			}
		}
	}
};

} // namespace ICEAVGLUMPROC_NS

#endif // AVGLUMPROC_H
