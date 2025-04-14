#pragma once
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AEGP_SuiteHandler.h"
#include "CrazyCurves.h"
#include <vector>

class MemoryManager {
public:
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }

    PF_Err AllocateSequenceData(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Err DeallocateSequenceData(
        PF_InData* in_data,
        PF_OutData* out_data);

    PF_Handle AllocateHandle(
        PF_InData* in_data,
        size_t size);

    void FreeHandle(PF_Handle handle);

    PF_Err TrackMemoryUsage(
        size_t size,
        const std::string& allocation_type);

private:
    MemoryManager() : totalAllocated(0) {}
    
    struct MemoryBlock {
        PF_Handle handle;
        size_t size;
        std::string type;
    };

    std::vector<MemoryBlock> allocatedBlocks;
    size_t totalAllocated;

    PF_Err ValidateMemoryUsage();
    void LogMemoryStatus();
};