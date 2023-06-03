//PCTP.h stands for Practial Components Thread Pool
#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

//https://chat.openai.com/share/65495b56-a1d2-48bf-b416-fea4f6ca20e9
namespace PC {
    class ThreadPool {
    public:
        ThreadPool(size_t threadsCount) : stop(false) {
            for (size_t i = 0; i < threadsCount; ++i) {
                threads.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->m);
                            this->cv.wait(lock, [this] { return this->stop.load() || !this->tasks.empty(); });

                            if (this->stop.load() && this->tasks.empty())
                                return;

                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                });
            }
        }

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args)
#if __cplusplus >= 201703L
            -> std::future<std::invoke_result_t<F, Args...>> {
#else
            ->std::future<typename std::result_of<F(Args...)>::type> {
#endif
            if (stop.load())
                throw std::runtime_error("enqueue on stopped ThreadPool");

#if __cplusplus >= 201703L
            using return_type = std::invoke_result_t<F, Args...>;
#else
            using return_type = typename std::result_of<F(Args...)>::type;
#endif

            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

            std::future<return_type> res = task->get_future();

            {
                std::unique_lock<std::mutex> lock(m);
                tasks.emplace([task]() { (*task)(); });
            }

            cv.notify_one();
            return res;
        }

        ~ThreadPool() {
            stop.store(true);
            cv.notify_all();
            for (std::thread& worker : threads)
                worker.join();
        }
    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex m;
        std::condition_variable cv;
        std::atomic<bool> stop;
    };
}
