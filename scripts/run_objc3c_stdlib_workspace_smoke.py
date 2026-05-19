#!/usr/bin/env python3
"""Compile the materialized stdlib smoke sources through the public compile path."""

from __future__ import annotations

import json
from datetime import datetime, timezone
from pathlib import Path
from typing import Any
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_any as load_json
from scripts.objc3c_workflow.public_command_api import public_workflow_command
from objc3c_tooling.subprocesses import python_script_command, run_capture


ROOT = Path(__file__).resolve().parents[1]
MATERIALIZER = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
WORKSPACE_CONTRACT_PATH = ROOT / "stdlib" / "workspace.json"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "workspace-smoke-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.workspace.smoke.summary.v1"





def extract_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return None



def main() -> int:
    run_id = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S_%f")
    materialize_result = run_capture(python_script_command(MATERIALIZER))
    if materialize_result.returncode != 0:
        raise RuntimeError("stdlib workspace materialization failed")
    workspace_root_text = extract_value(materialize_result.stdout, "workspace_root")
    summary_path_text = extract_value(materialize_result.stdout, "summary_path")
    if not workspace_root_text or not summary_path_text:
        raise RuntimeError("stdlib materializer did not publish workspace_root/summary_path")

    workspace_root = ROOT / workspace_root_text
    workspace_summary = load_json(ROOT / summary_path_text)
    workspace_contract = load_json(WORKSPACE_CONTRACT_PATH)
    lowering_import_surface_path = ROOT / str(workspace_contract["lowering_import_surface"])
    lowering_import_surface = load_json(lowering_import_surface_path)
    artifact_root = ROOT / str(lowering_import_surface["smoke_artifact_root"])
    artifact_run_root = artifact_root / run_id
    artifact_filenames = lowering_import_surface["artifact_filenames"]
    modules = workspace_summary.get("modules")
    if not isinstance(modules, list) or not modules:
        raise RuntimeError("stdlib materializer summary did not publish modules")

    compile_results: list[dict[str, object]] = []
    for module in modules:
        if not isinstance(module, dict):
            raise RuntimeError("stdlib materializer summary published a malformed module entry")
        canonical_module = str(module["canonical_module"])
        smoke_source = workspace_root / str(module["smoke_source"])
        compile_root = artifact_run_root / canonical_module.replace(".", "_")
        compile_root.mkdir(parents=True, exist_ok=True)
        compile_result = run_capture(
            public_workflow_command(
                "compile-objc3c",
                repo_rel(smoke_source),
                "--out-dir",
                repo_rel(compile_root),
                "--emit-prefix",
                "module",
            )
        )
        if compile_result.returncode != 0:
            raise RuntimeError(f"stdlib smoke compile failed for {canonical_module}")
        object_path = compile_root / str(artifact_filenames["object"])
        manifest_path = compile_root / str(artifact_filenames["compile_manifest"])
        registration_manifest_path = compile_root / str(
            artifact_filenames["runtime_registration_manifest"]
        )
        if not object_path.is_file():
            raise RuntimeError(f"stdlib smoke compile did not publish {repo_rel(object_path)}")
        if not manifest_path.is_file():
            raise RuntimeError(f"stdlib smoke compile did not publish {repo_rel(manifest_path)}")
        if not registration_manifest_path.is_file():
            raise RuntimeError(
                f"stdlib smoke compile did not publish {repo_rel(registration_manifest_path)}"
            )
        compile_results.append(
            {
                "canonical_module": canonical_module,
                "smoke_source": repo_rel(smoke_source),
                "artifact_root": repo_rel(compile_root),
                "object": repo_rel(object_path),
                "manifest": repo_rel(manifest_path),
                "registration_manifest": repo_rel(registration_manifest_path),
            }
        )

    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "generated_at_utc": datetime.now(timezone.utc).isoformat(),
                "status": "PASS",
                "workspace_root": repo_rel(workspace_root),
                "materialized_workspace_summary": summary_path_text,
                "lowering_import_surface": repo_rel(lowering_import_surface_path),
                "smoke_artifact_root": repo_rel(artifact_run_root),
                "compile_results": compile_results,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
