#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_interop_semantic_json_contract_fields.h"
#include "artifacts/objc3_frontend_interop_semantic_json_summary_fields.h"

namespace objc3::artifacts::frontend {

std::string BuildInteropInteropSemanticModelSummaryJson(
    const Objc3InteropInteropSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropInteropSemanticModelSummaryJsonFields(out, summary);
  out << "}";
  return out.str();
}

std::string BuildInteropInteropRuntimeParitySummaryJson(
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropInteropRuntimeParitySummaryJsonFields(out, summary);
  out << "}";
  return out.str();
}

std::string BuildInteropCppInteropInteractionSummaryJson(
    const Objc3InteropCppInteropInteractionSummary &summary) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropCppInteropInteractionSummaryJsonFields(out, summary);
  out << "}";
  return out.str();
}

std::string BuildInteropSwiftInteropIsolationSummaryJson(
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropSwiftInteropIsolationSummaryJsonFields(out, summary);
  out << "}";
  return out.str();
}

std::string BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropForeignSurfaceInterfacePreservationSummaryJsonFields(out,
                                                                        summary);
  out << "}";
  return out.str();
}

std::string BuildInteropHeaderModuleBridgeGenerationSummaryJson(
    const Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropHeaderModuleBridgeGenerationSummaryJsonFields(out, summary);
  out << "}";
  return out.str();
}

std::string BuildInteropInteropLoweringContractJson(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropInteropLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3InteropInteropLoweringContract(contract);
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::AppendInteropInteropLoweringContractJsonFields(
      out,
      semantic_summary,
      runtime_parity_summary,
      cpp_summary,
      swift_summary,
      preservation_summary,
      contract,
      replay_key,
      ready_for_ir_emission);
  out << "}";
  return out.str();
}

std::string BuildInteropForeignCallLifetimeLoweringContractJson(
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropForeignCallLifetimeLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3InteropForeignCallLifetimeLoweringContract(contract);
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropForeignCallLifetimeLoweringContractJsonFields(
          out,
          dependency_contract,
          cpp_summary,
          preservation_summary,
          contract,
          replay_key,
          ready_for_ir_emission);
  out << "}";
  return out.str();
}

std::string BuildInteropFfiMetadataInterfacePreservationContractJson(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract,
    const std::string &replay_key) {
  std::ostringstream out;
  out << "{";
  interop_semantic_json_detail::
      AppendInteropFfiMetadataInterfacePreservationContractJsonFields(
          out,
          lowering_replay_key,
          preservation_summary,
          contract,
          replay_key);
  out << "}";
  (void)lowering_contract;
  return out.str();
}

}  // namespace objc3::artifacts::frontend
