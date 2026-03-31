#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/platform_support_tier_policy.json"
SUPPORTED_PLATFORMS_PATH = ROOT / "tests/tooling/fixtures/packaging_channels/supported_platforms.json"
PLATFORM_RUNBOOK = ROOT / "docs/runbooks/objc3c_platform_hardening.md"
PACKAGING_RUNBOOK = ROOT / "docs/runbooks/objc3c_packaging_channels.md"
RELEASE_RUNBOOK = ROOT / "docs/runbooks/objc3c_release_operations.md"
OUT_DIR = ROOT / "tmp/reports/platform-hardening/support-tier-policy"
JSON_OUT = OUT_DIR / "support_tier_policy_summary.json"
MD_OUT = OUT_DIR / "support_tier_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    policy = read_json(POLICY_PATH)
    supported_platforms = read_json(SUPPORTED_PLATFORMS_PATH)
    platform_runbook_text = PLATFORM_RUNBOOK.read_text(encoding="utf-8")
    packaging_runbook_text = PACKAGING_RUNBOOK.read_text(encoding="utf-8")
    release_runbook_text = RELEASE_RUNBOOK.read_text(encoding="utf-8")

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

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Platform Support Tier Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Tiers: `{payload['tier_count']}`\n"
        f"- Tier 1 platforms: `{payload['tier_1_platform_count']}`\n"
        f"- Tier 2 platforms: `{payload['tier_2_platform_count']}`\n"
        f"- Experimental platforms: `{payload['experimental_platform_count']}`\n"
        f"- Forbidden claims: `{payload['forbidden_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
