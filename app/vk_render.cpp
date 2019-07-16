#include "vk_render.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

namespace defaults
{
    const bool enable_validation_layers = true;
    const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };
}

VKRender::VKRender(uint32_t width, uint32_t height)
    : m_width(width), m_height(height)
{
}

void VKRender::init(std::vector<const char*> const& extensions, std::function<vk::UniqueSurfaceKHR(vk::Instance)> create_surface_func)
{
    initInstance(extensions);

    initDevice(create_surface_func != nullptr);

    if (create_surface_func != nullptr)
    {
        // Create surface and present queue
        m_surface = create_surface_func(*m_instance);
        if ((m_present_queue_family_index = m_device->findPresentQueueFamilyIndex(m_surface.get())) < 0)
        {
            throw std::runtime_error("Can not find a graphics and present queue");
        }
        m_present_queue = m_device->getDevice().getQueue(m_present_queue_family_index, 0);

        createSwapchain();
    }

    //createImageViews();         // 创建图像视图
    //createRenderPass();         // 渲染通道
    //createDescriptorSetLayout();// 创建描述符设置布局
    //createGraphicsPipeline();   // 图形管线
    //createDepthResources();     // 创建深度图像
    //createFramebuffers();       // ֡帧缓冲区
    //createTextureImage();       // 创建加载纹理图片 stb库
    //createTextureImageView();   // 创建图像视图访问纹理图像
    //createTextureSampler();     // 创建配置采样器对象
    ////loadModel();                // 加载模型 tinyobjloader库
    ////loadModel1();               // 测试三角形
    //loadModel2();               // 画球
    //createVertexBuffer();       // 创建顶点缓冲区
    //createIndexBuffer();        // 创建顶点索引缓冲区
    //createUniformBuffer();      // 创建全局缓冲区
    //createDescriptorPool();     // 创建描述符集合
    //createDescriptorSet();      // 创建分配描述符集合
    //createCommandBuffers();     // 创建命令缓冲区
    //createSemaphores();         // 创建信号对象

    if (defaults::enable_validation_layers)
    {
        setupDebugCallback();
    }
}

void VKRender::destroy()
{
}

void VKRender::waitIdle()
{
    m_device->waitIdle();
}

