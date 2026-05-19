#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "config/objc3_language_profile.h"

enum class Objc3IrObjectBackend {
  kClang,
  kLLVMDirect,
};

enum class Objc3ArcMode {
  kDisabled,
  kEnabled,
};

enum class Objc3ConformanceProfile {
  kCore,
  kStrict,
  kStrictConcurrency,
  kStrictSystem,
};

enum class Objc3CliCommandMode {
  kCompile,
  kValidateConformance,
};

struct Objc3CliOptions {
  Objc3CliCommandMode command_mode = Objc3CliCommandMode::kCompile;
  std::filesystem::path input;
  std::filesystem::path out_dir = std::filesystem::path("tmp") / "artifacts" / "compilation" / "objc3c-native";
  std::string emit_prefix = "module";
  std::filesystem::path clang_path = std::filesystem::path("clang");
  std::filesystem::path llc_path = std::filesystem::path("llc");
  std::filesystem::path llvm_capabilities_summary;
  bool route_backend_from_capabilities = false;
  bool clang_path_explicit = false;
  bool llc_path_explicit = false;
  Objc3IrObjectBackend ir_object_backend = Objc3IrObjectBackend::kLLVMDirect;
  std::uint32_t language_version = objc3c::config::kCanonicalLanguageVersion;
  Objc3ArcMode arc_mode = Objc3ArcMode::kDisabled;
  bool allow_live_error_runtime_surface = false;
  Objc3ConformanceProfile conformance_profile =
      Objc3ConformanceProfile::kCore;
  bool emit_objc3_conformance = false;
  std::string emit_objc3_conformance_format = "json";
  std::filesystem::path validate_conformance_report_path;
  std::uint64_t bootstrap_registration_order_ordinal = 1;
  std::filesystem::path metaprogramming_cache_root;
  std::vector<std::filesystem::path> imported_runtime_surface_paths;
  std::size_t max_message_send_args = 4;
  std::string runtime_dispatch_symbol = "objc3_runtime_dispatch_i32";
};
