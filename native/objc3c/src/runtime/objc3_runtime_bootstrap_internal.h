#pragma once

// Private runtime contract aggregate. Owned declarations live in the
// subsystem headers below so bootstrap no longer owns unrelated ABI surfaces.
#include "runtime/classes/runtime_object_snapshot_contracts.h"
#include "runtime/concurrency/runtime_concurrency_snapshot_contracts.h"
#include "runtime/dispatch/dispatch_snapshot_contracts.h"
#include "runtime/errors/error_bridge_snapshot_contracts.h"
#include "runtime/memory/runtime_ownership_snapshot_contracts.h"
#include "runtime/metadata/runtime_artifact_snapshot_contracts.h"
#include "runtime/public/objc3_runtime_api.h"
#include "runtime/selectors/runtime_selector_snapshot_contracts.h"
#include "runtime/state/runtime_bootstrap_contracts.h"
