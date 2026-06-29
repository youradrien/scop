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
    create_swapchain();
    create_image_views();
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
std::vector<const char*> vulkan_context::get_instance_extensions() const
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
    std::vector<const char*> extensions = this->get_instance_extensions();

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

    std::cout << "\033[32m" << "[VK] Surface créée." << "\033[0m\n";
}




// queue families
// const std::vector<const char*> device_extensions = {
//     VK_KHR_SWAPCHAIN_EXTENSION_NAME
// };
queue_family_indices vulkan_context::find_queue_families(VkPhysicalDevice device)
{
    queue_family_indices indices;
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_families.size(); ++i)
    {
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


// verification portabilité macOs (VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME)
// verification du support de la swap chain (VK_KHR_SWAPCHAIN_EXTENSION_NAME)
bool vulkan_context::check_device_extensions_support(VkPhysicalDevice device)
{
    uint32_t c;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &c, nullptr);

    std::vector<VkExtensionProperties> available_extensions(c);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &c, available_extensions.data());

    std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

    if (!required_extensions.empty())
    {
        std::cout << "[VK] physical device: needed device extensions:\n";
        for (const auto& ext : required_extensions)
            std::cout << "\t" << ext << "\n";
    }
    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }
    if(required_extensions.empty())
    {
        std::cout << "\033[32m[VK] physical device: every device-extensions supported.\033[0m\n"; 
    }
    return required_extensions.empty();
}
bool vulkan_context::is_device_suitable(VkPhysicalDevice device)
{
    (void)(device);
    queue_family_indices indices = this->find_queue_families(device);
    bool extenstions_supported = check_device_extensions_support(device);

    bool swapchain_adequate = false;
    if (extenstions_supported) {
        swapchain_support_details swapchain_support = query_swapchain_support(device);
        swapchain_adequate = (
            !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty()
        );
        std::cout << "\033[32m[VK] physical device: swapchain adequate (support swapchain assuré).\033[0m\n";  
    }
    return indices.is_complete() &&  (extenstions_supported) && swapchain_adequate;
}
// PHYSICAL DEVICE (GPU)
// -> chercher et sélectionner une carte graphique (physical device)
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
            std::cout << "\033[32m[VK] physical device: [GPU] sélectionné: \033[0m " << properties.deviceName << std::endl;
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
// DEVICE EXTENSIONS <---------------
const std::vector<const char*> vulkan_context::device_extensions = {
    VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
void vulkan_context::create_logical_device()
{
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


// swapchain adequate or not for a given logical device
swapchain_support_details vulkan_context::query_swapchain_support(VkPhysicalDevice device)
{
    swapchain_support_details details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->_surface, &details.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->_surface, &format_count, nullptr);

    if (format_count != 0) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->_surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->_surface, &present_mode_count, nullptr);

    if (present_mode_count != 0) {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, this->_surface, &present_mode_count, details.present_modes.data()
        );
    }
    return details;
}

// choose swapchain best parameters
// quelques fonctions qui détermineront les bons paramètres pour obtenir la swap chain 
// la plus efficace possible. il y a 3 types de paramètres à déterminer :
// - format de la surface (profondeur de la couleur)
// - modes de présentation (conditions de "l'échange" des images avec l'écran)
// - swap extent (résolution des images dans la swap chain)

// best surface format possible -> (swapchain_support_details.formats)
VkSurfaceFormatKHR vulkan_context::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
    // chaque VkSurfaceFormatKHR contient les données format et colorSpace
    // - [format] indique les canaux de couleur disponibles et types qui contiennent valeurs des gradients
    //   (ex. VK_FORMAT_B8G8R8A8_SRGB signifie que nous stockons les canaux de couleur R, G, B et A)
    // - [colorSpace] permet de vérifier que sRGB est supporté en utilisant
    //   le champ de bits VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    for (const auto& available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB 
            && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        ){
            return available_format;
        }
    }
    //if(available_formats.size() > 0)
    return (available_formats[0]);
}
// best presentmode possible -> 
VkPresentModeKHR vulkan_context::choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes)
{
    /*
    voir (mode de presentation): https://vulkan-tutorial.com/fr/Dessiner_un_triangle/Presentation/Swap_chain
    VK_PRESENT_MODE_MAILBOX_KHR : ce mode est une autre variation du second mode (VK_PRESENT_MODE_FIFO_KHR). 
        Au lieu de bloquer l'application quand le file d'attente est pleine, 
        les images présentes dans la queue sont simplement remplacées par de nouvelles. 
        Ce mode peut être utilisé pour implémenter le triple buffering, qui vous permet d'éliminer le tearing 
        tout en réduisant le temps de latence entre le rendu et l'affichage qu'une file d'attente implique. 
        (1 front buffer ) + (2 back buffers)
        -> ready buffer -> rendering buffer -> presenting buffer -> 
    */
    for (const auto& available_present_mode : available_present_modes) {
        // selectionné triple buffering
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

// best swap extent
VkExtent2D vulkan_context::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    // swap extent donne la résolution des images dans la swap chain et correspond 
    // quasiment toujours à la résolution de la fenêtre que nous utilisons.
    // VkSurfaceCapabilitiesKHR: étendue des résolutions disponibles.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    } else {
        int WIDTH, HEIGHT;
        SDL_GetWindowSize(_window.get(), &WIDTH, &HEIGHT);

        VkExtent2D actual_extent = {
            static_cast<uint32_t>(WIDTH),
            static_cast<uint32_t>(HEIGHT)
        };

        actual_extent.width = std::max(
            capabilities.minImageExtent.width,
            std::min(actual_extent.width, capabilities.maxImageExtent.width)
        );

        actual_extent.height = std::max(
            capabilities.minImageExtent.height,
            std::min(actual_extent.height, capabilities.maxImageExtent.height)
        );
        return (actual_extent);
    }
}


