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
#include <cstring>
#include "window.hpp"

#define VK_ENABLE_BETA_EXTENSIONS

#include <vulkan/vulkan.h>

class vulkan_context
{
    public:
        explicit vulkan_context(window& window);
        ~vulkan_context();

        void upload_mesh(const mesh& mesh);
        void draw_frame();
        void wait_idle();

    private:
        void init_vulkan();
        void create_instance();
        void create_Surface();
        void pick_Physical_Device();
        void create_Logical_Device();
        void create_Swapchain();
        void create_Image_Views();
        void create_RenderPass();
        void create_Pipeline();
        void create_Framebuffers();
        void create_CommandBuffers();

    private:
        window& _window;

        // vulkan instance
        VkInstance _instance;
        // surface SDL → Vulkan
        VkSurfaceKHR _surface;

        // gpu physique device
        // VkPhysicalDevice _physical_Device;
        // // logical device
        // VkDevice _device;

        // // queues?
        // VkQueue _graphics_Queue;
        // VkQueue _present_Queue;

        // VkSwapchainKHR _swapchain;
        // std::vector<VkImage> _images;
        // std::vector<VkImageView> _imageViews;

        // VkRenderPass _renderPass;
        // VkPipeline _pipeline;
        // VkPipelineLayout _pipelineLayout;

        // std::vector<VkFramebuffer> _framebuffers;
        // std::vector<VkCommandBuffer> _cmdBuffers;

        // VkCommandPool _cmdPool;
};