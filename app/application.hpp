#pragma once

#include "vk_render.hpp"

#include "vertex.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>

// 管线类
class Application
{
public:
    Application(int argc, char* argv[]);
    void run();

    std::vector<const char*> getGLFWExtensions();

    void initVulkan();
    void initWindow();
    void initSwapChain();

    void mainLoop();    // 主循环将图形绘制到屏幕

    void cleanup();     // 清除资源

private:
    void createSwapChain();
    void cleanupSwapChain();
    void recreateSwapChain();

    // 窗口尺寸变化
    static void onWindowResized(GLFWwindow* window, int width, int height);

    // 更新窗体显示 这个函数会在每一帧中创建新的变换矩阵以确保几何图形旋转,移动,放缩等变换
    // uniform缓冲区是全局缓冲区 任何阶段的shader都可以访问
    void updateUniformBuffer();

    // 绘制帧 从交换链获取图像,在帧缓冲区中使用作为附件的图像来执行命令缓冲区中的命令,将图像返还给交换链最终呈现
    void drawFrame();

    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_window;
    std::unique_ptr<VKRender> m_vk;

    std::vector<Vertex> m_vertices;         // 顶点集合
};
