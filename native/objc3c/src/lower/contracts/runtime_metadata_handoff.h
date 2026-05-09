#pragma once

#include "ast/objc3_ast_declarations.h"

#include <cstddef>
#include <string>

struct Objc3RuntimeMetadataLoweringHandoff {
  std::string module_name;
  std::size_t class_count = 0;
  std::size_t protocol_count = 0;
  std::size_t category_count = 0;
  std::size_t property_count = 0;
  std::size_t method_count = 0;
  std::size_t ivar_layout_count = 0;
  bool has_runtime_registration_roots = false;
  bool deterministic = false;
  std::string replay_key;
};

Objc3RuntimeMetadataLoweringHandoff Objc3BuildRuntimeMetadataLoweringHandoff(
    const Objc3Program &program);
bool Objc3RuntimeMetadataLoweringHandoffIsReady(
    const Objc3RuntimeMetadataLoweringHandoff &handoff);
std::string Objc3RuntimeMetadataLoweringHandoffReplayKey(
    const Objc3RuntimeMetadataLoweringHandoff &handoff);
