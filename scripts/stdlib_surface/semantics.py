from __future__ import annotations

from dataclasses import dataclass
from typing import Any


@dataclass(frozen=True)
class SemanticPolicyValidation:
    module_semver: dict[str, Any]


def validate_semantic_policy(
    *,
    semantic_policy: dict[str, Any],
    inventory_module_names: set[str],
) -> tuple[str | None, SemanticPolicyValidation | None]:
    semantic_module_semver = semantic_policy.get("module_semver")
    if not isinstance(semantic_module_semver, dict) or not semantic_module_semver:
        return "semantic policy missing module_semver", None
    for module_name, version_payload in semantic_module_semver.items():
        if module_name not in inventory_module_names:
            return f"semantic policy referenced unknown module {module_name}", None
        if not isinstance(version_payload, dict):
            return f"semantic policy module_semver malformed for {module_name}", None
        if version_payload != {"major": 1, "minor": 0, "patch": 0}:
            return f"semantic policy module_semver drifted for {module_name}", None

    core_semantics = semantic_policy.get("core_semantics")
    if not isinstance(core_semantics, dict):
        return "semantic policy missing core_semantics", None
    if core_semantics.get("runtime_core_abi") != (
        "objc3.core helpers route through objc3_runtime_stdlib_core_* entrypoints and publish runtime-owned call evidence through objc3_runtime_copy_stdlib_core_state_for_testing"
    ):
        return "semantic policy runtime_core_abi drifted", None
    if core_semantics.get("capability_query_encoding") != (
        "known Core-profile stdlib capability ordinals 1 through 4 are present; strict-system ordinal 5, zero, negative, and unknown positive ids fail closed as missing"
    ):
        return "semantic policy capability_query_encoding drifted", None
    if core_semantics.get("string_view_length") != (
        "routes through objc3_runtime_stdlib_core_count_i32 and applies a zero floor to negative unit counts"
    ):
        return "semantic policy string_view_length drifted", None
    if core_semantics.get("string_view_prefix_units") != (
        "routes through objc3_runtime_stdlib_core_prefix_count_i32 and clamps both count and requested units to a zero floor"
    ):
        return "semantic policy string_view_prefix_units drifted", None
    if core_semantics.get("array_prefix_count") != (
        "routes through objc3_runtime_stdlib_core_prefix_count_i32 and clamps both count and requested elements to a zero floor"
    ):
        return "semantic policy array_prefix_count drifted", None

    error_semantics = semantic_policy.get("error_semantics")
    if not isinstance(error_semantics, dict):
        return "semantic policy missing error_semantics", None
    if error_semantics.get("result_ok_tag") != 1 or error_semantics.get("result_err_tag") != 2:
        return "semantic policy result tag values drifted", None
    if error_semantics.get("option_result_shape_diagnostic") != (
        "returns 0 when option presence matches the provided result tag, otherwise 30601"
    ):
        return "semantic policy option_result_shape_diagnostic drifted", None
    if error_semantics.get("text_data_shape_diagnostic") != (
        "returns 0 when unit_count equals byte_count, otherwise 30602"
    ):
        return "semantic policy text_data_shape_diagnostic drifted", None

    keypath_semantics = semantic_policy.get("keypath_semantics")
    if not isinstance(keypath_semantics, dict):
        return "semantic policy missing keypath_semantics", None
    if keypath_semantics.get("text_shape_diagnostic") != (
        "returns 0 when text_units is at least component_count, otherwise 30603"
    ):
        return "semantic policy text_shape_diagnostic drifted", None
    if keypath_semantics.get("typed_keypath_metadata") != (
        "preserves root and component counts as the current metadata token shape"
    ):
        return "semantic policy typed_keypath_metadata drifted", None
    if keypath_semantics.get("reflection_interop") != (
        "must preserve the caller-visible keypath component count and diagnostic behavior across module boundaries"
    ):
        return "semantic policy reflection_interop drifted", None
    if keypath_semantics.get("runtime_composition_shape") != (
        "must preserve explicit component counts without inventing ownership or allocation semantics"
    ):
        return "semantic policy runtime_composition_shape drifted", None

    concurrency_semantics = semantic_policy.get("concurrency_semantics")
    if not isinstance(concurrency_semantics, dict):
        return "semantic policy missing concurrency_semantics", None
    if concurrency_semantics.get("spawn_token") != (
        "routes structured spawn through objc3_runtime_spawn_task_i32 with task kind 1 and the caller-provided executor tag"
    ):
        return "semantic policy spawn_token drifted", None
    if concurrency_semantics.get("cancellation_checkpoint") != (
        "routes cancellation checkpoints through objc3_runtime_cancel_task_group_i32 when flagged and objc3_runtime_task_is_cancelled_i32 otherwise"
    ):
        return "semantic policy cancellation_checkpoint drifted", None
    if concurrency_semantics.get("public_task_spawn_api") != (
        "objc3_task_spawn, objc3_task_spawn_child, and objc3_task_spawn_detached are stable public aliases over the structured and detached runtime task spawn helpers"
    ):
        return "semantic policy public_task_spawn_api drifted", None
    if concurrency_semantics.get("public_task_join_api") != (
        "objc3_task_join routes non-cancelled public task joins through objc3_concurrency_join_status and preserves explicit result and executor-tag operands"
    ):
        return "semantic policy public_task_join_api drifted", None
    if concurrency_semantics.get("public_task_group_api") != (
        "objc3_task_group_run_two enters a task-group scope, adds two child tasks, and drains them through objc3_concurrency_task_group_scope_depth"
    ):
        return "semantic policy public_task_group_api drifted", None
    if concurrency_semantics.get("public_cancellation_api") != (
        "objc3_task_is_cancelled and objc3_task_cancel_if_needed expose cancellation observation and cancellation checkpoints without widening scheduler policy claims"
    ):
        return "semantic policy public_cancellation_api drifted", None
    if concurrency_semantics.get("public_executor_api") != (
        "objc3_executor_hop is the public executor-hop entrypoint over objc3_concurrency_executor_hop_token with distinct value and executor-tag operands"
    ):
        return "semantic policy public_executor_api drifted", None
    if concurrency_semantics.get("public_actor_mailbox_api") != (
        "objc3_actor_mailbox_send_and_drain binds, enqueues, and drains a single actor mailbox message through objc3_concurrency_actor_mailbox_token"
    ):
        return "semantic policy public_actor_mailbox_api drifted", None
    if concurrency_semantics.get("family_growth_rule") != (
        "structured-child-spawn detached-spawn join-and-wait task-group-scope cancellation-observation executor-hop actor-mailbox public-task-spawn public-task-join public-task-group public-cancellation public-executor-hop and public-actor-mailbox helpers may grow additively inside objc3.concurrency"
    ):
        return "semantic policy concurrency family_growth_rule drifted", None
    if concurrency_semantics.get("layering_rule") != (
        "objc3.concurrency may depend only on objc3.core and objc3.errors within stdlib major version 1"
    ):
        return "semantic policy concurrency layering_rule drifted", None

    system_semantics = semantic_policy.get("system_semantics")
    if not isinstance(system_semantics, dict):
        return "semantic policy missing system_semantics", None
    if system_semantics.get("resource_token") != (
        "returns seed plus 4 as the canonical deterministic strict-system resource token"
    ):
        return "semantic policy system resource_token drifted", None
    if system_semantics.get("profile_gate") != (
        "objc3.system helpers remain gated to Strict System and must not become unconditional Core imports"
    ):
        return "semantic policy system profile_gate drifted", None
    if system_semantics.get("runtime_composition_hook") != (
        "strict-system runtime-composition helpers must be explicit profile-gated entrypoints rather than implicit side effects in core helpers"
    ):
        return "semantic policy system runtime_composition_hook drifted", None
    if system_semantics.get("layering_rule") != (
        "objc3.system may depend on objc3.core objc3.errors objc3.concurrency and objc3.keypath but those modules may not depend on objc3.system"
    ):
        return "semantic policy system layering_rule drifted", None

    return None, SemanticPolicyValidation(module_semver=semantic_module_semver)
