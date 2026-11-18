#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector> 

class ThreadPool 
{
    private: 
        std::vector<std::thread> workers; 
        std::queue<std::function<void()>> tasks; 
        std::mutex queue_mutex; 
        std::condition_variable condition; 
        bool stop = false;

        void worker_function()
        {
          while (true)
          {
            std::unique_lock<std::mutex> lock(queue_mutex);
            while (!stop && tasks.empty())
            {
                condition.wait(lock); 
            }

            if (stop && tasks.empty())
            {
                return; 
            }

            std::function<void()> task = tasks.front(); 
            tasks.pop(); 

            lock.unlock(); 

            task(); 
          }  

        }
        
    public: 
        ThreadPool(std::size_t num_threads) 
        {
            for (std::size_t i = 0; i < num_threads; ++i)
            {
                workers.emplace_back(&ThreadPool::worker_function, this); // Construct an std::thread inside workers
                                                                                                 // without explicitly creating a temporary object first. 
            }
        }

        void enqueue(std::function<void()> task)
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex); 
                tasks.push(task); 
            }

            condition.notify_one(); 
            
        }

        ~ThreadPool() 
        {

            {
                std::unique_lock<std::mutex> lock(queue_mutex); 
                stop = true; 
            }

            // Wake all sleeping worker threads. 
            condition.notify_all(); 

            // Ensures std::terminate does not get called at the destruction of a ThreadPool object. 
            for (std::thread& worker : workers) // (std::thread& because std::thread objects cannot be copied). 
            {
                worker.join(); 
            }
        }

}; 

void say_hello()
{
    std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl; 
}


int main()
{

    ThreadPool pool(3); 
    pool.enqueue(say_hello); 
    pool.enqueue(say_hello); 
    pool.enqueue(say_hello); 
    
    return 0; 
}