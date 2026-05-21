"""Public advanced-runtime capability split contract for #8155."""

from __future__ import annotations

from typing import Any

ADVANCED_RUNTIME_SPLIT_ISSUE = 8155
ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID = "language.advanced-runtime-closure"
ADVANCED_RUNTIME_CAPABILITY_SPLIT_SOURCE = (
    "scripts/objc3c_runtime_acceptance/domains/advanced_runtime_capability_split.py"
)

ADVANCED_RUNTIME_PUBLIC_CAPABILITY_ROWS: tuple[dict[str, str], ...] = (
    {
        "capability_id": "language.blocks.escape-capture-legality",
        "support_claim": "objc3c.behavior.language.blocks.escape-capture-legality",
        "behavior_fixture": "tests/tooling/fixtures/native/capture_list_and_retainable_family_legality_completion_positive.objc3",
    },
    {
        "capability_id": "runtime.blocks.copy-dispose-invoke",
        "support_claim": "objc3c.behavior.runtime.blocks.copy-dispose-invoke",
        "behavior_fixture": "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
    },
    {
        "capability_id": "runtime.blocks.byref-forwarding",
        "support_claim": "objc3c.behavior.runtime.blocks.byref-forwarding",
        "behavior_fixture": "tests/tooling/fixtures/native/byref_cell_copy_dispose_runtime_positive.objc3",
    },
    {
        "capability_id": "language.arc-cleanup.integration",
        "support_claim": "objc3c.behavior.arc-cleanup.integration",
        "behavior_fixture": "tests/tooling/fixtures/native/arc_cleanup_source_construct_order_positive.objc3",
    },
    {
        "capability_id": "runtime.errors.live-bridge-cleanup",
        "support_claim": "objc3c.behavior.runtime.error-live-bridge-cleanup",
        "behavior_fixture": "tests/tooling/fixtures/native/live_error_runtime_integration_positive.objc3",
    },
    {
        "capability_id": "runtime.concurrency.task-continuation-lifecycle",
        "support_claim": "objc3c.behavior.runtime.concurrency-task-continuation-lifecycle",
        "behavior_fixture": "tests/tooling/fixtures/native/live_continuation_runtime_integration_positive.objc3",
    },
    {
        "capability_id": "runtime.concurrency.actor-mailbox-isolation",
        "support_claim": "objc3c.behavior.runtime.concurrency-actor-mailbox-isolation",
        "behavior_fixture": "tests/tooling/fixtures/native/live_actor_mailbox_runtime_positive.objc3",
    },
    {
        "capability_id": "language.metaprogramming.property-behavior-semantics",
        "support_claim": "objc3c.behavior.language.metaprogramming.property-behavior-semantics",
        "behavior_fixture": "tests/tooling/fixtures/native/property_behavior_legality_positive.objc3",
    },
    {
        "capability_id": "language.metaprogramming.derive-expansion-inventory",
        "support_claim": "objc3c.behavior.language.metaprogramming.derive-expansion-inventory",
        "behavior_fixture": "tests/tooling/fixtures/native/derive_expansion_inventory_positive.objc3",
    },
    {
        "capability_id": "language.metaprogramming.macro-safety-sandbox-determinism",
        "support_claim": "objc3c.behavior.language.metaprogramming.macro-safety-sandbox-determinism",
        "behavior_fixture": "tests/tooling/fixtures/native/macro_safety_sandbox_positive.objc3",
    },
    {
        "capability_id": "runtime.metaprogramming.host-cache-boundary",
        "support_claim": "objc3c.behavior.runtime.metaprogramming.host-cache-boundary",
        "behavior_fixture": "tests/tooling/fixtures/native/expansion_host_runtime_boundary_positive.objc3",
    },
    {
        "capability_id": "runtime.interop.package-loader-bridge",
        "support_claim": "objc3c.behavior.runtime.interop.package-loader-bridge",
        "behavior_fixture": "tests/tooling/fixtures/native/bridge_packaging_toolchain_consumer.objc3",
    },
    {
        "capability_id": "runtime.interop.mixed-image-replay",
        "support_claim": "objc3c.behavior.runtime.interop.mixed-image-replay",
        "behavior_fixture": "tests/tooling/fixtures/native/runtime_packaging_consumer.objc3",
    },
)

ADVANCED_RUNTIME_RESERVED_BOUNDARIES: tuple[dict[str, str], ...] = (
    {
        "boundary_id": "runtime.blocks.full-language-closure",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Only explicit capture legality, copy/dispose/invoke, and byref forwarding rows are public; broader block language closure remains unavailable.",
    },
    {
        "boundary_id": "runtime.arc.full-automation",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "ARC cleanup integration is public only for the checked helper-backed rows; broad ARC ownership automation is not claimed.",
    },
    {
        "boundary_id": "runtime.errors.generalized-foreign-exception-abi",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "NSError/status bridge cleanup has executable evidence; generalized foreign exception ABI support remains unclaimed.",
    },
    {
        "boundary_id": "runtime.concurrency.broad-async-actor-closure",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Task continuation and actor mailbox rows are public; scheduler fairness, distributed actors, and broad async ABI closure remain reserved.",
    },
    {
        "boundary_id": "runtime.metaprogramming.arbitrary-macro-ecosystem",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Checked derive inventory, property behavior semantics, and host/cache boundaries are public; arbitrary third-party macro expansion remains unavailable.",
    },
    {
        "boundary_id": "runtime.interop.broad-runtime-closure",
        "public_status": "reserved",
        "matrix_owner": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "reason": "Package-loader bridge and mixed-image replay rows are public; broad interop closure and public ABI widening remain reserved.",
    },
)


def build_advanced_runtime_capability_split_contract() -> dict[str, Any]:
    return {
        "issue": ADVANCED_RUNTIME_SPLIT_ISSUE,
        "reserved_umbrella": ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID,
        "source": ADVANCED_RUNTIME_CAPABILITY_SPLIT_SOURCE,
        "implemented_rows": list(ADVANCED_RUNTIME_PUBLIC_CAPABILITY_ROWS),
        "reserved_boundaries": list(ADVANCED_RUNTIME_RESERVED_BOUNDARIES),
    }


__all__ = [
    "ADVANCED_RUNTIME_CAPABILITY_SPLIT_SOURCE",
    "ADVANCED_RUNTIME_PUBLIC_CAPABILITY_ROWS",
    "ADVANCED_RUNTIME_RESERVED_BOUNDARIES",
    "ADVANCED_RUNTIME_RESERVED_UMBRELLA_ID",
    "ADVANCED_RUNTIME_SPLIT_ISSUE",
    "build_advanced_runtime_capability_split_contract",
]
