#pragma once

#include <vulkan/vulkan.hpp>

// 返回满足某个属性的队列簇索引.定义结构体,其中索引-1表示"未找到"
// 支持graphics命令的的队列簇和支持presentation命令的队列簇可能不是同一个簇
struct QueueFamilyIndices
{
    int graphicsFamily = -1;
    int presentFamily = -1;

    bool isComplete()
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

// 如果仅仅是为了测试交换链的有效性是远远不够的,因为它还不能很好的与窗体surface兼容
// 创建交换链同样也需要很多设置,所以我们需要了解一些有关设置的细节
// 交换链结构体详细信息
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

    static std::unique_ptr<VKRender> create();

protected:
    VKRender();

private:
    void initInstance(std::vector<const char*> const& extensions);
    void initDevice();
    void initCommandPool();
    void initCommandBuffer();

    bool checkLayers(std::vector<char const*> const& layers, std::vector<vk::LayerProperties> const& properties);

    void setupDebugCallback();

    // Vulkan instance
    vk::UniqueInstance m_instance;

    // Selected physical device
    vk::PhysicalDevice m_physical_device;
    // Selected queue family index
    uint32_t m_queue_family_index;
    // Logical device created from one physical device
    vk::UniqueDevice m_device;

    QueueFamilyIndices m_queue_family_indices;

    vk::UniqueCommandPool m_command_pool;

    std::vector<vk::CommandBuffer> m_commandBuffers;

    vk::UniqueDebugReportCallbackEXT m_debug_callback;

    vk::SurfaceKHR m_surface;       // surface就是Vulkan与窗体系统的连接桥梁


    vk::Queue m_graphicsQueue;      // 绘图队列句柄
    vk::Queue m_presentQueue;       // 演示队列句柄

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
