#pragma once

#include <string>

#include "lower/objc3_lowering_contract.h"

bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract);
bool IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
std::string Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &contract);
