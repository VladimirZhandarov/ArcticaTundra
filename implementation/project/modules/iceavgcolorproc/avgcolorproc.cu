#if !defined CUDA_DISABLER

#include <cfloat>
#include "opencv2/core/core.hpp"
#include "opencv2/gpu/device/common.hpp"
#include "opencv2/gpu/device/border_interpolate.hpp"
#include "opencv2/gpu/device/vec_traits.hpp"
#include "opencv2/gpu/device/vec_math.hpp"
#include "opencv2/gpu/device/saturate_cast.hpp"
#include "opencv2/gpu/device/filters.hpp"

namespace ICEAVGCOLORPROC_NS
{
namespace device
{

using namespace cv::gpu::device;

// kernels

template <typename T> __global__ void avg_color_proc(const PtrStepSz<T> src, PtrStepSz<uchar> dst,
	float minX, float minY, float minZ, float maxX, float maxY, float maxZ, float maxLum,
	float minLum)
{
	const int x = blockDim.x * blockIdx.x + threadIdx.x;
	const int y = blockDim.y * blockIdx.y + threadIdx.y;

	if(x < src.cols && y < src.rows && x < dst.cols && y < dst.rows)
	{
		T pixel = src(y, x);
		float lum = pixel.x + pixel.y + pixel.z;
		if(lum > maxLum || lum < minLum)
		{
			dst(y, x) = 0;
		}
		else
		{
			float pixelRelColorX;
			float pixelRelColorY;
			float pixelRelColorZ;
			
			if(lum == 0.0)
			{
				pixelRelColorX = 1.0/3.0;
				pixelRelColorY = 1.0/3.0;
				pixelRelColorZ = 1.0/3.0;
			}
			else
			{
				pixelRelColorX = (pixel.x == 0.0) ? 0.001 : pixel.x / lum;
				pixelRelColorY = (pixel.y == 0.0) ? 0.001 : pixel.y / lum;
				pixelRelColorZ = (pixel.z == 0.0) ? 0.001 : pixel.z / lum;
			}

			// Производим сравнение с фильтром
			if(pixelRelColorX <= maxX && pixelRelColorX >= minX &&
				pixelRelColorY <= maxY && pixelRelColorY >= minY &&
				pixelRelColorZ <= maxZ && pixelRelColorZ >= minZ)
			{
				dst(y, x) = 255;
			}
			else
			{
				dst(y, x) = 0;
			}
		}
	}
}

template <typename T> struct AvgColorProcDispatcher
{
	static void call(const PtrStepSz<T>& src, const PtrStepSz<uchar>& dst,
		float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
		float maxLum, float minLum, cudaStream_t stream)
	{
		const dim3 block(32, 8);
		const dim3 grid(divUp(dst.cols, block.x), divUp(dst.rows, block.y));
		avg_color_proc<<<grid, block, 0, stream>>>(src, dst, minX, minY, minZ, maxX,
			maxY, maxZ, maxLum, minLum);
		cudaSafeCall(cudaGetLastError());

		if(stream == 0)
		{
			cudaSafeCall(cudaDeviceSynchronize());
		}
	}
};

template <typename T>
void avgColorProc(const PtrStepSzb& src, const PtrStepSzb& dst,
	float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
	float maxLum, float minLum, cudaStream_t stream)
{
	AvgColorProcDispatcher<T>::call(static_cast< PtrStepSz<T> >(src), static_cast< PtrStepSz<uchar> >(dst), 
		minX, minY, minZ, maxX, maxY, maxZ, maxLum, minLum, stream);
}

template void avgColorProc<uchar4>(const PtrStepSzb& src, const PtrStepSzb& dst,
	float minX, float minY, float minZ, float maxX, float maxY, float maxZ,
	float maxLum, float minLum, cudaStream_t stream);

} // namespace device
} // namespace ICEAVGCOLORPROC_NS

#endif /* CUDA_DISABLER */
