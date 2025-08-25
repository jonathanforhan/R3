#pragma once

#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

namespace R3 {

namespace json {

// namespace json = rapidjson;
using namespace rapidjson;

inline void dump(const Document& doc) {
    OStreamWrapper osw(std::cout);
    PrettyWriter<OStreamWrapper> writer(osw);
    doc.Accept(writer);
    std::cout << '\n';
}

inline void dump(const Value& value) {
    OStreamWrapper osw(std::cout);
    PrettyWriter<OStreamWrapper> writer(osw);
    value.Accept(writer);
    std::cout << '\n';
}

} // namespace json

} // namespace R3