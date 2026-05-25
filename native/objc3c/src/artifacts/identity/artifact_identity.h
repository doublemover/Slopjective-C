#pragma once

#include <filesystem>
#include <string>
#include <string_view>

// Literal macros keep host-specific artifact names constexpr for contract
// headers; they are private to this header and undefined at the end.
#define OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL "module"
#define OBJC3_NATIVE_MANIFEST_ARTIFACT_SUFFIX_LITERAL ".manifest.json"
#define OBJC3_NATIVE_IR_ARTIFACT_SUFFIX_LITERAL ".ll"
#define OBJC3_NATIVE_RUNTIME_METADATA_BINARY_ARTIFACT_SUFFIX_LITERAL \
  ".runtime-metadata.bin"
#define OBJC3_NATIVE_RUNTIME_METADATA_LINKER_OPTIONS_ARTIFACT_SUFFIX_LITERAL \
  ".runtime-metadata-linker-options.rsp"
#define OBJC3_NATIVE_RUNTIME_METADATA_DISCOVERY_ARTIFACT_SUFFIX_LITERAL \
  ".runtime-metadata-discovery.json"
#define OBJC3_NATIVE_RUNTIME_REGISTRATION_MANIFEST_ARTIFACT_SUFFIX_LITERAL \
  ".runtime-registration-manifest.json"

#if defined(_WIN32)
#define OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL ".exe"
#define OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL ".obj"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_KIND_LITERAL "static-archive"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_BASENAME_LITERAL "objc3_runtime"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_RELATIVE_PATH_LITERAL \
  "artifacts/lib/objc3_runtime.lib"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_NAME_LITERAL "objc3_runtime.lib"
#elif defined(__APPLE__) && defined(__MACH__)
#define OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL ""
#define OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL ".o"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_KIND_LITERAL "shared-library"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_BASENAME_LITERAL "libobjc3-runtime"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_RELATIVE_PATH_LITERAL \
  "artifacts/lib/libobjc3-runtime.dylib"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_NAME_LITERAL "libobjc3-runtime.dylib"
#else
#define OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL ""
#define OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL ".o"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_KIND_LITERAL "shared-library"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_BASENAME_LITERAL "libobjc3-runtime"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_RELATIVE_PATH_LITERAL \
  "artifacts/lib/libobjc3-runtime.so"
#define OBJC3_NATIVE_RUNTIME_LIBRARY_NAME_LITERAL "libobjc3-runtime.so"
#endif

