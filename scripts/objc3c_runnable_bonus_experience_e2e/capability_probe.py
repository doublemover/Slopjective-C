from __future__ import annotations

import sys
from pathlib import Path

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.subprocesses import run_capture

from .assertions import expect


def run_capability_probe(
    *,
    package_root: Path,
    capability_probe_script: Path,
    capability_report: Path,
) -> object:
    capability_report.parent.mkdir(parents=True, exist_ok=True)
    capability_probe_result = run_capture(
        [sys.executable, str(capability_probe_script), "--summary-out", str(capability_report)],
        cwd=package_root,
    )
    if capability_probe_result.returncode != 0:
        raise RuntimeError("packaged capability probe failed")
    capability_payload = load_json(capability_report)
    expect(capability_payload.get("ok") is True, "packaged capability probe did not report ok=true")
    expect(
        capability_payload.get("sema_type_system_parity", {}).get("parity_ready") is True,
        "packaged capability probe parity_ready drifted",
    )
    return capability_probe_result
