#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

// 顶点数据
struct Vertex
{
    glm::vec3 pos;      // 顶点xyz
    glm::vec3 color;    // 颜色rgb
    glm::vec2 texCoord; // 纹理坐标UV
    glm::vec3 normal;   // 顶点法线

    // 顶点输入绑定 描述了在整个顶点数据从内存加载的速率.换句话说,它指定数据条目之间的间隔字节数以及是否每个顶点之后或者每个instance之后移动到下一个条目
    static vk::VertexInputBindingDescription getBindingDescription()
    {
        vk::VertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;    // 移动到每个顶点后的下一个数据条目
        /*
        inputRate参数可以具备一下值之一：
            VK_VERTEX_INPUT_RATE_VERTEX:移动到每个顶点后的下一个数据条目
            VK_VERTEX_INPUT_RATE_INSTANCE:在每个instance之后移动到下一个数据条目
        */

        return bindingDescription;
    }

    // 处理顶点的输入
    static std::array<vk::VertexInputAttributeDescription, 4> getAttributeDescriptions()
    {
        std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;       // binding参数告诉了Vulkan每个顶点数据的来源
        attributeDescriptions[0].location = 0;      // location参数引用了vertex shader作为输入的location指令.顶点着色器中,location为0代表position,它是32bit单精度数据
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat; // format参数描述了属性的类型.该格式使用与颜色格式一样的枚举
        attributeDescriptions[0].offset = offsetof(Vertex, pos);        // offset参数指定了每个顶点数据读取的字节宽度偏移量,绑定一次加载一个Vertex,position属性(pos)的偏移量在字节数据中为0字节.这是使用offsetof macro宏自动计算的

        attributeDescriptions[1].binding = 0;       // binding参数告诉了Vulkan每个顶点颜色数据的来源
        attributeDescriptions[1].location = 1;      // location参数引用了vertex shader作为输入的location指令.顶点着色器中,location为1代表color,它是32bit单精度数据
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat; // format参数描述了属性的类型
        attributeDescriptions[1].offset = offsetof(Vertex, color);      // offset参数指定了每个颜色数据读取的字节宽度偏移量

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
