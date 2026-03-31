#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "security_hardening" / "security_response_disclosure_policy.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_security_hardening.md"
OPERATOR_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "distribution_credibility" / "operator_release_policy.json"
EXTERNAL_TRUST_POLICY_PATH = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "trust_policy.json"
OUT_DIR = ROOT / "tmp" / "reports" / "security-hardening" / "response-policy"
JSON_OUT = OUT_DIR / "response_policy_summary.json"
MD_OUT = OUT_DIR / "response_policy_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    operator_policy = read_json(OPERATOR_POLICY_PATH)
    external_policy = read_json(EXTERNAL_TRUST_POLICY_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    checks = {
        "states_match_operator_policy": contract["states"] == operator_policy["states"],
        "blocking_conditions_extend_operator_policy": set(operator_policy["blocking_conditions"]).issubset(set(contract["blocking_conditions"])),
        "disclosure_rules_present": len(contract["disclosure_rules"]) >= 4,
        "incident_classes_present": len(contract["incident_classes"]) >= 5,
        "external_policy_mentions_disclosure_uncertainty": any(
            "disclosure uncertainty" in rule for rule in external_policy["fail_closed_rules"]
        ),
        "runbook_mentions_security_response_policy": "## Security Response And Disclosure Policy" in runbook_text,
        "runbook_mentions_ready_degraded_blocked": all(token in runbook_text for token in ("`ready`", "`degraded`", "`blocked`")),
        "runbook_mentions_disclosure_uncertainty": "unresolved disclosure uncertainty must fail closed" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.security.hardening.response.disclosure.policy.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_security_hardening_response_policy_summary.py",
        "state_count": len(contract["states"]),
        "blocking_condition_count": len(contract["blocking_conditions"]),
        "degraded_condition_count": len(contract["degraded_conditions"]),
        "incident_class_count": len(contract["incident_classes"]),
        "disclosure_rule_count": len(contract["disclosure_rules"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Security Response Policy Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- States: `{payload['state_count']}`\n"
        f"- Blocking conditions: `{payload['blocking_condition_count']}`\n"
        f"- Degraded conditions: `{payload['degraded_condition_count']}`\n"
        f"- Incident classes: `{payload['incident_class_count']}`\n"
        f"- Disclosure rules: `{payload['disclosure_rule_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
