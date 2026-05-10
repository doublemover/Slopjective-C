#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

namespace {

constexpr const char *kObjc3IRFrontendProfilePrefix =
    "; frontend_profile = language_version=";
constexpr const char *kObjc3IRFrontendProfileLanguageProfileField =
    ", language_profile=";
constexpr const char *kObjc3IRFrontendProfileArcModeField = ", arc_mode=";
constexpr const char *kObjc3IRFrontendProfileCanonicalLiteralRejectionField =
    ", canonical_literal_rejection_total=";

}  // namespace

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata) {
  std::ostringstream out;
  out << kObjc3IRFrontendProfilePrefix
      << static_cast<unsigned>(metadata.language_version)
      << kObjc3IRFrontendProfileLanguageProfileField
      << metadata.language_profile << kObjc3IRFrontendProfileArcModeField
      << metadata.arc_mode
      << kObjc3IRFrontendProfileCanonicalLiteralRejectionField
      << metadata.canonical_literal_rejection_total();
  return out.str();
}
