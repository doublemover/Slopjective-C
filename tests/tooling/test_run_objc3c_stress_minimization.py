from __future__ import annotations

import json
import shutil
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from scripts.objc3c_stress_minimization.models import MinCase
from scripts.objc3c_stress_minimization.runner import materialize_case


SIGNATURE = "b" * 64


def _fake_compile_source(
    compiler: Path,
    source_text: str,
    work_dir: Path,
    timeout_sec: float,
) -> dict[str, Any]:
    del compiler, work_dir, timeout_sec
    diagnostic = "error: synthetic minimization failure O3MIN001"
    return {
        "returncode": 1,
        "stdout": "",
        "stderr": diagnostic + "\n",
        "diagnostic_lines": [diagnostic],
        "signature_sha256": SIGNATURE,
        "signature_payload": {
            "returncode": 1,
            "diagnostic_lines": [diagnostic],
        },
    }


def test_stress_minimization_materializes_failure_and_reducer_capsules(monkeypatch: Any) -> None:
    from scripts.objc3c_stress_minimization import minimization
    from scripts.objc3c_stress_minimization import runner

    monkeypatch.setattr(runner, "compile_source", _fake_compile_source)
    monkeypatch.setattr(minimization, "compile_source", _fake_compile_source)
    run_root = ROOT / "tmp" / "tests" / "stress-minimization-unit"
    shutil.rmtree(run_root, ignore_errors=True)
    source_path = run_root / "fixtures" / "case.objc3"
    source_path.parent.mkdir(parents=True, exist_ok=True)
    source_path.write_text(
        "@interface Broken\n"
        "- (i32)value;\n"
        "@end\n"
        "@implementation Broken\n"
        "@end\n",
        encoding="utf-8",
    )
    case = MinCase(
        case_id="synthetic_minimization_case",
        subsystem="parser",
        source_path=source_path,
    )
    failure_root = run_root / "failures"
    minimized_root = run_root / "minimized"

    try:
        summary = materialize_case(
            ROOT / "tmp" / "fake-objc3c.exe",
            case,
            failure_root,
            minimized_root,
            1.0,
        )

        failure_dir = failure_root / "synthetic_minimization_case"
        minimized_dir = minimized_root / "synthetic_minimization_case"
        failure_summary = json.loads((failure_dir / "failure-summary.json").read_text(encoding="utf-8"))
        stable_signature = json.loads((failure_dir / "stable-signature.json").read_text(encoding="utf-8"))
        reducer_plan = json.loads((minimized_dir / "reducer-plan.json").read_text(encoding="utf-8"))
        reduced_summary = json.loads((minimized_dir / "reduced-summary.json").read_text(encoding="utf-8"))

        assert summary["case_id"] == "synthetic_minimization_case"
        assert summary["signature_sha256"] == SIGNATURE
        assert failure_summary["signature_sha256"] == SIGNATURE
        assert stable_signature["diagnostic_lines"] == [
            "error: synthetic minimization failure O3MIN001"
        ]
        assert reducer_plan["attempt_count"] > 0
        assert reducer_plan["accepted_reduction_count"] > 0
        assert reduced_summary["reduced_bytes"] < reduced_summary["original_bytes"]
        assert (failure_dir / "source.objc3").is_file()
        assert (minimized_dir / "candidate.objc3").is_file()
    finally:
        shutil.rmtree(run_root, ignore_errors=True)
