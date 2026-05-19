#include "driver/objc3_driver_runtime_registration_manifest_inputs.h"

#include "driver/objc3_driver_runtime_bootstrap_manifest_inputs.h"
#include "driver/objc3_driver_runtime_registration_manifest_summary_inputs.h"

Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs
BuildObjc3DriverRuntimeRegistrationManifestInputs(
    const Objc3FrontendArtifactBundle &artifacts,
    const std::filesystem::path &object_out,
    const std::filesystem::path &backend_out) {
  Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs inputs;
  PopulateObjc3DriverRuntimeRegistrationManifestSummaryInputs(inputs,
                                                              artifacts);
  PopulateObjc3DriverRuntimeRegistrationDescriptorSourceInputs(inputs,
                                                               artifacts);
  PopulateObjc3DriverRuntimeBootstrapSemanticsInputs(inputs, artifacts);
  PopulateObjc3DriverRuntimeBootstrapApiInputs(inputs, artifacts);
  PopulateObjc3DriverRuntimeBootstrapRegistrarInputs(inputs);
  PopulateObjc3DriverRuntimeBootstrapResetInputs(inputs);
  PopulateObjc3DriverRuntimeBootstrapLoweringInputs(inputs, artifacts);
  PopulateObjc3DriverRuntimeRegistrationOutputArtifactInputs(
      inputs, artifacts, object_out, backend_out);
  return inputs;
}
