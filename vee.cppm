export module vee;
export import :debug_utils_messenger;
export import :instance;
export import :physical_device;

#ifdef __APPLE__
export import :metal;
#elif __ANDROID__
export import :android;
#elif defined(_WIN32)
export import :windows;
#endif
