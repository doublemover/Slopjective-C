#pragma once

#include "runtime/public/objc3_runtime_result.h"
#include "runtime/public/objc3_runtime_result_code_contract.h"
#include "runtime/public/objc3_runtime_result_materialization_contract.h"
#include "runtime/public/objc3_runtime_result_message_contract.h"
#include "runtime/public/objc3_runtime_result_status_contract.h"

namespace objc3c::runtime {

/*
 * Aggregate contract header for internal runtime public-ABI result helpers.
 * These helpers are not exported C entrypoints; objc3_runtime_result.h remains
 * the C layout owner.
 *
 * Header ownership:
 * - objc3_runtime_result_code_contract.h owns diagnostic code access.
 * - objc3_runtime_result_message_contract.h owns diagnostic message access.
 * - objc3_runtime_result_status_contract.h owns checked-result status access.
 * - objc3_runtime_result_materialization_contract.h owns checked-result
 *   materialization.
 */

}  // namespace objc3c::runtime
