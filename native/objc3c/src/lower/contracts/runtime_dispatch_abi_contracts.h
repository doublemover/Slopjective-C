#pragma once

#include <cstddef>
#include <string>

#include "lower/contracts/runtime_dispatch_boundary_contracts.h"
#include "lower/contracts/runtime_dispatch_strict_abi_lowering_contracts.h"

inline constexpr const char *kObjc3RuntimeLinkHostLinkLaneContract =
    "objc3c.runtime.dispatch.host.link.v1";

struct Objc3RuntimeLinkHostLinkContract {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RuntimeDispatchLoweringAbiContract {
  std::size_t message_send_sites = 0;
  std::size_t fixed_argument_slot_count = kObjc3RuntimeDispatchDefaultArgs;
  std::size_t runtime_dispatch_parameter_count = 0;
  std::string lowering_boundary_model =
      kObjc3RuntimeDispatchLoweringAbiBoundaryModel;
  std::string canonical_runtime_dispatch_symbol =
      kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;
  std::string default_lowering_target_symbol = kObjc3RuntimeDispatchSymbol;
  std::string selector_lookup_symbol =
      kObjc3RuntimeDispatchLoweringSelectorLookupSymbol;
  std::string selector_handle_type =
      kObjc3RuntimeDispatchLoweringSelectorHandleType;
  std::string receiver_abi_type = kObjc3RuntimeDispatchLoweringReceiverAbiType;
  std::string selector_abi_type = kObjc3RuntimeDispatchLoweringSelectorAbiType;
  std::string argument_abi_type = kObjc3RuntimeDispatchLoweringArgumentAbiType;
  std::string result_abi_type = kObjc3RuntimeDispatchLoweringResultAbiType;
  std::string selector_operand_model =
      kObjc3RuntimeDispatchLoweringSelectorOperandModel;
  std::string selector_handle_model =
      kObjc3RuntimeDispatchLoweringSelectorHandleModel;
  std::string argument_padding_model =
      kObjc3RuntimeDispatchLoweringArgumentPaddingModel;
  std::string default_lowering_target_model =
      kObjc3RuntimeDispatchLoweringDefaultTargetModel;
  std::string strict_dispatch_error_model =
      kObjc3RuntimeDispatchLoweringStrictDispatchErrorModel;
  std::string deferred_cases_model =
      kObjc3RuntimeDispatchLoweringDeferredCasesModel;
  bool fail_closed = true;
  bool deterministic = true;
};

bool IsValidObjc3RuntimeLinkHostLinkContract(
    const Objc3RuntimeLinkHostLinkContract &contract);
std::string Objc3RuntimeLinkHostLinkReplayKey(
    const Objc3RuntimeLinkHostLinkContract &contract);
bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
