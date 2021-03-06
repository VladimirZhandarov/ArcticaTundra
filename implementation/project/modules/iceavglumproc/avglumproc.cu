#if !defined CUDA_DISABLER

#include <cfloat>
#include "opencv2/core/core.hpp"
#include "opencv2/gpu/device/common.hpp"
#include "opencv2/gpu/device/border_interpolate.hpp"
#include "opencv2/gpu/device/vec_traits.hpp"
#include "opencv2/gpu/device/vec_math.hpp"
#include "opencv2/gpu/device/saturate_cast.hpp"
#include "opencv2/gpu/device/filters.hpp"

namespace ICEAVGLUMPROC_NS
{
namespace device
{

using namespace cv::gpu::device;

// kernels

template <typename T> __global__ void avg_lum_proc(const PtrStepSz<T> src, PtrStepSz<uchar> dst,
	int channelTypeValue, float maxLum, float minLum)
{
	const int x = blockDim.x * blockIdx.x + threadIdx.x;
	const int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x < src.cols && y < src.rows && x < dst.cols && y < dst.rows)
	{
		T pixel = src(y, x);
		float lum = pixel.x + pixel.y + pixel.z;
		if(channelTypeValue == 3)
		{
			lum = pixel.x;
		}
		else if(channelTypeValue == 2)
		{
			lum = pixel.y;
		}
		else if(channelTypeValue == 1)
		{
			lum = pixel.z;
		}
		// Производим сравнение с фильтром
		if(lum <= maxLum && lum >= minLum)
		{
			dst(y, x) = 255;
		}
		else
		{
			dst(y, x) = 0;
		}
	}
}

template <typename T> struct AvgLumProcDispatcher
{
	static void call(const PtrStepSz<T>& src, const PtrStepSz<uchar>& dst,
		int channelTypeValue, float maxLum, float minLum,
		cudaStream_t stream)
	{
		const dim3 block(32, 8);
		const dim3 grid(divUp(dst.cols, block.x), divUp(dst.rows, block.y));
		avg_lum_proc<<<grid, block, 0, stream>>>(src, dst, channelTypeValue, maxLum, minLum);
		cudaSafeCall(cudaGetLastError());

		if(stream == 0)
		{
			cudaSafeCall(cudaDeviceSynchronize());
		}
	}
};

template <typename T>
void avgLumProc(const PtrStepSzb& src, const PtrStepSzb& dst,
	int channelTypeValue, float maxLum, float minLum, cudaStream_t stream)
{
	AvgLumProcDispatcher<T>::call(static_cast< PtrStepSz<T> >(src), static_cast< PtrStepSz<uchar> >(dst), 
		channelTypeValue, maxLum, minLum, stream);
}

template void avgLumProc<uchar4>(const PtrStepSzb& src, const PtrStepSzb& dst,
	int channelTypeValue, float maxLum, float minLum, cudaStream_t stream);

} // namespace device
} // namespace ICEAVGLUMPROC_NS

#endif /* CUDA_DISABLER */
