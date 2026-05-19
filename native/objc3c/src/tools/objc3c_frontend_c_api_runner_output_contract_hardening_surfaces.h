#pragma once

#include <string>

#include "io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_consistency_surface.h"
#include "io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h"
#include "io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract_core.h"

struct FrontendCApiRunnerOutputContractHardeningSurfaces {
  Objc3CliReportingOutputContractEdgeCaseConsistencySurface
      edge_case_consistency;
  Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface
      edge_case_robustness;
  Objc3CliReportingOutputContractDiagnosticsHardeningSurface
      diagnostics_hardening;
  Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface
      recovery_determinism;
};

bool BuildFrontendCApiRunnerOutputContractHardeningSurfaceChain(
    const FrontendCApiRunnerOutputContractCoreSurfaces &core_surfaces,
    FrontendCApiRunnerOutputContractHardeningSurfaces &hardening_surfaces,
    std::string &error);
