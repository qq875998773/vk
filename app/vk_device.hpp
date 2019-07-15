#pragma once

#include <vulkan/vulkan.hpp>

class VKDevice
{
public:
    using UPtr = std::unique_ptr<VKDevice>;

    static UPtr create(vk::PhysicalDevice const& physical_device, vk::QueueFlags required_queues = vk::QueueFlagBits::eGraphics);

    void waitIdle() const;

protected:
    VKDevice() = default;
    VKDevice(vk::PhysicalDevice const& physical_device, vk::QueueFlags required_queues);

private:
    void init(vk::QueueFlags required_queues);

    void printDeviceInfo();

    // Vulkan handle of current device
    vk::UniqueDevice m_device;

    // The parent physical device
    vk::PhysicalDevice m_physical_device;

    // The queue family indices of selected queues
    int m_graphics_queue_family_index = -1;
    int m_compute_queue_family_index = -1;
    int m_transfer_queue_family_index = -1;

    // Queues of this device
    vk::Queue m_graphics_queue;
    vk::Queue m_compute_queue;
    vk::Queue m_transfer_queue;

    // Command pools created from the queues
    vk::UniqueCommandPool m_graphics_command_pool;
    vk::UniqueCommandPool m_compute_command_pool;
    vk::UniqueCommandPool m_transfer_command_pool;
};