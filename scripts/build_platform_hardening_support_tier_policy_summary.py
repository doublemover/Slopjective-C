#!/usr/bin/env python3
from __future__ import annotations

import json

from platform_hardening_contracts import (
    PACKAGING_RUNBOOK_PATH,
    PLATFORM_RUNBOOK_PATH,
    RELEASE_RUNBOOK_PATH,
    SUPPORT_TIER_POLICY_PATH,
    SUPPORT_TIER_POLICY_SUMMARY_PATH,
    SUPPORTED_PLATFORMS_PATH,
    load_json_object,
    write_json,
    write_markdown_summary,
)


def main() -> int:
    policy = load_json_object(SUPPORT_TIER_POLICY_PATH)
    supported_platforms = load_json_object(SUPPORTED_PLATFORMS_PATH)
    platform_runbook_text = PLATFORM_RUNBOOK_PATH.read_text(encoding="utf-8")
    packaging_runbook_text = PACKAGING_RUNBOOK_PATH.read_text(encoding="utf-8")
    release_runbook_text = RELEASE_RUNBOOK_PATH.read_text(encoding="utf-8")

    tiers = policy["tiers"]
    tier_index = {entry["tier_id"]: entry for entry in tiers}
    supported_ids = sorted(entry["platform_id"] for entry in supported_platforms["supported_platforms"])

    checks = {
        "tier_1_matches_supported_platform_fixture": sorted(tier_index["tier-1"]["platform_ids"]) == supported_ids,
        "default_platform_is_tier_1": supported_platforms["default_platform_id"] in tier_index["tier-1"]["platform_ids"],
        "platform_runbook_mentions_tier_policy": "## Platform Support Tier Policy" in platform_runbook_text,
        "platform_runbook_mentions_tier_1_windows_x64": "`Tier 1`" in platform_runbook_text and "`windows-x64`" in platform_runbook_text,
        "packaging_runbook_mentions_tiered_supported_platforms": "tiered:" in packaging_runbook_text and "`Tier 1`" in packaging_runbook_text,
        "release_runbook_mentions_platform_support_tier_boundary": "Current platform support-tier boundary:" in release_runbook_text,
        "publication_rules_are_present": len(policy["publication_rules"]) >= 4,
        "forbidden_claims_are_present": len(policy["forbidden_claims"]) >= 4,
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.support.tier.policy.summary.v1",
        "source_contract_id": policy["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_support_tier_policy_summary.py",
        "tier_count": len(tiers),
        "tier_1_platform_count": len(tier_index["tier-1"]["platform_ids"]),
        "tier_2_platform_count": len(tier_index["tier-2"]["platform_ids"]),
        "experimental_platform_count": len(tier_index["experimental"]["platform_ids"]),
        "forbidden_claim_count": len(policy["forbidden_claims"]),
        "checks": checks,
    }

    write_json(SUPPORT_TIER_POLICY_SUMMARY_PATH, payload)
    write_markdown_summary(
        SUPPORT_TIER_POLICY_SUMMARY_PATH.with_suffix(".md"),
        "Platform Support Tier Policy Summary",
        (
            ("Contract", payload["source_contract_id"]),
            ("Tiers", payload["tier_count"]),
            ("Tier 1 platforms", payload["tier_1_platform_count"]),
            ("Tier 2 platforms", payload["tier_2_platform_count"]),
            ("Experimental platforms", payload["experimental_platform_count"]),
            ("Forbidden claims", payload["forbidden_claim_count"]),
            ("Status", payload["status"]),
        ),
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