namespace objc3::artifacts::identity {

inline constexpr const char *kObjc3NativeDefaultArtifactStem =
    OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL;
inline constexpr const char *kObjc3NativeExecutableFileExtension =
    OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL;
inline constexpr const char *kObjc3NativeObjectFileExtension =
    OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL;
inline constexpr const char *kObjc3NativeManifestArtifactSuffix =
    OBJC3_NATIVE_MANIFEST_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char *kObjc3NativeIrArtifactSuffix =
    OBJC3_NATIVE_IR_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char *kObjc3NativeRuntimeMetadataBinaryArtifactSuffix =
    OBJC3_NATIVE_RUNTIME_METADATA_BINARY_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char
    *kObjc3NativeRuntimeMetadataLinkerOptionsArtifactSuffix =
        OBJC3_NATIVE_RUNTIME_METADATA_LINKER_OPTIONS_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char *kObjc3NativeRuntimeMetadataDiscoveryArtifactSuffix =
    OBJC3_NATIVE_RUNTIME_METADATA_DISCOVERY_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char
    *kObjc3NativeRuntimeRegistrationManifestArtifactSuffix =
        OBJC3_NATIVE_RUNTIME_REGISTRATION_MANIFEST_ARTIFACT_SUFFIX_LITERAL;

inline constexpr const char *kObjc3NativeExecutableRelativePath =
    "artifacts/bin/objc3c-native"
    OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL;
inline constexpr const char *kObjc3NativeFrontendRunnerRelativePath =
    "artifacts/bin/objc3c-frontend-c-api-runner"
    OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL;
inline constexpr const char *kObjc3NativeRuntimeLibraryRelativePath =
    OBJC3_NATIVE_RUNTIME_LIBRARY_RELATIVE_PATH_LITERAL;
inline constexpr const char *kObjc3NativeRuntimeLibraryName =
    OBJC3_NATIVE_RUNTIME_LIBRARY_NAME_LITERAL;
inline constexpr const char *kObjc3NativeRuntimeLibraryKind =
    OBJC3_NATIVE_RUNTIME_LIBRARY_KIND_LITERAL;
inline constexpr const char *kObjc3NativeRuntimeLibraryBasename =
    OBJC3_NATIVE_RUNTIME_LIBRARY_BASENAME_LITERAL;

inline constexpr const char *kObjc3NativeDefaultManifestArtifactName =
    OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
    OBJC3_NATIVE_MANIFEST_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char *kObjc3NativeDefaultIrArtifactName =
    OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
    OBJC3_NATIVE_IR_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char *kObjc3NativeDefaultObjectArtifactName =
    OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
    OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL;
inline constexpr const char
    *kObjc3NativeDefaultRuntimeMetadataBinaryArtifactName =
        OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
        OBJC3_NATIVE_RUNTIME_METADATA_BINARY_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char
    *kObjc3NativeDefaultRuntimeMetadataLinkerOptionsArtifactName =
        OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
        OBJC3_NATIVE_RUNTIME_METADATA_LINKER_OPTIONS_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char
    *kObjc3NativeDefaultRuntimeMetadataDiscoveryArtifactName =
        OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
        OBJC3_NATIVE_RUNTIME_METADATA_DISCOVERY_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char
    *kObjc3NativeDefaultRuntimeRegistrationManifestArtifactName =
        OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
        OBJC3_NATIVE_RUNTIME_REGISTRATION_MANIFEST_ARTIFACT_SUFFIX_LITERAL;
inline constexpr const char *kObjc3NativeDefaultObjectSectionInventoryCommand =
    "llvm-readobj --sections "
    OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
    OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL;
inline constexpr const char *kObjc3NativeDefaultObjectSymbolInventoryCommand =
    "llvm-objdump --syms "
    OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
    OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL;

struct Objc3NativeArtifactIdentity {
  std::string platform_id;
  std::string host_promotion_state;
  std::string target_triple;
  std::string object_file_extension;
  std::string object_format;
  std::string debug_format;
  std::string runtime_library_kind;
  std::string native_executable_relative_path;
  std::string frontend_runner_relative_path;
  std::string runtime_library_relative_path;
  std::string runtime_library_name;
};

struct Objc3TranslationUnitIdentityEvidence {
  std::filesystem::path input_path;
  std::string parse_artifact_replay_key;
  std::string lowering_boundary_replay_key;
};

[[nodiscard]] Objc3NativeArtifactIdentity
BuildObjc3NativeArtifactIdentityForHost();

[[nodiscard]] std::string BuildObjc3NativeArtifactName(
    std::string_view stem,
    std::string_view suffix);

[[nodiscard]] std::string BuildObjc3NativeObjectArtifactName(
    std::string_view stem);

[[nodiscard]] inline std::string BuildObjc3NativeFrontendRunnerProbeCommand(
    std::string_view input_path,
    std::string_view out_dir,
    std::string_view emit_prefix,
    bool suppress_ir,
    bool suppress_object) {
  if (input_path.empty() || out_dir.empty() || emit_prefix.empty()) {
    return {};
  }

  std::string command = std::string(kObjc3NativeFrontendRunnerRelativePath) +
                        " " + std::string(input_path) + " --out-dir " +
                        std::string(out_dir) + " --emit-prefix " +
                        std::string(emit_prefix);
  if (suppress_ir) {
    command += " --no-emit-ir";
  }
  if (suppress_object) {
    command += " --no-emit-object";
  }
  return command;
}

[[nodiscard]] std::string BuildObjc3TranslationUnitIdentityKey(
    const Objc3TranslationUnitIdentityEvidence &evidence);

}  // namespace objc3::artifacts::identity

#undef OBJC3_NATIVE_DEFAULT_ARTIFACT_STEM_LITERAL
#undef OBJC3_NATIVE_MANIFEST_ARTIFACT_SUFFIX_LITERAL
#undef OBJC3_NATIVE_IR_ARTIFACT_SUFFIX_LITERAL
#undef OBJC3_NATIVE_RUNTIME_METADATA_BINARY_ARTIFACT_SUFFIX_LITERAL
#undef OBJC3_NATIVE_RUNTIME_METADATA_LINKER_OPTIONS_ARTIFACT_SUFFIX_LITERAL
#undef OBJC3_NATIVE_RUNTIME_METADATA_DISCOVERY_ARTIFACT_SUFFIX_LITERAL
#undef OBJC3_NATIVE_RUNTIME_REGISTRATION_MANIFEST_ARTIFACT_SUFFIX_LITERAL
#undef OBJC3_NATIVE_EXECUTABLE_FILE_EXTENSION_LITERAL
#undef OBJC3_NATIVE_OBJECT_FILE_EXTENSION_LITERAL
#undef OBJC3_NATIVE_RUNTIME_LIBRARY_KIND_LITERAL
#undef OBJC3_NATIVE_RUNTIME_LIBRARY_BASENAME_LITERAL
#undef OBJC3_NATIVE_RUNTIME_LIBRARY_RELATIVE_PATH_LITERAL
#undef OBJC3_NATIVE_RUNTIME_LIBRARY_NAME_LITERAL
