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


void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // �������� ���óߴ�ͱ���
    m_window.reset(glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Demo", nullptr, nullptr));

    glfwSetWindowUserPointer(m_window.get(), this);
    glfwSetWindowSizeCallback(m_window.get(), Application::onWindowResized);
}

void Application::initVulkan()
{
    m_vk = std::make_unique<VKRender>();

    auto glfw_exts = getGLFWExtensions();
    m_vk->init(glfw_exts);
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

// ���ڳߴ�仯
void Application::onWindowResized(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0) return;

    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    //app->recreateSwapChain(); // ���´���������
}

#if 0
// ���´�����ʾ �����������ÿһ֡�д����µı任������ȷ������ͼ����ת,�ƶ�,�����ȱ任
// uniform��������ȫ�ֻ����� �κν׶ε�shader�����Է���
void Application::updateUniformBuffer()
{
    // ��ʼ�������
    static glm::vec3 position = glm::vec3(2.0f, 2.f, 2.f);  // ��ʼ�����λ��
    static glm::vec3 centre = glm::vec3(0.0f, 0.0f, 0.0f);  // ģ������
    float FoV = 45.0f;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);// ����

    // �����̲���
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
    ubo.view = glm::lookAt(position, centre, up); // �����λ��/����λ��/��������
    // ѡ��ʹ��FOVΪ45�ȵ�͸��ͶӰ.���������ǿ�߱�,���ü����Զ�ü���.��Ҫ����ʹ�õ�ǰ�Ľ�������չ�������߱�,�Ա��ڴ��������С��ο����µĴ����Ⱥ͸߶�.
    ubo.proj = glm::perspective(glm::radians(FoV), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1; // GLM�����ΪOpenGL��Ƶ�,���Ĳü������Y�Ƿ�ת��.�������������򵥵ķ�������ͶӰ������Y����������ӷ�ת.�����������ͼ��ᱻ����.

    // ��һ�������õĵ��
    ubo.testlight = glm::vec3(10.f, 10.f, 10.f);

    // ���ڶ��������еı任,���Խ�UBO�е����ݸ��Ƶ�uniform������.����û���ݴ滺����,���붥�㻺�����Ĳ�����ȫ��ͬ.
    // ʹ��ubo�������Ǿ����仯��ֵ���ݸ���ɫ���Ƿǳ���Ч�ķ�ʽ.��ȴ���һ����С�����ݻ���������ɫ����,����Ч�ķ�ʽ��ʹ�ó���.
    void* data;
    m_device.mapMemory(uniformBufferMemory, 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    m_device.unmapMemory(uniformBufferMemory);
}

// ����֡ �ӽ�������ȡͼ��,��֡��������ʹ����Ϊ������ͼ����ִ����������е�����,��ͼ�񷵻������������ճ���
void drawFrame()
{
    // vkAcquireNextImageKHRǰ��������������ϣ����ȡ��ͼ����߼��豸�ͽ�����.����������ָ����ȡ��Чͼ��Ĳ���timeout,��λ����.����ʹ��64λ�޷������ֵ��ֹtimeout
    // ����������ָ��ʹ�õ�ͬ������,��presentation���������ͼ��ĳ��ֺ��ʹ�øö������ź�,����ǿ�ʼ���Ƶ�ʱ���,������ָ��һ���ź���semaphore����դ����������,����ʹ��imageAvailableSemaphore
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    // ������󱨸�
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // ���´���������
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // �����ύ��ͬ��
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // �ȴ��������źż�
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // �����ӦpWaitSemaphores�о�����ͬ�������ź�
    submitInfo.waitSemaphoreCount = 1; // �ȴ��źŵ�����
    submitInfo.pWaitSemaphores = waitSemaphores; // �ȴ���Щ�����ź�
    submitInfo.pWaitDstStageMask = waitStages; // ָ��pWaitSemaphores�о�����ͬ�������ź�

    submitInfo.commandBufferCount = 1; // ָ����ִ�е���������ź�����
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex]; // ָ���ĸ����������ʵ���ύִ��

    // ��Ⱦ�����źż�
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1; // ָ����ָ������������ź�����
    submitInfo.pSignalSemaphores = signalSemaphores;// ָ�����屻�ύָ�����������

    // ��ͼ������ύ�������
    // ���������رȽϴ��ʱ��,����Ч�ʿ���,�������Գ���VkSubmitInfo�ṹ������
    // ���һ������������һ����ѡ��դ��,���������ִ�����ʱ�����ᱻ�����ź�
    // ����ʹ���ź�������ͬ��,����������Ҫ����VK_NULL_HANDLE
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // ������ύ��������,ʹ��������ʾ����Ļ��
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // ָ���ڽ���presentation֮ǰҪ�ȴ����ź���
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    // ����������
    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    // �ύ������ֽ������е�ͼ��
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
