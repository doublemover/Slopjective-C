#!/usr/bin/env python3
from __future__ import annotations

import json
from pathlib import Path
from typing import Any
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel, resolve_repo_path
try:
    from build_full_envelope_claimability_contracts import (
        CLAIMABILITY_REPORT_MD,
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_ARTIFACT_FIELDS,
    )
except ModuleNotFoundError:
    from scripts.build_full_envelope_claimability_contracts import (
        CLAIMABILITY_REPORT_MD,
        DASHBOARD_DECISION_FIELDS,
        DASHBOARD_SUMMARY,
        PUBLIC_SUMMARY,
        PUBLIC_SUMMARY_DECISION_FIELDS,
        RELEASE_ARTIFACT_FIELDS,
    )

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




def main() -> int:
    contract = read_json(CONTRACT_PATH)
    schema = read_json(resolve_repo_path(contract["required_schemas"][0]))
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
        "dashboard_output_paths_are_canonical": contract["report_output_paths"] == [
            DASHBOARD_SUMMARY,
            PUBLIC_SUMMARY,
        ],
        "claimability_report_path_is_canonical": contract["artifact_output_paths"] == [
            CLAIMABILITY_REPORT_MD,
        ],
        "schema_requires_dashboard_decision_fields": set(DASHBOARD_DECISION_FIELDS).issubset(
            schema.get("required", [])
        ),
        "contract_requires_dashboard_decision_fields": set(DASHBOARD_DECISION_FIELDS).issubset(
            contract["required_dashboard_fields"]
        ),
        "contract_requires_public_summary_decision_fields": set(PUBLIC_SUMMARY_DECISION_FIELDS).issubset(
            contract["required_public_summary_fields"]
        ),
        "contract_release_artifacts_match_owner_constants": set(contract["required_release_artifact_fields"]) == set(RELEASE_ARTIFACT_FIELDS),
        "contract_acceptance_families_cover_dashboard_outputs": len(
            contract["required_acceptance_matrix_families"]
        )
        == 8,
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
        "required_dashboard_field_count": len(contract["required_dashboard_fields"]),
        "required_public_summary_field_count": len(contract["required_public_summary_fields"]),
        "required_release_artifact_field_count": len(contract["required_release_artifact_fields"]),
        "required_acceptance_matrix_family_count": len(contract["required_acceptance_matrix_families"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    write_json_file(JSON_OUT, payload)
    MD_OUT.write_text(
        "# Full-Envelope Dashboard Contract Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Required source summaries: `{payload['required_source_summary_count']}`\n"
        f"- Required integration reports: `{payload['required_integration_report_count']}`\n"
        f"- Required schemas: `{payload['required_schema_count']}`\n"
        f"- Required dashboard fields: `{payload['required_dashboard_field_count']}`\n"
        f"- Required public summary fields: `{payload['required_public_summary_field_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
