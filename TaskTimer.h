#pragma once
class TaskTimer
{
public:
    template <typename Callable>
    double MeasureTask(Callable task) {
        auto start = std::chrono::high_resolution_clock::now();
        task();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0;
    }
};

