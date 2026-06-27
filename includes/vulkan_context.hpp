/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkancontext.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 12:47:29 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 12:47:30 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mesh.hpp>
#include <set>
#include <cstring>
#include <cstdint> // uint32_t
#include <limits> // std::numeric_limits
#include <algorithm> // std::clamp
#include "window.hpp"

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>

struct queue_family_indices
{
    uint32_t graphics_family;
    uint32_t present_family;

    queue_family_indices()
        : graphics_family(UINT32_MAX),
          present_family(UINT32_MAX)
    {
    }

    bool is_complete() const
    {
        return graphics_family != UINT32_MAX &&
               present_family != UINT32_MAX;
    }
};

struct swapchain_support_details
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

class vulkan_context
{
    public:
        explicit vulkan_context(window& window);
        ~vulkan_context();

        void upload_mesh(const mesh& mesh);
        void draw_frame();
        void wait_idle();

    private:
        // init
        void init_vulkan();
        // vulkan instance
        void create_instance();
        // surface
        void create_surface();
        // extensions
        std::vector<const char*> get_instance_extensions() const;
        // validation layers (sdk) 
        bool check_validation_layer_support();
        // debugger
        void populate_debug_messenger_create_info( VkDebugUtilsMessengerCreateInfoEXT& create_info );
        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
            void* user_data
        );
        void setup_debug_messenger();

        // gpu physique device
        void pick_physical_device();
        bool check_device_extensions_support(VkPhysicalDevice device);
        bool is_device_suitable(VkPhysicalDevice device);

        // queue families
        queue_family_indices find_queue_families(VkPhysicalDevice device);

        // logical device
        void create_logical_device();

        // swapchain
        swapchain_support_details query_swapchain_support(VkPhysicalDevice device);
        VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
        VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes);
        VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
        void create_swapchain();

    private:
        window& _window;

        // vulkan instance
        VkInstance _instance;
        // surface SDL2 → Vulkan
        VkSurfaceKHR _surface;

        // validation layers (sdk) 
        static const std::vector<const char*> validation_layers;
        std::vector<VkLayerProperties> _available_layers;
        // debugger
        VkDebugUtilsMessengerEXT _debug_messenger;

        // gpu physique device
        VkPhysicalDevice _physical_device = VK_NULL_HANDLE
        ;
        // logical device
        static const std::vector<const char*> device_extensions;
        VkDevice _device;

        // // queues?
        VkQueue _graphics_queue;
        VkQueue _present_queue;

        // swapchain
        VkSwapchainKHR _swapchain;
        std::vector<VkImage> _swapchain_images;
        VkFormat _swapchain_image_format;
        VkExtent2D _swapchain_extent;
        // std::vector<VkImageView> _imageViews;

        // VkRenderPass _renderPass;
        // VkPipeline _pipeline;
        // VkPipelineLayout _pipelineLayout;

        // std::vector<VkFramebuffer> _framebuffers;
        // std::vector<VkCommandBuffer> _cmdBuffers;

        // VkCommandPool _cmdPool;
};