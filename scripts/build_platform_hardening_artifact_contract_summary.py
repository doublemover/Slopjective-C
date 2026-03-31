#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = ROOT / "tests/tooling/fixtures/platform_hardening/platform_matrix_artifact_contract.json"
SCHEMA_PATH = ROOT / "schemas/objc3c-platform-support-matrix-v1.schema.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_platform_hardening.md"
OUT_DIR = ROOT / "tmp/reports/platform-hardening/artifact-contract"
JSON_OUT = OUT_DIR / "artifact_contract_summary.json"
MD_OUT = OUT_DIR / "artifact_contract_summary.md"


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


def main() -> int:
    subprocess.run([sys.executable, str(ROOT / "scripts" / "build_objc3c_platform_support_matrix.py")], cwd=ROOT, check=True)
    contract = read_json(CONTRACT_PATH)
    schema = read_json(SCHEMA_PATH)
    artifact = read_json(resolve_repo_path(contract["generated_artifact_path"]))
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    publication_surface = artifact["publication_surface"]
    checks = {
        "schema_exists": SCHEMA_PATH.is_file(),
        "generator_script_exists": resolve_repo_path(contract["generator_script"]).is_file(),
        "artifact_exists": resolve_repo_path(contract["generated_artifact_path"]).is_file(),
        "summary_exists": resolve_repo_path(contract["generated_summary_path"]).is_file(),
        "artifact_contract_id_matches_schema": artifact["contract_id"] == schema["properties"]["contract_id"]["const"],
        "artifact_has_required_fields": all(field in artifact for field in contract["required_fields"]),
        "artifact_has_required_publication_fields": all(field in publication_surface for field in contract["required_publication_surface_fields"]),
        "runbook_mentions_machine_owned_artifact_contract": "## Machine-Owned Artifact Contract" in runbook_text,
        "runbook_mentions_support_matrix_artifact_path": "`tmp/artifacts/platform-hardening/objc3c-platform-support-matrix.json`" in runbook_text,
    }

    payload = {
        "contract_id": "objc3c.platform.hardening.platform.matrix.artifact.contract.summary.v1",
        "source_contract_id": contract["contract_id"],
        "status": "PASS" if all(checks.values()) else "FAIL",
        "runner_path": "scripts/build_platform_hardening_artifact_contract_summary.py",
        "required_field_count": len(contract["required_fields"]),
        "required_publication_field_count": len(contract["required_publication_surface_fields"]),
        "checks": checks,
    }

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    JSON_OUT.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    MD_OUT.write_text(
        "# Platform Matrix Artifact Contract Summary\n\n"
        f"- Contract: `{payload['source_contract_id']}`\n"
        f"- Required fields: `{payload['required_field_count']}`\n"
        f"- Required publication fields: `{payload['required_publication_field_count']}`\n"
        f"- Status: `{payload['status']}`\n",
        encoding="utf-8",
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
