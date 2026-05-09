#include "adr/objc3_adr_markdown.h"

#include <sstream>

namespace objc3c::adr {

std::filesystem::path BuildObjc3AdrMarkdownPath(
    const std::filesystem::path &adr_root,
    const Objc3AdrRecord &record) {
  const std::string_view file_name = Objc3AdrFileName(record);
  if (file_name.empty()) {
    return std::filesystem::path();
  }
  return adr_root / std::string(file_name);
}

std::string RenderObjc3AdrMetadataBlock(const Objc3AdrRecord &record) {
  std::ostringstream out;
  out << "- Status: " << Objc3AdrStatusName(record.status) << "\n";
  out << "- Date: " << record.date << "\n";
  out << "- Deciders: " << record.deciders << "\n";
  out << "- Related surfaces: `" << record.related_surfaces << "`\n";
  return out.str();
}

}  // namespace objc3c::adr
