from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path

import pytest


ROOT = Path(__file__).resolve().parents[2]


def _powershell() -> str | None:
    return shutil.which("pwsh") or shutil.which("powershell")


def _run_powershell(script: str) -> dict[str, str]:
    shell = _powershell()
    if shell is None:
        pytest.skip("PowerShell is not available")

    result = subprocess.run(
        [shell, "-NoLogo", "-NoProfile", "-Command", script],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    return json.loads(result.stdout)


def test_execution_smoke_case_context_uses_platform_executable_name(tmp_path: Path) -> None:
    run_dir = tmp_path / "run"
    fixture = ROOT / "tests" / "native" / "parser" / "positive" / "canonical_module_main.objc3"
    script = f"""
$ErrorActionPreference = 'Stop'
Import-Module '{ROOT / "scripts" / "objc3c_native_execution_smoke_runner" / "case_context.psm1"}' -Force -DisableNameChecking
$fixture = Get-Item -LiteralPath '{fixture}'
$context = [pscustomobject]@{{
  repo_root = '{ROOT}'
  run_dir = '{run_dir}'
  case_executable_name = 'module'
}}
$case = New-ExecutionSmokeCaseContext -Fixture $fixture -Context $context -Kind 'positive'
[ordered]@{{
  exe_name = $case.exe_name
  exe_leaf = Split-Path -Leaf $case.exe_path
}} | ConvertTo-Json -Depth 4
"""
    payload = _run_powershell(script)

    assert payload == {"exe_name": "module", "exe_leaf": "module"}


def test_execution_smoke_case_context_preserves_legacy_default_name(tmp_path: Path) -> None:
    run_dir = tmp_path / "run"
    fixture = ROOT / "tests" / "native" / "parser" / "positive" / "canonical_module_main.objc3"
    script = f"""
$ErrorActionPreference = 'Stop'
Import-Module '{ROOT / "scripts" / "objc3c_native_execution_smoke_runner" / "case_context.psm1"}' -Force -DisableNameChecking
$fixture = Get-Item -LiteralPath '{fixture}'
$context = [pscustomobject]@{{
  repo_root = '{ROOT}'
  run_dir = '{run_dir}'
}}
$case = New-ExecutionSmokeCaseContext -Fixture $fixture -Context $context -Kind 'positive'
[ordered]@{{
  exe_name = $case.exe_name
  exe_leaf = Split-Path -Leaf $case.exe_path
}} | ConvertTo-Json -Depth 4
"""
    payload = _run_powershell(script)

    assert payload == {"exe_name": "module.exe", "exe_leaf": "module.exe"}


def test_execution_smoke_platform_models_use_host_native_executable_names() -> None:
    config = (
        ROOT / "scripts" / "objc3c_native_execution_smoke_runner" / "config.psm1"
    ).read_text(encoding="utf-8")
    evidence_producers = (
        ROOT / "scripts" / "objc3c_platform_host_evidence_producers.psm1"
    ).read_text(encoding="utf-8")

    assert '$caseExecutableName = if ($isWindows) { "module.exe" } else { "module" }' in config
    assert (
        '$executableName = if ($PlatformId -in @("darwin-arm64", "linux-x64")) '
        '{ "module" } else { "module.exe" }'
    ) in evidence_producers
    assert (
        "Get-Objc3cRuntimeLoadProbeExecutablePath -RepoRoot $RepoRoot "
        "-Result $result -PlatformId $PlatformId"
    ) in evidence_producers


def test_execution_smoke_orchestration_writes_failed_summary_before_rethrow() -> None:
    summary = (
        ROOT / "scripts" / "objc3c_native_execution_smoke_runner" / "summary.psm1"
    ).read_text(encoding="utf-8")
    orchestration = (
        ROOT
        / "scripts"
        / "objc3c_native_execution_smoke_runner"
        / "orchestration.psm1"
    ).read_text(encoding="utf-8")

    assert "function Write-FailedExecutionSmokeSummary" in summary
    assert "New-ExecutionSmokeFailureResult" in summary
    assert "StatusOverride \"FAIL\"" in summary
    assert "passed = $false" in summary
    assert "Write-FailedExecutionSmokeSummary" in orchestration
    assert "-ErrorRecord $_" in orchestration
    assert "throw" in orchestration


def _native_execution_platform_model() -> dict[str, object]:
    return {
        "platform_id": "windows-x64",
        "platform_ids": ["windows-x64"],
        "host_promotion_state": "supported",
        "target_triple": "x86_64-pc-windows-msvc",
        "native_executable": "artifacts/bin/objc3c.exe",
        "object_artifact": "module.obj",
        "object_file_extension": ".obj",
        "object_format": "coff",
        "debug_format": "codeview",
        "runtime_library_relative_path": "artifacts/lib/objc3rt.lib",
        "runtime_library_kind": "static-library",
        "runtime_library_names": ["objc3rt.lib"],
        "shared_runtime": False,
        "runtime_load_path": [],
        "runtime_load_environment_variable": "",
        "loader_path_policy": "PATH-owned loader resolution for supported Windows package roots",
    }


def test_hosted_normalization_distinguishes_failed_summary_from_missing(
    tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    from scripts.objc3c_workflow.actions import test_orchestration_native as native

    run_id = "failed-run"
    artifact_root = tmp_path / "artifacts"
    failed_summary = artifact_root / run_id / "summary.json"
    failed_summary.parent.mkdir(parents=True)
    failed_summary.write_text(
        json.dumps(
            {
                "status": "FAIL",
                "results": [
                    {
                        "runtime_library": "artifacts/lib/objc3rt.lib",
                        "driver_linker_flags": ["-lobjc3rt"],
                        "passed": False,
                    }
                ],
            }
        ),
        encoding="utf-8",
    )
    monkeypatch.setattr(native, "ROOT", tmp_path)
    monkeypatch.setattr(native, "NATIVE_EXECUTION_SMOKE_ARTIFACT_ROOT", artifact_root)
    monkeypatch.setattr(
        native, "native_execution_platform_model", _native_execution_platform_model
    )

    failed = native.normalize_native_execution_summary(run_id, 1, "FAIL")
    missing = native.normalize_native_execution_summary("missing-run", 1, "FAIL")

    assert failed["source_summary_present"] is True
    assert failed["native_object_emission"] is False
    assert failed["skip_reason"] == "native-execution-smoke-summary-failed"
    assert (
        native.native_execution_object_emission_status(failed)
        == "native_execution_summary_failed"
    )
    assert native.hosted_execution_effective_exit_code(run_id, 0) == 1
    assert missing["source_summary_present"] is False
    assert missing["skip_reason"] == "native-execution-smoke-summary-missing"
    assert (
        native.native_execution_object_emission_status(missing)
        == "native_execution_summary_missing"
    )
