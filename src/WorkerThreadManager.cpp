#include "WorkerThreadManager.hpp"
#include "CurveEvaluator.hpp"

WorkerThreadManager::WorkerThreadManager() : shutdownFlag(false) {}

WorkerThreadManager::~WorkerThreadManager() {
    Shutdown();
}

PF_Err WorkerThreadManager::Initialize(PF_InData* in_data) {
    PF_Err err = PF_Err_NONE;

    // Create worker threads based on CPU cores
    size_t threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 2; // Fallback if detection fails

    for (size_t i = 0; i < threadCount; i++) {
        workerThreads.emplace_back(&WorkerThreadManager::WorkerLoop, this);
    }

    return err;
}

PF_Err WorkerThreadManager::Shutdown() {
    PF_Err err = PF_Err_NONE;

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        shutdownFlag = true;
    }
    condition.notify_all();

    // Wait for all threads to finish
    for (auto& thread : workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    workerThreads.clear();

    return err;
}

PF_Err WorkerThreadManager::QueueRenderTask(
    const PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves,
    const PF_Rect& area)
{
    PF_Err err = PF_Err_NONE;

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        taskQueue.push({input, output, curves, area});
    }
    condition.notify_one();

    return err;
}

void WorkerThreadManager::WorkerLoop() {
    while (true) {
        RenderTask task;
        
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() {
                return !taskQueue.empty() || shutdownFlag;
            });

            if (shutdownFlag && taskQueue.empty()) {
                break;
            }

            task = taskQueue.front();
            taskQueue.pop();
        }

        ProcessRenderTask(task);
    }
}

PF_Err WorkerThreadManager::ProcessRenderTask(const RenderTask& task) {
    PF_Err err = PF_Err_NONE;

    // Process pixels in the assigned area
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

    return err;
}