#pragma once

#include "Flags.hpp"
#include "RenderHandle.hpp"
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/MovableHandle.hpp"

namespace R3 {

/// @brief Sampler configuration for 3D rendering, specifying filtering, addressing, and other sampling options.
class R3_API Sampler {
public:
    R3_CTOR_DEFAULT(Sampler);
    R3_COPY_DELETE(Sampler);
    R3_MOVE_DEFAULT(Sampler);

    /// @brief Constructs a Sampler with the specified filtering, mipmapping, addressing, and other sampling options.
    /// @param magFilter The filter to use when magnifying (upscaling) textures.
    /// @param minFilter The filter to use when minifying (downscaling) textures.
    /// @param mipmapMode The mode to use for mipmap selection.
    /// @param addressModeUVW The addressing mode for U, V, and W texture coordinates.
    /// @param anisotrophyEnable Enables or disables anisotropic filtering.
    /// @param compareEnable Enables or disables comparison sampling.
    /// @param minLod The minimum level of detail (LOD) that can be accessed.
    /// @param maxLod The maximum level of detail (LOD) that can be accessed.
    Sampler(Filter magFilter,
            Filter minFilter,
            MipmapMode mipmapMode,
            AddressMode addressModeUVW = AddressMode::Repeat,
            bool anisotrophyEnable     = true,
            bool compareEnable         = false,
            float minLod               = 0.0f,
            float maxLod               = 1000.0f);

    /// @brief Constructs a Sampler with the specified filtering, mipmapping, addressing, and other sampling options.
    /// @param magFilter The filter to use when magnifying (upscaling) textures.
    /// @param minFilter The filter to use when minifying (downscaling) textures.
    /// @param mipmapMode The mode to use for mipmap selection.
    /// @param addressModeU The addressing mode for U texture coordinates.
    /// @param addressModeV The addressing mode for V texture coordinates.
    /// @param addressModeW The addressing mode for W texture coordinates.
    /// @param anisotrophyEnable Enables or disables anisotropic filtering.
    /// @param compareEnable Enables or disables comparison sampling.
    /// @param minLod The minimum level of detail (LOD) that can be accessed.
    /// @param maxLod The maximum level of detail (LOD) that can be accessed.
    Sampler(Filter magFilter,
            Filter minFilter,
            MipmapMode mipmapMode,
            AddressMode addressModeU,
            AddressMode addressModeV,
            AddressMode addressModeW,
            bool anisotrophyEnable = true,
            bool compareEnable     = false,
            float minLod           = 0.0f,
            float maxLod           = 1000.0f);

    /// @brief Destroys the Sampler.
    ~Sampler() noexcept;

    /// @return Magnification filter.
    Filter magFilter() const noexcept { return m_magFilter; }
    /// @return Minification filter.
    Filter minFilter() const noexcept { return m_minFilter; }
    /// @return Mipmap mode.
    MipmapMode mipmapMode() const noexcept { return m_mipmapMode; }
    /// @return Addressing mode for U texture coordinate.
    AddressMode addressModeU() const noexcept { return m_addressModeU; }
    /// @return Addressing mode for V texture coordinate.
    AddressMode addressModeV() const noexcept { return m_addressModeV; }
    /// @return Addressing mode for W texture coordinate.
    AddressMode addressModeW() const noexcept { return m_addressModeW; }
    /// @return True if anisotropic filtering is enabled, false otherwise.
    bool isAnisotrophyEnabled() const noexcept { return m_anisotrophyEnabled; }
    /// @return True if comparison sampling is enabled, false otherwise.
    bool isCompareEnabled() const noexcept { return m_compareEnabled; }
    /// @return Minimum level of detail (LOD).
    float minLod() const noexcept { return m_minLod; }
    /// @return Maximum level of detail (LOD).
    float maxLod() const noexcept { return m_maxLod; }
    /// @return A reference to the underlying sampler handle.
    SamplerRenderHandle& samplerHandle() noexcept { return m_sampler; }
    /// @return A const reference to the underlying sampler handle.
    const SamplerRenderHandle& samplerHandle() const noexcept { return m_sampler; }

private:
    void create();

private:
    MovableHandle<SamplerRenderHandle> m_sampler;
    Filter m_magFilter         = Filter::Linear;
    Filter m_minFilter         = Filter::Linear;
    MipmapMode m_mipmapMode    = MipmapMode::Linear;
    AddressMode m_addressModeU = AddressMode::Repeat;
    AddressMode m_addressModeV = AddressMode::Repeat;
    AddressMode m_addressModeW = AddressMode::Repeat;
    bool m_anisotrophyEnabled  = true;
    bool m_compareEnabled      = false;
    float m_minLod             = 0.0f;
    float m_maxLod             = 1000.0f;
};

} // namespace R3
