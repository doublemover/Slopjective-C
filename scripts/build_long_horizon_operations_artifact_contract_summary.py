#!/usr/bin/env python3
"""Build the long-horizon operations artifact contract summary."""

from __future__ import annotations

from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "long_horizon_operations" / "artifact_contract.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "long-horizon-operations" / "artifact-contract-summary.json"




def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    schema_path = ROOT / str(contract["schema"])
    failures: list[str] = []
    expect(schema_path.is_file(), f"missing schema {contract['schema']}", failures)
    if schema_path.is_file():
        schema = load_json(schema_path)
        expect(schema.get("$schema") == "https://json-schema.org/draft/2020-12/schema", "schema draft drifted", failures)
        required_fields = schema.get("required", [])
        for field in ("support_window", "upgrade_replay", "revert_readiness", "soak", "aging_regression", "claim_audit"):
            expect(field in required_fields, f"schema missing required field {field}", failures)

    checked_paths = [str(contract["runbook"]), str(contract["schema"])]
    checked_paths += [str(path) for path in contract.get("source_contracts", [])]
    checked_paths += [str(path) for path in contract.get("required_generation_scripts", [])]
    missing_paths = [path for path in checked_paths if not (ROOT / path).is_file()]
    expect(not missing_paths, f"artifact contract missing checked paths: {missing_paths}", failures)

    generated_artifacts = contract.get("generated_artifacts", [])
    expect(isinstance(generated_artifacts, list) and len(generated_artifacts) >= 2, "generated artifact set is too narrow", failures)
    for artifact in generated_artifacts if isinstance(generated_artifacts, list) else []:
        if not isinstance(artifact, dict):
            failures.append("generated artifact entry must be an object")
            continue
        expect(str(artifact.get("path", "")).startswith(str(contract["artifact_root"])), f"{artifact.get('artifact_id')} path outside artifact root", failures)
        expect(artifact.get("schema") == contract["schema"], f"{artifact.get('artifact_id')} schema drifted", failures)

    payload = {
        "contract_id": "objc3c.long_horizon_operations.artifact_contract.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "artifact_contract": repo_rel(CONTRACT_PATH),
        "schema": str(contract["schema"]),
        "artifact_root": str(contract["artifact_root"]),
        "report_root": str(contract["report_root"]),
        "source_contract_count": len(contract.get("source_contracts", [])),
        "generated_artifact_count": len(generated_artifacts) if isinstance(generated_artifacts, list) else 0,
        "generated_report_count": len(contract.get("generated_reports", [])),
        "claim_rules": contract.get("claim_rules", []),
        "checked_paths": sorted(set(checked_paths)),
        "missing_paths": missing_paths,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("long-horizon-artifact-contract: PASS" if not failures else "long-horizon-artifact-contract: FAIL")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
