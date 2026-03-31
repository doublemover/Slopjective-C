#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
POLICY_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/unsupported_host_fallback_policy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_platform_hardening.md"
OUT_DIR = ROOT / "tmp/reports/platform-hardening/unsupported-host-policy"
JSON_OUT = OUT_DIR / "unsupported_host_policy_summary.json"
MD_OUT = OUT_DIR / "unsupported_host_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    policy = read_json(POLICY_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
        "hard_fail_classes_present": len(policy["hard_fail_classes"]) >= 4,
        "allowed_fallback_classes_present": len(policy["allowed_fallback_classes"]) >= 1,
        "forbidden_phrases_present": len(policy["forbidden_phrases"]) >= 3,
        "required_claims_present": len(policy["required_claims"]) >= 3,
        "runbook_mentions_unsupported_host_policy": "## Unsupported-Host And Fallback Policy" in runbook_text,
        "runbook_mentions_hard_fail_host_matrix": "host OS or host architecture outside the checked-in support matrix" in runbook_text,
        "runbook_mentions_allowed_fallback_behavior": "capability inspection and docs-only policy checks may still run" in runbook_text,
        "runbook_mentions_forbidden_supported_language": "`best effort supported`" in runbook_text and "`supported if LLVM is installed`" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.unsupported.host.fallback.policy.summary.v1",
        "source_contract_id": policy["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_unsupported_host_policy_summary.py",
        "hard_fail_class_count": len(policy["hard_fail_classes"]),
        "allowed_fallback_class_count": len(policy["allowed_fallback_classes"]),
        "forbidden_phrase_count": len(policy["forbidden_phrases"]),
        "required_claim_count": len(policy["required_claims"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Unsupported Host Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Hard-fail classes: `{payload['hard_fail_class_count']}`\n"
        f"- Allowed fallback classes: `{payload['allowed_fallback_class_count']}`\n"
        f"- Forbidden phrases: `{payload['forbidden_phrase_count']}`\n"
        f"- Required claims: `{payload['required_claim_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
