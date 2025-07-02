#pragma once
class TaskTimer
{
public:
    template <typename Callable>
    long long MeasureTask(Callable task) {
        auto start = std::chrono::high_resolution_clock::now();

        task();

        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }
};

