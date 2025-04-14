#include "GPUProcessingManager.hpp"

PF_Err GPUProcessingManager::InitializeGPU(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Set GPU capabilities flags
    out_data->out_flags2 |= PF_OutFlag2_SUPPORTS_GPU_RENDER;
    out_data->out_flags2 |= PF_OutFlag2_GPU_RENDER_POSSIBLE;

    // Initialize GPU resources
    resources.initialized = false;
    resources.curveBuffer = nullptr;
    resources.lookupTexture = nullptr;

    return err;
}

PF_Err GPUProcessingManager::ProcessFrameGPU(
    PF_InData* in_data,
    PF_OutData* out_data,
    PF_GPUWorld* input,
    PF_GPUWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Initialize or update GPU resources if needed
    if (!resources.initialized) {
        ERR(CreateGPUBuffers(in_data, curves));
    }

    if (!err) {
        // Update lookup texture if curves have changed
        if (curves[0].dirty || curves[1].dirty || 
            curves[2].dirty || curves[3].dirty) {
            ERR(UpdateLookupTexture(in_data, curves));
        }

        if (!err) {
            // Set up GPU rendering parameters
            PF_GPURenderParams params;
            AEFX_CLR_STRUCT(params);
            params.input = input;
            params.output = output;
            params.lookupTexture = resources.lookupTexture;

            // Execute GPU render
            ERR(suites.GPUSuite1()->gpu_render(
                in_data->effect_ref,
                &params));
        }
    }

    return err;
}

PF_Err GPUProcessingManager::ShutdownGPU() {
    PF_Err err = PF_Err_NONE;

    // Clean up GPU resources
    if (resources.initialized) {
        if (resources.curveBuffer) {
            // Release curve buffer
            resources.curveBuffer = nullptr;
        }
        if (resources.lookupTexture) {
            // Release lookup texture
            resources.lookupTexture = nullptr;
        }
        resources.initialized = false;
    }

    return err;
}

PF_Err GPUProcessingManager::CreateGPUBuffers(
    PF_InData* in_data,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Create curve buffer
    PF_GPUBufferDesc bufferDesc;
    AEFX_CLR_STRUCT(bufferDesc);
    bufferDesc.width = 256; // LUT size
    bufferDesc.height = 4;  // RGB + Master curves
    bufferDesc.format = PF_GPUBufferFormat_RGBA32F;

    ERR(suites.GPUSuite1()->create_buffer(
        in_data->effect_ref,
        &bufferDesc,
        &resources.curveBuffer));

    if (!err) {
        // Create lookup texture
        PF_GPUTextureDesc textureDesc;
        AEFX_CLR_STRUCT(textureDesc);
        textureDesc.width = 256;
        textureDesc.height = 1;
        textureDesc.format = PF_GPUTextureFormat_RGBA32F;

        ERR(suites.GPUSuite1()->create_texture(
            in_data->effect_ref,
            &textureDesc,
            &resources.lookupTexture));

        if (!err) {
            resources.initialized = true;
        }
    }

    return err;
}

PF_Err GPUProcessingManager::UpdateLookupTexture(
    PF_InData* in_data,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    // Prepare LUT data
    const int LUT_SIZE = 256;
    float lutData[LUT_SIZE * 4]; // RGBA

    // Generate lookup values
    for (int i = 0; i < LUT_SIZE; i++) {
        float x = static_cast<float>(i) / (LUT_SIZE - 1);
        
        // Evaluate curves
        lutData[i * 4 + 0] = CurveEvaluator::EvaluateCurve(&curves[1], x); // R
        lutData[i * 4 + 1] = CurveEvaluator::EvaluateCurve(&curves[2], x); // G
        lutData[i * 4 + 2] = CurveEvaluator::EvaluateCurve(&curves[3], x); // B
        lutData[i * 4 + 3] = CurveEvaluator::EvaluateCurve(&curves[0], x); // RGB
    }

    // Update texture data
    ERR(suites.GPUSuite1()->update_texture(
        in_data->effect_ref,
        resources.lookupTexture,
        lutData,
        LUT_SIZE * sizeof(float) * 4));

    return err;
}