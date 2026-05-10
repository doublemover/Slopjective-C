#pragma once

#include <string>

#include "ir/objc3_ir_frontend_metadata_ir_emission_readiness.h"
#include "ir/objc3_ir_frontend_metadata_lowering_readiness.h"

struct Objc3IRFrontendPipelineReadinessMetadata
    : Objc3IRFrontendIREmissionReadinessMetadata,
      Objc3IRFrontendLoweringReadinessMetadata {
  bool ownership_aware_lowering_core_feature_expansion_ready = false;
  std::string ownership_aware_lowering_core_feature_expansion_key;
  bool ownership_aware_lowering_performance_quality_guardrails_ready = false;
  std::string ownership_aware_lowering_performance_quality_guardrails_key;
  bool ownership_aware_lowering_cross_lane_integration_ready = false;
  std::string ownership_aware_lowering_cross_lane_integration_key;
};
