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

vulkan_context::vulkan_context(window &_window): _window(_window),
    _instance(VK_NULL_HANDLE),
    _surface(VK_NULL_HANDLE),
    _debug_messenger(VK_NULL_HANDLE),
    _physical_device(VK_NULL_HANDLE),
    _device(VK_NULL_HANDLE),
    _graphics_queue(VK_NULL_HANDLE),
    _present_queue(VK_NULL_HANDLE)
{
    (void)(_window);
    //this->_window = (_window);
    init_vulkan();
}



void vulkan_context::init_vulkan()
{
    create_instance();
    #ifndef NDEBUG
        setup_debug_messenger();
    #endif
    create_surface();
    pick_physical_device();
    create_logical_device();
}


// validations layers
const std::vector<const char*> vulkan_context::validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};
bool vulkan_context::check_validation_layer_support()
{
    uint32_t layer_count = 0;
    vkEnumerateInstanceLayerProperties(
        &layer_count,
        nullptr
    );
    _available_layers.resize(layer_count);

    vkEnumerateInstanceLayerProperties(
        &layer_count,
        _available_layers.data()
    );

    for (const char* required_layer : validation_layers)
    {
        bool found = false;
        for (const auto& available_layer : _available_layers)
        {
            if (strcmp(
                    required_layer,
                    available_layer.layerName
                ) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    std::cout << "\033[36m(VK) Available validation-layers:\n";
    for (const auto& layer : _available_layers)
    {
        std::cout
            << "\t"
            << layer.layerName
            << '\n';
    }
    std::cout << "\033[0m";
    return true;
}





// fetch required extensions
std::vector<const char*> vulkan_context::get_required_extensions() const
{
    uint32_t c = 0;

    if (!SDL_Vulkan_GetInstanceExtensions(
            _window.get(),
            &c,
            nullptr))
    {
        std::cout << "SDL2 extensions fur vulkan not found" << std::endl;
        throw std::runtime_error(SDL_GetError());
    }
    std::vector<const char*> extensions(c);
    if (!SDL_Vulkan_GetInstanceExtensions(
            _window.get(),
            &c,
            extensions.data()))
    {
        throw std::runtime_error(SDL_GetError());
    }
    std::cout << "\033[33m[SDL2] extensions requises:\n";
    for (uint32_t i = 0; i < c; ++i)
    {
        std::cout << '\t' << extensions[i] << '\n';
    }
    std::cout << "\033[0m" << std::endl;

    #ifdef NDEBUG
        constexpr bool enable_validation_layers = false;
    #else
        constexpr bool enable_validation_layers = true;
    #endif

        if (enable_validation_layers)
        {
            extensions.push_back(
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            );
        }
    #ifdef __APPLE__
        extensions.push_back(
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        );

        extensions.push_back(
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
        );
    #endif
        return extensions;
}






/*
le paramètre pCallbackData est une structure du type VkDebugUtilsMessengerCallbackDataEXT contenant les détails du message.
ses membres les plus importants sont :
    pMessage: Le message sous la forme d'une chaîne de type C terminée par le caractère nul \0
    pObjects: Un tableau d'objets Vulkan liés au message
    objectCount: Le nombre d'objets dans le tableau précédent
*/
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_context::debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
        void* user_data
){
    (void)type;
    (void)user_data;
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        std::cerr << "\033[31m[VK ERROR]\033[0m " << callback_data->pMessage << std::endl;
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
        std::cerr << "\033[33m[VK WARNING]\033[0m " << callback_data->pMessage << std::endl;
    }
    else{
        std::cout << "\033[36m[VK INFO]\033[0m " << callback_data->pMessage<< std::endl;
    }
    return VK_FALSE;
}


void vulkan_context::populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT& info)
{
    info = {};

    info.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    info.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    info.pfnUserCallback = debug_callback;

    info.pUserData = nullptr;
}


// INSTANCE (SDL2 EXTENSIONS mandatory)
void vulkan_context::create_instance()
{
    // [VK] extensions
    std::vector<const char*> extensions = this->get_required_extensions();

    // [VK] aapplication info
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "scop";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No-Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    // [VK] validations layers
    #ifdef NDEBUG
        constexpr bool enable_validation_layers = false;
    #else
        constexpr bool enable_validation_layers = true;
    #endif

    if (enable_validation_layers &&
        !check_validation_layer_support()
    ){
        throw std::runtime_error(
            "Validation layers requested but unavailable"
        );
    }

    // [VK] instance (create_info)
    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &app_info;
    info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    info.ppEnabledExtensionNames = extensions.data();
    info.enabledLayerCount = 0;
    VkDebugUtilsMessengerCreateInfoEXT debug_info{};
    if(enable_validation_layers)
    {
        info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        info.ppEnabledLayerNames = validation_layers.data();
        populate_debug_messenger_create_info(debug_info);
        info.pNext = &debug_info;
    }
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    // [VK] instance
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
    

    // [VK] extensions support
    uint32_t c = 0;
    if ((vkEnumerateInstanceExtensionProperties(nullptr, &c, nullptr))
        != VK_SUCCESS)
    {
        throw std::runtime_error("failed to enumerate extensions");
    }
    std::vector<VkExtensionProperties> VK_extensions(c);
    vkEnumerateInstanceExtensionProperties( nullptr, &c, VK_extensions.data() );
    std::cout << "\033[32m(VK) extensions disponibles/supported :\n";
    for (const auto& extension : VK_extensions)
    {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    std::cout << "\033[0m" << std::endl;
}



static VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* create_info,
    const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* messenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT")
        );

    if (func != nullptr)
        return func(
            instance,
            create_info,
            allocator,
            messenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}
static void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* allocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT")
        );

    if (func)
        func(instance, messenger, allocator);
}

