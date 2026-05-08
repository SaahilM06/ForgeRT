#pragma once

#include <cstdint>
#include <functional>



enum class TaskType {
    Generic,          
    SyntheticCompute, 
    MandelbrotTile,   
    MergeSortRange,   
    MatrixBlock,      
};

struct Task {
    uint64_t task_id   = 0;
    uint64_t parent_id = 0;      
    uint32_t depth     = 0;      
    uint32_t cost_hint = 1;     

    TaskType type = TaskType::Generic;

    std::function<void()> fn;   
};
