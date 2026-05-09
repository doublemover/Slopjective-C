#pragma once

#include <filesystem>
#include <string>

#include "adr/objc3_adr_record.h"

namespace objc3c::adr {

std::filesystem::path BuildObjc3AdrMarkdownPath(
    const std::filesystem::path &adr_root,
    const Objc3AdrRecord &record);

std::string RenderObjc3AdrMetadataBlock(const Objc3AdrRecord &record);

}  // namespace objc3c::adr