// make debug messenger used by vulkan -> that uses validation layers
void vulkan_context::setup_debug_messenger()
{
#ifdef NDEBUG
    return;
#endif
    VkDebugUtilsMessengerCreateInfoEXT info;
    populate_debug_messenger_create_info(
        info
    );
    if (CreateDebugUtilsMessengerEXT(
            _instance,
            &info,
            nullptr,
            &_debug_messenger)
        != VK_SUCCESS)
    {
        throw std::runtime_error(
            "le messager n'a pas pu être créé!"
        );
    }
    std::cout << "\033[32m[VK] Debug Messenger Ready\033[0m" << std::endl;
}






// surface SDL2 -> Vulkan
void vulkan_context::create_surface()
{
    if (!SDL_Vulkan_CreateSurface(
            _window.get(),
            _instance,
            &(this->_surface) ))
    {
        throw std::runtime_error(SDL_GetError());
    }

    std::cout << "\033[32m"
              << "[VK] Surface créée."
              << "\033[0m\n";
}




// queue families
queue_family_indices vulkan_context::find_queue_families(VkPhysicalDevice device)
{
    queue_family_indices indices;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_families.size(); ++i)
    {
        // VkBool32 present_support = false;
        // vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->_surface, &present_support);
        // graphics queue
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;    
        }
        // PRESENT (SURFACE)
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            device,
            i,
            _surface,
            &present_support
        );
        if (present_support)
        {
            indices.present_family = i;
        }
        if (indices.is_complete())
            break;
    }
    return indices;
}
// PHYSICAL DEVICE (GPU)
// -> chercher et sélectionner une carte graphique (physical device)
bool vulkan_context::is_device_suitable(VkPhysicalDevice device)
{
    (void)(device);
    queue_family_indices indices = this->find_queue_families(device);

    VkPhysicalDeviceProperties _propreties;
    vkGetPhysicalDeviceProperties(device, &_propreties);

    VkPhysicalDeviceFeatures _features;
    vkGetPhysicalDeviceFeatures(device, &_features);

    return /* _propreties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && */
           // _features.geometryShader; 
           true && indices.is_complete();
}
void vulkan_context::pick_physical_device()
{
    uint32_t devices_c = 0;
    vkEnumeratePhysicalDevices(this->_instance, &devices_c, nullptr);
    if (devices_c == 0) {
        throw std::runtime_error("aucune carte graphique ne supporte Vulkan!");
    }

    std::vector<VkPhysicalDevice> devices(devices_c);
    vkEnumeratePhysicalDevices(this->_instance, &devices_c, devices.data());

    for (const auto& device : devices)
    {
        if (is_device_suitable(device))
        {
            this->_physical_device = device;
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties( device, &properties );
            std::cout << "\033[32m[VK] API Vulkan:\033[0m "
                << VK_VERSION_MAJOR(properties.apiVersion) << "."
                << VK_VERSION_MINOR(properties.apiVersion) << "."
                << VK_VERSION_PATCH(properties.apiVersion)
            << std::endl;
            std::cout << "\033[32m[VK] GPU sélectionné:\033[0m " << properties.deviceName << std::endl;
            break;
        }
    }

    if (this->_physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("aucun GPU ne peut exécuter ce programme!");
    }
    // this->_physical_Device = devices[0]; // version simplifiée
}




// LOGICAL DEVICE
// -> générer un logical device pour servir d'interface
// création d'un logical device requiert encore que nous remplissions des informations 
// dans des structures. La première de ces structures s'appelle VkDeviceQueueCreateInfo. 
// Elle indique le nombre de queues que nous désirons pour chaque queue family.
//  Pour le moment nous n'avons besoin que d'une queue originaire d'une unique queue family : la première avec un support pour les graphismes.
const std::vector<const char*> vulkan_context::device_extensions = {
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
};
void vulkan_context::create_logical_device()
{
    /*  
    queue_family_indices indices = find_queue_families(this->_physical_device);

    // create info of "graphique" queue
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = indices.graphics_family;
    queue_create_info.queueCount = 1;

    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;
    */
    queue_family_indices indices = find_queue_families(this->_physical_device);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {
        indices.graphics_family, 
        indices.present_family
    };

    float queue_priority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = (queue_family);
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // create_info.pQueueCreateInfos = &(queue_create_info);
    // create_info.queueCreateInfoCount = 1;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = nullptr;

    #ifdef NDEBUG
        constexpr bool enable_validation_layers = false;
    #else
        constexpr bool enable_validation_layers = true;
    #endif
    if (enable_validation_layers)
    {
        // create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        // create_info.ppEnabledLayerNames = validation_layers.data();
    }

    //instanciation du logical device
    if (vkCreateDevice(this->_physical_device, &create_info, nullptr, &(this->_device)) != VK_SUCCESS)
    {
        throw std::runtime_error("échec lors de la création d'un logical device!");
    }

    vkGetDeviceQueue(this->_device, indices.graphics_family, 0, &_graphics_queue);
    vkGetDeviceQueue(this->_device, indices.present_family, 0, &_present_queue);
}


vulkan_context::~vulkan_context()
{
    if(this->_instance)
    {
        if (this->_device != VK_NULL_HANDLE)
        {
            vkDestroyDevice(_device, nullptr);
            _device = VK_NULL_HANDLE;
        }
        if (this->_surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = VK_NULL_HANDLE;
        }
        #ifndef NDEBUG
            if (this->_instance && _debug_messenger)
            {
                DestroyDebugUtilsMessengerEXT(
                    _instance,
                    _debug_messenger,
                    nullptr
                );
            }
        #endif

        if (this->_instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance(_instance, nullptr);
            _instance = VK_NULL_HANDLE;
        }  
    }
}