#pragma once

#include "runtime/public/objc3_runtime_artifact_contract.h"
#include "runtime/public/objc3_runtime_diagnostic_contract.h"
#include "runtime/public/objc3_runtime_language_semantics.h"
#include "runtime/public/objc3_runtime_ownership_contract.h"
#include "runtime/public/objc3_runtime_reflection.h"
#include "runtime/public/objc3_runtime_result_entrypoint_contract.h"
#include "runtime/public/objc3_runtime_string_contract.h"
#include "runtime/stdlib/collections_runtime_contract.h"
#include "runtime/stdlib/core_runtime_contract.h"
#include "runtime/stdlib/text_runtime_contract.h"

/*
 * Umbrella header for the exported runtime C ABI.
 *
 * Header ownership:
 * - objc3_runtime_artifact_contract.h owns image registration and registration
 *   snapshot artifact entrypoints.
 * - objc3_runtime_diagnostic_contract.h owns checked dispatch diagnostics.
 * - objc3_runtime_language_semantics.h owns public language-semantics runtime
 *   evidence snapshots.
 * - objc3_runtime_ownership_contract.h owns public runtime state ownership
 *   reset.
 * - objc3_runtime_reflection.h owns public runtime introspection snapshots.
 * - objc3_runtime_result_entrypoint_contract.h owns narrow value dispatch.
 * - objc3_runtime_diagnostic_contract.h owns checked i32 and typed dispatch.
 * - objc3_runtime_string_contract.h owns borrowed selector string lookup.
 * - runtime/stdlib contract headers own runtime-backed stdlib helper ABI.
 * - Focused layout/status headers own the caller-visible structs and enums.
 */
