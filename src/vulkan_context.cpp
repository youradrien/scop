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
        vkDestroyInstance(this->_instance, nullptr);
    }
}


void vulkan_context::init_vulkan()
{
    create_instance();
    // create_Surface();
    // pick_Physical_Device();
    // create_Logical_Device();
    // create_Swapchain();
    // create_Image_Views();
    // create_RenderPass();
    // create_Pipeline();
    // create_Framebuffers();
    // create_CommandBuffers();
}

// INSTANCE (SDL2 EXTENSIONS mandatory)
void vulkan_context::create_instance()
{

    // get SDL2 extensions
    uint32_t extensions_count = 0;
    if (!SDL_Vulkan_GetInstanceExtensions(
        _window.get(),
        &extensions_count,
        nullptr)) // check 1st
    {
        std::cout << "SDL2 extensions fur vulkan not found" << std::endl;
        throw std::runtime_error(SDL_GetError());
    }
    SDL_Vulkan_GetInstanceExtensions(
        _window.get(), &extensions_count, nullptr
    );
    std::vector<const char*> extensions(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(
        _window.get(), &extensions_count, extensions.data()
    );
    std::cout << "\033[33m[SDL2] extensions requises:\n";
    for (uint32_t i = 0; i < extensions_count; ++i)
    {
        std::cout << '\t' << extensions[i] << '\n';
    }
    std::cout << "\033[0m" << std::endl;
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);



    // VK aapplication info
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "scop";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;



    // VK validations layers
    const uint32_t WIDTH = 800,
                   HEIGHT = 600;
    const std::vector<const char*> validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    #ifdef NDEBUG
        constexpr bool enable_validation_layers = false;
    #else
        constexpr bool enable_validation_layers = true;
    #endif
    uint32_t C;
    vkEnumerateInstanceLayerProperties(&C, nullptr);
    std::vector<VkLayerProperties> layers(C);
    vkEnumerateInstanceLayerProperties(&C, layers.data());
    uint32_t ext_found = 0;
    bool validations_layers_supported = false;
    for (const char* name : validation_layers)
    {
        for (const auto& layer : layers)
        {
            if (strcmp(name, layer.layerName) == 0)
                ext_found++;
        }
    }
    if(ext_found == validation_layers.size())
        validations_layers_supported = (true);
    if (enable_validation_layers && !validations_layers_supported) {
        throw std::runtime_error("les validations layers sont activées mais ne sont pas disponibles!");
    }else{
        std::cout << "\033[36m(VK) SDK validations layers disponibles: " << std::endl;
        for (const auto& layer : layers)
            std::cout << "\t layer: "<<layer.layerName << " supported." << std::endl;
        std::cout << "\033[0m" << std::endl;
    }


    // VK instance
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &app_info;
    info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();
    info.enabledLayerCount = 0;
    if(enable_validation_layers)
    {
        info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        info.ppEnabledLayerNames = validation_layers.data();
    }
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    // create instance
    VkResult instance_res = vkCreateInstance(&info, nullptr, &(this->_instance));
    if (instance_res != VK_SUCCESS)
    {
        std::cerr << "vkCreateInstance failed: "
                << instance_res
                << std::endl;
        if(instance_res == -9){
            // brew install molten-vk
            std::cerr << "on (macOS) run: brew install molten-vk";
        }
        if(instance_res == -6){
            // manually install vulkan SDK
            std::cerr << "install vulkan (SDK) 2.5/5go at: https://vulkan.lunarg.com/sdk/home";
        }
        throw std::runtime_error("(VK) echec de la création de l'instance!");
    }
    

    // VK extensions support
    uint32_t c = 0;
    VkResult extension_res = vkEnumerateInstanceExtensionProperties(
        nullptr, &c, nullptr
    );
    if ((extension_res) != VK_SUCCESS)
        throw std::runtime_error("failed to enumerate extensions");
    std::vector<VkExtensionProperties> VK_extensions(c);
    vkEnumerateInstanceExtensionProperties(
        nullptr, &c, VK_extensions.data()
    );
    std::cout << "\033[32m(VK) extensions disponibles :\n";
    for (const auto& extension : VK_extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    std::cout << "\033[0m" << std::endl;
}



// surface SDL2 -> Vulkan
void vulkan_context::create_Surface()
{
    if (!SDL_Vulkan_CreateSurface(_window.get(), this->_instance, &_surface))
        throw std::runtime_error("Failed to create surface");
}



// PHYSICAL DEVICE (GPU)
// -> chercher et sélectionner une carte graphique (physical device)
void vulkan_context::pick_Physical_Device()
{
    // uint32_t count = 0;
    // vkEnumeratePhysicalDevices(this->_instance, &count, nullptr);

    // std::vector<VkPhysicalDevice> devices(count);
    // vkEnumeratePhysicalDevices(this->_instance, &count, devices.data());
    // if (count == 0) {
    //     throw std::runtime_error("aucune carte graphique ne supporte Vulkan!");
    // }

    // this->_physical_Device = devices[0]; // version simplifiée
}



// LOGICAL DEVICE
// -> générer un logical device pour servir d'interface
// création d'un logical device requiert encore que nous remplissions des informations 
// dans des structures. La première de ces structures s'appelle VkDeviceQueueCreateInfo. 
// Elle indique le nombre de queues que nous désirons pour chaque queue family.
//  Pour le moment nous n'avons besoin que d'une queue originaire d'une unique queue family : la première avec un support pour les graphismes.
void vulkan_context::create_Logical_Device()
{
    // float priority = 1.0f;

    // VkDeviceQueueCreateInfo queue_info{};
    // queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    // queue_info.queueCount = 1;
    // queue_info.pQueuePriorities = &priority;

    // VkDeviceCreateInfo info{};
    // info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // info.pQueueCreateInfos = &(queue_info);
    // info.queueCreateInfoCount = 1;

    // vkCreateDevice(this->_physicalDevice, &info, nullptr, &(this->_device));

    // vkGetDeviceQueue(this->_device, 0, 0, &(this->_graphics_Queue));
    // vkGetDeviceQueue(this->_device, 0, 0, &(this->_present_Queue));
}
