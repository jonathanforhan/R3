#include "glTF-Extensions.hxx"
#include "glTF-Util.hxx"

namespace R3::glTF {

void KHR_materials_pbrSpecularGlossiness::parse(Extension* extension, rapidjson::Value& value) {
    auto* self = reinterpret_cast<KHR_materials_pbrSpecularGlossiness*>(extension);

    if (value.HasMember("diffuseFactor")) {
        for (uint32 i = 0; auto& itDiffuseFactor : value["diffuseFactor"].GetArray()) {
            self->diffuseFactor[i++] = itDiffuseFactor.GetFloat();
        }
    }

    if (value.HasMember("diffuseTexture")) {
        self->diffuseTexture = TextureInfo{};
        populateTextureInfo(*self->diffuseTexture, value["diffuseTexture"]);
    }

    if (value.HasMember("specularFactor")) {
        for (uint32 i = 0; auto& itSpecularFactor : value["specularFactor"].GetArray()) {
            self->specularFactor[i++] = itSpecularFactor.GetFloat();
        }
    }

    maybeAssign(self->glossinessFactor, value, "glossinessFactor");

    if (value.HasMember("specularGlossinessTexture")) {
        self->specularGlossinessTexture = TextureInfo{};
        populateTextureInfo(*self->specularGlossinessTexture, value["specularGlossinessTexture"]);
    }
}

} // namespace R3::glTF