bool IsValidObjc3UnsafePointerExtensionLoweringContract(
    const Objc3UnsafePointerExtensionLoweringContract &contract) {
  if (contract.unsafe_keyword_sites > contract.unsafe_pointer_extension_sites ||
      contract.pointer_arithmetic_sites >
          contract.unsafe_pointer_extension_sites ||
      contract.raw_pointer_type_sites > contract.unsafe_pointer_extension_sites ||
      contract.unsafe_operation_sites > contract.unsafe_pointer_extension_sites ||
      contract.normalized_sites > contract.unsafe_pointer_extension_sites ||
      contract.gate_blocked_sites > contract.unsafe_pointer_extension_sites ||
      contract.contract_violation_sites > contract.unsafe_pointer_extension_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.unsafe_pointer_extension_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3UnsafePointerExtensionLoweringReplayKey(
    const Objc3UnsafePointerExtensionLoweringContract &contract) {
  return std::string("unsafe_pointer_extension_sites=") +
             std::to_string(contract.unsafe_pointer_extension_sites) +
         ";unsafe_keyword_sites=" +
         std::to_string(contract.unsafe_keyword_sites) +
         ";pointer_arithmetic_sites=" +
         std::to_string(contract.pointer_arithmetic_sites) +
         ";raw_pointer_type_sites=" +
         std::to_string(contract.raw_pointer_type_sites) +
         ";unsafe_operation_sites=" +
         std::to_string(contract.unsafe_operation_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3UnsafePointerExtensionLoweringLaneContract;
}

bool IsValidObjc3InlineAsmIntrinsicGovernanceLoweringContract(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract) {
  if (contract.inline_asm_sites > contract.inline_asm_intrinsic_sites ||
      contract.intrinsic_sites > contract.inline_asm_intrinsic_sites ||
      contract.governed_intrinsic_sites > contract.intrinsic_sites ||
      contract.privileged_intrinsic_sites > contract.governed_intrinsic_sites ||
      contract.normalized_sites > contract.inline_asm_intrinsic_sites ||
      contract.gate_blocked_sites > contract.inline_asm_intrinsic_sites ||
      contract.contract_violation_sites > contract.inline_asm_intrinsic_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.inline_asm_intrinsic_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3InlineAsmIntrinsicGovernanceLoweringReplayKey(
    const Objc3InlineAsmIntrinsicGovernanceLoweringContract &contract) {
  return std::string("inline_asm_intrinsic_sites=") +
             std::to_string(contract.inline_asm_intrinsic_sites) +
         ";inline_asm_sites=" + std::to_string(contract.inline_asm_sites) +
         ";intrinsic_sites=" + std::to_string(contract.intrinsic_sites) +
         ";governed_intrinsic_sites=" +
         std::to_string(contract.governed_intrinsic_sites) +
         ";privileged_intrinsic_sites=" +
         std::to_string(contract.privileged_intrinsic_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3InlineAsmIntrinsicGovernanceLoweringLaneContract;
}
