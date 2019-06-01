#pragma once

#include <vulkan/vulkan.hpp>

// ��������ĳ�����ԵĶ��д�����.����ṹ��,��������-1��ʾ"δ�ҵ�"
// ֧��graphics����ĵĶ��дغ�֧��presentation����Ķ��дؿ��ܲ���ͬһ����
struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete()
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

// ���������Ϊ�˲��Խ���������Ч����ԶԶ������,��Ϊ�������ܺܺõ��봰��surface����
// ����������ͬ��Ҳ��Ҫ�ܶ�����,����������Ҫ�˽�һЩ�й����õ�ϸ��
// �������ṹ����ϸ��Ϣ
struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class VKRender
{
public:
    void init(std::vector<const char*> const& extensions = {});
    void destroy();

    void waitIdle();

private:
    void initInstance(std::vector<const char*> const& extensions);
    void initDevice();

    bool checkLayers(std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties);

    void setupDebugCallback();

    vk::UniqueInstance m_instance;

    vk::PhysicalDevice m_physical_device;
    vk::UniqueDevice m_device;

    vk::UniqueDebugReportCallbackEXT m_debug_callback;

    vk::SurfaceKHR m_surface;       // surface����Vulkan�봰��ϵͳ����������


    vk::Queue m_graphicsQueue;      // ��ͼ���о��
    vk::Queue m_presentQueue;       // ��ʾ���о��

    vk::SwapchainKHR m_swapChain;               // ����������
    std::vector<vk::Image> m_swapChainImages;   // ������ͼ��, ͼ�񱻽���������,Ҳ���ڽ��������ٵ�ͬʱ�Զ�����,���Բ���Ҫ����������
    vk::Format m_swapChainImageFormat;          // ������ͼ��任
    vk::Extent2D m_swapChainExtent;             // ��������չ
    std::vector<vk::ImageView> m_swapChainImageViews;       // ����ͼ����ͼ�ľ����
    std::vector<vk::Framebuffer> m_swapChainFramebuffers;   // ������֡��������

    vk::RenderPass m_renderPass;                      // ��Ⱦͨ��
    vk::DescriptorSetLayout m_descriptorSetLayout;    // ������������
    vk::PipelineLayout m_pipelineLayout;              // ���߲���
    vk::Pipeline m_graphicsPipeline;                  // ���ƹ���

    vk::CommandPool m_commandPool;          // �����

    vk::Image m_depthImage;                 // ͼ����ȸ���
    vk::DeviceMemory m_depthImageMemory;    // ͼ����ȸ�����¼
    vk::ImageView m_depthImageView;         // ͼ�������ͼ

    vk::Image m_textureImage;               // ����ͼƬ
    vk::DeviceMemory m_textureImageMemory;  // ����ͼƬ��¼
    vk::ImageView m_textureImageView;       // ����ͼ����ͼ
    vk::Sampler m_textureSampler;           // ���������

    /*
    �Ƽ��ڵ����ڴ��з�������Դ,�绺����,����ʵ����,Ӧ�ø���һ��ϸ��.����Nvidia���������򿪷��߽��齫���������(���㻺����������������)�洢������VkBuffer��
    ��������vkCmdBindVertexBuffers֮���������ʹ��ƫ����
    �ŵ�����,�����������,���ݻ���ӳ�ֵ����û���,��Ϊ����������һ������.������ͬһ����Ⱦ�����п��Ը���������ͬ�ڴ��Ķ����Դ��,ֻҪˢ�����ݼ���
    �ü��ɳ�Ϊ��Ϊaliasing,һЩVulkan��������ȷ�ı�־ָ������������ͼ
    */
    std::vector<uint32_t> m_indices;        // ������������
    vk::Buffer m_vertexBuffer;              // ���㻺����
    vk::DeviceMemory m_vertexBufferMemory;  // ���㻺������¼
    vk::Buffer m_indexBuffer;               // ����������
    vk::DeviceMemory m_indexBufferMemory;   // ������������¼

    vk::Buffer m_uniformBuffer;             // ͳһ�������� Uniform:һ���������͵�GLSL����.����ȫ�ֵ�(��һ����ɫ��������ÿһ����ɫ�����ܹ�����uniform����)����ֻ�ܱ��趨һ��.
    vk::DeviceMemory m_uniformBufferMemory; // ͳһ����������¼

    vk::DescriptorPool m_descriptorPool;    // ��������
    vk::DescriptorSet m_descriptorSet;      // ����������

    std::vector<vk::CommandBuffer> m_commandBuffers;    // ���������

    vk::Semaphore m_imageAvailableSemaphore;    // ��Ⱦ��ʼ�ź�
    vk::Semaphore m_renderFinishedSemaphore;    // ��Ⱦ�����ź�
};
