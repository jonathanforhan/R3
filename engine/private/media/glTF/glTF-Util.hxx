#pragma once

#include <rapidjson/document.h>
#include "glTF-Model.hxx"
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

static usize datatypeSize(uint32 datatype) {
    switch (datatype) {
        case glTF::UNSIGNED_BYTE:
        case glTF::BYTE:
            return sizeof(uint8);
        case glTF::UNSIGNED_SHORT:
        case glTF::SHORT:
            return sizeof(uint16);
        case glTF::UNSIGNED_INT:
        case glTF::INT:
            return sizeof(uint32);
        case glTF::FLOAT:
            return sizeof(float);
        default:
            ENSURE(false);
    }
};

static usize componentElements(std::string_view component) {
    if (component == "SCALAR") {
        return 1;
    } else if (component == "VEC2") {
        return 2;
    } else if (component == "VEC3") {
        return 3;
    } else if (component == "VEC4") {
        return 4;
    } else if (component == "MAT2") {
        return 4;
    } else if (component == "MAT3") {
        return 9;
    } else if (component == "MAT4") {
        return 16;
    } else {
        ENSURE(false);
    }
};

/// @brief Read an accessor as a buffer of type SrcT[] and convert to type DstT[]
/// @code
/// // example, convert a vec3[] input to a vector<vec4> output
/// std::vector<vect> dstVector; // empty vector
/// glTF::readAccessor<vec4, vec3>(model, index, dstVector, [](const vec3& v) { return vec4(v, 0.0f); });
/// @endcode
/// @tparam DstT Type of dst vector buffer
/// @tparam SrcT Type of src components in model buffer
/// @tparam Cast A lambda to cast SrcT to DstT on generation
/// @param model gltf model
/// @param accessorIndex index of model accessor
/// @param[out] data Empty vector to write to
/// @param cast functor
template <typename DstT, typename SrcT = DstT, typename Cast>
static void readAccessor(glTF::Model& model, usize accessorIndex, std::vector<DstT>& data, Cast&& cast) {
    CHECK(data.empty());

    glTF::Accessor& accessor = model.accessors[accessorIndex];
    glTF::BufferView& bufferView = model.bufferViews[accessor.bufferView];

    // ensure our datatypes are correct
    CHECK(datatypeSize(accessor.componentType) * componentElements(accessor.type) == sizeof(SrcT));

    uint32 offset = accessor.byteOffset + bufferView.byteOffset;
    data.resize(accessor.count);

    // can take a vector of <typename DstT> and read the accessor as <typename SrcT>
    // eg accesor<uint16>[] --> std::vector<uint32>
    std::ranges::generate(data, [&, i = 0]() mutable -> DstT {
        const usize bufferIndex = offset + (i * sizeof(SrcT));
        i++;
        return cast(*std::bit_cast<const SrcT*>(&model.buffer()[bufferIndex]));
    });
}

template <typename DstT, typename SrcT = DstT>
static void readAccessor(glTF::Model& model, usize accessorIndex, std::vector<DstT>& data) {
    readAccessor<DstT, SrcT>(model, accessorIndex, data, [](const SrcT& x) -> DstT { return x; });
}

} // namespace R3::glTF
