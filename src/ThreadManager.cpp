#include "ThreadManager.hpp"
#include <thread>
#include <algorithm>

ThreadManager::ThreadManager() {
    threadCount = std::thread::hardware_concurrency();
    if (threadCount == 0) threadCount = 2; // Fallback if detection fails
}

PF_Err ThreadManager::ProcessInParallel(
    PF_InData* in_data,
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;

    // Divide work into tasks
    ERR(DivideIntoTasks(input, output, curves));

    if (!err) {
        std::vector<std::thread> threads;
        threads.reserve(threadCount);

        // Create and launch threads
        for (A_long i = 0; i < threadCount && i < tasks.size(); i++) {
            threads.emplace_back([this, i]() {
                std::lock_guard<std::mutex> lock(taskMutex);
                ProcessThreadTask(tasks[i]);
            });
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    return err;
}

PF_Err ThreadManager::ProcessThreadTask(const ThreadTask& task) {
    PF_Err err = PF_Err_NONE;

    // Process rows assigned to this thread
    for (A_long y = task.startLine; y < task.endLine; y++) {
        PF_Pixel8* inP = (PF_Pixel8*)((char*)task.input->data + y * task.input->rowbytes);
        PF_Pixel8* outP = (PF_Pixel8*)((char*)task.output->data + y * task.output->rowbytes);

        for (A_long x = 0; x < task.input->width; x++) {
            // Process RGB channels
            PF_FpLong rgb_r = inP->red / 255.0f;
            PF_FpLong rgb_g = inP->green / 255.0f;
            PF_FpLong rgb_b = inP->blue / 255.0f;

            // Apply curves
            outP->red = static_cast<A_u_char>(CurvesData::evaluate(&task.curves[0], rgb_r) * 255.0f);
            outP->green = static_cast<A_u_char>(CurvesData::evaluate(&task.curves[0], rgb_g) * 255.0f);
            outP->blue = static_cast<A_u_char>(CurvesData::evaluate(&task.curves[0], rgb_b) * 255.0f);
            outP->alpha = inP->alpha;

            inP++;
            outP++;
        }
    }

    return err;
}

PF_Err ThreadManager::DivideIntoTasks(
    PF_EffectWorld* input,
    PF_EffectWorld* output,
    const CurveData* curves)
{
    PF_Err err = PF_Err_NONE;
    
    tasks.clear();

    // Calculate rows per thread
    A_long rowsPerThread = output->height / threadCount;
    A_long remainingRows = output->height % threadCount;
    A_long currentRow = 0;

    // Create tasks
    for (A_long i = 0; i < threadCount; i++) {
        ThreadTask task;
        task.startLine = currentRow;
        task.endLine = currentRow + rowsPerThread;
        
        // Distribute remaining rows
        if (remainingRows > 0) {
            task.endLine++;
            remainingRows--;
        }
        
        task.input = input;
        task.output = output;
        task.curves = curves;
        
        tasks.push_back(task);
        currentRow = task.endLine;
    }

    return err;
}

void ThreadManager::SetThreadCount(A_long count) {
    threadCount = std::max(1L, std::min(count, 32L)); // Limit range
}

A_long ThreadManager::GetOptimalThreadCount() const {
    return threadCount;
}