void VKRender::initInstance(std::vector<const char*> const& extensions)
{
    std::vector<const char*> layers;
    std::vector<const char*> exts = extensions;
    if (defaults::enable_validation_layers)
    {
        auto instance_layer_properties = vk::enumerateInstanceLayerProperties();
        if (checkLayers(defaults::validation_layers, instance_layer_properties))
        {
            exts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            layers = defaults::validation_layers;
        }
        else
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
    }

    vk::ApplicationInfo app_info(
        "VK Demo",
        VK_MAKE_VERSION(1, 0, 0),
        "No Engine",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_0
    );

    vk::InstanceCreateInfo create_info(
        vk::InstanceCreateFlags(),
        &app_info,
        layers.size(),
        layers.data(),
        exts.size(),
        exts.data()
    );

    m_instance = vk::createInstanceUnique(create_info);
    if (!m_instance.get())
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

void VKRender::initDevice(bool swapchain_required)
{
    auto const& physical_devices = m_instance->enumeratePhysicalDevices();
    if (physical_devices.size() == 0)
    {
        throw std::runtime_error("Failed to find physical device!");
    }

    std::vector<const char*> exts;
    if (swapchain_required)
    {
        exts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }
    m_device = VKDevice::create(physical_devices[0], exts);

    printPhysicalDeviceInfo();
}

void VKRender::createSwapchain()
{
    auto const& physical_device = m_device->getPhysicalDevice();

    auto const& capabilities = physical_device.getSurfaceCapabilitiesKHR(m_surface.get());
    auto const& formats = physical_device.getSurfaceFormatsKHR(m_surface.get());
    auto const& present_modes = physical_device.getSurfacePresentModesKHR(m_surface.get());
    assert(!formats.empty() && !present_modes.empty());

    auto format = formats[0];
    for (auto const& f : formats)
    {
        if (f.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            format = f;
            break;
        }
    }

    auto present_mode = vk::PresentModeKHR::eFifo;

    auto extent = vk::Extent2D(
        std::clamp(m_width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp(m_height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    );

    uint32_t image_count = std::min(capabilities.minImageCount + 1, capabilities.maxImageCount);

    auto create_info = vk::SwapchainCreateInfoKHR(
        vk::SwapchainCreateFlagsKHR(),
        m_surface.get(),
        image_count,
        format.format, format.colorSpace,
        extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        vk::SharingMode::eExclusive,
        0, nullptr,
        vk::SurfaceTransformFlagBitsKHR::eIdentity,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        present_mode,
        true,
        nullptr
    );

    // If present and graphics queues are not the same one, set image sharing mode to concurrent
    auto graphics_queue_family_index = m_device->getGraphicsQueueFamilyIndex();
    if (m_present_queue_family_index != graphics_queue_family_index)
    {
        uint32_t queue_family_indices[] = { (uint32_t)graphics_queue_family_index, (uint32_t)m_present_queue_family_index };
        create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }

    m_swapchain = m_device->getDevice().createSwapchainKHRUnique(create_info);

    m_swapchain_images = m_device->getDevice().getSwapchainImagesKHR(m_swapchain.get());
}

bool VKRender::checkLayers(std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties)
{
    // return true if all layers are listed in the properties
    return std::all_of(layers.begin(), layers.end(), [&properties](char const* name)
        {
            return std::find_if(properties.begin(), properties.end(), [&name](vk::LayerProperties const& property)
                {
                    return strcmp(property.layerName, name) == 0;
                }) != properties.end();
        });
}

PFN_vkCreateDebugReportCallbackEXT  pfnVkCreateDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT pfnVkDestroyDebugReportCallbackEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
    return pfnVkCreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pCallback);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
    pfnVkDestroyDebugReportCallbackEXT(instance, callback, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugFunc(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData)
{
    std::ostringstream message;

    switch (flags)
    {
    case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
        message << "INFORMATION: ";
        break;
    case VK_DEBUG_REPORT_WARNING_BIT_EXT:
        message << "WARNING: ";
        break;
    case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
        message << "PERFORMANCE WARNING: ";
        break;
    case VK_DEBUG_REPORT_ERROR_BIT_EXT:
        message << "ERROR: ";
        break;
    case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
        message << "DEBUG: ";
        break;
    default:
        message << "unknown flag (" << flags << "): ";
        break;
    }
    message << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;

    std::cout << message.str() << std::endl;

    return VK_FALSE;
}

void VKRender::setupDebugCallback()
{
    if (!defaults::enable_validation_layers) return;

    // vkCreateDebugReportCallbackEXT() and vkDestroyDebugReportCallbackEXT() are not part of Vulkan core
    // The extension function address need to be dynamically loaded
    pfnVkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(m_instance->getProcAddr("vkCreateDebugReportCallbackEXT"));
    if (!pfnVkCreateDebugReportCallbackEXT)
    {
        throw std::runtime_error("GetInstanceProcAddr: Unable to find vkCreateDebugReportCallbackEXT function.");
    }
    pfnVkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(m_instance->getProcAddr("vkDestroyDebugReportCallbackEXT"));
    if (!pfnVkDestroyDebugReportCallbackEXT)
    {
        throw std::runtime_error("GetInstanceProcAddr: Unable to find vkDestroyDebugReportCallbackEXT function.");
    }

    m_debug_callback = m_instance->createDebugReportCallbackEXTUnique(vk::DebugReportCallbackCreateInfoEXT(
        vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning,
        vkDebugFunc));

    if (!m_debug_callback.get())
    {
        throw std::runtime_error("Failed to set up debug callback!");
    }
}

void VKRender::printPhysicalDeviceInfo()
{
    if (!m_instance.get())
    {
        return;
    }

    auto const& physical_devices = m_instance->enumeratePhysicalDevices();

    std::cout << "Found available device(s): " << std::endl;
    for (auto i = 0; i < physical_devices.size(); ++i)
    {
        auto const& properties = physical_devices[i].getProperties();
        std::cout << i << ": " << properties.deviceName << std::endl;

        auto queue_family_properties = physical_devices[i].getQueueFamilyProperties();
        for (auto j = 0; j < queue_family_properties.size(); ++j)
        {
            auto const& queue_family = queue_family_properties[j];
            std::cout << "    Queue family #" << j << "  count: " << queue_family.queueCount << "  " << vk::to_string(queue_family.queueFlags) << std::endl;
        }
    }
}


namespace
{
    struct VKRenderConcrete : public VKRender
    {
        VKRenderConcrete(uint32_t width, uint32_t height)
            : VKRender(width, height) {}
    };
}

VKRender::UPtr VKRender::create(uint32_t width, uint32_t height)
{
    return std::make_unique<VKRenderConcrete>(width, height);
}
