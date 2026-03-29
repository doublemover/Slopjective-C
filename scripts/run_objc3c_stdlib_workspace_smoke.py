#!/usr/bin/env python3
"""Compile the materialized stdlib smoke sources through the public compile path."""

from __future__ import annotations

import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
MATERIALIZER = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "stdlib" / "smoke"
REPORT_PATH = ROOT / "tmp" / "reports" / "stdlib" / "workspace-smoke-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.workspace.smoke.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: list[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        command,
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def extract_value(stdout: str, key: str) -> str | None:
    prefix = f"{key}:"
    for raw_line in stdout.splitlines():
        line = raw_line.strip()
        if line.startswith(prefix):
            return line.split(":", 1)[1].strip()
    return None


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def main() -> int:
    materialize_result = run_capture([sys.executable, str(MATERIALIZER)])
    if materialize_result.returncode != 0:
        raise RuntimeError("stdlib workspace materialization failed")
    workspace_root_text = extract_value(materialize_result.stdout, "workspace_root")
    summary_path_text = extract_value(materialize_result.stdout, "summary_path")
    if not workspace_root_text or not summary_path_text:
        raise RuntimeError("stdlib materializer did not publish workspace_root/summary_path")

    workspace_root = ROOT / workspace_root_text
    workspace_summary = load_json(ROOT / summary_path_text)
    modules = workspace_summary.get("modules")
    if not isinstance(modules, list) or not modules:
        raise RuntimeError("stdlib materializer summary did not publish modules")

    compile_results: list[dict[str, object]] = []
    for module in modules:
        if not isinstance(module, dict):
            raise RuntimeError("stdlib materializer summary published a malformed module entry")
        canonical_module = str(module["canonical_module"])
        smoke_source = workspace_root / str(module["smoke_source"])
        compile_root = ARTIFACT_ROOT / canonical_module.replace(".", "_")
        compile_root.mkdir(parents=True, exist_ok=True)
        compile_result = run_capture(
            [
                sys.executable,
                str(RUNNER),
                "compile-objc3c",
                repo_rel(smoke_source),
                "--out-dir",
                repo_rel(compile_root),
                "--emit-prefix",
                "module",
            ]
        )
        if compile_result.returncode != 0:
            raise RuntimeError(f"stdlib smoke compile failed for {canonical_module}")
        manifest_path = compile_root / "module.manifest.json"
        registration_manifest_path = compile_root / "module.runtime-registration-manifest.json"
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
