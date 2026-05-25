from __future__ import annotations

import json
import os
import subprocess
from pathlib import Path

from scripts.objc3c_tooling.artifact_identity import current_host_artifact_identity


ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
NATIVE_EXE = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native"


def _compile_fixture(fixture_name: str, out_dir: Path) -> subprocess.CompletedProcess[str]:
    env = os.environ.copy()
    env.update(
        {
            "CMAKE_BUILD_PARALLEL_LEVEL": "4",
            "CL_MPCount": "4",
            "LLVM_PARALLEL_COMPILE_JOBS": "4",
            "OBJC3C_NATIVE_BUILD_PARALLELISM": "4",
        }
    )
    return subprocess.run(
        [
            str(NATIVE_EXE),
            str(FIXTURE_ROOT / fixture_name),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        env=env,
        text=True,
        capture_output=True,
        check=False,
    )


def _diagnostic_codes(out_dir: Path) -> list[str]:
    diagnostics_path = out_dir / "module.diagnostics.json"
    payload = json.loads(diagnostics_path.read_text(encoding="utf-8"))
    return [str(item.get("code", "")) for item in payload.get("diagnostics", [])]


def test_direct_native_error_control_flow_lowers_without_live_flag(tmp_path: Path) -> None:
    out_dir = tmp_path / "positive"
    result = _compile_fixture("try_do_catch_semantics_positive.objc3", out_dir)

    assert result.returncode == 0, result.stderr
    assert (out_dir / ARTIFACT_IDENTITY.module_object_artifact_name).is_file()
    assert (out_dir / "module.ll").is_file()

    manifest = json.loads((out_dir / "module.manifest.json").read_text(encoding="utf-8"))
    surface = manifest["frontend"]["pipeline"]["semantic_surface"][
        "objc_error_handling_try_do_catch_semantics"
    ]
    assert surface["native_emit_remains_fail_closed"] is False
    assert surface["ready_for_lowering_and_runtime"] is True
    assert surface["try_expression_sites"] == 3
    assert surface["throw_statement_sites"] == 1
    assert surface["do_catch_sites"] == 1


def test_direct_native_throwing_call_without_try_still_fails_closed(tmp_path: Path) -> None:
    out_dir = tmp_path / "negative"
    result = _compile_fixture("throwing_call_requires_try_negative.objc3", out_dir)

    assert result.returncode == 1
    codes = _diagnostic_codes(out_dir)
    assert "O3S341" in codes
    assert "O3S221" not in codes
