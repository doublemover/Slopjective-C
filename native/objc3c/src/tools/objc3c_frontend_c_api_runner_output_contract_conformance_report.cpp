#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report.h"

#include "io/objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h"
#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractConformanceReport(
    const FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error) {
  const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
      conformance_matrix =
          BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(
              hardening_surfaces.recovery_determinism);
  std::string conformance_matrix_reason;
  if (!IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(
          conformance_matrix,
          conformance_matrix_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "conformance matrix implementation", conformance_matrix_reason, error);
  }

  const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
      conformance_corpus =
          BuildObjc3CliReportingOutputContractConformanceCorpusExpansionSurface(
              conformance_matrix);
  std::string conformance_corpus_reason;
  if (!IsObjc3CliReportingOutputContractConformanceCorpusExpansionSurfaceReady(
          conformance_corpus,
          conformance_corpus_reason)) {
    return FailFrontendCApiRunnerOutputContract(
        "conformance corpus expansion", conformance_corpus_reason, error);
  }

  output_contract.conformance_matrix_surface = conformance_matrix;
  output_contract.conformance_corpus_surface = conformance_corpus;
  error.clear();
  return true;
}
