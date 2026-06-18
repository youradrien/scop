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

#include <mesh.hpp>
#include "window.hpp"

class vulkan_context
{
    public:
        explicit vulkan_context(window& window);
        ~vulkan_context();

        void upload_mesh(const mesh& mesh);
        void draw_frame();
        void wait_idle();

    private:
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapchain();
        void createRenderPass();
        void createPipeline();
};