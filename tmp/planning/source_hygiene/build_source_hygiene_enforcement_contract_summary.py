#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(r"C:/Users/sneak/Development/Slopjective-C")
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/source_hygiene_enforcement_contract.json"
POLICY_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/stable_identifier_authenticity_policy.json"
CLASS_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json"
GENUINE_PATH = ROOT / "tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json"
OUT_DIR = ROOT / "tmp/reports/m315/M315-D001"
JSON_OUT = OUT_DIR / "source_hygiene_enforcement_contract_summary.json"
MD_OUT = OUT_DIR / "source_hygiene_enforcement_contract_summary.md"


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    policy = read_json(POLICY_PATH)
    classification = read_json(CLASS_PATH)
    genuine = read_json(GENUINE_PATH)

    check_ids = [entry["check_id"] for entry in contract["enforcement_checks"]]
    checks = {
        "policy_link_matches": contract["policy_contract"] == "tests/tooling/fixtures/source_hygiene/stable_identifier_authenticity_policy.json",
        "classification_link_matches": contract["classification_contract"] == "tests/tooling/fixtures/source_hygiene/artifact_authenticity_classification.json",
        "genuine_contract_link_matches": contract["genuine_provenance_contract"] == "tests/tooling/fixtures/source_hygiene/genuine_artifact_provenance_contract.json",
        "future_live_audit_entrypoint_declared": contract["future_live_audit_entrypoint"] == "python scripts/check_source_hygiene_authenticity.py",
        "canonical_report_root_under_tmp_reports": contract["canonical_report_root"].startswith("tmp/reports/"),
        "check_ids_unique": len(check_ids) == len(set(check_ids)),
        "contract_has_expected_check_count": len(contract["enforcement_checks"]) == 4,
        "policy_generated_truth_surface_count_matches_expectation": len(policy["scope"]["generated_truth_files"]) == 3,
        "classification_has_replay_ll_rule": any(rule["rule_id"] == "objc3c.fixture.synthetic.replayll.v1" for rule in classification["classes"]["synthetic_fixture"]["rules"]),
        "classification_has_replay_manifest_rule": any(rule["rule_id"] == "objc3c.fixture.synthetic.replaymanifest.v1" for rule in classification["classes"]["synthetic_fixture"]["rules"]),
        "genuine_contract_requires_output_path": "output_path" in genuine["required_envelope_fields"],
    }

    summary = {
        "issue": "M315-D001",
        "contract_id": contract["contract_id"],
        "check_ids": check_ids,
        "future_live_audit_entrypoint": contract["future_live_audit_entrypoint"],
        "future_archive_boundary_contract": contract["future_archive_boundary_contract"],
        "fail_closed_condition_count": len(contract["fail_closed_conditions"]),
        "checks": checks,
        "ok": all(checks.values()),
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# M315-D001 Source Hygiene Enforcement Contract Summary\n\n"
        f"- Contract: `{summary['contract_id']}`\n"
        f"- Canonical future audit entrypoint: `{summary['future_live_audit_entrypoint']}`\n"
        f"- Enforcement checks: `{', '.join(summary['check_ids'])}`\n"
        f"- Status: `{'PASS' if summary['ok'] else 'FAIL'}`\n",
        encoding="utf-8",
    )
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
