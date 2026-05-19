#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"
#include "runtime/public/objc3_runtime_dispatch_status.h"
#include "runtime/public/objc3_runtime_registration_status.h"

/*
 * Canonical public runtime result/status include point.
 *
 * Header ownership:
 * - objc3_runtime_registration_status.h owns registration status codes.
 * - objc3_runtime_dispatch_status.h owns checked dispatch status codes.
 * - objc3_runtime_dispatch_result.h owns checked i32 and typed dispatch result
 *   payloads.
 *
 * Status values are result classifications, not language capability claims.
 * Construction remains internal to the runtime public contract helpers.
 */
