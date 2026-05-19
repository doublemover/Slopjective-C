#pragma once

#include "lower/contracts/block_abi_lowering_lane_contracts.h"

#include <string>

// Executable block ABI summary contracts publish the lowering-owned ABI
// artifact, invoke-thunk, byref-helper, and escape-runtime-hook boundaries.
std::string Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary();
std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary();
std::string Objc3ExecutableBlockByrefHelperLoweringSummary();
std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary();
