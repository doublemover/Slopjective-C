#include "tools/objc3c_frontend_c_api_runner_output_contract_corpus_json.h"

#include "tools/objc3c_frontend_c_api_runner_output_contract_corpus_json_internal.h"

void WriteFrontendCApiRunnerOutputContractCorpusJson(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &corpus) {
  WriteFrontendCApiRunnerOutputContractCorpusIdentityJsonRows(out,
                                                              indent,
                                                              corpus);
  WriteFrontendCApiRunnerOutputContractCorpusExpectationJsonRows(out,
                                                                 indent,
                                                                 corpus);
}
