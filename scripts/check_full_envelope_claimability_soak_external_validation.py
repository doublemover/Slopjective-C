#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/soak_external_validation_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_DIR = ROOT / "tmp/reports/full-envelope-claimability/soak-external-validation"
JSON_OUT = OUT_DIR / "soak_external_validation_summary.json"
MD_OUT = OUT_DIR / "soak_external_validation_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def resolve_repo_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    reports = {
        repo_rel(resolve_repo_path(path)): read_json(resolve_repo_path(path))
        for path in contract["required_reports"]
    }
    dashboard = reports["tmp/reports/full-envelope-claimability/dashboard-summary.json"]
    acceptance_matrix = dashboard.get("acceptance_matrix", [])
    acceptance_families = {
        row.get("family")
        for row in acceptance_matrix
        if isinstance(row, dict)
    }

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/check_full_envelope_claimability_soak_external_validation.py",
        "runbook_mentions_soak_external_validation_section": "## Soak, Stress, And External Validation Integration" in runbook_text,
        "all_required_reports_pass": all(report.get("status") == "PASS" for report in reports.values()),
        "dashboard_includes_required_acceptance_matrix_families": all(
            family in acceptance_families for family in contract["required_acceptance_matrix_families"]
        ),
        "public_conformance_remains_caution_or_better": reports["tmp/reports/public-conformance/integration-summary.json"].get("public_status") in {"claim-ready", "caution"},
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.soak.external.validation.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/check_full_envelope_claimability_soak_external_validation.py",
        "acceptance_matrix_family_count": len(acceptance_families),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Full-Envelope Soak/Stress/External Validation Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Acceptance-matrix families: `{payload['acceptance_matrix_family_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
