#if defined(__GNUC__) || defined(__clang__)
    #define CAB_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define CAB_DEPRECATED(msg) __declspec(deprecated(msg))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #define CAB_DEPRECATED(msg) [[deprecated(msg)]]
#else
    #define CAB_DEPRECATED(msg)
#endif
