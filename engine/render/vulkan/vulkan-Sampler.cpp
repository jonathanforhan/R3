#include "render/Sampler.hpp"

#include <vulkan/vulkan.h>
#include "api/MovableHandle.hpp"
#include "render/Flags.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-Translation.hpp"

extern VkDevice g_device;
extern VkPhysicalDevice g_physicalDevice;

namespace R3 {

Sampler::Sampler(Filter magFilter,
                 Filter minFilter,
                 MipmapMode mipmapMode,
                 AddressMode addressModeUVW,
                 bool anisotrophyEnable,
                 bool compareEnable,
                 float minLod,
                 float maxLod)
    : m_magFilter{magFilter},
      m_minFilter{minFilter},
      m_mipmapMode{mipmapMode},
      m_addressModeU{addressModeUVW},
      m_addressModeV{addressModeUVW},
      m_addressModeW{addressModeUVW},
      m_anisotrophyEnabled{anisotrophyEnable},
      m_compareEnabled{compareEnable},
      m_minLod{minLod},
      m_maxLod{maxLod} {
    create();
}

Sampler::Sampler(Filter magFilter,
                 Filter minFilter,
                 MipmapMode mipmapMode,
                 AddressMode addressModeU,
                 AddressMode addressModeV,
                 AddressMode addressModeW,
                 bool anisotrophyEnable,
                 bool compareEnable,
                 float minLod,
                 float maxLod)

    : m_magFilter{magFilter},
      m_minFilter{minFilter},
      m_mipmapMode{mipmapMode},
      m_addressModeU{addressModeU},
      m_addressModeV{addressModeV},
      m_addressModeW{addressModeW},
      m_anisotrophyEnabled{anisotrophyEnable},
      m_compareEnabled{compareEnable},
      m_minLod{minLod},
      m_maxLod{maxLod} {
    create();
}

Sampler::~Sampler() noexcept {
    vkDestroySampler(g_device, m_sampler, nullptr);
}

void Sampler::create() {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(g_physicalDevice, &properties);

    const VkSamplerCreateInfo samplerInfo = {
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter               = TO_VK_FILTER(m_magFilter),
        .minFilter               = TO_VK_FILTER(m_minFilter),
        .mipmapMode              = TO_VK_SAMPLER_MIPMAP_MODE(m_mipmapMode),
        .addressModeU            = TO_VK_SAMPLER_ADDRESS_MODE(m_addressModeU),
        .addressModeV            = TO_VK_SAMPLER_ADDRESS_MODE(m_addressModeV),
        .addressModeW            = TO_VK_SAMPLER_ADDRESS_MODE(m_addressModeW),
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = m_anisotrophyEnabled ? VK_TRUE : VK_FALSE,
        .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = m_minLod,
        .maxLod                  = m_maxLod,
        .borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };
    VK_CHECK(vkCreateSampler(g_device, &samplerInfo, nullptr, &*m_sampler));
}

} // namespace R3
