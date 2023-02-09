export module vee;
export import :actions;
export import :buffer;
export import :command_pool;
export import :debug_utils_messenger;
export import :descriptor_set_layout;
export import :device;
export import :device_memory;
export import :fence;
export import :framebuffer;
export import :gr_pipeline;
export import :image;
export import :image_view;
export import :instance;
export import :physical_device;
export import :pipeline_layout;
export import :render_pass;
export import :semaphore;
export import :shader_module;
export import :surface_capabilities;
export import :swapchain;

#ifdef __APPLE__
export import :metal;
#elif __ANDROID__
export import :android;
#elif defined(_WIN32)
export import :windows;
#endif

namespace vee {
export struct api_failure;
export struct out_of_date_error;
} // namespace vee
