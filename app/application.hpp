#pragma once

#include "vk_render.hpp"

#include "vertex.hpp"

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <vector>

// ������
class Application
{
public:
    Application(int argc, char* argv[]);
    void run();

    std::vector<const char*> getGLFWExtensions();

    void initWindow();  // ��ʼ������
    void initVulkan();  // ��ʼ��vulkan|��Ⱦ����

    void mainLoop();    // ��ѭ����ͼ�λ��Ƶ���Ļ

    void cleanup();     // �����Դ

private:
    void createSwapChain();
    void cleanupSwapChain();
    void recreateSwapChain();

    // ���ڳߴ�仯
    static void onWindowResized(GLFWwindow* window, int width, int height);

    // ���´�����ʾ �����������ÿһ֡�д����µı任������ȷ������ͼ����ת,�ƶ�,�����ȱ任
    // uniform��������ȫ�ֻ����� �κν׶ε�shader�����Է���
    void updateUniformBuffer();

    // ����֡ �ӽ�������ȡͼ��,��֡��������ʹ����Ϊ������ͼ����ִ����������е�����,��ͼ�񷵻������������ճ���
    void drawFrame();

    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_window;
    std::unique_ptr<VKRender> m_vk;

    std::vector<Vertex> m_vertices;         // ���㼯��
};
