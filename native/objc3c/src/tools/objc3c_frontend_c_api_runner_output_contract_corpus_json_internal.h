#pragma once

#include <ostream>
#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h"

void WriteFrontendCApiRunnerOutputContractCorpusIdentityJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &corpus);

void WriteFrontendCApiRunnerOutputContractCorpusExpectationJsonRows(
    std::ostream &out,
    const std::string &indent,
    const Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &corpus);
