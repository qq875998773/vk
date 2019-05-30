#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec3 fragLight;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightColor = vec3(1.0,1.0,1.0);// 为点光添加颜色
    // 计算法线和光
    vec3 lightDir = normalize(fragLight - fragPos);
    float diff = max(dot(fragNormal, lightDir), 0.0); // 取最大值函数，当法向和光的夹角大于90度时代表这个表面背光，取大值0.0，也就是不加光
    vec3 diffuse = diff * lightColor;
    
    vec3 scnen_lightColor = vec3(1.0,1.0,1.0); // 这里用来模拟环境光，这个值应该cpp buffer传进来
    float ambientStrength = 0.1; // 环境光系数
    vec3 ambient = ambientStrength * scnen_lightColor;
    
    vec4 light = vec4(ambient+diffuse,1.0);
    
    vec4 bleColor = vec4(fragColor,1.0); // 输入颜色
    outColor = texture(texSampler, fragTexCoord)*bleColor*light; // 纹理颜色x输入颜色x光照
}
