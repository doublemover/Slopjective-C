#!/usr/bin/env python3
"""Build the application architecture/testing artifact contract summary."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests" / "tooling" / "fixtures" / "application_architecture_testing" / "artifact_contract.json"
PACKAGE_JSON = ROOT / "package.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "application-architecture-testing" / "artifact-contract-summary.json"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    package = load_json(PACKAGE_JSON)
    package_scripts = package.get("scripts", {})
    if not isinstance(package_scripts, dict):
        raise RuntimeError("package.json scripts field drifted from an object")

    required_paths = [
        str(contract["schema"]),
        str(contract["runbook"]),
        *[str(path) for path in contract["source_contracts"]],
    ]
    missing_paths = [raw for raw in required_paths if not (ROOT / raw).exists()]
    public_scripts = [str(name) for name in contract["public_scripts"]]
    missing_public_scripts = [name for name in public_scripts if name not in package_scripts]

    payload = {
        "contract_id": "objc3c.application.architecture.testing.artifact_contract.summary.v1",
        "status": "PASS" if not missing_paths and not missing_public_scripts else "FAIL",
        "artifact_contract": repo_rel(CONTRACT_PATH),
        "schema_path": str(contract["schema"]),
        "generated_report_root": str(contract["generated_report_root"]),
        "report_slot_count": len(contract["report_slots"]),
        "source_contract_count": len(contract["source_contracts"]),
        "generated_artifact_root_count": len(contract["generated_artifact_roots"]),
        "public_action_count": len(contract["public_actions"]),
        "public_script_count": len(public_scripts),
        "report_slots": contract["report_slots"],
        "generated_artifact_roots": contract["generated_artifact_roots"],
        "public_actions": contract["public_actions"],
        "public_scripts": public_scripts,
        "artifact_claim_rules": contract["artifact_claim_rules"],
        "missing_paths": missing_paths,
        "missing_public_scripts": missing_public_scripts,
        "non_goals": contract["non_goals"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("application-architecture-artifact-contract: PASS" if payload["status"] == "PASS" else "application-architecture-artifact-contract: FAIL")
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
