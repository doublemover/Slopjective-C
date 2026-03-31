#!/usr/bin/env python3
from __future__ import annotations

import json
from collections import Counter
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/full_envelope_claimability/support_matrix_claim_taxonomy.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_full_envelope_claimability.md"
OUT_DIR = ROOT / "tmp/reports/full-envelope-claimability/support-matrix"
JSON_OUT = OUT_DIR / "support_matrix_summary.json"
MD_OUT = OUT_DIR / "support_matrix_summary.md"


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"JSON object expected at {path}")
    return payload


def repo_rel(path: Path) -> str:
    return str(path.relative_to(ROOT)).replace("\\", "/")


def resolve_repo_path(raw_path: str) -> Path:
    path = Path(raw_path)
    return path if path.is_absolute() else ROOT / path


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    evidence_status: dict[str, dict[str, Any]] = {}
    for family in contract["evidence_families"]:
        report_path = resolve_repo_path(family["report"])
        expect(report_path.is_file(), f"missing evidence report for {family['family']}: {repo_rel(report_path)}")
        payload = read_json(report_path)
        evidence_status[family["family"]] = {
            "report": repo_rel(report_path),
            "contract_id": payload.get("contract_id"),
            "status": payload.get("status"),
            "matches_expected_contract": payload.get("contract_id") == family["required_contract_id"],
            "passes": payload.get("status") == "PASS",
        }

    support_class_counter = Counter(str(row["current_class"]) for row in contract["support_matrix"])
    required_surface_paths = {
        raw_path
        for row in contract["support_matrix"]
        for raw_path in row.get("checked_in_surfaces", [])
    }
    public_claim_paths = [resolve_repo_path(path) for path in contract["public_claim_surfaces"]]

    checks = {
        "summary_script_link_matches": contract["summary_script"] == "scripts/build_full_envelope_claimability_support_matrix_summary.py",
        "runbook_mentions_support_classes": all(
            marker in runbook_text
            for marker in ("`supported`", "`experimental`", "`unsupported`", "`release-blocking`")
        ),
        "runbook_mentions_demotion_model": "## Demotion Model" in runbook_text,
        "runbook_mentions_evidence_families": "## Evidence Families" in runbook_text,
        "all_evidence_reports_match_expected_contracts": all(
            item["matches_expected_contract"] for item in evidence_status.values()
        ),
        "all_evidence_reports_pass": all(item["passes"] for item in evidence_status.values()),
        "all_checked_in_surface_paths_exist": all(resolve_repo_path(path).exists() for path in required_surface_paths),
        "all_public_claim_surfaces_exist": all(path.exists() for path in public_claim_paths),
        "support_matrix_contains_supported_surface": support_class_counter["supported"] > 0,
        "support_matrix_contains_unsupported_surface": support_class_counter["unsupported"] > 0,
        "demotion_model_covers_release_blocking": any(
            trigger["demotes_to"] == "release-blocking"
            for trigger in contract["demotion_triggers"]
        ),
        "successor_tracks_cover_post_m324_program": len(contract["successor_tracks"]) == 8,
    }

    payload = {
        "contract_id": "objc3c.full_envelope.claimability.support.matrix.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_full_envelope_claimability_support_matrix_summary.py",
        "runbook": repo_rel(RUNBOOK_PATH),
        "support_class_count": len(contract["support_classes"]),
        "evidence_family_count": len(contract["evidence_families"]),
        "support_row_count": len(contract["support_matrix"]),
        "demotion_trigger_count": len(contract["demotion_triggers"]),
        "public_claim_surface_count": len(contract["public_claim_surfaces"]),
        "support_row_counts_by_class": dict(sorted(support_class_counter.items())),
        "evidence_status": evidence_status,
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Full-Envelope Claimability Support Matrix Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Support classes: `{payload['support_class_count']}`\n"
        f"- Evidence families: `{payload['evidence_family_count']}`\n"
        f"- Support rows: `{payload['support_row_count']}`\n"
        f"- Demotion triggers: `{payload['demotion_trigger_count']}`\n"
        f"- Public claim surfaces: `{payload['public_claim_surface_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
