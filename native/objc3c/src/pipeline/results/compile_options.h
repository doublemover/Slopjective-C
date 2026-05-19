#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "config/objc3_language_profile.h"
#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_diagnostics_bus.h"
#include "parse/objc3_parser_contract.h"
#include "pipeline/results/canonical_literal_rejection_counts.h"
#include "sema/objc3_sema_contract.h"
#include "sema/objc3_sema_pass_manager_contract.h"
#include "token/objc3_token_contract.h"

inline constexpr std::uint8_t kObjc3DefaultLanguageVersion =
    objc3c::config::kCanonicalLanguageVersion;

enum class Objc3FrontendLanguageProfile : std::uint8_t {
  kCanonical = 0u,
};

enum class Objc3FrontendArcMode : std::uint8_t {
  kDisabled = 0u,
  kEnabled = 1u,
};

struct Objc3FrontendOptions {
  std::uint8_t language_version = kObjc3DefaultLanguageVersion;
  Objc3FrontendLanguageProfile language_profile = Objc3FrontendLanguageProfile::kCanonical;
  Objc3FrontendArcMode arc_mode = Objc3FrontendArcMode::kDisabled;
  bool emit_manifest = true;
  bool emit_ir = true;
  bool emit_object = true;
  bool allow_live_error_runtime_surface = false;
  std::uint64_t bootstrap_registration_order_ordinal = 1u;
  std::string metaprogramming_cache_root_relative_path;
  std::vector<std::string> imported_runtime_surface_paths;
  Objc3LoweringContract lowering;
};

struct Objc3FrontendLanguageVersionPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
};

struct Objc3FrontendNamedIdentifierPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
  std::string identifier;
};

struct Objc3FrontendBootstrapRegistrationSourcePragmaContract {
  Objc3FrontendNamedIdentifierPragmaContract registration_descriptor;
  Objc3FrontendNamedIdentifierPragmaContract image_root;
};
