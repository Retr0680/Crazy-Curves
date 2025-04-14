#include "MemoryManager.hpp"

PF_Err MemoryManager::AllocateSequenceData(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    size_t sequenceSize = sizeof(SequenceData);
    PF_Handle seqH = suites.HandleSuite1()->host_new_handle(sequenceSize);

    if (!seqH) {
        return PF_Err_OUT_OF_MEMORY;
    }

    SequenceData* seqP = static_cast<SequenceData*>(suites.HandleSuite1()->host_lock_handle(seqH));
    if (!seqP) {
        suites.HandleSuite1()->host_dispose_handle(seqH);
        return PF_Err_OUT_OF_MEMORY;
    }

    // Initialize sequence data
    AEFX_CLR_STRUCT(*seqP);
    seqP->rgb_curve.num_points = 2;
    seqP->rgb_curve.points[0] = {0.0f, 0.0f};
    seqP->rgb_curve.points[1] = {1.0f, 1.0f};

    // Track memory allocation
    ERR(TrackMemoryUsage(sequenceSize, "SequenceData"));

    suites.HandleSuite1()->host_unlock_handle(seqH);
    out_data->sequence_data = seqH;

    return err;
}

PF_Err MemoryManager::DeallocateSequenceData(
    PF_InData* in_data,
    PF_OutData* out_data)
{
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    if (in_data->sequence_data) {
        PF_Handle seqH = static_cast<PF_Handle>(in_data->sequence_data);
        
        // Remove from tracking
        for (auto it = allocatedBlocks.begin(); it != allocatedBlocks.end(); ++it) {
            if (it->handle == seqH) {
                totalAllocated -= it->size;
                allocatedBlocks.erase(it);
                break;
            }
        }

        suites.HandleSuite1()->host_dispose_handle(seqH);
        out_data->sequence_data = nullptr;
    }

    return err;
}

PF_Handle MemoryManager::AllocateHandle(
    PF_InData* in_data,
    size_t size)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    PF_Handle handle = suites.HandleSuite1()->host_new_handle(size);

    if (handle) {
        ERR(TrackMemoryUsage(size, "GenericHandle"));
    }

    return handle;
}

void MemoryManager::FreeHandle(PF_Handle handle)
{
    if (!handle) return;

    // Remove from tracking
    for (auto it = allocatedBlocks.begin(); it != allocatedBlocks.end(); ++it) {
        if (it->handle == handle) {
            totalAllocated -= it->size;
            allocatedBlocks.erase(it);
            break;
        }
    }

    suites.HandleSuite1()->host_dispose_handle(handle);
}

PF_Err MemoryManager::TrackMemoryUsage(
    size_t size,
    const std::string& allocation_type)
{
    PF_Err err = PF_Err_NONE;

    totalAllocated += size;
    allocatedBlocks.push_back({nullptr, size, allocation_type});

    ERR(ValidateMemoryUsage());
    if (!err) {
        LogMemoryStatus();
    }

    return err;
}

PF_Err MemoryManager::ValidateMemoryUsage()
{
    // Check if total memory usage is within reasonable limits
    const size_t MAX_MEMORY = 1024 * 1024 * 1024; // 1GB limit
    
    if (totalAllocated > MAX_MEMORY) {
        return PF_Err_OUT_OF_MEMORY;
    }

    return PF_Err_NONE;
}

void MemoryManager::LogMemoryStatus()
{
    #ifdef _DEBUG
    // Log current memory usage statistics
    size_t totalBlocks = allocatedBlocks.size();
    std::string message = "Memory Usage: " + 
                         std::to_string(totalAllocated / 1024) + 
                         "KB in " + 
                         std::to_string(totalBlocks) + 
                         " blocks";
    // Log message using AE's logging system
    #endif
}