#include "application.hpp"

const int WIDTH = 1280;
const int HEIGHT = 720;

Application::Application(int argc, char* argv[])
    : m_window(nullptr, glfwDestroyWindow)  // Provide a destroy function for unique_ptr
{
}

void Application::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void Application::initVulkan()
{
    m_vk = VKRender::create();

    auto glfw_exts = getGLFWExtensions();
    m_vk->init(glfw_exts);
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // 创建窗体 设置尺寸和标题
    m_window.reset(glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Demo", nullptr, nullptr));

    glfwSetWindowUserPointer(m_window.get(), this);
    glfwSetWindowSizeCallback(m_window.get(), Application::onWindowResized);
}

void Application::initSwapChain()
{
    //glfwCreateWindowSurface()
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(m_window.get()))
    {
        glfwPollEvents();
        //updateUniformBuffer();
        //drawFrame();
    }

    m_vk->waitIdle();
}

void Application::cleanup()
{
    m_vk->destroy();

    m_window = nullptr;

    glfwTerminate();
}

std::vector<const char*> Application::getGLFWExtensions()
{
    std::vector<const char*> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; i++)
    {
        extensions.push_back(glfwExtensions[i]);
    }

    return extensions;
}

// 窗口尺寸变化
void Application::onWindowResized(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0) return;

    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    //app->recreateSwapChain(); // 重新创建交换链
}

#if 0
// 更新窗体显示 这个函数会在每一帧中创建新的变换矩阵以确保几何图形旋转,移动,放缩等变换
// uniform缓冲区是全局缓冲区 任何阶段的shader都可以访问
void Application::updateUniformBuffer()
{
    // 初始化摄像机
    static glm::vec3 position = glm::vec3(2.0f, 2.f, 2.f);  // 初始摄像机位置
    static glm::vec3 centre = glm::vec3(0.0f, 0.0f, 0.0f);  // 模型中心
    float FoV = 45.0f;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);// 仰角

    // 鼠标键盘操作
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetCursorPosCallback(window, OnMouseMove);
    glfwSetKeyCallback(window, OnKey);
    glfwSetScrollCallback(window, OnMouseScroll);

    double speed = -0.001f;
    float  i_xpos = g_mouse_delta.x;
    float  i_ypos = g_mouse_delta.y;
    position = glm::vec3(position.x * std::cos(speed * i_xpos) - position.y * std::sin(speed * i_xpos), position.x * std::sin(speed * i_xpos) + position.y * std::cos(speed * i_xpos), position.z);
    position = glm::vec3(position.x, position.y * std::cos(-speed * i_ypos) - position.z * std::sin(-speed * i_ypos), position.y * std::sin(-speed * i_ypos) + position.z * std::cos(-speed * i_ypos));

    UniformBufferObject ubo = {};
    ubo.view = glm::lookAt(position, centre, up); // 摄像机位置/中心位置/上下仰角
    // 选择使用FOV为45度的透视投影.其他参数是宽高比,近裁剪面和远裁剪面.重要的是使用当前的交换链扩展来计算宽高比,以便在窗体调整大小后参考最新的窗体宽度和高度.
    ubo.proj = glm::perspective(glm::radians(FoV), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1; // GLM最初是为OpenGL设计的,它的裁剪坐标的Y是反转的.修正该问题的最简单的方法是在投影矩阵中Y轴的缩放因子反转.如果不这样做图像会被倒置.

    // 加一个测试用的点光
    ubo.testlight = glm::vec3(10.f, 10.f, 10.f);

    // 现在定义了所有的变换,所以将UBO中的数据复制到uniform缓冲区.除了没有暂存缓冲区,这与顶点缓冲区的操作完全相同.
    // 使用ubo将并不是经常变化的值传递给着色器是非常有效的方式.相比传递一个更小的数据缓冲区到着色器中,更有效的方式是使用常量.
    void* data;
    m_device.mapMemory(uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    m_device.unmapMemory(uniformBufferMemory);
}

// 绘制帧 从交换链获取图像,在帧缓冲区中使用作为附件的图像来执行命令缓冲区中的命令,将图像返还给交换链最终呈现
void drawFrame()
{
    // vkAcquireNextImageKHR前两个参数是我们希望获取到图像的逻辑设备和交换链.第三个参数指定获取有效图像的操作timeout,单位纳秒.我们使用64位无符号最大值禁止timeout
    // 后两个参数指定使用的同步对象,当presentation引擎完成了图像的呈现后会使用该对象发起信号,这就是开始绘制的时间点,它可以指定一个信号量semaphore或者栅栏或者两者,这里使用imageAvailableSemaphore
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    // 如果错误报告
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // 重新创建交换链
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // 队列提交和同步
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // 等待缓冲区信号集
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // 数组对应pWaitSemaphores中具有相同索引的信号
    submitInfo.waitSemaphoreCount = 1; // 等待信号的数量
    submitInfo.pWaitSemaphores = waitSemaphores; // 等待哪些具体信号
    submitInfo.pWaitDstStageMask = waitStages; // 指定pWaitSemaphores中具有相同索引的信号

    submitInfo.commandBufferCount = 1; // 指定被执行的命令缓冲区信号数量
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex]; // 指定哪个命令缓冲区被实际提交执行

    // 渲染结束信号集
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1; // 指定被指定的命令缓冲区信号数量
    submitInfo.pSignalSemaphores = signalSemaphores;// 指定具体被提交指定的命令缓冲区

    // 向图像队列提交命令缓冲区
    // 当开销负载比较大的时候,处于效率考虑,函数可以持有VkSubmitInfo结构体数组
    // 最后一个参数引用了一个可选的栅栏,当命令缓冲区执行完毕时候它会被发送信号
    // 我们使用信号量进行同步,所以我们需要传递VK_NULL_HANDLE
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // 将结果提交到交换链,使其最终显示在屏幕上
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // 指定在进行presentation之前要等待的信号量
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    // 交换链集合
    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    // 提交请求呈现交换链中的图像
    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    vkQueueWaitIdle(presentQueue);
}
#endif
