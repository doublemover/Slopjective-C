#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata) {
  std::ostringstream out;
  out << "; frontend_profile = language_version="
      << static_cast<unsigned>(metadata.language_version)
      << ", language_profile=" << metadata.language_profile
      << ", arc_mode=" << metadata.arc_mode
      << ", canonical_literal_rejection_total="
      << metadata.canonical_literal_rejection_total();
  return out.str();
}

std::string BuildObjc3IRFrontendMetadataNode(
    const Objc3IRFrontendMetadata &metadata) {
  std::ostringstream out;
  out << "!0 = !{i32 " << static_cast<unsigned>(metadata.language_version)
      << ", !\"" << EscapeCStringLiteral(metadata.language_profile)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_yes_rejection_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_no_rejection_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_null_rejection_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_rejection_total())
      << "}";
  return out.str();
}
