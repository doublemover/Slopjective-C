from __future__ import annotations

import subprocess
from dataclasses import dataclass
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import ROOT, display_path

from objc3c_editor_tooling.paths import EditorToolingPaths


@dataclass(frozen=True)
class CompileInvocationResult:
    returncode: int
    stdout: str
    stderr: str
    summary_available: bool


@dataclass(frozen=True)
class EditorToolingInputs:
    summary: dict[str, Any]
    diagnostics: dict[str, Any]
    manifest: dict[str, Any]
    source_text: str
    diagnostics_path_text: str
    manifest_path_text: str | None
    object_path_text: str | None


def run_frontend_compile(paths: EditorToolingPaths) -> CompileInvocationResult:
    paths.report_dir.mkdir(parents=True, exist_ok=True)
    paths.artifact_dir.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [
            str(paths.frontend_runner),
            paths.source.display_path,
            "--out-dir",
            display_path(paths.artifact_dir),
            "--emit-prefix",
            "module",
            "--summary-out",
            display_path(paths.compile_summary),
        ],
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    return CompileInvocationResult(
        returncode=result.returncode,
        stdout=result.stdout,
        stderr=result.stderr,
        summary_available=paths.compile_summary.is_file(),
    )


def load_editor_tooling_inputs(paths: EditorToolingPaths) -> EditorToolingInputs:
    summary = load_json(paths.compile_summary)
    summary_paths = summary.get("paths", {})
    diagnostics_path_text = str(summary_paths.get("diagnostics", "")) if isinstance(summary_paths, dict) else ""
    manifest_path_text = str(summary_paths.get("manifest", "")) if isinstance(summary_paths, dict) else ""
    object_path_text = str(summary_paths.get("object", "")) if isinstance(summary_paths, dict) else ""

    diagnostics = load_json(ROOT / diagnostics_path_text) if diagnostics_path_text else {"diagnostics": []}
    manifest = load_json(ROOT / manifest_path_text) if manifest_path_text else {}
    return EditorToolingInputs(
        summary=summary,
        diagnostics=diagnostics,
        manifest=manifest,
        source_text=paths.source.path.read_text(encoding="utf-8"),
        diagnostics_path_text=diagnostics_path_text,
        manifest_path_text=manifest_path_text or None,
        object_path_text=object_path_text or None,
    )
