from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from stdlib_surface.semantics import validate_semantic_policy


def _base_semantic_policy(capability_query_encoding: str) -> dict[str, object]:
    return {
        "module_semver": {
            "objc3.core": {"major": 1, "minor": 0, "patch": 0},
        },
        "core_semantics": {
            "runtime_core_abi": (
                "objc3.core helpers route through objc3_runtime_stdlib_core_* "
                "entrypoints and publish runtime-owned call evidence through "
                "objc3_runtime_copy_stdlib_core_state_for_testing"
            ),
            "capability_query_encoding": capability_query_encoding,
            "string_view_length": (
                "routes through objc3_runtime_stdlib_core_count_i32 and applies "
                "a zero floor to negative unit counts"
            ),
            "string_view_prefix_units": (
                "routes through objc3_runtime_stdlib_core_prefix_count_i32 and "
                "clamps both count and requested units to a zero floor"
            ),
            "array_prefix_count": (
                "routes through objc3_runtime_stdlib_core_prefix_count_i32 and "
                "clamps both count and requested elements to a zero floor"
            ),
        },
        "error_semantics": {
            "result_ok_tag": 1,
            "result_err_tag": 2,
            "option_result_shape_diagnostic": (
                "returns 0 when option presence matches the provided result tag, "
                "otherwise 30601"
            ),
            "text_data_shape_diagnostic": (
                "returns 0 when unit_count equals byte_count, otherwise 30602"
            ),
        },
        "keypath_semantics": {
            "text_shape_diagnostic": (
                "returns 0 when text_units is at least component_count, otherwise "
                "30603"
            ),
            "typed_keypath_metadata": (
                "preserves root and component counts as the current metadata token "
                "shape"
            ),
            "reflection_interop": (
                "must preserve the caller-visible keypath component count and "
                "diagnostic behavior across module boundaries"
            ),
            "runtime_composition_shape": (
                "must preserve explicit component counts without inventing "
                "ownership or allocation semantics"
            ),
        },
        "concurrency_semantics": {
            "spawn_token": (
                "routes structured spawn through objc3_runtime_spawn_task_i32 "
                "with task kind 1 and the caller-provided executor tag"
            ),
            "cancellation_checkpoint": (
                "routes cancellation checkpoints through "
                "objc3_runtime_cancel_task_group_i32 when flagged and "
                "objc3_runtime_task_is_cancelled_i32 otherwise"
            ),
            "public_task_spawn_api": (
                "objc3_task_spawn, objc3_task_spawn_child, and "
                "objc3_task_spawn_detached are stable public aliases over the "
                "structured and detached runtime task spawn helpers"
            ),
            "public_task_join_api": (
                "objc3_task_join routes non-cancelled public task joins through "
                "objc3_concurrency_join_status and preserves explicit result "
                "and executor-tag operands"
            ),
            "public_task_group_api": (
                "objc3_task_group_run_two enters a two-child task-group scope "
                "and objc3_task_group_run_bounded accepts a caller-provided "
                "child count; both drain through "
                "objc3_concurrency_task_group_scope_depth and negative counts "
                "fail closed through cancellation"
            ),
            "public_cancellation_api": (
                "objc3_task_is_cancelled and objc3_task_cancel_if_needed expose "
                "cancellation observation and cancellation checkpoints without "
                "widening scheduler policy claims"
            ),
            "public_executor_api": (
                "objc3_executor_hop is the public executor-hop entrypoint over "
                "objc3_concurrency_executor_hop_token with distinct value and "
                "executor-tag operands"
            ),
            "public_actor_mailbox_api": (
                "objc3_actor_mailbox_send_and_drain binds, enqueues, and drains "
                "a single actor mailbox message through "
                "objc3_concurrency_actor_mailbox_token"
            ),
            "family_growth_rule": (
                "structured-child-spawn detached-spawn join-and-wait "
                "task-group-scope cancellation-observation executor-hop "
                "actor-mailbox public-task-spawn public-task-join "
                "public-task-group public-cancellation public-executor-hop and "
                "public-actor-mailbox helpers may grow additively inside "
                "objc3.concurrency"
            ),
            "layering_rule": (
                "objc3.concurrency may depend only on objc3.core and objc3.errors "
                "within stdlib major version 1"
            ),
        },
        "system_semantics": {
            "resource_token": (
                "returns seed plus 4 as the canonical deterministic strict-system "
                "resource token"
            ),
            "profile_gate": (
                "objc3.system helpers remain gated to Strict System and must not "
                "become unconditional Core imports"
            ),
            "runtime_composition_hook": (
                "strict-system runtime-composition helpers must be explicit "
                "profile-gated entrypoints rather than implicit side effects in "
                "core helpers"
            ),
            "layering_rule": (
                "objc3.system may depend on objc3.core objc3.errors "
                "objc3.concurrency and objc3.keypath but those modules may not "
                "depend on objc3.system"
            ),
        },
    }


def test_semantic_policy_accepts_fail_closed_capability_table() -> None:
    error, validation = validate_semantic_policy(
        semantic_policy=_base_semantic_policy(
            "known Core-profile stdlib capability ordinals 1 through 4 are "
            "present; strict-system ordinal 5, zero, negative, and unknown "
            "positive ids fail closed as missing"
        ),
        inventory_module_names={"objc3.core"},
    )

    assert error is None
    assert validation is not None


def test_semantic_policy_rejects_positive_integer_capability_fail_open() -> None:
    error, validation = validate_semantic_policy(
        semantic_policy=_base_semantic_policy(
            "positive capability ids mean present; zero and negative ids fail "
            "closed as missing"
        ),
        inventory_module_names={"objc3.core"},
    )

    assert error == "semantic policy capability_query_encoding drifted"
    assert validation is None
