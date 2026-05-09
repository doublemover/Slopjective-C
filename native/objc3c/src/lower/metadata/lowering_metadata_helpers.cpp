#include "lower/metadata/lowering_metadata_helpers.h"

extern const std::array<const char *,
                        kObjc3RuntimeMetadataLayoutPolicyFamilyCount>
    kCanonicalRuntimeMetadataFamilyOrder = {
        kObjc3RuntimeMetadataLayoutPolicyClassFamily,
        kObjc3RuntimeMetadataLayoutPolicyProtocolFamily,
        kObjc3RuntimeMetadataLayoutPolicyCategoryFamily,
        kObjc3RuntimeMetadataLayoutPolicyPropertyFamily,
        kObjc3RuntimeMetadataLayoutPolicyIvarFamily,
};

const char *BoolToken(bool value) { return value ? "true" : "false"; }

// object-format policy expansion anchor: lowering selects one
// supported host object format and derives emitted section spellings from the
// logical metadata ABI surface before IR emission begins.
const char *HostRuntimeMetadataObjectFormat() {
#if defined(_WIN32)
  return kObjc3RuntimeMetadataObjectFormatCoff;
#elif defined(__APPLE__)
  return kObjc3RuntimeMetadataObjectFormatMachO;
#else
  return kObjc3RuntimeMetadataObjectFormatElf;
#endif
}

const char *HostRuntimeMetadataSectionSpellingModel() {
#if defined(_WIN32)
  return kObjc3RuntimeMetadataSectionSpellingModelCoff;
#elif defined(__APPLE__)
  return kObjc3RuntimeMetadataSectionSpellingModelMachO;
#else
  return kObjc3RuntimeMetadataSectionSpellingModelElf;
#endif
}

const char *HostRuntimeMetadataRetentionAnchorModel() {
#if defined(_WIN32)
  return kObjc3RuntimeMetadataRetentionAnchorModelCoff;
#elif defined(__APPLE__)
  return kObjc3RuntimeMetadataRetentionAnchorModelMachO;
#else
  return kObjc3RuntimeMetadataRetentionAnchorModelElf;
#endif
}

bool IsSupportedRuntimeMetadataObjectFormat(const std::string &object_format) {
  return object_format == kObjc3RuntimeMetadataObjectFormatCoff ||
         object_format == kObjc3RuntimeMetadataObjectFormatElf ||
         object_format == kObjc3RuntimeMetadataObjectFormatMachO;
}

std::string MapRuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section) {
  if (logical_section.empty()) {
    return "";
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatCoff ||
      object_format == kObjc3RuntimeMetadataObjectFormatElf) {
    return logical_section;
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatMachO) {
    constexpr const char *kLogicalPrefix = "objc3.runtime.";
    const std::string logical_prefix = kLogicalPrefix;
    if (logical_section.rfind(logical_prefix, 0) != 0) {
      return "";
    }
    return "__DATA,__objc3_" + logical_section.substr(logical_prefix.size());
  }
  return "";
}

std::string BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name) {
  if (symbol_name.empty()) {
    return "";
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatCoff) {
    return std::string("-Wl,/include:") + symbol_name;
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatElf) {
    return std::string("-Wl,--undefined=") + symbol_name;
  }
  if (object_format == kObjc3RuntimeMetadataObjectFormatMachO) {
    return std::string("-Wl,-u,_") + symbol_name;
  }
  return "";
}

std::size_t CountRuntimeMetadataLayoutDescriptors(
    const std::array<Objc3RuntimeMetadataLayoutPolicyFamily,
                     kObjc3RuntimeMetadataLayoutPolicyFamilyCount> &families) {
  std::size_t total = 0;
  for (const auto &family : families) {
    total += family.descriptor_count;
  }
  return total;
}