// [SWAPCHAIN]
// Vulkan ne possède pas de concept comme le framebuffer par défaut, et nous devons donc créer une infrastructure
// qui contiendra les buffers sur lesquels nous effectuerons les rendus avant de les présenter à l'écran. 
// Cette infrastructure s'appelle swap chain sur Vulkan et doit être créée explicitement. 
// La swap chain est essentiellement une file d'attente d'images attendant d'être affichées
/*
VkSwapchainKHR
    │
    ├── VkImage
    │       │
    │       └── VkImageView
    │               │
    │               └── VkFramebuffer
    │
    ├── VkImage
    │       │
    │       └── VkImageView
    │               │
    │               └── VkFramebuffer
    │
    └── VkImage
            │
            └── VkImageView
                    │
                    └── VkFramebuffer
*/
void    vulkan_context::create_swapchain()
{
    swapchain_support_details swapchain_support = this->query_swapchain_support(this->_physical_device);

    VkSurfaceFormatKHR surface_format = this->choose_swap_surface_format(swapchain_support.formats);
    VkPresentModeKHR present_mode = this->choose_swap_present_mode(swapchain_support.present_modes);
    VkExtent2D extent = this->choose_swap_extent(swapchain_support.capabilities);

    uint32_t image_count = swapchain_support.capabilities.minImageCount + 1;
    if (swapchain_support.capabilities.maxImageCount > 0
        && image_count > swapchain_support.capabilities.maxImageCount)
    {
        image_count = swapchain_support.capabilities.maxImageCount;
    }
    VkSwapchainCreateInfoKHR swapchain_info{};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = this->_surface;
    swapchain_info.minImageCount = (image_count);
    swapchain_info.imageFormat = surface_format.format;
    swapchain_info.imageColorSpace = surface_format.colorSpace;
    swapchain_info.imageExtent = extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queue_family_indices indices = this->find_queue_families(this->_physical_device);
    uint32_t queueFamilyIndices[] = {indices.graphics_family, indices.present_family};

    // mode concurrent(VK_SHARING_MODE_CONCURRENT), 
    // si la queue des graphismes n'est pas la même que la queue de présentation. 
    // nous devrons alors dessiner avec la graphics queue puis fournir l'image à la presentation queue.
    // -> swapchain -> 1 image utilisée par -> plusieurs queues concurrentes
    if (indices.graphics_family != indices.present_family) {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0; // optionnel
        swapchain_info.pQueueFamilyIndices = nullptr; // optionnel
    }
    // transformation à appliquer aux images: pour linstant aucune
    swapchain_info.preTransform = swapchain_support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = VK_NULL_HANDLE;

    // création de la SWAPCHAIN
    if (vkCreateSwapchainKHR(this->_device, &swapchain_info, nullptr, &(this->_swapchain)) != VK_SUCCESS)
    {
        throw std::runtime_error("échec de la création de la swap chain!");
    }

    vkGetSwapchainImagesKHR(this->_device, this->_swapchain, &image_count, nullptr);
    this->_swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(this->_device, this->_swapchain, &image_count, this->_swapchain_images.data());
    this->_swapchain_image_format = surface_format.format;
    this->_swapchain_extent = extent;
}



// [IMAGE VIEWS]
void vulkan_context::create_image_views()
{
    this->_swapchain_imagesviews.resize(this->_swapchain_images.size());
    // each image of the swapchain
    for(size_t i = 0; i < this->_swapchain_images.size(); i++)
    {
        VkImageViewCreateInfo imgview_info{};
        imgview_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgview_info.image = _swapchain_images[i];
        imgview_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imgview_info.format = this->_swapchain_image_format;
        imgview_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgview_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgview_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgview_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imgview_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgview_info.subresourceRange.baseMipLevel = 0;
        imgview_info.subresourceRange.levelCount = 1;
        imgview_info.subresourceRange.baseArrayLayer = 0;
        imgview_info.subresourceRange.layerCount = 1;
        if (vkCreateImageView(this->_device, &imgview_info, nullptr, &(_swapchain_imagesviews[i])) != VK_SUCCESS) {
            throw std::runtime_error("échec de la création d'une image view!");
        }
    }
}


vulkan_context::~vulkan_context()
{
    if(this->_instance)
    {
        if(this->_swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(_device, this->_swapchain, nullptr);
        }
        if (this->_device != VK_NULL_HANDLE)
        {
            for (auto img_view : this->_swapchain_imagesviews) {
                vkDestroyImageView(this->_device, img_view, nullptr);
            }
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