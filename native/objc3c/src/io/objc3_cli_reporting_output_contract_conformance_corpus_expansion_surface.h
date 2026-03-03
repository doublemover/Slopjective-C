#pragma once

#include <cstddef>
#include <sstream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"

inline constexpr std::size_t
    kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseCount = 1u;
inline constexpr std::size_t
    kObjc3CliReportingOutputContractConformanceCorpusRejectCaseCount = 1u;
inline constexpr const char
    *kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseId =
        "M243-D010-C001";
inline constexpr const char
    *kObjc3CliReportingOutputContractConformanceCorpusRejectCaseId =
        "M243-D010-R001";

struct Objc3CliReportingOutputContractConformanceCorpusExpansionSurface {
  bool summary_output_path_contract_consistent = false;
  bool diagnostics_output_path_contract_consistent = false;
  bool diagnostics_filename_matches_emit_prefix = false;
  bool conformance_matrix_consistent = false;
  bool conformance_matrix_ready = false;
  bool conformance_matrix_key_ready = false;
  bool conformance_corpus_consistent = false;
  bool conformance_corpus_ready = false;
  bool conformance_corpus_key_ready = false;
  bool core_feature_impl_ready = false;
  std::size_t conformance_corpus_case_count = 0u;
  std::size_t conformance_corpus_accept_case_count = 0u;
  std::size_t conformance_corpus_reject_case_count = 0u;
  std::string conformance_matrix_key;
  std::string conformance_corpus_key;
  std::string recovery_determinism_key;
  std::string summary_output_path;
  std::string diagnostics_output_path;
  std::string failure_reason;
};

inline std::string BuildObjc3CliReportingOutputContractConformanceCorpusExpansionKey(
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &surface) {
  std::ostringstream key;
  key << "cli-reporting-output-contract-conformance-corpus-expansion:v1:"
      << "conformance-matrix-consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance-corpus-consistent="
      << (surface.conformance_corpus_consistent ? "true" : "false")
      << ";conformance-corpus-ready="
      << (surface.conformance_corpus_ready ? "true" : "false")
      << ";conformance-corpus-key-ready="
      << (surface.conformance_corpus_key_ready ? "true" : "false")
      << ";conformance-corpus-case-count=" << surface.conformance_corpus_case_count
      << ";conformance-corpus-accept-case-count="
      << surface.conformance_corpus_accept_case_count
      << ";conformance-corpus-reject-case-count="
      << surface.conformance_corpus_reject_case_count
      << ";accept-case-id="
      << kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseId
      << ";reject-case-id="
      << kObjc3CliReportingOutputContractConformanceCorpusRejectCaseId
      << ";summary_output_path=" << surface.summary_output_path
      << ";diagnostics_output_path=" << surface.diagnostics_output_path
      << ";conformance_matrix_key=" << surface.conformance_matrix_key;
  return key.str();
}

inline Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &conformance_matrix_surface) {
  Objc3CliReportingOutputContractConformanceCorpusExpansionSurface surface;
  surface.summary_output_path_contract_consistent =
      conformance_matrix_surface.summary_output_path_contract_consistent;
  surface.diagnostics_output_path_contract_consistent =
      conformance_matrix_surface.diagnostics_output_path_contract_consistent;
  surface.diagnostics_filename_matches_emit_prefix =
      conformance_matrix_surface.diagnostics_filename_matches_emit_prefix;
  surface.conformance_matrix_consistent =
      conformance_matrix_surface.conformance_matrix_consistent;
  surface.conformance_matrix_ready =
      conformance_matrix_surface.conformance_matrix_ready;
  surface.conformance_matrix_key_ready =
      conformance_matrix_surface.conformance_matrix_key_ready;
  surface.core_feature_impl_ready = conformance_matrix_surface.core_feature_impl_ready;
  surface.conformance_matrix_key = conformance_matrix_surface.conformance_matrix_key;
  surface.recovery_determinism_key =
      conformance_matrix_surface.recovery_determinism_key;
  surface.summary_output_path = conformance_matrix_surface.summary_output_path;
  surface.diagnostics_output_path =
      conformance_matrix_surface.diagnostics_output_path;

  surface.conformance_corpus_accept_case_count =
      kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseCount;
  surface.conformance_corpus_reject_case_count =
      kObjc3CliReportingOutputContractConformanceCorpusRejectCaseCount;
  surface.conformance_corpus_case_count =
      surface.conformance_corpus_accept_case_count +
      surface.conformance_corpus_reject_case_count;

  surface.conformance_corpus_consistent =
      surface.conformance_matrix_consistent &&
      surface.conformance_matrix_key_ready &&
      surface.summary_output_path_contract_consistent &&
      surface.diagnostics_output_path_contract_consistent &&
      surface.diagnostics_filename_matches_emit_prefix &&
      surface.conformance_corpus_case_count ==
          (kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseCount +
           kObjc3CliReportingOutputContractConformanceCorpusRejectCaseCount) &&
      !surface.recovery_determinism_key.empty();
  surface.conformance_corpus_ready =
      surface.conformance_corpus_consistent &&
      surface.conformance_matrix_ready &&
      !surface.summary_output_path.empty() &&
      !surface.diagnostics_output_path.empty();
  surface.conformance_corpus_key =
      BuildObjc3CliReportingOutputContractConformanceCorpusExpansionKey(surface);
  surface.conformance_corpus_key_ready =
      surface.conformance_corpus_ready &&
      !surface.conformance_corpus_key.empty() &&
      surface.conformance_corpus_key.find(
          ";accept-case-id=" +
          std::string(
              kObjc3CliReportingOutputContractConformanceCorpusAcceptCaseId)) !=
          std::string::npos &&
      surface.conformance_corpus_key.find(
          ";reject-case-id=" +
          std::string(
              kObjc3CliReportingOutputContractConformanceCorpusRejectCaseId)) !=
          std::string::npos &&
      surface.conformance_corpus_key.find(
          ";conformance_matrix_key=" + surface.conformance_matrix_key) !=
          std::string::npos;
  surface.core_feature_impl_ready =
      surface.core_feature_impl_ready && surface.conformance_corpus_ready &&
      surface.conformance_corpus_key_ready;

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!conformance_matrix_surface.core_feature_impl_ready) {
    surface.failure_reason = conformance_matrix_surface.failure_reason.empty()
                                 ? "cli/reporting output conformance matrix implementation surface is not ready"
                                 : conformance_matrix_surface.failure_reason;
  } else if (!surface.conformance_corpus_consistent) {
    surface.failure_reason =
        "cli/reporting output conformance corpus expansion is inconsistent";
  } else if (!surface.conformance_corpus_ready) {
    surface.failure_reason =
        "cli/reporting output conformance corpus expansion is not ready";
  } else if (!surface.conformance_corpus_key_ready) {
    surface.failure_reason =
        "cli/reporting output conformance corpus expansion key is not ready";
  } else {
    surface.failure_reason =
        "cli/reporting output conformance corpus expansion surface is not ready";
  }

  return surface;
}

inline bool IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "cli/reporting output conformance corpus expansion surface not ready"
               : surface.failure_reason;
  return false;
}
