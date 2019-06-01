#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

// ��������
struct Vertex
{
    glm::vec3 pos;      // ����xyz
    glm::vec3 color;    // ��ɫrgb
    glm::vec2 texCoord; // ��������UV
    glm::vec3 normal;   // ���㷨��

    // ��������� �������������������ݴ��ڴ���ص�����.���仰˵,��ָ��������Ŀ֮��ļ���ֽ����Լ��Ƿ�ÿ������֮�����ÿ��instance֮���ƶ�����һ����Ŀ
    static vk::VertexInputBindingDescription getBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;    // �ƶ���ÿ����������һ��������Ŀ
        /*
        inputRate�������Ծ߱�һ��ֵ֮һ��
            VK_VERTEX_INPUT_RATE_VERTEX:�ƶ���ÿ����������һ��������Ŀ
            VK_VERTEX_INPUT_RATE_INSTANCE:��ÿ��instance֮���ƶ�����һ��������Ŀ
        */

        return bindingDescription;
    }

    // �����������
    static std::array<vk::VertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;       // binding����������Vulkanÿ���������ݵ���Դ
        attributeDescriptions[0].location = 0;      // location����������vertex shader��Ϊ�����locationָ��.������ɫ����,locationΪ0����position,����32bit����������
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat; // format�������������Ե�����.�ø�ʽʹ������ɫ��ʽһ����ö��
        attributeDescriptions[0].offset = offsetof(Vertex, pos);        // offset����ָ����ÿ���������ݶ�ȡ���ֽڿ��ƫ����,��һ�μ���һ��Vertex,position����(pos)��ƫ�������ֽ�������Ϊ0�ֽ�.����ʹ��offsetof macro���Զ������

        attributeDescriptions[1].binding = 0;       // binding����������Vulkanÿ��������ɫ���ݵ���Դ
        attributeDescriptions[1].location = 1;      // location����������vertex shader��Ϊ�����locationָ��.������ɫ����,locationΪ1����color,����32bit����������
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat; // format�������������Ե�����
        attributeDescriptions[1].offset = offsetof(Vertex, color);      // offset����ָ����ÿ����ɫ���ݶ�ȡ���ֽڿ��ƫ����

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[3].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
    }
};
