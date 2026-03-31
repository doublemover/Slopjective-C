#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/dashboard_reporting_contract.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_DIR = ROOT / "tmp/reports/full-envelope-claimability/dashboard-contract"
JSON_OUT = OUT_DIR / "dashboard_contract_summary.json"
MD_OUT = OUT_DIR / "dashboard_contract_summary.md"


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

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_full_envelope_claimability_dashboard_contract_summary.py",
        "runbook_mentions_dashboard_surface": "## Dashboard And Claim Publication Surface" in runbook_text,
        "all_required_source_summaries_exist": all(
            resolve_repo_path(path).is_file() for path in contract["required_source_summaries"]
        ),
        "all_required_integration_reports_exist": all(
            resolve_repo_path(path).is_file() for path in contract["required_integration_reports"]
        ),
        "all_required_schemas_exist": all(
            resolve_repo_path(path).is_file() for path in contract["required_schemas"]
        ),
        "report_output_paths_are_tmp_reports": all(
            path.startswith("tmp/reports/full-envelope-claimability/") for path in contract["report_output_paths"]
        ),
        "artifact_output_paths_are_tmp_artifacts": all(
            path.startswith("tmp/artifacts/full-envelope-claimability/") for path in contract["artifact_output_paths"]
        ),
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.dashboard.contract.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_full_envelope_claimability_dashboard_contract_summary.py",
        "required_source_summary_count": len(contract["required_source_summaries"]),
        "required_integration_report_count": len(contract["required_integration_reports"]),
        "required_schema_count": len(contract["required_schemas"]),
        "report_output_count": len(contract["report_output_paths"]),
        "artifact_output_count": len(contract["artifact_output_paths"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Full-Envelope Dashboard Contract Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Required source summaries: `{payload['required_source_summary_count']}`\n"
        f"- Required integration reports: `{payload['required_integration_report_count']}`\n"
        f"- Required schemas: `{payload['required_schema_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
