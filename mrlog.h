#ifndef MR_LOG_H_
#define MR_LOG_H_

#ifdef ANDROID_LOG
#include <android/log.h>
#define LOG_TAG "mrlog"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__))
#elif IOS_LOG
    #define LOGI(...)  printf(__VA_ARGS__)
    #define LOGD(...)  printf(__VA_ARGS__)
    #define LOGE(...)  printf(__VA_ARGS__)
#else
    #define LOGI(...)
    #define LOGD(...)
    #define LOGE(...)
#endif

#if _WIN32
    #include <windows.h>
    class MRTimerSingleton{
    public:
		LONGLONG _freq;
        static MRTimerSingleton * getInstance(){
            static MRTimerSingleton instance;
            return &instance;
        }
    private:
        MRTimerSingleton(){
			LARGE_INTEGER tmp;
            QueryPerformanceFrequency(&tmp);
            _freq = tmp.QuadPart;
        }
	};
    inline float mrtimer_elapse(const LARGE_INTEGER &func){
        LARGE_INTEGER func_e;
        QueryPerformanceCounter(&func_e);
        return (LONGLONG)((func_e.QuadPart - func.QuadPart) * 1000.0 / MRTimerSingleton::getInstance()->_freq);
	};
    #define MRTIMER_START(func) LARGE_INTEGER func; QueryPerformanceCounter(&func);
    #define MRTIMER_END(func) mrtimer_elapse(func)
#else
    #include <sys/time.h>

    #ifdef __cplusplus
        inline float mrtimer_elapse(const timeval& func){
            timeval func_e;
            gettimeofday(&func_e, NULL);
            return (func_e.tv_sec - func.tv_sec) * 1000 + (func_e.tv_usec - func.tv_usec) / 1000.0;
        }
        #define MRTIMER_START(func) timeval func; gettimeofday(&func, NULL);
        #define MRTIMER_END(func) mrtimer_elapse(func)
    #endif

    static long getTime(){
        struct timeval tp;
        gettimeofday(&tp, NULL);
        long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        return ms;
    }

    static int fps()
    {
        static int fps = 0;
        static long lastTime = 0;
        static int frameCount = 0;
        long curTime = getTime();
        if (curTime - lastTime > 1000){
            fps = frameCount;
            frameCount = 0;
            lastTime = curTime;
        }
        ++frameCount;
        return fps;
    }
#endif

#endif
