#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess

from objc3c_tooling.paths import resolve_repo_path
from objc3c_tooling.subprocesses import python_script_command
from platform_hardening_contracts import (
    ARTIFACT_CONTRACT_SUMMARY_PATH,
    BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT,
    PLATFORM_MATRIX_ARTIFACT_CONTRACT_PATH,
    PLATFORM_RUNBOOK_PATH,
    PLATFORM_SUPPORT_MATRIX_SCHEMA_PATH,
    ROOT,
    load_json_object,
    write_json,
    write_markdown_summary,
)

def main() -> int:
    subprocess.run(
        python_script_command(BUILD_PLATFORM_SUPPORT_MATRIX_SCRIPT),
        cwd=ROOT,
        check=True,
    )
    contract = load_json_object(PLATFORM_MATRIX_ARTIFACT_CONTRACT_PATH)
    schema = load_json_object(PLATFORM_SUPPORT_MATRIX_SCHEMA_PATH)
    artifact = load_json_object(resolve_repo_path(contract["generated_artifact_path"]))
    runbook_text = PLATFORM_RUNBOOK_PATH.read_text(encoding="utf-8")

    publication_surface = artifact["publication_surface"]
    checks = {
        "schema_exists": PLATFORM_SUPPORT_MATRIX_SCHEMA_PATH.is_file(),
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

    write_json(ARTIFACT_CONTRACT_SUMMARY_PATH, payload)
    write_markdown_summary(
        ARTIFACT_CONTRACT_SUMMARY_PATH.with_suffix(".md"),
        "Platform Matrix Artifact Contract Summary",
        (
            ("Contract", payload["source_contract_id"]),
            ("Required fields", payload["required_field_count"]),
            ("Required publication fields", payload["required_publication_field_count"]),
            ("Status", payload["status"]),
        ),
    )
    print(json.dumps(payload, indent=2))
    return 0 if payload["status"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
