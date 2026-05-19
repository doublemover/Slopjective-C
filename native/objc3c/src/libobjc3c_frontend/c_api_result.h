#ifndef OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_H_
#define OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_H_

#include "c_api_result_artifacts.h"
#include "c_api_result_error.h"
#include "c_api_result_lifecycle.h"

/*
 * Ownership/accessor surface for C-only embedders.
 * - result_destroy releases only result-owned payload strings, then zeros the
 *   supplied compile result. Passing NULL is a no-op.
 * - owned_result_destroy releases the opaque result handle and its payload.
 * - result_* accessors return borrowed pointers/views valid until result
 *   destruction.
 * - string_release is for standalone owned strings only; do not pass
 *   result-owned strings returned by result_error_message/result_artifact_path.
 * - undefined artifact kinds and NULL inputs fail closed as NULL/empty/0.
 * - NULL results, absent payloads, and undefined artifact kinds never
 *   manufacture retired route values: pointer accessors return NULL, views
 *   return {NULL, 0}, and boolean predicates return 0.
 */
/*
 * Aggregate C-only result surface. Lifetime, artifact selectors, and error
 * payload accessors each have a narrow owner header; embedders may include this
 * header to use the whole C result API.
 */

#endif  // OBJC3C_LIBOBJC3C_FRONTEND_C_API_RESULT_H_
