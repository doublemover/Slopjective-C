#pragma once

#include <string>

// Runtime metadata object-format publication owns logical-to-host section
// spellings and linker retention anchor policy for COFF, ELF, and Mach-O.
inline constexpr const char
    *kObjc3RuntimeMetadataObjectFormatSurfaceContractId =
        "objc3c.runtime.metadata.object.format.policy.v1";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatCoff = "coff";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatElf = "elf";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatMachO = "mach-o";
inline constexpr const char *kObjc3RuntimeMetadataSectionSpellingModelCoff =
    "coff-logical-section-spellings";
inline constexpr const char *kObjc3RuntimeMetadataSectionSpellingModelElf =
    "elf-logical-section-spellings";
inline constexpr const char *kObjc3RuntimeMetadataSectionSpellingModelMachO =
    "mach-o-data-segment-comma-section-spellings";
inline constexpr const char *kObjc3RuntimeMetadataRetentionAnchorModelCoff =
    "llvm.used-appending-global+coff-timestamp-normalization";
inline constexpr const char *kObjc3RuntimeMetadataRetentionAnchorModelElf =
    "llvm.used-appending-global+elf-stable-sections";
inline constexpr const char *kObjc3RuntimeMetadataRetentionAnchorModelMachO =
    "llvm.used-appending-global+mach-o-data-segment-sections";

std::string Objc3RuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section);
std::string Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name);
std::string Objc3RuntimeMetadataHostSectionForLogicalName(
    const std::string &logical_section);
