#!/usr/bin/env python3
from __future__ import annotations

import json

from platform_hardening_contracts import (
    BOUNDARY_INVENTORY_PATH,
    BOUNDARY_INVENTORY_SUMMARY_PATH,
    PLATFORM_RUNBOOK_PATH,
    SUPPORTED_PLATFORMS_PATH,
    current_host,
    load_json_object,
    policy_path_entries,
    required_tool_probes,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
    write_json,
    write_markdown_summary,
)
from objc3c_tooling.paths import resolve_repo_path
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


def main() -> int:
    contract = load_json_object(BOUNDARY_INVENTORY_PATH)
    owner_policy = require_platform_hardening_owner_policy(contract, surface_name="platform hardening boundary inventory")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        contract,
        surface_name="platform hardening boundary inventory",
        required_blockers=("platform claim outside checked-in support matrix",),
    )
    supported_platforms = load_json_object(SUPPORTED_PLATFORMS_PATH)
    runbook_text = PLATFORM_RUNBOOK_PATH.read_text(encoding="utf-8")
    registered_actions = set(public_workflow_action_names())
    missing_actions = [
        str(action)
        for action in contract["public_actions"]
        if str(action) not in registered_actions
    ]

    probes = required_tool_probes()
    host = current_host()

    supported_platform_entries = supported_platforms.get("supported_platforms", [])
    if not isinstance(supported_platform_entries, list):
        raise RuntimeError("supported_platforms.json did not publish supported_platforms")

    checks = {
        "summary_script_link_matches": contract["summary_implementation_anchor"] == "scripts/build_platform_hardening_boundary_inventory_summary.py",
        "all_authoritative_code_paths_exist": all(resolve_repo_path(path).exists() for path in contract["authoritative_code_paths"]),
        "all_policy_contract_paths_exist": all(path.is_file() for path in policy_path_entries()),
        "all_public_actions_registered": not missing_actions,
        "supported_platform_fixture_matches_boundary": sorted(entry["platform_id"] for entry in supported_platform_entries) == sorted(contract["supported_platform_ids"]),
        "default_platform_is_supported": supported_platforms.get("default_platform_id") in contract["supported_platform_ids"],
        "runbook_mentions_current_support_matrix": "## Current Support Matrix" in runbook_text,
        "runbook_mentions_tier_1_windows_x64": "`Tier 1`" in runbook_text and "`windows-x64`" in runbook_text,
        "runbook_mentions_fail_closed_unsupported_hosts": "Unsupported hosts and unsupported toolchain shapes must not degrade into vague" in runbook_text,
        "runbook_mentions_no_cross_platform_parity_claim": "no cross-platform parity claim exists today" in runbook_text,
        "required_tools_detected_on_current_host": all(probe["available"] for probe in probes.values()),
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.boundary.inventory.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_boundary_inventory_summary.py",
        "authoritative_code_path_count": len(contract["authoritative_code_paths"]),
        "policy_contract_path_count": len(contract["policy_contract_paths"]),
        "package_bridge": contract["package_bridge"],
        "public_action_count": len(contract["public_actions"]),
        "missing_actions": missing_actions,
        "report_path_count": len(contract["report_paths"]),
        "supported_platform_count": len(contract["supported_platform_ids"]),
        "supported_channel_count": len(contract["supported_channels"]),
        "gap_claim_count": len(contract["gap_claims"]),
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "host_probe": {
            "os": host.os,
            "arch": host.arch,
            "python": probes["python"],
            "pwsh": probes["pwsh"],
            "clang": probes["clang"],
            "cmake": probes["cmake"],
            "ninja": probes["ninja"],
            "node": probes["node"],
        },
        "checks": checks,
    }

    write_json(BOUNDARY_INVENTORY_SUMMARY_PATH, payload)
    write_markdown_summary(
        BOUNDARY_INVENTORY_SUMMARY_PATH.with_suffix(".md"),
        "Platform Hardening Boundary Inventory Summary",
        (
            ("Contract", payload["source_contract_id"]),
            ("Authoritative code paths", payload["authoritative_code_path_count"]),
            ("Policy contracts", payload["policy_contract_path_count"]),
            ("Package bridge", payload["package_bridge"]),
            ("Public actions", payload["public_action_count"]),
            ("Supported platforms", payload["supported_platform_count"]),
            ("Supported channels", payload["supported_channel_count"]),
            ("Gap claims", payload["gap_claim_count"]),
            ("Status", payload["status"]),
        ),
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
