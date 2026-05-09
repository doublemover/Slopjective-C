#pragma once

#include "runtime/public/objc3_runtime_dispatch_result.h"
#include "runtime/public/objc3_runtime_dispatch_status.h"
#include "runtime/public/objc3_runtime_registration_status.h"

/*
 * Public runtime result/status contract.
 *
 * Aggregate header retained for stable public ABI includes.
 *
 * Header ownership:
 * - objc3_runtime_registration_status.h owns registration status codes.
 * - objc3_runtime_dispatch_status.h owns checked dispatch status codes.
 * - objc3_runtime_dispatch_result.h owns checked i32 dispatch result payloads.
 *
 * Status values are result classifications, not language capability claims.
 * The checked dispatch payload always carries the canonical diagnostic
 * code/message pair for non-OK statuses.
 */
