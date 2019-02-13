#ifndef DATALOADER_POOL_HEADER_H
#define DATALOADER_POOL_HEADER_H

#include "utils.h"
#include "buffer.h"

#include <functional>
#include <future>

#undef DLOG
#define DLOG(...)

/**
 *  FixedThreadPool
 *      with a fixed amount of worker and a max amount of requests
 */
template<typename Input, typename Output>
class ThreadPool{
private:
    struct Task{
        using FunctionCall = std::function<Output(Input a)>;

        Task() = default;

        Task(Task&&) = default;

        Task(Task&) = default;

        Task(Input const& in, FunctionCall const& work):
            in(in), work(work)
        {}

        Input in;
        FunctionCall work;
        std::promise<Output> promise;
    };
public:
    using FunctionCall = std::function<Output(Input a)>;

public:
    ThreadPool(std::size_t worker_num, std::size_t max_request):
        _queue(max_request, []() { return nullptr; })
    {
        _workers.reserve(worker_num);
        for(int i = 0; i < worker_num; ++i)
            _workers.emplace_back(this);

        was_empty = true;
        empty_queue = TimeIt();
        run_time = TimeIt();
    }

    /**
     *  Insert a request inside the TaskQueue and return a `some(future<Output>)`
     *  If the queue is full `none()` is returned
     */
    std::optional<std::shared_future<Output>> insert_task(Input const& in, FunctionCall const& work){
        std::lock_guard lock(mutex);
        if (!first_arrival){
            arrival_rate += arrival_time.stop();
        }

        Task* val = _queue.emplace_back(new Task(in, work));

        if (is_full() && !was_full){
            full_queue = TimeIt();
            was_full = true;
        }

        if (was_empty){
            was_empty = false;
            total_empty_queue_time += empty_queue.stop();
        }

        if (val == nullptr){
            return {};
        }

        arrival_time = TimeIt();
        first_arrival = false;
        return std::make_optional(val->promise.get_future());
    }

    bool is_full() const {
        return _queue.full();
    }

    void shutdown(bool safe = true){
        for(auto& worker: _workers){
            worker.running = false;
        }

        // make sure all worker stopped and are not stuck
        // on a task
        if (safe){
            for(auto& worker: _workers){
                DLOG("%s", "Joining worker thread");
                worker.thread.join();
            }
        }

        live_time = run_time.stop();
    }

    void report() const;

    std::size_t size() const {
        return _queue.size();
    }

private:
    Task* next_task(){
        std::lock_guard lock(mutex);

        if (!first_deparature){
            deparature_rate += deparature_time.stop();
        }

        if (was_full){
            was_full = false;
            total_full_queue_time += full_queue.stop();
        }

        if (_queue.size() == 1 && !was_empty){
            was_empty = true;
            empty_queue = TimeIt();
        }

        // only register depature if queue is not empty
        auto ptr = _queue.pop();
        //if (ptr != nullptr)
        {
            deparature_time = TimeIt();
            first_deparature = false;
        }
        return ptr;
    }

    struct WorkerThread{
        WorkerThread(ThreadPool* pool):
            pool(pool), thread(WorkerThread::run, this), running(true)
        {}

        static void run(WorkerThread* self){
            if (self == nullptr){
                ELOG("%s", "Worker Thread badly initialized");
                return;
            }

            while(self->running){
                Task* task = self->pool->next_task();

                if (task != nullptr){
                    TimeIt work_time;

                    try{
                        Output val = task->work(task->in);
                        task->promise.set_value(val);
                    } catch (...){
                        ELOG("%s", "Exception occured inside worker thread");
                        task->promise.set_exception(std::current_exception());
                    }

                    delete task;
                    self->total_work_time += work_time.stop();
                    self->task_count += 1;
                } else {
                    TimeIt idle_time;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    self->total_idle_time += idle_time.stop();
                }
            }

            DLOG("%s", "Worker Thread exiting");
        }

        ThreadPool* pool = nullptr;
        std::thread thread;

        bool running = true;

        std::size_t task_count = 0;
        double total_idle_time = 0;
        double total_work_time = 0;

    };

    bool was_empty = true;
    bool was_full = false;

    TimeIt full_queue;
    TimeIt empty_queue;
    TimeIt run_time;

    double total_full_queue_time = 0;
    double total_empty_queue_time = 0;
    mutable double live_time = 0;

    bool first_arrival = true;
    bool first_deparature = true;

    TimeIt arrival_time;
    TimeIt deparature_time;

    StatStream arrival_rate;
    StatStream deparature_rate;

    std::mutex mutex;
    RingBuffer<Task*> _queue;
    std::vector<WorkerThread> _workers;
};


template<typename Input, typename Output>
void ThreadPool<Input, Output>::report() const {
    double total_work = 0;
    double total_idle = 0;
    std::size_t task_count = 0;
    live_time = run_time.stop();

    printf("Thread Pool Report\n");
    printf("   ID       WORK       IDLE   (%%)  TASKS  WORK/TASK\n");
    for(int i = 0; i < _workers.size(); ++i){
        double a = _workers[i].total_work_time;
        double b = _workers[i].total_idle_time;
        std::size_t c = _workers[i].task_count;

        total_work += a;
        total_idle += b;
        task_count += c;

        printf("%5d %10.4f %10.4f %6.2f %5lu %10.4f\n", i, a, b, 100.0 * a / (a + b), c, a / double(c));
    }

    printf("Total %10.4f %10.4f %6.2f %5lu %10.4f\n",
           total_work, total_idle, 100.0 * total_work / (total_idle + total_work),
           task_count, total_work / double(task_count));

    printf("\n                 TIME   (%%)\n");
    printf("Empty Queue %9.2f %6.2f\n", total_empty_queue_time, total_empty_queue_time * 100.0 / live_time);
    printf("Full  Queue %9.2f %6.2f\n", total_full_queue_time, total_full_queue_time * 100.0 / live_time);
    printf("   All Time %9.2f %6.2f\n\n", live_time, 100.0);

    printf("    Arrival Rate %9.2f\n", arrival_rate.mean());
    printf("  Departure Rate %9.2f\n", deparature_rate.mean());
}


#endif
