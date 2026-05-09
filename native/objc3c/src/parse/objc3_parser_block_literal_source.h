#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"

namespace objc3c::parse {

enum class Objc3BlockLiteralSourceUseKind {
  ExpressionSite,
  GlobalInitializer,
  LocalBindingInitializer,
  AssignmentValue,
  ReturnValue,
  CallArgument,
  MessageArgument,
};

std::vector<std::string> BuildObjc3BlockLiteralCaptureSet(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const std::vector<std::string> &parameter_names,
    bool &deterministic);

std::vector<std::string> BuildObjc3BlockLiteralMutatedCaptureSet(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const std::vector<std::string> &capture_names);

std::string BuildObjc3BlockEscapeShapeSymbol(
    Objc3BlockLiteralSourceUseKind use_kind);

bool Objc3BlockEscapeShapePromotesToHeapCandidate(
    Objc3BlockLiteralSourceUseKind use_kind);

std::string BuildObjc3BlockHelperIntentProfile(
    std::size_t mutated_capture_count,
    std::size_t byref_capture_count,
    bool copy_helper_intent_required,
    bool dispose_helper_intent_required,
    Objc3BlockLiteralSourceUseKind use_kind);

std::string BuildObjc3BlockEscapeShapeProfile(
    Objc3BlockLiteralSourceUseKind use_kind,
    bool promotes_to_heap_candidate,
    std::size_t capture_count,
    std::size_t byref_capture_count);

}  // namespace objc3c::parse
