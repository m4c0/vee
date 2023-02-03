export module vee;
export import :command_pool;
export import :debug_utils_messenger;
export import :device;
export import :instance;
export import :physical_device;
export import :render_pass;
export import :surface_format;
export import :swapchain;

#ifdef __APPLE__
export import :metal;
#elif __ANDROID__
export import :android;
#elif defined(_WIN32)
export import :windows;
#endif
