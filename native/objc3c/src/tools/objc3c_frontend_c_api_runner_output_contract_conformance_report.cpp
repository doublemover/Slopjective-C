#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report_corpus.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report_matrix.h"

bool BuildFrontendCApiRunnerOutputContractConformanceReport(
    const FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    FrontendCApiRunnerOutputContract &output_contract,
    std::string &error) {
  Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
      conformance_matrix;
  if (!BuildFrontendCApiRunnerOutputContractConformanceMatrix(
          hardening_surfaces,
          conformance_matrix,
          error)) {
    return false;
  }

  Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
      conformance_corpus;
  if (!BuildFrontendCApiRunnerOutputContractConformanceCorpus(
          conformance_matrix,
          conformance_corpus,
          error)) {
    return false;
  }

  output_contract.conformance_matrix_surface = conformance_matrix;
  output_contract.conformance_corpus_surface = conformance_corpus;
  error.clear();
  return true;
}
