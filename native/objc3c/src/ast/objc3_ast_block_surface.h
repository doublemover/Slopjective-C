#pragma once

#include "ast/objc3_ast_core.h"

#include <cstddef>
#include <string>
#include <vector>

struct Objc3BlockParameterSourceModel {
  std::string name;
  std::string type_spelling = "implicit-unspecified";
  bool explicit_type = false;
};

bool Objc3ExprIsBlockLiteral(const Expr &expr);
bool Objc3BlockRequiresByrefStorage(const Expr &expr);
bool Objc3BlockRequiresRuntimeCopyDispose(const Expr &expr);
bool Objc3BlockCanUseStackInvokeLowering(const Expr &expr);
bool Objc3BlockRequiresRuntimePromotion(const Expr &expr);
std::string Objc3BlockLoweringReplayKey(const Expr &expr);

std::string BuildBlockLiteralCaptureProfile(
    const std::vector<std::string> &capture_names_lexicographic);
std::string BuildBlockParameterSignatureEntry(
    const Objc3BlockParameterSourceModel &parameter);
std::vector<std::string> BuildBlockParameterSignatureEntriesLexicographic(
    const std::vector<Objc3BlockParameterSourceModel> &parameters);
std::vector<ValueType> BuildBlockParameterTypesSourceOrder(
    const std::vector<Objc3BlockParameterSourceModel> &parameters);
std::string BuildBlockSignatureProfile(
    const std::vector<Objc3BlockParameterSourceModel> &parameters);
std::string BuildBlockCaptureInventoryEntry(const std::string &capture_name);
std::vector<std::string> BuildBlockCaptureInventoryEntriesLexicographic(
    const std::vector<std::string> &capture_names_lexicographic);
std::string BuildBlockCaptureInventoryProfile(
    std::size_t capture_count,
    std::size_t byvalue_readonly_capture_count);
std::string BuildBlockLiteralAbiLayoutProfile(std::size_t parameter_count,
                                              std::size_t capture_count,
                                              std::size_t body_statement_count);
std::string BuildBlockLiteralAbiDescriptorSymbol(unsigned line,
                                                 unsigned column,
                                                 std::size_t parameter_count,
                                                 std::size_t capture_count);
std::string BuildBlockLiteralInvokeTrampolineSymbol(unsigned line,
                                                    unsigned column,
                                                    std::size_t parameter_count,
                                                    std::size_t capture_count);
std::vector<std::string> BuildBlockInvokeSurfaceEntriesLexicographic(
    const Expr &block);
std::string BuildBlockInvokeSurfaceProfile(const Expr &block);
std::string BuildBlockSourceModelReplayKey(const Expr &block);
std::string BuildBlockStorageEscapeProfile(std::size_t mutable_capture_count,
                                           std::size_t byref_capture_count,
                                           bool escapes_to_heap,
                                           std::size_t body_statement_count);
std::string BuildBlockStorageByrefLayoutSymbol(unsigned line,
                                               unsigned column,
                                               std::size_t mutable_capture_count,
                                               std::size_t byref_capture_count,
                                               bool escapes_to_heap);
std::string BuildBlockCopyDisposeProfile(std::size_t mutable_capture_count,
                                         std::size_t byref_capture_count,
                                         bool escapes_to_heap,
                                         std::size_t body_statement_count);
std::string BuildBlockCopyHelperSymbol(unsigned line,
                                       unsigned column,
                                       std::size_t mutable_capture_count,
                                       std::size_t byref_capture_count,
                                       bool escapes_to_heap);
std::string BuildBlockDisposeHelperSymbol(unsigned line,
                                          unsigned column,
                                          std::size_t mutable_capture_count,
                                          std::size_t byref_capture_count,
                                          bool escapes_to_heap);
std::size_t BuildBlockDeterminismPerfBaselineWeight(std::size_t parameter_count,
                                                    std::size_t capture_count,
                                                    std::size_t body_statement_count,
                                                    bool copy_helper_required,
                                                    bool dispose_helper_required);
std::string BuildBlockDeterminismPerfBaselineProfile(std::size_t parameter_count,
                                                     std::size_t capture_count,
                                                     std::size_t body_statement_count,
                                                     bool copy_helper_required,
                                                     bool dispose_helper_required,
                                                     bool deterministic_capture_set,
                                                     bool copy_dispose_profile_is_normalized,
                                                     std::size_t baseline_weight);
