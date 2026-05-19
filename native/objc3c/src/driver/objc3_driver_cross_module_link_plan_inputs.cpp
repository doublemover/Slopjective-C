#include "driver/objc3_driver_cross_module_link_plan_inputs.h"

#include "driver/objc3_driver_cross_module_link_plan_base_inputs.h"
#include "driver/objc3_driver_cross_module_link_plan_expected_contracts.h"
#include "driver/objc3_driver_cross_module_link_plan_local_feature_inputs.h"

Objc3CrossModuleRuntimeLinkPlanArtifactInputs
BuildObjc3DriverCrossModuleRuntimeLinkPlanInputs(
    const Objc3CliOptions &cli_options,
    const Objc3FrontendArtifactBundle &artifacts,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts
        &linker_retention_artifacts,
    const std::filesystem::path &object_out) {
  Objc3CrossModuleRuntimeLinkPlanArtifactInputs link_plan_inputs =
      BuildObjc3DriverCrossModuleRuntimeLinkPlanBaseInputs(
          cli_options, artifacts, linker_retention_artifacts, object_out);
  PopulateObjc3DriverCrossModuleRuntimeLinkPlanLocalFeatureInputs(
      link_plan_inputs, artifacts);
  PopulateObjc3DriverCrossModuleRuntimeLinkPlanExpectedContracts(
      link_plan_inputs, artifacts);
  return link_plan_inputs;
}
