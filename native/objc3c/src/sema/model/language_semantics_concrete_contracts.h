#pragma once

#include <array>
#include <string_view>

namespace objc3c::sema {

inline constexpr const char *kObjc3LanguageSemanticsConcreteContractId =
    "objc3c.language-semantics.concrete-contracts.v1";

enum class Objc3LanguageSemanticsConcretePhase {
  kParser,
  kSema,
  kLowering,
  kRuntimeExecution,
};

struct Objc3LanguageSemanticsConcreteFixtureContract {
  int issue_ref = 0;
  Objc3LanguageSemanticsConcretePhase owner_phase =
      Objc3LanguageSemanticsConcretePhase::kSema;
  std::string_view semantic_surface;
  std::string_view fixture_path;
  std::string_view support_claim;
  std::string_view expected_diagnostic_code;
  bool parser_ast_required = false;
  bool typed_sema_required = false;
  bool lowering_handoff_required = false;
  bool runtime_execution_required = false;
  bool unsupported_patterns_fail_closed = true;
};

inline constexpr std::array<Objc3LanguageSemanticsConcreteFixtureContract, 17>
    kObjc3LanguageSemanticsConcreteFixtureContracts = {{
        {8160,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "generic-type-system",
         "tests/tooling/fixtures/native/type_semantic_protocol_generic_positive.objc3",
         "objc3c.behavior.language.generics.public-type-parameters",
         "",
         true,
         true,
         true,
         false,
         true},
        {8160,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "generic-variance-specialization",
         "tests/tooling/fixtures/native/type_semantic_generic_variance_positive.objc3",
         "objc3c.behavior.language.generics.public-type-parameters",
         "",
         true,
         true,
         true,
         false,
         true},
        {8160,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "nested-generic-substitution",
         "tests/tooling/fixtures/native/type_semantic_nested_generic_positive.objc3",
         "objc3c.behavior.language.generics.public-type-parameters",
         "",
         true,
         true,
         true,
         false,
         true},
        {8160,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "generic-method-substitution",
         "tests/tooling/fixtures/native/type_semantic_generic_method_substitution_positive.objc3",
         "objc3c.behavior.language.generics.public-type-parameters",
         "",
         true,
         true,
         true,
         false,
         true},
        {8164,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "protocol-existential-value-flow",
         "tests/tooling/fixtures/native/protocol_qualified_existential_value_flow.objc3",
         "objc3c.behavior.language.protocols.existential-witness-model",
         "",
         true,
         true,
         true,
         false,
         true},
        {8164,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "protocol-existential-unknown-member-rejection",
         "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3",
         "objc3c.behavior.language.protocols.existential-witness-model",
         "O3S216",
         true,
         true,
         false,
         false,
         true},
        {8164,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "protocol-qualified-runtime-value",
         "tests/tooling/fixtures/native/execution/positive/id_protocol_qualifier_alias_signature.objc3",
         "objc3c.behavior.language.protocols.existential-witness-model",
         "",
         true,
         true,
         true,
         true,
         true},
        {8166,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "ownership-block-capture-runtime",
         "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
         "objc3c.behavior.language.ownership.memory-model",
         "",
         true,
         true,
         true,
         true,
         true},
        {8166,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "ownership-conflicting-capture-rejection",
         "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_conflicting_capture_ownership.objc3",
         "objc3c.behavior.language.ownership.memory-model",
         "O3S301",
         true,
         true,
         false,
         false,
         true},
        {8167,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "async-await-executor-sema",
         "tests/native/sema/concurrency/async_await_value_flow.objc3",
         "objc3c.behavior.language.concurrency.public-usability-model",
         "",
         true,
         true,
         true,
         false,
         true},
        {8167,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "await-outside-async-rejection",
         "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_await_outside_async.objc3",
         "objc3c.behavior.language.concurrency.public-usability-model",
         "O3S223",
         true,
         true,
         false,
         false,
         true},
        {8167,
         Objc3LanguageSemanticsConcretePhase::kSema,
         "executor-on-sync-callable-rejection",
         "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_executor_on_sync_function.objc3",
         "objc3c.behavior.language.concurrency.public-usability-model",
         "O3S224",
         true,
         true,
         false,
         false,
         true},
        {8160,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "generic-runtime-metadata-api",
         "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
         "objc3c.behavior.language.generics.public-type-parameters",
         "",
         true,
         true,
         true,
         true,
         true},
        {8164,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "protocol-witness-runtime-api",
         "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
         "objc3c.behavior.language.protocols.existential-witness-model",
         "",
         true,
         true,
         true,
         true,
         true},
        {8166,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "ownership-runtime-api",
         "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
         "objc3c.behavior.language.ownership.memory-model",
         "",
         true,
         true,
         true,
         true,
         true},
        {8167,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "concurrency-runtime-api-surface",
         "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
         "objc3c.behavior.language.concurrency.public-usability-model",
         "",
         true,
         true,
         true,
         true,
         true},
        {8160,
         Objc3LanguageSemanticsConcretePhase::kRuntimeExecution,
         "foundation-next-combined-runtime-semantics",
         "tests/tooling/fixtures/native/execution/positive/language_semantics_public_runtime_contract.objc3",
         "objc3c.behavior.language.generics.public-type-parameters",
         "",
         true,
         true,
         true,
         true,
         true},
    }};

inline constexpr bool IsReadyObjc3LanguageSemanticsConcreteFixtureContract(
    const Objc3LanguageSemanticsConcreteFixtureContract &contract) {
  const bool has_expected_diagnostic = !contract.expected_diagnostic_code.empty();
  return contract.issue_ref >= 8160 && !contract.semantic_surface.empty() &&
         !contract.fixture_path.empty() && !contract.support_claim.empty() &&
         contract.parser_ast_required && contract.typed_sema_required &&
         (contract.lowering_handoff_required || has_expected_diagnostic) &&
         contract.unsupported_patterns_fail_closed;
}

inline constexpr bool
AllObjc3LanguageSemanticsConcreteFixtureContractsReady() {
  for (const auto &contract :
       kObjc3LanguageSemanticsConcreteFixtureContracts) {
    if (!IsReadyObjc3LanguageSemanticsConcreteFixtureContract(contract)) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3c::sema
