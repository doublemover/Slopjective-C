from __future__ import annotations

from typing import Any

from .constants import WORKSPACE_CONTRACT_ID


def validate_workspace_contract(
    entry: dict[str, Any],
    workspace_payload: dict[str, Any],
) -> str | None:
    example_id = entry.get("id")
    if not isinstance(example_id, str):
        return "example entry missing id/source"
    if workspace_payload.get("contract_id") != WORKSPACE_CONTRACT_ID:
        return f"workspace manifest contract drifted for {example_id}"
    if workspace_payload.get("schema_version") != 1:
        return f"workspace manifest schema drifted for {example_id}"
    if workspace_payload.get("example_id") != example_id:
        return f"workspace manifest example_id drifted for {example_id}"
    if workspace_payload.get("workspace_root") != f"showcase/{example_id}":
        return f"workspace manifest workspace_root drifted for {example_id}"
    if workspace_payload.get("entry_source") != entry.get("source"):
        return f"workspace manifest entry_source drifted for {example_id}"
    if workspace_payload.get("emit_prefix") != "module":
        return f"workspace manifest emit_prefix drifted for {example_id}"
    if workspace_payload.get("artifact_root") != f"tmp/artifacts/showcase/{example_id}":
        return f"workspace manifest artifact_root drifted for {example_id}"
    if workspace_payload.get("package_stage_root") != f"showcase/{example_id}":
        return f"workspace manifest package_stage_root drifted for {example_id}"
    stdlib_followup_modules = workspace_payload.get("stdlib_followup_modules")
    if not isinstance(stdlib_followup_modules, list) or not all(
        isinstance(value, str) and value for value in stdlib_followup_modules
    ):
        return f"workspace manifest stdlib_followup_modules malformed for {example_id}"
    if workspace_payload.get("story_capabilities") != entry.get("story_capabilities"):
        return f"workspace manifest story_capabilities drifted for {example_id}"
    if stdlib_followup_modules != entry.get("stdlib_followup_modules"):
        return f"workspace manifest stdlib_followup_modules drifted for {example_id}"
    runtime_surface = workspace_payload.get("runtime_surface")
    if runtime_surface != {
        "launch_contract_helper": "scripts/objc3c_runtime_launch_contract.ps1",
        "runtime_library_resolution_model": "registration-manifest-runtime-archive-path-is-authoritative",
        "driver_linker_flag_consumption_model": "registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands",
        "runtime_output_root": f"tmp/artifacts/showcase/{example_id}/runtime",
        "expected_exit_code": {"auroraBoard": 33, "signalMesh": 13, "patchKit": 7}[example_id],
    }:
        return f"workspace manifest runtime_surface drifted for {example_id}"
    presentation = workspace_payload.get("presentation")
    expected_headlines = {
        "auroraBoard": "categories, reflection, synthesized behaviors",
        "signalMesh": "status bridging, actor-shaped messaging, runtime messaging",
        "patchKit": "derives, macros, property behaviors, interop",
    }
    if presentation != {
        "title": entry.get("title"),
        "headline": expected_headlines[example_id],
    }:
        return f"workspace manifest presentation drifted for {example_id}"
    return None
