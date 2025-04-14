#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

class BackgroundProcessor {
public:
    static BackgroundProcessor& getInstance() {
        static BackgroundProcessor instance;
        return instance;
    }

    PF_Err Initialize(PF_InData* in_data);
    PF_Err Shutdown();

    PF_Err QueueTask(
        const CurveData* curves,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const PF_Rect& area);

    bool IsProcessing() const { return !taskQueue.empty() || isProcessing; }

private:
    BackgroundProcessor();
    ~BackgroundProcessor();

    struct ProcessingTask {
        CurveData curves[4];
        PF_EffectWorld* input;
        PF_EffectWorld* output;
        PF_Rect area;
    };

    std::queue<ProcessingTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> shutdownRequested;
    std::atomic<bool> isProcessing;
    std::thread workerThread;

    void WorkerLoop();
    PF_Err ProcessTask(const ProcessingTask& task);
};