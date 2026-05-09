#pragma once

#include "ast/objc3_ast_core.h"

#include <cstddef>
#include <string>

struct Objc3BlockArcLoweringPlan {
  bool block_literal = false;
  bool normalized = false;
  bool stack_invoke_lowering = false;
  bool byref_storage_required = false;
  bool copy_helper_required = false;
  bool dispose_helper_required = false;
  bool runtime_promotion_required = false;
  bool ownership_runtime_required = false;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t byref_capture_count = 0;
  std::size_t owned_capture_count = 0;
  std::string descriptor_symbol;
  std::string invoke_symbol;
  std::string copy_helper_symbol;
  std::string dispose_helper_symbol;
  std::string replay_key;
};

Objc3BlockArcLoweringPlan Objc3BuildBlockArcLoweringPlan(const Expr &expr);
bool Objc3BlockArcLoweringPlanIsRunnable(
    const Objc3BlockArcLoweringPlan &plan);
std::string Objc3BlockArcLoweringPlanReplayKey(
    const Objc3BlockArcLoweringPlan &plan);
