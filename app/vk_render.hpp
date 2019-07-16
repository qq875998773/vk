#pragma once

#include "vk_device.hpp"

#include <functional>

#include <vulkan/vulkan.hpp>

class VKRender
{
public:
    void init(std::vector<const char*> const& extensions = {}, std::function<vk::UniqueSurfaceKHR(vk::Instance)> create_surface_func = nullptr);
    void destroy();

    void waitIdle();

    using UPtr = std::unique_ptr<VKRender>;
    static UPtr create(uint32_t width, uint32_t height);

protected:
    VKRender(uint32_t width, uint32_t height);

private:
    void initInstance(std::vector<const char*> const& extensions);
    void initDevice(bool swapchain_required);
    void createSwapchain();

    bool checkLayers(std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties);

    void setupDebugCallback();

    void printPhysicalDeviceInfo();

    // Vulkan instance
    vk::UniqueInstance m_instance;

    // logical device wrapper
    VKDevice::UPtr m_device;

    vk::UniqueSurfaceKHR m_surface;
    int m_present_queue_family_index = -1;
    vk::Queue m_present_queue;

    vk::UniqueSwapchainKHR m_swapchain;
    std::vector<vk::Image> m_swapchain_images;
    std::vector<vk::UniqueImageView> m_swapchain_image_views;

    vk::UniqueDebugReportCallbackEXT m_debug_callback;

    uint32_t m_width = 640;
    uint32_t m_height = 480;


    vk::SwapchainKHR m_swapChain;               // 交换链对象
    std::vector<vk::Image> m_swapChainImages;   // 交换链图像, 图像被交换链创建,也会在交换链销毁的同时自动清理,所以不需要添加清理代码
    vk::Format m_swapChainImageFormat;          // 交换链图像变换
    vk::Extent2D m_swapChainExtent;             // 交换链扩展
    std::vector<vk::ImageView> m_swapChainImageViews;       // 保存图像视图的句柄集
    std::vector<vk::Framebuffer> m_swapChainFramebuffers;   // 交换链帧缓冲区集

    vk::RenderPass m_renderPass;                      // 渲染通道
    vk::DescriptorSetLayout m_descriptorSetLayout;    // 描述符集布局
    vk::PipelineLayout m_pipelineLayout;              // 管线布局
    vk::Pipeline m_graphicsPipeline;                  // 绘制管线

    vk::Image m_depthImage;                 // 图像深度附件
    vk::DeviceMemory m_depthImageMemory;    // 图像深度附件记录
    vk::ImageView m_depthImageView;         // 图像深度视图

    vk::Image m_textureImage;               // 纹理图片
    vk::DeviceMemory m_textureImageMemory;  // 纹理图片记录
    vk::ImageView m_textureImageView;       // 纹理图像视图
    vk::Sampler m_textureSampler;           // 纹理采样器

    /*
    推荐在单个内存中分配多个资源,如缓冲区,但是实际上,应该更进一步细化.来自Nvidia的驱动程序开发者建议将多个缓冲区(顶点缓冲区、索引缓冲区)存储到单个VkBuffer中
    并在诸如vkCmdBindVertexBuffers之类的命令中使用偏移量
    优点在于,在这种情况下,数据会更加充分的利用缓存,因为它们排列在一块区域.甚至在同一个渲染操作中可以复用来自相同内存块的多个资源块,只要刷新数据即可
    该技巧称为称为aliasing,一些Vulkan函数有明确的标志指定这样做的意图
    */
    std::vector<uint32_t> m_indices;        // 顶点索引集合
    vk::Buffer m_vertexBuffer;              // 顶点缓冲区
    vk::DeviceMemory m_vertexBufferMemory;  // 顶点缓冲区记录
    vk::Buffer m_indexBuffer;               // 索引缓冲区
    vk::DeviceMemory m_indexBufferMemory;   // 索引缓冲区记录

    vk::Buffer m_uniformBuffer;             // 统一化缓冲区 Uniform:一个特殊类型的GLSL变量.它是全局的(在一个着色器程序中每一个着色器都能够访问uniform变量)并且只能被设定一次.
    vk::DeviceMemory m_uniformBufferMemory; // 统一化缓冲区记录

    vk::DescriptorPool m_descriptorPool;    // 描述符集
    vk::DescriptorSet m_descriptorSet;      // 描述符设置

    vk::Semaphore m_imageAvailableSemaphore;    // 渲染开始信号
    vk::Semaphore m_renderFinishedSemaphore;    // 渲染结束信号
};
