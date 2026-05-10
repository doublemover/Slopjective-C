#include "tools/objc3c_frontend_c_api_runner_output_contract_corpus_json_internal.h"

void WriteFrontendCApiRunnerOutputContractCorpusExpectationJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &corpus) {
  out << indent << "\"conformance_corpus_consistent\": "
      << (corpus.conformance_corpus_consistent ? "true" : "false") << ",\n";
  out << indent << "\"conformance_corpus_ready\": "
      << (corpus.conformance_corpus_ready ? "true" : "false") << ",\n";
  out << indent << "\"conformance_corpus_key_ready\": "
      << (corpus.conformance_corpus_key_ready ? "true" : "false") << ",\n";
  out << indent << "\"core_feature_impl_ready\": "
      << (corpus.core_feature_impl_ready ? "true" : "false") << "\n";
}
