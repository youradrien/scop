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

class Scop;
class Window;
class Mesh;
class Camera;

class VulkanContext
{
    public:
        explicit VulkanContext(Window& window);
        ~VulkanContext();

        void uploadMesh(const Mesh& mesh);
        void drawFrame();
        void waitIdle();

    private:
        void createInstance();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapchain();
        void createRenderPass();
        void createPipeline();
};