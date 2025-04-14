#include "BatchProcessor.hpp"
#include "CurveEvaluator.hpp"

PF_Err BatchProcessor::ProcessImageBatch(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves,
    A_long startLine,
    A_long endLine)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    bool deepColor = PF_WORLD_IS_DEEP(output);
    A_long rowbytes = output->rowbytes;
    A_long width = output->width;

    for (A_long y = startLine; y < endLine; y++) {
        if (deepColor) {
            PF_PixelFloat* inP = (PF_PixelFloat*)((char*)input->data + y * input->rowbytes);
            PF_PixelFloat* outP = (PF_PixelFloat*)((char*)output->data + y * rowbytes);
            
            // Process pixels in batches
            for (A_long x = 0; x < width; x += BATCH_SIZE) {
                A_long batchSize = PF_MIN(BATCH_SIZE, width - x);
                ERR(ProcessPixelBatchFloat(inP + x, outP + x, curves, batchSize));
            }
        } else {
            PF_Pixel8* inP = (PF_Pixel8*)((char*)input->data + y * input->rowbytes);
            PF_Pixel8* outP = (PF_Pixel8*)((char*)output->data + y * rowbytes);
            
            // Process pixels in batches
            for (A_long x = 0; x < width; x += BATCH_SIZE) {
                A_long batchSize = PF_MIN(BATCH_SIZE, width - x);
                ERR(ProcessPixelBatch8(inP + x, outP + x, curves, batchSize));
            }
        }
    }

    return err;
}

PF_Err BatchProcessor::QueueBatchOperation(
    PF_InData* in_data,
    const CurveData* curves,
    A_long startFrame,
    A_long endFrame)
{
    PF_Err err = PF_Err_NONE;

    // Calculate optimal batch size based on image height
    A_long imageHeight = in_data->height;
    A_long optimalBatchSize = imageHeight / PF_MAX(1, in_data->effect_ref->global_data);

    // Create batch tasks
    for (A_long frame = startFrame; frame <= endFrame; frame++) {
        for (A_long y = 0; y < imageHeight; y += optimalBatchSize) {
            BatchTask task;
            task.startLine = y;
            task.endLine = PF_MIN(y + optimalBatchSize, imageHeight);
            task.curves = curves;
            taskQueue.push_back(task);
        }
    }

    return err;
}

PF_Err BatchProcessor::ProcessPixelBatch8(
    PF_Pixel8* inP,
    PF_Pixel8* outP,
    const CurveData* curves,
    A_long numPixels)
{
    PF_Err err = PF_Err_NONE;

    for (A_long i = 0; i < numPixels; i++) {
        ERR(CurveEvaluator::EvaluatePixel8(curves, &inP[i], &outP[i]));
        if (err) break;
    }

    return err;
}

PF_Err BatchProcessor::ProcessPixelBatchFloat(
    PF_PixelFloat* inP,
    PF_PixelFloat* outP,
    const CurveData* curves,
    A_long numPixels)
{
    PF_Err err = PF_Err_NONE;

    for (A_long i = 0; i < numPixels; i++) {
        ERR(CurveEvaluator::EvaluatePixelFloat(curves, &inP[i], &outP[i]));
        if (err) break;
    }

    return err;
}