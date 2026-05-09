#ifndef OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_H_

#include "objc3c_frontend_artifact.h"
#include "objc3c_frontend_context.h"
#include "objc3c_frontend_diagnostic.h"
#include "objc3c_frontend_error.h"
#include "objc3c_frontend_options.h"
#include "objc3c_frontend_result.h"
#include "objc3c_frontend_string.h"
#include "objc3c_frontend_version.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Public embedding ABI contract:
 * - This header aggregates the exported symbol and struct-layout surface for
 *   libobjc3c_frontend.
 * - Callers should gate startup with objc3c_frontend_is_exact_abi_version().
 * - Reserved struct fields must be zero-initialized by callers; they do not
 *   imply a compatibility window.
 * - ABI evolution is hard-cutover: embedders must compile and run against the
 *   exact exposed ABI version.
 *
 * Header ownership:
 * - objc3c_frontend_version.h owns export macros, version values, and ABI gates.
 * - objc3c_frontend_context.h owns the opaque context lifecycle.
 * - objc3c_frontend_options.h owns borrowed compile input/output options.
 * - objc3c_frontend_result.h owns compile result storage and status values.
 * - objc3c_frontend_diagnostic.h owns stage summaries and severity values.
 * - objc3c_frontend_artifact.h owns artifact path selectors.
 * - objc3c_frontend_string.h owns string/view lifetime rules.
 * - objc3c_frontend_error.h owns context error copy semantics.
 * - c_api.h owns the C-only mirror names over this same ABI.
 */

/*
 * Compile entrypoint for file-backed embedding.
 * Pipeline-backed behavior:
 * - Runs lexer/parser/sema/lower/emit through the extracted frontend pipeline.
 * - Writes selected artifacts to out_dir based on explicit emit flags,
 *   out_dir, and emit_prefix options.
 * - Returns OBJC3C_FRONTEND_STATUS_DIAGNOSTICS on source diagnostics,
 *   OBJC3C_FRONTEND_STATUS_EMIT_ERROR on object emission failures,
 *   and OBJC3C_FRONTEND_STATUS_USAGE_ERROR for invalid arguments.
 */
OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_file(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result);

/*
 * Compile entrypoint for in-memory source embedding.
 * Pipeline-backed behavior mirrors objc3c_frontend_compile_file and accepts
 * compile_options.source_text as the source input.
 */
OBJC3C_FRONTEND_API objc3c_frontend_status_t objc3c_frontend_compile_source(
    objc3c_frontend_context_t *context,
    const objc3c_frontend_compile_options_t *options,
    objc3c_frontend_compile_result_t *result);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_OBJC3C_FRONTEND_H_
