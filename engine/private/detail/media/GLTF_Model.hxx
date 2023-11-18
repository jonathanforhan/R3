#pragma once
#include <string_view>
#include <vector>
#include "api/Types.hpp"
#include "detail/spec/glTF.hxx"

namespace R3 {

class GLTF_Model : public GLTF_Root {
public:
    explicit GLTF_Model(std::string_view path);

    auto buffer() const -> const std::vector<char>& { return m_buffer; }

private:
    void populateRoot();

    void checkVersion(std::string_view version) const;
    void checkVersion(uint32 major, uint32 minor) const;

private:
    rapidjson::Document m_document;
    std::vector<char> m_buffer;
    std::string m_path;
};

} // namespace R3