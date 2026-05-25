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
