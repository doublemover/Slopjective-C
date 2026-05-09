#pragma once

#include <string>

inline constexpr const char *kObjc3SimdVectorLaneContract = "2,4,8,16";
inline constexpr const char *kObjc3SimdVectorBaseI32 = "i32";
inline constexpr const char *kObjc3SimdVectorBaseBool = "bool";

bool IsSupportedObjc3SimdVectorLaneCount(unsigned lane_count);
bool TryBuildObjc3SimdVectorLLVMType(const std::string &base_spelling,
                                     unsigned lane_count,
                                     std::string &llvm_type);
std::string Objc3SimdVectorTypeLoweringReplayKey();
