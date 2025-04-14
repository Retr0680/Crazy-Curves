#include "BackgroundProcessor.hpp"
#include "CurveEvaluator.hpp"
#include "ProfileDataManager.hpp"

BackgroundProcessor::BackgroundProcessor() : 
    shutdownRequested(false),
    isProcessing(false) {}

BackgroundProcessor::~BackgroundProcessor() {
    Shutdown();
}

PF_Err BackgroundProcessor::Initialize(PF_InData* in_data) {
    PF_Err err = PF_Err_NONE;

    // Start worker thread
    workerThread = std::thread(&BackgroundProcessor::WorkerLoop, this);

    return err;
}

PF_Err BackgroundProcessor::Shutdown() {
    PF_Err err = PF_Err_NONE;

    // Signal shutdown and wait for worker thread
    shutdownRequested = true;
    condition.notify_one();
    
    if (workerThread.joinable()) {
        workerThread.join();
    }

    return err;
}

PF_Err BackgroundProcessor::QueueTask(
    const CurveData* curves,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const PF_Rect& area)
{
    PF_Err err = PF_Err_NONE;

    // Create task
    ProcessingTask task;
    for (int i = 0; i < 4; i++) {
        task.curves[i] = curves[i];
    }
    task.input = input;
    task.output = output;
    task.area = area;

    // Queue task
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    condition.notify_one();

    return err;
}

void BackgroundProcessor::WorkerLoop() {
    while (!shutdownRequested) {
        ProcessingTask task;
        bool hasTask = false;

        // Get task from queue
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() {
                return !taskQueue.empty() || shutdownRequested;
            });

            if (shutdownRequested) break;

            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
                hasTask = true;
            }
        }

        // Process task
        if (hasTask) {
            isProcessing = true;
            ProcessTask(task);
            isProcessing = false;
        }
    }
}

PF_Err BackgroundProcessor::ProcessTask(const ProcessingTask& task) {
    PF_Err err = PF_Err_NONE;

    ProfileDataManager::getInstance().BeginProfiling("BackgroundProcessing");

    // Process pixel data
    bool deepColor = PF_WORLD_IS_DEEP(task.output);
    A_long rowbytes = task.output->rowbytes;

    for (A_long y = task.area.top; y < task.area.bottom; y++) {
        if (deepColor) {
            PF_PixelFloat* inP = (PF_PixelFloat*)((char*)task.input->data + 
                y * task.input->rowbytes);
            PF_PixelFloat* outP = (PF_PixelFloat*)((char*)task.output->data + 
                y * rowbytes);

            for (A_long x = task.area.left; x < task.area.right; x++) {
                ERR(CurveEvaluator::EvaluatePixelFloat(task.curves, inP + x, outP + x));
            }
        } else {
            PF_Pixel8* inP = (PF_Pixel8*)((char*)task.input->data + 
                y * task.input->rowbytes);
            PF_Pixel8* outP = (PF_Pixel8*)((char*)task.output->data + 
                y * rowbytes);

            for (A_long x = task.area.left; x < task.area.right; x++) {
                ERR(CurveEvaluator::EvaluatePixel8(task.curves, inP + x, outP + x));
            }
        }
    }

    ProfileDataManager::getInstance().EndProfiling("BackgroundProcessing");

    return err;
}