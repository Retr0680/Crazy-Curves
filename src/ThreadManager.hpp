#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>
#include <mutex>

class ThreadManager {
public:
    static ThreadManager& getInstance() {
        static ThreadManager instance;
        return instance;
    }

    PF_Err ProcessInParallel(
        PF_InData* in_data,
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves);

    void SetThreadCount(A_long count);
    A_long GetOptimalThreadCount() const;

private:
    ThreadManager();
    
    struct ThreadTask {
        A_long startLine;
        A_long endLine;
        PF_EffectWorld* input;
        PF_EffectWorld* output;
        const CurveData* curves;
    };

    std::vector<ThreadTask> tasks;
    A_long threadCount;
    std::mutex taskMutex;

    PF_Err ProcessThreadTask(const ThreadTask& task);
    PF_Err DivideIntoTasks(
        PF_EffectWorld* input,
        PF_EffectWorld* output,
        const CurveData* curves);
};