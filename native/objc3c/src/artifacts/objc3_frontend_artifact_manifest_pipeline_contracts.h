#pragma once

#include <cstddef>
#include <string>

namespace objc3::artifacts::frontend {

inline constexpr const char *kObjc3ArtifactSimdVectorLaneContract =
    "2,4,8,16";
inline constexpr const char *kObjc3ArtifactSimdVectorTypeLoweringReplayKey =
    "i32x2=<2 x i32>;i32x4=<4 x i32>;i32x8=<8 x i32>;i32x16=<16 x i32>;"
    "boolx2=<2 x i1>;boolx4=<4 x i1>;boolx8=<8 x i1>;boolx16=<16 x i1>;"
    "lane_contract=2,4,8,16";
inline constexpr const char
    *kObjc3ArtifactPropertySynthesisIvarBindingLaneContract =
        "objc3c.property.synthesis.ivar.binding.v1";

struct Objc3FrontendArtifactManifestLoweringHeaderFields {
  std::size_t vector_signature_functions = 0;
  std::string property_synthesis_ivar_binding_replay_key;
  bool property_synthesis_ivar_binding_deterministic = true;
};

}  // namespace objc3::artifacts::frontend
