#pragma once

#include "vk_render.hpp"

#include "vertex.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>

class Application
{
public:
    Application(int argc, char* argv[]);
    void run();

    std::vector<const char*> getGLFWExtensions();

    void initVulkan();
    void initWindow();
    void initSwapChain();

    void mainLoop();

    void cleanup();

private:
    void createSwapChain();
    void cleanupSwapChain();
    void recreateSwapChain();

    static void onWindowResized(GLFWwindow* window, int width, int height);

    void updateUniformBuffer();

    void drawFrame();

    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_window;
    std::unique_ptr<VKRender> m_vk;

    std::vector<Vertex> m_vertices;
};
