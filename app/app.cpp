// vk.cpp : Defines the entry point for the application.
//

#include "app.h"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

using namespace std;

//GLFWwindow* window = nullptr;
//
//void InitWindow()
//{
//    glfwInit();
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//
//    window = glfwCreateWindow(800, 600, "VK", nullptr, nullptr);
//
//    glfwSetWindowUserPointer(window, this);
//    glfwSetWindowSizeCallback(window, Application::onWindowResized);
//}

void InitVulkan()
{
    vk::ApplicationInfo appInfo("vk", VK_MAKE_VERSION(0, 0, 1), "vk1", VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_1);

    uint32_t glfwExtCount = 0;
    const char** glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    vk::InstanceCreateInfo instanceCreateInfo({}, &appInfo, 0, nullptr, glfwExtCount, glfwExts);
    vk::UniqueInstance instance = vk::createInstanceUnique(instanceCreateInfo);

    std::vector<vk::PhysicalDevice> devices = instance->enumeratePhysicalDevices();
    for (auto& device : devices)
    {
        auto& qfp = device.getQueueFamilyProperties();
    }
}

int main()
{
    //InitWindow();
    InitVulkan();

    return 0;
}
