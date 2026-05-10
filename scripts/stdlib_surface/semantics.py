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

    error_semantics = semantic_policy.get("error_semantics")
    if not isinstance(error_semantics, dict):
        return "semantic policy missing error_semantics", None
    if error_semantics.get("result_ok_tag") != 1 or error_semantics.get("result_err_tag") != 2:
        return "semantic policy result tag values drifted", None
    if error_semantics.get("result_bridge_diagnostic") != (
        "returns 0 when option presence matches the provided result tag, otherwise 30601"
    ):
        return "semantic policy result_bridge_diagnostic drifted", None
    if error_semantics.get("text_data_compatibility_diagnostic") != (
        "returns 0 when unit_count equals byte_count, otherwise 30602"
    ):
        return "semantic policy text_data_compatibility_diagnostic drifted", None

    keypath_semantics = semantic_policy.get("keypath_semantics")
    if not isinstance(keypath_semantics, dict):
        return "semantic policy missing keypath_semantics", None
    if keypath_semantics.get("text_compatibility_diagnostic") != (
        "returns 0 when text_units is at least component_count, otherwise 30603"
    ):
        return "semantic policy text_compatibility_diagnostic drifted", None
    if keypath_semantics.get("typed_keypath_metadata") != (
        "remains count-and-component preserving until runtime-backed keypath metadata lands"
    ):
        return "semantic policy typed_keypath_metadata drifted", None
    if keypath_semantics.get("reflection_interop") != (
        "must preserve the caller-visible keypath component count and diagnostic behavior across module boundaries"
    ):
        return "semantic policy reflection_interop drifted", None
    if keypath_semantics.get("runtime_composition_adapter") != (
        "must not invent ownership or allocation semantics beyond the checked-in keypath component and compatibility helpers"
    ):
        return "semantic policy runtime_composition_adapter drifted", None

    concurrency_semantics = semantic_policy.get("concurrency_semantics")
    if not isinstance(concurrency_semantics, dict):
        return "semantic policy missing concurrency_semantics", None
    if concurrency_semantics.get("spawn_token") != (
        "returns seed plus 1 as the current deterministic child-spawn token placeholder"
    ):
        return "semantic policy spawn_token drifted", None
    if concurrency_semantics.get("cancellation_checkpoint") != (
        "returns 1 when the provided flag is nonzero and 0 otherwise"
    ):
        return "semantic policy cancellation_checkpoint drifted", None
    if concurrency_semantics.get("family_growth_rule") != (
        "structured-child-spawn detached-spawn join-and-wait task-group-scope cancellation-observation and executor-hop helpers may grow additively inside objc3.concurrency"
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
        "returns seed plus 4 as the current deterministic strict-system resource token placeholder"
    ):
        return "semantic policy system resource_token drifted", None
    if system_semantics.get("profile_gate") != (
        "objc3.system helpers remain reserved for Strict System claims and must not become unconditional Core imports"
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
