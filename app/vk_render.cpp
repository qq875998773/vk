#include "vk_render.hpp"

#include <iostream>
#include <sstream>

bool enableValidationLayers = true;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

void VKRender::init(std::vector<const char*> const& extensions)
{
    initInstance(extensions);
    initDevice();
    initCommandBuffer();

    //createSurface();            // 创建窗体和vulkan实例连接
    //createLogicalDevice();      // 创建逻辑设备
    //createSwapChain();          // 交换链
    //createImageViews();         // 创建图像视图
    //createRenderPass();         // 渲染通道
    //createDescriptorSetLayout();// 创建描述符设置布局
    //createGraphicsPipeline();   // 图形管线
    //createCommandPool();        // 创建令缓冲区
    //createDepthResources();     // 创建深度图像
    //createFramebuffers();       // 帧缓冲区
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

    if (enableValidationLayers)
    {
        setupDebugCallback();
    }
}

void VKRender::destroy()
{
    //vkDestroySampler(device, textureSampler, nullptr);// 销毁纹理采样器
    //vkDestroyImageView(device, textureImageView, nullptr);// 销毁纹理图像视图

    //vkDestroyImage(device, textureImage, nullptr);// 清除贴图图像
    //vkFreeMemory(device, textureImageMemory, nullptr);// 清除贴图图像记录

    //vkDestroyDescriptorPool(device, descriptorPool, nullptr); // 销毁描述对象池

    //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    //vkDestroyBuffer(device, uniformBuffer, nullptr);
    //vkFreeMemory(device, uniformBufferMemory, nullptr);

    //vkDestroyBuffer(device, indexBuffer, nullptr); // 清除顶点索引缓冲区
    //vkFreeMemory(device, indexBufferMemory, nullptr);// 清除顶点索引缓冲区记录

    //vkDestroyBuffer(device, vertexBuffer, nullptr); // 清除顶点缓冲区
    //vkFreeMemory(device, vertexBufferMemory, nullptr);// 清除顶点缓冲区记录

    //vkDestroySemaphore(device, renderFinishedSemaphore, nullptr); // 清除渲染信号
    //vkDestroySemaphore(device, imageAvailableSemaphore, nullptr); // 清除图像信号

    //vkDestroyCommandPool(device, commandPool, nullptr);// 销毁命令缓冲区

    //vkDestroyDevice(device, nullptr); // 清除逻辑设备资源
    //DestroyDebugReportCallbackEXT(instance, callback, nullptr);
    //vkDestroySurfaceKHR(instance, surface, nullptr); // GLFW没有提供专用的函数销毁surface,可以简单的通过Vulkan原始的API
    //vkDestroyInstance(instance, nullptr); // 确保surface的清理是在instance销毁之前完成
}

void VKRender::waitIdle()
{
    m_device->waitIdle();
}

void VKRender::initInstance(std::vector<const char*> const& extensions)
{
    std::vector<const char*> layers;
    std::vector<const char*> exts = extensions;
    if (enableValidationLayers)
    {
        auto instanceLayerProperties = vk::enumerateInstanceLayerProperties();
        if (checkLayers(validationLayers, instanceLayerProperties))
        {
            exts.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            layers = validationLayers;
        }
        else
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
    }

    vk::ApplicationInfo appInfo(
        "VK Demo",
        VK_MAKE_VERSION(1, 0, 0),
        "No Engine",
        VK_MAKE_VERSION(1, 0, 0),
        VK_API_VERSION_1_0
    );

    vk::InstanceCreateInfo createInfo(
        vk::InstanceCreateFlags(),
        &appInfo,
        layers.size(),
        layers.data(),
        exts.size(),
        exts.data()
    );

    m_instance = vk::createInstanceUnique(createInfo);
    if (!m_instance.get())
    {
        throw std::runtime_error("Failed to create instance!");
    }
}

void VKRender::initDevice()
{
    auto const& physical_devices = m_instance->enumeratePhysicalDevices();
    if (physical_devices.size() == 0)
    {
        throw std::runtime_error("Failed to find GPU with Vulkan support!");
    }

    int physical_device_idx = -1;
    int queue_family_idx = -1;

    std::cout << "Found available device(s): " << std::endl;
    for (auto i = 0; i < physical_devices.size(); ++i)
    {
        auto const& properties = physical_devices[i].getProperties();
        std::cout << i << ": " << properties.deviceName << std::endl;
        auto queue_family_properties = physical_devices[i].getQueueFamilyProperties();
        for (auto j = 0; j < queue_family_properties.size(); ++j)
        {
            auto const& queue_family = queue_family_properties[j];

            // Pick the first physical device and queue which supports graphics
            if (physical_device_idx < 0 && queue_family_idx < 0 && queue_family.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                physical_device_idx = i;
                queue_family_idx = j;
                std::cout << "  * ";
            }
            else
            {
                std::cout << "    ";
            }

            std::cout << "Queue family #" << j << "  count: " << queue_family.queueCount << "  " << vk::to_string(queue_family.queueFlags) << std::endl;
        }
    }

    if (physical_device_idx >= 0 && queue_family_idx >= 0)
    {
        m_physical_device = physical_devices[physical_device_idx];
        float queuePriority = 0.f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queue_family_idx, 1, &queuePriority);
        m_device = m_physical_device.createDeviceUnique(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), 1, &deviceQueueCreateInfo));
    }
}

void VKRender::initCommandBuffer()
{
    //m_commandPool = m_device->createCommandPoolUnique(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), ))
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
    if (!enableValidationLayers) return;

    // vkCreateDebugReportCallbackEXT() and vkDestroyDebugReportCallbackEXT() are not part of Vulkan core
    // The function address need to be dynamically loaded
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
