#include "vk_device.hpp"

#include <vector>

VKDevice::VKDevice(vk::PhysicalDevice const& physical_device, vk::QueueFlags required_queues)
    : m_physical_device(physical_device)
{
    init(required_queues);
}

void VKDevice::init(vk::QueueFlags required_queues)
{
    assert(m_physical_device);

    auto queue_family_properties = m_physical_device.getQueueFamilyProperties();
    for (auto i = 0; i < queue_family_properties.size(); ++i)
    {
        auto flags = queue_family_properties[i].queueFlags;

        if (flags & vk::QueueFlagBits::eGraphics)
        {
            m_graphics_queue_family_index = i;
        }
        if (flags & vk::QueueFlagBits::eCompute)
        {
            // Prefer dedicated compute queue family
            if (m_compute_queue_family_index < 0 || !(flags & vk::QueueFlagBits::eGraphics))
            {
                m_compute_queue_family_index = i;
            }
        }
        if (flags & vk::QueueFlagBits::eTransfer)
        {
            // Prefer dedicated transfer queue family
            if (m_transfer_queue_family_index < 0 || !(flags & vk::QueueFlagBits::eGraphics))
            {
                m_transfer_queue_family_index = i;
            }
        }
    }

    bool require_graphics = bool(required_queues & vk::QueueFlagBits::eGraphics);
    bool require_compute = bool(required_queues & vk::QueueFlagBits::eCompute);
    bool require_transfer = bool(required_queues & vk::QueueFlagBits::eTransfer);

    // Construct array of DeviceQueueCreateInfo
    std::vector<vk::DeviceQueueCreateInfo> device_queue_create_infos;
    if (require_graphics)
    {
        float queuePriority = 0.f;
        device_queue_create_infos.emplace_back(vk::DeviceQueueCreateFlags(), m_graphics_queue_family_index, 1, &queuePriority);
    }
    if (require_compute)
    {
        float queuePriority = 0.f;
        device_queue_create_infos.emplace_back(vk::DeviceQueueCreateFlags(), m_compute_queue_family_index, 1, &queuePriority);
    }
    if (require_transfer)
    {
        float queuePriority = 0.f;
        device_queue_create_infos.emplace_back(vk::DeviceQueueCreateFlags(), m_transfer_queue_family_index, 1, &queuePriority);
    }

    // Create the logical device with required queues
    m_device = m_physical_device.createDeviceUnique(vk::DeviceCreateInfo(vk::DeviceCreateFlags(),
        device_queue_create_infos.size(), device_queue_create_infos.data()));

    // Get all the required queues and create command pools
    if (require_graphics)
    {
        m_graphics_queue = m_device->getQueue(m_graphics_queue_family_index, 0);
        m_graphics_command_pool = m_device->createCommandPoolUnique(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), m_graphics_queue_family_index));
    }
    if (require_compute)
    {
        m_compute_queue = m_device->getQueue(m_compute_queue_family_index, 0);
        m_compute_command_pool = m_device->createCommandPoolUnique(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), m_compute_queue_family_index));
    }
    if (require_transfer)
    {
        m_transfer_queue = m_device->getQueue(m_transfer_queue_family_index, 0);
        m_transfer_command_pool = m_device->createCommandPoolUnique(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), m_transfer_queue_family_index));
    }
}

void VKDevice::waitIdle() const
{
    m_device->waitIdle();
}

namespace
{
    struct VKDeviceConcrete : public VKDevice
    {
        VKDeviceConcrete(vk::PhysicalDevice const& physical_device, vk::QueueFlags required_queue)
            : VKDevice(physical_device, required_queue) {}
    };
}

VKDevice::UPtr VKDevice::create(vk::PhysicalDevice const& physical_device, vk::QueueFlags required_queue)
{
    return std::make_unique<VKDeviceConcrete>(physical_device, required_queue);
}
