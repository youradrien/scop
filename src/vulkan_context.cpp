/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vulkancontext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julessainthorant <marvin@42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 13:32:32 by julessainthor     #+#    #+#             */
/*   Updated: 2026/06/18 13:32:33 by julessainthor    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vulkan_context.hpp"

vulkan_context::vulkan_context(window &_window): _window(_window)
{
    (void)(_window);
    //this->_window = (_window);

    // get SDL2 extensions
    /*
    uint32_t count = 0;
    SDL_Vulkan_GetInstanceExtensions(_window.get(), &count, nullptr);

    std::vector<const char*> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(_window.get(), &count, extensions.data());

    // create VKinstance
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.enabledExtensionCount = count;
    info.ppEnabledExtensionNames = extensions.data();

    vkCreateInstance(&info, nullptr, &instance);

    // surface SDL → Vulkan
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(_window.get(), instance, &surface))
    {
        throw std::runtime_error("Failed to create Vulkan surface");
    }
    */

    init_vulkan();
}
vulkan_context::~vulkan_context()
{
    if(this->_instance){
        vkDestroyInstance(instance, nullptr);
    }
}


void VulkanContext::initVulkan()
{
    create_Instance();
    create_Surface();
    pick_Physical_Device();
    create_Logical_Device();
    create_Swapchain();
    create_Image_Views();
    create_RenderPass();
    create_Pipeline();
    create_Framebuffers();
    create_CommandBuffers();
}

// INSTANCE (SDL2 EXTENSIONS mandatory)
void VulkanContext::create_Instance()
{
    uint32_t sdl2_extensions_count;
    // get SDL2 extensions
    SDL_Vulkan_GetInstanceExtensions(
        _window.get(), &sdl2_extensions_count, nullptr
    );
    std::vector<const char*> extensions(sdl2_extensions_count);
    SDL_Vulkan_GetInstanceExtensions(
        _window.get(), &sdl2_extensions_count, extensions.data()
    );

    // VK app_info
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "scop";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    // VK instance
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = (sdl2_extensions_count);
    info.ppEnabledExtensionNames = extensions.data();
    info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    info.apiVersion = VK_API_VERSION_1_0;
    info.pApplicationName = "scop";
    info.enabledLayerCount = 0;
    // create instance
    vkCreateInstance(&info, nullptr, &(this->_instance));
    if (vkCreateInstance(&info, nullptr, &(this->instance)) != VK_SUCCESS) {
        throw std::runtime_error("(VK) echec de la création de l'instance!");
    }

    // VK extensions support
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
    std::cout << "(VK) extensions disponibles :\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
}

// surface SDL2 -> Vulkan
void VulkanContext::create_Surface()
{
    if (!SDL_Vulkan_CreateSurface(_window.get(), this->_instance, &_surface))
        throw std::runtime_error("Failed to create surface");
}

// PHYSICAL DEVICE (GPU)
// -> chercher et sélectionner une carte graphique (physical device)
void VulkanContext::pick_Physical_Device()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(this->_instance, &count, nullptr);

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(this->_instance, &count, devices.data());
    if (count == 0) {
        throw std::runtime_error("aucune carte graphique ne supporte Vulkan!");
    }

    this->_physical_Device = devices[0]; // version simplifiée
}

// LOGICAL DEVICE
// -> générer un logical device pour servir d'interface
// création d'un logical device requiert encore que nous remplissions des informations 
// dans des structures. La première de ces structures s'appelle VkDeviceQueueCreateInfo. 
// Elle indique le nombre de queues que nous désirons pour chaque queue family.
//  Pour le moment nous n'avons besoin que d'une queue originaire d'une unique queue family : la première avec un support pour les graphismes.
void VulkanContext::create_Logical_Device()
{
    float priority = 1.0f;

    VkDeviceQueueCreateInfo queue_info{};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = &priority;

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pQueueCreateInfos = &(queue_info);
    info.queueCreateInfoCount = 1;

    vkCreateDevice(this->_physicalDevice, &info, nullptr, &(this->_device));

    vkGetDeviceQueue(this->_device, 0, 0, &(this->_graphics_Queue));
    vkGetDeviceQueue(this->_device, 0, 0, &(this->_present_Queue));
}
