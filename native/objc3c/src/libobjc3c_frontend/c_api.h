#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_

#include "c_api_compile.h"
#include "c_api_contract.h"
#include "c_api_lifecycle.h"
#include "c_api_result.h"
#include "c_api_stage_summary.h"
#include "c_api_string.h"
#include "c_api_types.h"
#include "c_api_version.h"

/*
 * C ABI contract for non-C++ embedding environments. This header exposes
 * C-only names over the same objc3c_frontend ABI data structures and primary
 * libobjc3c_frontend entrypoints.
 *
 * Result/string ownership is explicit on this surface:
 * - compile_result storage is caller-owned and must be zero-initialized before
 *   first use.
 * - result payload strings are result-owned and released only by
 *   objc3c_frontend_c_result_destroy().
 * - standalone owned strings are released with
 *   objc3c_frontend_c_string_release().
 * - borrowed option strings/paths must remain valid for the duration of the
 *   call.
 * - compile entrypoints require non-NULL context/options/result pointers.
 *
 * Header ownership:
 * - c_api_types.h owns C-only type aliases over the stable frontend ABI.
 * - c_api_contract.h owns the public C API owner/policy registry.
 * - c_api_version.h owns ABI/version probes.
 * - c_api_lifecycle.h owns context lifecycle entrypoints.
 * - c_api_compile.h owns compile/error entrypoints.
 * - c_api_result.h aggregates the C-only result surface.
 * - c_api_result_lifecycle.h owns C-only result-owned payload destruction.
 * - c_api_result_artifacts.h owns C-only artifact selector accessors.
 * - c_api_result_error.h owns C-only error payload accessors.
 * - c_api_string.h owns standalone string lifetime helpers.
 * - c_api_stage_summary.h owns stage summary predicates.
 *
 * Hard-cutover owner registry:
 * - OBJC3C_FRONTEND_C_API_RESULT_LIFECYCLE_OWNER owns result destruction.
 * - OBJC3C_FRONTEND_C_API_STRING_OWNER owns standalone string release/view.
 * - OBJC3C_FRONTEND_C_API_DIAGNOSTICS_OWNER owns diagnostics artifact access.
 * - OBJC3C_FRONTEND_C_API_ARTIFACT_OWNER owns artifact selectors.
 * - OBJC3C_FRONTEND_C_API_STAGE_SUMMARY_OWNER owns stage summary predicates.
 * - OBJC3C_FRONTEND_C_API_NULL_INVALID_INPUT_OWNER owns fail-closed NULL and
 *   undefined enum behavior.
 * - OBJC3C_FRONTEND_C_API_ABI_VERSION_OWNER owns exact ABI version gating.
 * - OBJC3C_FRONTEND_C_API_PUBLIC_PRIVATE_PARTITION_OWNER owns the rule that
 *   embedders include c_api.h for C package-facing names and never internal
 *   C++ owner headers.
 */

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_H_
