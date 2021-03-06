#if !defined CUDA_DISABLER

#include <cfloat>
#include "opencv2/core/core.hpp"
#include "opencv2/gpu/device/common.hpp"
#include "opencv2/gpu/device/border_interpolate.hpp"
#include "opencv2/gpu/device/vec_traits.hpp"
#include "opencv2/gpu/device/vec_math.hpp"
#include "opencv2/gpu/device/saturate_cast.hpp"
#include "opencv2/gpu/device/filters.hpp"

namespace ICEIMAGE_NS
{
namespace device
{

using namespace cv::gpu::device;

// kernels

template <typename T> __global__ void LUT(const PtrStepSz<T> src, const PtrStepSz<uchar> lut, PtrStepSz<uchar> dst)
{
	const int x = blockDim.x * blockIdx.x + threadIdx.x;
	const int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x < src.cols && y < src.rows && x < dst.cols && y < dst.rows)
	{
		dst(y, x) = lut(0, src(y, x));
	}
}

template <typename T> struct LUTDispatcher
{
	static void call(const PtrStepSz<T>& src, const PtrStepSz<uchar>& lut, const PtrStepSz<T>& dst,
		cudaStream_t stream)
	{
		const dim3 block(32, 8);
		const dim3 grid(divUp(dst.cols, block.x), divUp(dst.rows, block.y));
		LUT<<<grid, block, 0, stream>>>(src, lut, dst);
		cudaSafeCall(cudaGetLastError());

		if(stream == 0)
		{
			cudaSafeCall(cudaDeviceSynchronize());
		}
	}
};

template <typename T>
void LUT(const PtrStepSzb& src, const PtrStepSzb& lut, const PtrStepSzb& dst, cudaStream_t stream)
{
	LUTDispatcher<T>::call(static_cast< PtrStepSz<T> >(src), static_cast< PtrStepSz<uchar> >(lut), 
		static_cast< PtrStepSz<T> >(dst), stream);
}

template void LUT<uchar>(const PtrStepSzb& src, const PtrStepSzb& lut, const PtrStepSzb& dst, cudaStream_t stream);

} // namespace device
} // namespace ICEIMAGE_NS

#endif /* CUDA_DISABLER */
