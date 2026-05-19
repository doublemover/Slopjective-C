#pragma once

#include <string>

#include "io/objc3_cli_reporting_output_contract_conformance_corpus_expansion_surface.h"
#include "io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h"

bool BuildFrontendCApiRunnerOutputContractConformanceCorpus(
    const Objc3CliReportingOutputContractConformanceMatrixImplementationSurface
        &conformance_matrix,
    Objc3CliReportingOutputContractConformanceCorpusExpansionSurface
        &conformance_corpus,
    std::string &error);
