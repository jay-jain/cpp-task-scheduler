# C++ ThreadPool Task Scheduler

A multithreaded task scheduler implemented in modern C++ using the C++ Standard Library.

This project implements a basic thread pool that allows multiple worker threads to execute tasks from a shared queue concurrently.

## Features

- **Multithreading:** Creates a configurable number of worker threads.
- **Task Queue:** Stores tasks using `std::queue<std::function<void()>>`.
- **Thread Synchronization:** Uses `std::mutex` to protect the shared task queue.
- **Condition Variables:** Uses `std::condition_variable` to put idle workers to sleep until tasks become available.
- **Task Submission:** Provides an `enqueue()` function to submit tasks.
- **Graceful Shutdown:** Uses a destructor to notify workers, finish queued tasks, and join all worker threads.

## How It Works

1. The `ThreadPool` constructor creates a specified number of worker threads.
2. Each worker executes `worker_function()`, continuously checking the shared task queue.
3. If the queue is empty, the worker sleeps using `condition.wait(lock)`.
4. When `enqueue()` adds a task, it calls `condition.notify_one()` to wake a waiting worker.
5. The worker removes a task from the queue, releases the mutex, and executes the task.
6. When the ThreadPool is destroyed, the destructor sets `stop = true` and calls `condition.notify_all()`.
7. Workers finish any remaining queued tasks before exiting, and the destructor joins all worker threads.

## Example Usage

```cpp
int main()
{
    ThreadPool pool(3);

    pool.enqueue(say_hello);
    pool.enqueue(say_hello);
    pool.enqueue(say_hello);

    return 0;
}
```

This example creates three worker threads and submits three tasks. Each task prints the ID of the thread executing it.

## Compile and Run

Compile using Clang:

```bash
clang++ -std=c++11 -Wall -pthread main.cpp -o main
```

Run:

```bash
./main
```

Example output:

```text
Hello from thread 0x70000e056000
Hello from thread 0x70000e0d9000
Hello from thread 0x70000e15c000
```

Thread IDs and task execution order may vary between runs.

## Project Structure

```text
cpp-task-scheduler/
├── main.cpp
├── README.md
└── .gitignore
```

The entire ThreadPool implementation is contained in `main.cpp`.