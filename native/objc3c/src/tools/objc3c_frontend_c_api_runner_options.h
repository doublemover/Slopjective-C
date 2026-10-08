#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiRunnerOptions {
  std::filesystem::path input_path;
  std::filesystem::path out_dir =
      std::filesystem::path("tmp") / "artifacts" / "compilation" /
      "objc3c-native";
  std::filesystem::path metaprogramming_cache_root;
  std::vector<std::filesystem::path> imported_runtime_surface_paths;
  std::string emit_prefix = "module";
  std::filesystem::path clang_path = std::filesystem::path("clang");
  std::filesystem::path llc_path = std::filesystem::path("llc");
  bool clang_path_explicit = false;
  bool llc_path_explicit = false;
  objc3c_frontend_c_ir_object_backend_t ir_object_backend =
      OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG;
  std::uint32_t max_message_send_args = 0;
  std::string runtime_dispatch_symbol;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  bool allow_live_error_runtime_surface = false;
  std::uint64_t translation_unit_registration_order_ordinal = 0;
  std::filesystem::path summary_out;
  bool dump_summary_json = false;
  bool dump_observability_json = false;
  bool dump_playground_repro_json = false;
  bool dump_runtime_inspector_json = false;
  bool dump_stage_trace_json = false;
};
