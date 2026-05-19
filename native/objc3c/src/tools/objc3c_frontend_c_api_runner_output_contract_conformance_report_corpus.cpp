#include "tools/objc3c_frontend_c_api_runner_output_contract_conformance_report_corpus.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool BuildFrontendCApiRunnerOutputContractConformanceCorpus(
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &conformance_matrix,
    Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &conformance_corpus,
    std::string &error) {
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

  return true;
}
