#include "vk_render.hpp"

#include <iostream>
#include <sstream>

bool enableValidationLayers = true;

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

void VKRender::init(std::vector<const char*> const& extensions)
{
    initInstance(extensions);
    initDevice();

    //createSurface();            // ���������vulkanʵ������
    //createLogicalDevice();      // �����߼��豸
    //createSwapChain();          // ������
    //createImageViews();         // ����ͼ����ͼ
    //createRenderPass();         // ��Ⱦͨ��
    //createDescriptorSetLayout();// �������������ò���
    //createGraphicsPipeline();   // ͼ�ι���
    //createCommandPool();        // ���������
    //createDepthResources();     // �������ͼ��
    //createFramebuffers();       // ֡������
    //createTextureImage();       // ������������ͼƬ stb��
    //createTextureImageView();   // ����ͼ����ͼ��������ͼ��
    //createTextureSampler();     // �������ò���������
    ////loadModel();                // ����ģ�� tinyobjloader��
    ////loadModel1();               // ����������
    //loadModel2();               // ����
    //createVertexBuffer();       // �������㻺����
    //createIndexBuffer();        // ������������������
    //createUniformBuffer();      // ����ȫ�ֻ�����
    //createDescriptorPool();     // ��������������
    //createDescriptorSet();      // ������������������
    //createCommandBuffers();     // �����������
    //createSemaphores();         // �����źŶ���

    if (enableValidationLayers)
    {
        setupDebugCallback();
    }
}

void VKRender::destroy()
{
    //vkDestroySampler(device, textureSampler, nullptr);// �������������
    //vkDestroyImageView(device, textureImageView, nullptr);// ��������ͼ����ͼ

    //vkDestroyImage(device, textureImage, nullptr);// �����ͼͼ��
    //vkFreeMemory(device, textureImageMemory, nullptr);// �����ͼͼ���¼

    //vkDestroyDescriptorPool(device, descriptorPool, nullptr); // �������������

    //vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    //vkDestroyBuffer(device, uniformBuffer, nullptr);
    //vkFreeMemory(device, uniformBufferMemory, nullptr);

    //vkDestroyBuffer(device, indexBuffer, nullptr); // �����������������
    //vkFreeMemory(device, indexBufferMemory, nullptr);// �������������������¼

    //vkDestroyBuffer(device, vertexBuffer, nullptr); // ������㻺����
    //vkFreeMemory(device, vertexBufferMemory, nullptr);// ������㻺������¼

    //vkDestroySemaphore(device, renderFinishedSemaphore, nullptr); // �����Ⱦ�ź�
    //vkDestroySemaphore(device, imageAvailableSemaphore, nullptr); // ���ͼ���ź�

    //vkDestroyCommandPool(device, commandPool, nullptr);// �����������

    //vkDestroyDevice(device, nullptr); // ����߼��豸��Դ
    //DestroyDebugReportCallbackEXT(instance, callback, nullptr);
    //vkDestroySurfaceKHR(instance, surface, nullptr); // GLFWû���ṩר�õĺ�������surface,���Լ򵥵�ͨ��Vulkanԭʼ��API
    //vkDestroyInstance(instance, nullptr); // ȷ��surface����������instance����֮ǰ���
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
    auto physical_devices = m_instance->enumeratePhysicalDevices();
    if (physical_devices.size() == 0)
    {
        throw std::runtime_error("Failed to find GPU with Vulkan support!");
    }

    std::cout << "Found available device(s): " << std::endl;
    for (auto i = 0; i < physical_devices.size(); ++i)
    {
        auto properties = physical_devices[i].getProperties();
        std::cout << i << ": " << properties.deviceName << std::endl;
        auto queue_family_properties = physical_devices[i].getQueueFamilyProperties();
    }
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
