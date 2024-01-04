#pragma once

#include <rapidjson/document.h>
#include "glTF.hxx"

namespace R3::glTF {

template <typename T>
static constexpr void maybeAssign(T& dst, const rapidjson::Value& value, const char* key) {
    if (!value.HasMember(key))
        return;

    if constexpr (std::is_same_v<T, bool>) {
        dst = value[key].GetBool();
    } else if constexpr (std::is_same_v<T, uint32>) {
        dst = value[key].GetUint();
    } else if constexpr (std::is_same_v<T, int32>) {
        dst = value[key].GetInt();
    } else if constexpr (std::is_same_v<T, uint64>) {
        dst = value[key].GetUint64();
    } else if constexpr (std::is_same_v<T, int64>) {
        dst = value[key].GetInt64();
    } else if constexpr (std::is_same_v<T, float>) {
        dst = value[key].GetFloat();
    } else if constexpr (std::is_same_v<T, double>) {
        dst = value[key].GetDouble();
    } else if constexpr (std::is_same_v<T, std::string>) {
        dst = value[key].GetString();
    }
}

// TextureInfo helper
static void populateTextureInfo(TextureInfo& textureInfo, rapidjson::Value& value) {
    // index
    textureInfo.index = value["index"].GetUint();

    // texCoord
    maybeAssign(textureInfo.texCoord, value, "texCoord");

    // extensions
    if (value.HasMember("extensions")) {
        textureInfo.extensions = std::move(value["extensions"]);
    }

    // extras
#if R3_GLTF_JSON_EXTRAS
    if (value.HasMember("extras")) {
        textureInfo.extras = std::move(value["extras"]);
    }
#endif
}

} // namespace R3::glTF
