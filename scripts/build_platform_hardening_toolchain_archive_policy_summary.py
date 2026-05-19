#!/usr/bin/env python3
from __future__ import annotations

import json

from platform_hardening_contracts import (
    PACKAGING_RUNBOOK_PATH,
    PLATFORM_RUNBOOK_PATH,
    RELEASE_RUNBOOK_PATH,
    TOOLCHAIN_ARCHIVE_POLICY_PATH,
    TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH,
    load_json_object,
    require_platform_hardening_blocker_metadata,
    require_platform_hardening_owner_policy,
    write_json,
    write_markdown_summary,
)


def main() -> int:
    policy = load_json_object(TOOLCHAIN_ARCHIVE_POLICY_PATH)
    owner_policy = require_platform_hardening_owner_policy(policy, surface_name="toolchain archive claim policy")
    blocker_metadata = require_platform_hardening_blocker_metadata(
        policy,
        surface_name="toolchain archive claim policy",
        required_blockers=("toolchain archive claim outside checked-in package channel set",),
    )
    platform_runbook_text = PLATFORM_RUNBOOK_PATH.read_text(encoding="utf-8")
    packaging_runbook_text = PACKAGING_RUNBOOK_PATH.read_text(encoding="utf-8")
    release_runbook_text = RELEASE_RUNBOOK_PATH.read_text(encoding="utf-8")
    claim_boundary_rules = policy["claim_boundary_rules"]

    checks = {
        "claim_boundary_rules_present": len(claim_boundary_rules) >= 4,
        "toolchain_range_rules_present": len(policy["toolchain_range_rules"]) >= 3,
        "forbidden_claims_present": len(policy["forbidden_claims"]) >= 4,
        "required_artifact_links_present": len(policy["required_artifact_links"]) >= 3,
        "platform_runbook_mentions_toolchain_archive_policy": "## Toolchain-Range And Archive Compatibility Policy" in platform_runbook_text,
        "platform_runbook_mentions_same_payload_family": "same runnable payload family" in platform_runbook_text,
        "packaging_runbook_mentions_archive_compatibility_boundary": "archive compatibility claims must remain tied to the same `windows-x64`" in packaging_runbook_text,
        "release_runbook_mentions_support_tier_archive_overclaim_warning": "support-tier or archive compatibility overclaim attempts" in release_runbook_text,
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.toolchain.archive.claim.policy.summary.v1",
        "source_contract_id": policy["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_toolchain_archive_policy_summary.py",
        "owner_policy": owner_policy,
        "blocker_metadata": blocker_metadata,
        "claim_boundary_rule_count": len(claim_boundary_rules),
        "toolchain_range_rule_count": len(policy["toolchain_range_rules"]),
        "forbidden_claim_count": len(policy["forbidden_claims"]),
        "required_artifact_link_count": len(policy["required_artifact_links"]),
        "checks": checks,
    }

    write_json(TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH, payload)
    write_markdown_summary(
        TOOLCHAIN_ARCHIVE_POLICY_SUMMARY_PATH.with_suffix(".md"),
        "Toolchain And Archive Claim Policy Summary",
        (
            ("Contract", payload["source_contract_id"]),
            ("Claim-boundary rules", payload["claim_boundary_rule_count"]),
            ("Toolchain-range rules", payload["toolchain_range_rule_count"]),
            ("Forbidden claims", payload["forbidden_claim_count"]),
            ("Required artifact links", payload["required_artifact_link_count"]),
            ("Status", payload["status"]),
        ),
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
