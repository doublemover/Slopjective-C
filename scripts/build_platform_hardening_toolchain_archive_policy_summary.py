#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/toolchain_archive_compatibility_policy.json"
PLATFORM_RUNBOOK = ROOT / "docs/runbooks/objc3c_platform_hardening.md"
PACKAGING_RUNBOOK = ROOT / "docs/runbooks/objc3c_packaging_channels.md"
RELEASE_RUNBOOK = ROOT / "docs/runbooks/objc3c_release_operations.md"
OUT_DIR = ROOT / "tmp/reports/platform-hardening/toolchain-archive-policy"
JSON_OUT = OUT_DIR / "toolchain_archive_policy_summary.json"
MD_OUT = OUT_DIR / "toolchain_archive_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    policy = read_json(POLICY_PATH)
    platform_runbook_text = PLATFORM_RUNBOOK.read_text(encoding="utf-8")
    packaging_runbook_text = PACKAGING_RUNBOOK.read_text(encoding="utf-8")
    release_runbook_text = RELEASE_RUNBOOK.read_text(encoding="utf-8")

    checks = {
        "compatibility_rules_present": len(policy["compatibility_rules"]) >= 4,
        "toolchain_range_rules_present": len(policy["toolchain_range_rules"]) >= 3,
        "forbidden_claims_present": len(policy["forbidden_claims"]) >= 4,
        "required_artifact_links_present": len(policy["required_artifact_links"]) >= 3,
        "platform_runbook_mentions_toolchain_archive_policy": "## Toolchain-Range And Archive Compatibility Policy" in platform_runbook_text,
        "platform_runbook_mentions_same_payload_family": "same runnable payload family" in platform_runbook_text,
        "packaging_runbook_mentions_archive_compatibility_boundary": "archive compatibility claims must remain tied to the same `windows-x64`" in packaging_runbook_text,
        "release_runbook_mentions_support_tier_archive_overclaim_warning": "support-tier or archive compatibility overclaim attempts" in release_runbook_text,
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.toolchain.archive.compatibility.policy.summary.v1",
        "source_contract_id": policy["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_toolchain_archive_policy_summary.py",
        "compatibility_rule_count": len(policy["compatibility_rules"]),
        "toolchain_range_rule_count": len(policy["toolchain_range_rules"]),
        "forbidden_claim_count": len(policy["forbidden_claims"]),
        "required_artifact_link_count": len(policy["required_artifact_links"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Toolchain And Archive Compatibility Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Compatibility rules: `{payload['compatibility_rule_count']}`\n"
        f"- Toolchain-range rules: `{payload['toolchain_range_rule_count']}`\n"
        f"- Forbidden claims: `{payload['forbidden_claim_count']}`\n"
        f"- Required artifact links: `{payload['required_artifact_link_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
