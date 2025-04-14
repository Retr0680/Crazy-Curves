#include "GPUHandler.hpp"

PF_Err GPUHandler::SetupGPU(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    out_data->out_flags2 |= PF_OutFlag2_SUPPORTS_GPU_RENDER;

    // Enable floating point support
    out_data->out_flags |= PF_OutFlag_DEEP_COLOR_AWARE;
    out_data->out_flags2 |= PF_OutFlag2_FLOAT_COLOR_AWARE;

    return err;
}

PF_Err GPUHandler::RenderGPU(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_GPUWorld* input,
    PF_GPUWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Create lookup textures for curves
    ERR(BuildGPULookupTextures(in_data, curves));

    if (!err) {
        // Create and setup GPU kernels
        ERR(CreateGPUKernels());
    }

    if (!err) {
        // Set kernel parameters and dispatch
        PF_Handle kernelHandle = suites.GPUSuite1()->GetKernelHandle();
        if (kernelHandle) {
            // Set input/output textures
            ERR(suites.GPUSuite1()->SetTextureForKernel(
                kernelHandle, 
                "inputTexture",
                input));

            ERR(suites.GPUSuite1()->SetTextureForKernel(
                kernelHandle,
                "outputTexture",
                output));

            // Dispatch kernel
            ERR(suites.GPUSuite1()->DispatchKernel(
                kernelHandle,
                output->width,
                output->height,
                1));
        }
    }

    return err;
}

PF_Err GPUHandler::BuildGPULookupTextures(
    PF_InData* in_data,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Create 1D textures for each curve
    const int TEXTURE_SIZE = 256;
    PF_FpShort* rgbLUT = new PF_FpShort[TEXTURE_SIZE];
    PF_FpShort* rLUT = new PF_FpShort[TEXTURE_SIZE];
    PF_FpShort* gLUT = new PF_FpShort[TEXTURE_SIZE];
    PF_FpShort* bLUT = new PF_FpShort[TEXTURE_SIZE];

    // Build lookup tables
    for (int i = 0; i < TEXTURE_SIZE; i++) {
        PF_FpLong x = static_cast<PF_FpLong>(i) / (TEXTURE_SIZE - 1);
        rgbLUT[i] = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[0], x));
        rLUT[i] = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[1], x));
        gLUT[i] = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[2], x));
        bLUT[i] = static_cast<PF_FpShort>(CurvesData::evaluate(&curves[3], x));
    }

    // Create GPU textures
    ERR(suites.GPUSuite1()->CreateTexture1D(TEXTURE_SIZE, rgbLUT, "rgbLUT"));
    ERR(suites.GPUSuite1()->CreateTexture1D(TEXTURE_SIZE, rLUT, "rLUT"));
    ERR(suites.GPUSuite1()->CreateTexture1D(TEXTURE_SIZE, gLUT, "gLUT"));
    ERR(suites.GPUSuite1()->CreateTexture1D(TEXTURE_SIZE, bLUT, "bLUT"));

    delete[] rgbLUT;
    delete[] rLUT;
    delete[] gLUT;
    delete[] bLUT;

    return err;
}

PF_Err GPUHandler::CreateGPUKernels()
{
    // GPU kernel implementation would go here
    // This would include the GLSL/Metal shader code for curve processing
    return PF_Err_NONE;
}