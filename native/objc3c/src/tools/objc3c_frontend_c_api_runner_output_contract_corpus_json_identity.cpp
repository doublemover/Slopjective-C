#include "tools/objc3c_frontend_c_api_runner_output_contract_corpus_json_internal.h"

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerOutputContractCorpusIdentityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &corpus) {
  out << indent << "\"conformance_corpus_key\": \""
      << EscapeJsonString(corpus.conformance_corpus_key) << "\",\n";
  out << indent << "\"conformance_corpus_case_count\": "
      << corpus.conformance_corpus_case_count << ",\n";
  out << indent << "\"conformance_corpus_accept_case_count\": "
      << corpus.conformance_corpus_accept_case_count << ",\n";
  out << indent << "\"conformance_corpus_reject_case_count\": "
      << corpus.conformance_corpus_reject_case_count << ",\n";
}
