#pragma once

#include <rapidjson/document.h>
#include "glTF.hxx"

namespace R3::glTF {

static constexpr const char* EXTENSION_KHR_materials_pbrSpecularGlossiness = "KHR_materials_pbrSpecularGlossiness";

struct KHR_materials_pbrSpecularGlossiness : public Extension {
    KHR_materials_pbrSpecularGlossiness()
        : Extension(EXTENSION_KHR_materials_pbrSpecularGlossiness) {}

    static void parse(Extension* extension, rapidjson::Value& value);

    float diffuseFactor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    std::optional<TextureInfo> diffuseTexture;
    float specularFactor[3] = {1.0f, 1.0f, 1.0f};
    float glossinessFactor = 1.0f;
    std::optional<TextureInfo> specularGlossinessTexture;
};

} // namespace R3::glTF