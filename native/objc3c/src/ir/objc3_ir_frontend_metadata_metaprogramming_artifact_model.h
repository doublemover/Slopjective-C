#pragma once

#include "ir/objc3_ir_frontend_metadata_metaprogramming_derived_method_model.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_macro_artifact_model.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_property_behavior_model.h"

struct Objc3IRFrontendMetaprogrammingArtifactModelMetadata
    : Objc3IRFrontendMetaprogrammingDerivedMethodModelMetadata,
      Objc3IRFrontendMetaprogrammingMacroArtifactModelMetadata,
      Objc3IRFrontendMetaprogrammingPropertyBehaviorModelMetadata {};
