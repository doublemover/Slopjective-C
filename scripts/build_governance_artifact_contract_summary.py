#!/usr/bin/env python3
"""Build the governance artifact contract summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "governance_sustainability"
CONTRACT_PATH = FIXTURE_ROOT / "artifact_contract.json"
SCHEMA_SURFACE_PATH = FIXTURE_ROOT / "schema_surface.json"
RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_governance_sustainability.md"
OUT_DIR = ROOT / "tmp" / "reports" / "governance-sustainability" / "artifact-contract"
SUMMARY_PATH = OUT_DIR / "governance_artifact_contract_summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    schema_surface = read_json(SCHEMA_SURFACE_PATH)
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")
    failures: list[str] = []

    expect(contract.get("contract_id") == "objc3c.governance.sustainability.artifact_contract.v1", "contract_id drifted", failures)
    expect(contract.get("schema_version") == 1, "schema_version drifted", failures)
    expect(contract.get("summary_script") == "scripts/build_governance_artifact_contract_summary.py", "summary_script drifted", failures)

    source_contracts = [str(path) for path in contract.get("source_contracts", [])]
    generated_reports = [str(path) for path in contract.get("generated_reports", [])]
    publication_artifacts = [str(path) for path in contract.get("publication_artifacts", [])]
    required_generation_scripts = [str(path) for path in contract.get("required_generation_scripts", [])]
    evidence_schema = str(contract.get("evidence_schema", ""))

    missing_sources = sorted(path for path in source_contracts if not (ROOT / path).is_file())
    missing_generation_scripts = sorted(path for path in required_generation_scripts if not (ROOT / path).is_file())
    expect(not missing_sources, f"missing source contracts: {missing_sources}", failures)
    expect(not missing_generation_scripts, f"missing generation scripts: {missing_generation_scripts}", failures)
    expect(bool(evidence_schema) and (ROOT / evidence_schema).is_file(), f"missing evidence schema: {evidence_schema}", failures)
    expect(schema_surface.get("governance_evidence_schema") == evidence_schema, "schema surface omits governance evidence schema", failures)
    expect(len(source_contracts) >= 8, "source contract set is too narrow", failures)
    expect(len(generated_reports) >= 8, "generated report set is too narrow", failures)
    expect(len(publication_artifacts) >= 2, "publication artifact set is too narrow", failures)
    expect(len(required_generation_scripts) >= 9, "generation script set is too narrow", failures)
    expect(any("tmp" in rule and "source of truth" in rule for rule in contract.get("fail_closed_conditions", [])), "tmp source-of-truth fail-closed condition missing", failures)

    required_mentions = {
        "runbook_mentions_contract": "tests/tooling/fixtures/governance_sustainability/artifact_contract.json" in runbook_text,
        "runbook_mentions_summary": "python scripts/build_governance_artifact_contract_summary.py" in runbook_text,
        "runbook_mentions_schema": evidence_schema in runbook_text,
    }
    for key, value in required_mentions.items():
        expect(value, f"{key} is false", failures)

    summary = {
        "contract_id": "objc3c.governance.artifact_contract.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "artifact_contract": repo_rel(CONTRACT_PATH),
        "evidence_schema": evidence_schema,
        "evidence_artifact": contract.get("evidence_artifact"),
        "source_contract_count": len(source_contracts),
        "generated_report_count": len(generated_reports),
        "publication_artifact_count": len(publication_artifacts),
        "required_generation_script_count": len(required_generation_scripts),
        "missing_sources": missing_sources,
        "missing_generation_scripts": missing_generation_scripts,
        **required_mentions,
        "failures": failures,
    }
    summary["ok"] = not failures

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
