#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool
{
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex taskMutex;
    std::condition_variable cv;

    bool shutdown = false;
    size_t activeTasks = 0;

public:
    explicit ThreadPool(size_t numThreads)
    {
        for (size_t i = 0; i < numThreads; i++)
        {
            workers.emplace_back([this]()
                                 {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(taskMutex);

                        cv.wait(lock, [this]() {
                            return shutdown || !tasks.empty();
                        });

                        if (shutdown && tasks.empty())
                            return;

                        task = std::move(tasks.front());
                        tasks.pop();
                        activeTasks++;
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(taskMutex);
                        activeTasks--;

                        if (tasks.empty() && activeTasks == 0) {
                            cv.notify_all();
                        }
                    }
                } });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            shutdown = true;
        }

        cv.notify_all();

        for (auto &worker : workers)
        {
            if (worker.joinable())
                worker.join();
        }
    }

    template <typename F>
    void enqueue(F &&task)
    {
        {
            std::unique_lock<std::mutex> lock(taskMutex);
            tasks.emplace(std::forward<F>(task));
        }

        cv.notify_one();
    }

    void waitAll()
    {
        std::unique_lock<std::mutex> lock(taskMutex);

        cv.wait(lock, [this]()
                { return tasks.empty() && activeTasks == 0; });
    }
};