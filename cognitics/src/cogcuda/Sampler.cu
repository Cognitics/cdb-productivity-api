
#include <cogcuda/Sampler.cuh>

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

namespace cognitics
{
    namespace cuda
    {
        bool Available()
        {
            int device_count;
            auto err = cudaGetDeviceCount(&device_count);
            return (err == cudaSuccess) && (device_count > 0);
        }

        namespace
        {
            __global__ void kernel_sample(SamplerRaster* input_array, size_t input_count, SamplerRaster* out)
            {
                int x = threadIdx.x + (blockIdx.x * blockDim.x);
                int y = threadIdx.y + (blockIdx.y * blockDim.y);
                double lat = out->South + (y * ((out->North - out->South) / out->Height));
                double lon = out->West + (x * ((out->East - out->West) / out->Width));

                // pick the first matching input raster
                // input_array should be sorted from highest resolution to lowest
                SamplerRaster* input = nullptr;
                for(size_t input_index = 0; input_index < input_count; ++input_index)
                {
                    if(lat <= input_array[input_index].South)
                        continue;
                    if(lat >= input_array[input_index].North)
                        continue;
                    if(lon <= input_array[input_index].West)
                        continue;
                    if(lon >= input_array[input_index].East)
                        continue;
                    input = &input_array[input_index];
                    break;
                }

                if(input == nullptr)
                    return;

                double spacing_x = (input->East - input->West) * input->Width;
                double spacing_y = (input->North - input->South) * input->Height;
                int input_x = std::floor((lon - input->West) / spacing_x);
                int input_y = std::floor((lat - input->South) / spacing_y);
                int sw_index = (y * input->Width) + x;
                double sw_lat = input->South + (input_y * spacing_y);
                double sw_lon = input->West + (input_x * spacing_x);

                float sw_value = input->Data[sw_index];
                float se_value = input->Data[sw_index + 1];
                float nw_value = input->Data[sw_index + input->Width];
                float ne_value = input->Data[sw_index + input->Width + 1];

                // bilinear interpolation
                float nx = (lon - sw_lon) / spacing_x;
                float ny = (lat - sw_lat) / spacing_y;
                float a00 = sw_value;
                float a10 = se_value - sw_value;
                float a01 = nw_value - sw_value;
                float a11 = sw_value - se_value - nw_value + ne_value;
                float value = a00 + (a10 * nx) + (a01 * ny) + (a11 * nx * ny);

                out->Data[(y * out->Width) + x] = value;
            }

        }

        Sampler::Sampler(const std::vector<SamplerRaster>& rasters)
        {
            // input must be sorted from highest resolution to lowest
            // all input rasters must contain valid width/height
            InputCount = rasters.size();
            cudaMallocManaged(&InputArray, sizeof(SamplerRaster) * InputCount);
            for(size_t i = 0; i < InputCount; ++i)
            {
                auto& source = rasters[i];
                auto& input = InputArray[i];
                input = source;
                cudaMallocManaged(&input.Data, input.Width * input.Height * sizeof(float));
                std::copy(source.Data, source.Data + (input.Width * input.Height), input.Data);
            }
        }

        void Sampler::Sample(SamplerRaster* output, float nodata)
        {
            // output must contain a valid width/height
            SamplerRaster* kernel_output;
            cudaMallocManaged(&kernel_output, sizeof(SamplerRaster));
            *kernel_output = *output;
            cudaMallocManaged(&kernel_output->Data, output->Width * output->Height * sizeof(float));
            for(size_t i = 0, c = output->Width * output->Height; i < c; ++i)
                kernel_output->Data[i] = nodata;
            dim3 block_size(16, 16); // block threads must be <= 1024 (32x32 max)
            dim3 block_count(kernel_output->Width / block_size.x, kernel_output->Height / block_size.y);
            kernel_sample<<<block_count, block_size>>>(InputArray, InputCount, kernel_output);
            cudaDeviceSynchronize();
            for(size_t i = 0, c = output->Width * output->Height; i < c; ++i)
                output->Data[i] = kernel_output->Data[i];
            cudaFree(kernel_output->Data);
            cudaFree(kernel_output);
        }


    }
}
