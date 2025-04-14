#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

class WorkerThreadManager {
public:
    static WorkerThreadManager& getInstance() {
        static WorkerThreadManager instance;
        return instance;
    }

    PF_Err Initialize(PF_InData* in_data);
    PF_Err Shutdown();
    PF_Err QueueRenderTask(const PF_EffectWorld* input, 
                          PF_EffectWorld* output,
                          const CurveData* curves,
                          const PF_Rect& area);
    bool IsProcessing() const { return !taskQueue.empty(); }

private:
    WorkerThreadManager();
    ~WorkerThreadManager();

    struct RenderTask {
        const PF_EffectWorld* input;
        PF_EffectWorld* output;
        const CurveData* curves;
        PF_Rect area;
    };

    std::vector<std::thread> workerThreads;
    std::queue<RenderTask> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool shutdownFlag;

    void WorkerLoop();
    PF_Err ProcessRenderTask(const RenderTask& task);
};