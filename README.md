# Ikun_log

more faster and only header c++ logger，using c++20

## Install

```
git clone https://github.com/ikunikun123456/ikun_log.git
cmake -B build 
cmake --build build 
cmake --install build
```

## Usage

```
#define IKUN_LOG_QUEUE_CAPACITY 131072 //set queue capacity equal 131072

//set loglevel filtering in compile,default loglevel filtering is info
#if defined(IKUN_LOG_ACTIVE_LEVEL_TRACE)  
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::TRACE
#elif defined(IKUN_LOG_ACTIVE_LEVEL_DEBUG)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::DEBUG
#elif defined(IKUN_LOG_ACTIVE_LEVEL_WARN)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::WARN
#elif defined(IKUN_LOG_ACTIVE_LEVEL_ERROR)
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::ERROR
#else
#define IKUN_LOG_ACTIVE_LEVEL Loglevel::INFO
#endif
```



## Platforms

- linux 
- Windows

## Benchmark

test repository:  [https://github.com/ikunikun123456/ikun_log_test.git]()

 on Debian14.4( wsl ) , amd 7640hs cpu (6 core and 12 threads)

calculate push queue time(the queue capacity ishigher than log count)

note: it is system clock counting (the spdlog cpu time is higner than ikun_log when thread count is 4 or 8 )

| async file logger | ikun_log | spdlog(1.17.0) |
| ----------------- | -------- | -------------- |
| 1 thread/per log  | 79.60ns  | 178.95ns       |
| 2 thread/per log  | 93.44ns  | 312.85ns       |
| 4 thread /per log | 132.47ns | 483.05ns       |
| 8 thread/per log  | 211.89ns | 321.28ns       |

calculate push queue time(the queue capacity is lower than log count)

note: it is system clock counting (the spdlog cpu time is higher than ikun_log when thread count is 8)

| async file logger | ikun_log | spdlog(1.17.0) |
| ----------------- | -------- | -------------- |
| 1 thread/per log  | 280.54ns | 311.16ns       |
| 2 thread/per log  | 273.87ns | 657.94ns       |
| 4 thread /per log | 230.50ns | 614.18ns       |
| 8 thread/per log  | 278.11ns | 562.68ns       |

note: the 10000 args don't test exactly when  the 10000 args and 1'000'000 args tested on the same time,because the 1'000'000 args probably make the queue block,so you must make the queue cleaned to test the 10000 args, it is a easiest way that the test apart

```cpp
    #define IKUN_LOG_QUEUE_CAPACITY 131072
    
    static spdlog::logger &get_async_logger() {
      static auto logger = [] {
        spdlog::init_thread_pool(131072, 2);
        auto l = spdlog::basic_logger_mt<spdlog::async_factory>("async_file_logger",
                                                                "logs/async.log");
        l->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        return l;
      }();
      return *logger;
    }
        
    static void BM_TestSpdlog(benchmark::State &state) {
    
        // 2. 创建异步文件 Logger
        auto async_file = get_async_logger();
        int count = state.range();
        int remain = count % state.threads() < state.thread_index() ? 1 : 0;
    
        int thread_count = count / state.threads() + remain;
        for (int i = 0; i <= 100; i++) {
            async_file.info(
                "This message is logged asynchronously!This message! "
                "This message is logged asynchronously!This message is logged ");
        }
    
        for (auto _ : state) {
            for (int i = 0; i <= thread_count; i++) {
            async_file.info("This message is logged asynchronously!This message is!"
                            "This message is logged asynchronously!This message is ");
            }
        }
    }
    
    static void BM_TestIkunlog(benchmark::State &state) {
        int count = state.range();
        int remain = count % state.threads() < state.thread_index() ? 1 : 0;
        int thread_count = count / state.threads() + remain;
    
        ikun::log::AsyncInfo("This message is logged asynchronously");
        for (int i = 0; i <= 100; i++) {
            ikun::log::AsyncInfo(
                "This message is logged asynchronously!This message! "
                "This message is logged asynchronously!This message is logged ");
        }
    
        for (auto _ : state) {
            for (int i = 0; i <= thread_count; i++) {
            ikun::log::AsyncInfo(
                "This message is logged asynchronously!This message! "
                "This message is logged asynchronously!This message is logged ");
            }
        }
    }


   
   BENCHMARK(BM_TestIkunlog)
        ->Arg(1000000)
        ->Threads(1)
        ->Threads(2)
        ->Threads(4)
        ->Threads(8)
        ->UseRealTime()
        ->Iterations(1);
    
    BENCHMARK(BM_TestSpdlog)
        ->Arg(1000000)
        ->Threads(1)
        ->Threads(2)
        ->Threads(4)
        ->Threads(8)
        ->UseRealTime()
        ->Iterations(1);


    BENCHMARK(BM_TestIkunlog)
        ->Arg(10000)
        ->Threads(1)
        ->Threads(2)
        ->Threads(4)
        ->Threads(8)
        ->UseRealTime()
        ->Iterations(1);
    
    BENCHMARK(BM_TestSpdlog)
        ->Arg(10000)
        ->Threads(1)
        ->Threads(2)
        ->Threads(4)
        ->Threads(8)
        ->UseRealTime()
        ->Iterations(1);
```

## Example

```
    //logger,write to console
    Trace("hello world!");
    Debug("hello world!");
   	Info("hello world!");
    Warn("hello world!");
    Error("hello world!");
    
    //async logger,write to file
    AsyncTrace("hello world!");
    AsyncDebug("hello world!");
    AsyncInfo("hello world!");
    AsyncWarn("hello world!");
    AsyncError("hello world!");
      
    //format is std::format
    std::string name = "cxk";
    Trace("hello world!,my name is {}", name);
    Debug("hello world!,my name is {}", name);
    Info("hello world!,my name is {}", name);
    Warn("hello world!,my name is {}", name);
    Error("hello world!,my name is {}", name);
    AsyncTrace("hello world!,my name is {}", name);
    AsyncDebug("hello world!,my name is {}", name);
    AsyncInfo("hello world!,my name is {}", name);
    AsyncWarn("hello world!,my name is {}", name);
    AsyncError("hello world!,my name is {}", name);
```

## Refernce 

**Dmitry Vyukov's MPMC Queue**

include/ikun_log/detail/cpuwait.h

include/ikun_log/detail/cas_queue.h

[https://github.com/MengRao/tscns.git]()

include/ikun_log/detail/tsctons.h