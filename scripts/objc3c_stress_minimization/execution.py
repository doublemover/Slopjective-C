"""Compiler process execution for stress minimization candidates."""

from __future__ import annotations

import hashlib
import json
import subprocess
from pathlib import Path
from typing import Any

from .paths import ROOT


def compile_source(
    compiler: Path,
    source_text: str,
    work_dir: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    work_dir.mkdir(parents=True, exist_ok=True)
    source_path = work_dir / "candidate.objc3"
    out_dir = work_dir / "out"
    source_path.write_text(source_text, encoding="utf-8")
    completed = subprocess.run(
        [
            str(compiler),
            str(source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        timeout=timeout_sec,
    )
    stdout_text = completed.stdout.replace("\r\n", "\n")
    stderr_text = completed.stderr.replace("\r\n", "\n")
    combined_output = f"{stdout_text}\n{stderr_text}"
    diagnostic_lines = [
        line.strip()
        for line in combined_output.splitlines()
        if line.strip() and ("error" in line.lower() or "o3" in line.lower())
    ]
    if not diagnostic_lines:
        diagnostic_lines = [line.strip() for line in combined_output.splitlines() if line.strip()][:5]
    if not diagnostic_lines:
        diagnostic_lines = [f"returncode:{completed.returncode}"]
    signature_payload = {
        "returncode": completed.returncode,
        "diagnostic_lines": diagnostic_lines,
    }
    signature_text = json.dumps(signature_payload, sort_keys=True)
    return {
        "returncode": completed.returncode,
        "stdout": stdout_text,
        "stderr": stderr_text,
        "diagnostic_lines": diagnostic_lines,
        "signature_sha256": hashlib.sha256(signature_text.encode("utf-8")).hexdigest(),
        "signature_payload": signature_payload,
    }


__all__ = ["compile_source"]
