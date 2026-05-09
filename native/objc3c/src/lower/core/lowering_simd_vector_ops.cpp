#include "lower/core/lowering_simd_vector_ops.h"

namespace {

bool IsSupportedVectorBaseSpelling(const std::string &base_spelling) {
  return base_spelling == kObjc3SimdVectorBaseI32 ||
         base_spelling == kObjc3SimdVectorBaseBool;
}

std::string VectorTypeSpelling(const std::string &base_spelling,
                               unsigned lane_count) {
  return base_spelling + "x" + std::to_string(lane_count);
}

}  // namespace

bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count) {
  return lane_count == 2u || lane_count == 4u || lane_count == 8u ||
         lane_count == 16u;
}

bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling,
                                     unsigned lane_count,
                                     std::string &llvm_type) {
  if (!IsSupportedVectorBaseSpelling(base_spelling) ||
      !IsSupportedObjc3SimdVectorLaneCount(lane_count)) {
    // Unknown vector shapes are rejected instead of lowered permissively.
    return false;
  }

  llvm_type = "<" + std::to_string(lane_count) + " x " +
              (base_spelling == kObjc3SimdVectorBaseBool ? "i1" : "i32") +
              ">";
  return true;
}

std::string Objc3SimdVectorTypeLoweringReplayKey() {
  std::string replay_key;
  const std::string base_spellings[2] = {kObjc3SimdVectorBaseI32,
                                         kObjc3SimdVectorBaseBool};
  const unsigned lane_counts[4] = {2u, 4u, 8u, 16u};
  bool first = true;

  for (const std::string &base_spelling : base_spellings) {
    for (const unsigned lane_count : lane_counts) {
      std::string llvm_type;
      if (!TryBuildObjc3SimdVectorLLVMType(base_spelling, lane_count,
                                           llvm_type)) {
        continue;
      }
      if (!first) {
        replay_key += ";";
      }
      replay_key += VectorTypeSpelling(base_spelling, lane_count) + "=" +
                    llvm_type;
      first = false;
    }
  }

  replay_key += ";lane_contract=";
  replay_key += kObjc3SimdVectorLaneContract;
  return replay_key;
}
