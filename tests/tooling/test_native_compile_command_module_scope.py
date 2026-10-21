from __future__ import annotations

import shutil
import subprocess
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]


def test_compile_command_module_scope_receives_resolved_compiler_path() -> None:
    shell = shutil.which("pwsh") or shutil.which("powershell")
    if shell is None:
        pytest.skip("PowerShell is required for the native compile command module test")
    command_module = ROOT / "scripts" / "objc3c_native_compile_command.psm1"
    fixture = ROOT / "tests" / "tooling" / "fixtures" / "native" / "synthesized_accessor_property_lowering_positive.objc3"
    compiler_identity_file = fixture
    script = r"""
$ErrorActionPreference = "Stop"
Import-Module $env:OBJC3C_TEST_COMMAND_MODULE -Force -DisableNameChecking
$result = New-Objc3cNativeCompileCommand `
  -RepoRoot $env:OBJC3C_TEST_ROOT `
  -ParsedArgs ([pscustomobject]@{}) `
  -EffectiveCompileArgs @($env:OBJC3C_TEST_FIXTURE, "--emit-ir") `
  -WrapperScriptPath (Join-Path $env:OBJC3C_TEST_ROOT "scripts/objc3c_native_compile.ps1") `
  -CompilerBinaryPath $env:OBJC3C_TEST_COMPILER_IDENTITY
if ($result.input_path -ne [System.IO.Path]::GetFullPath($env:OBJC3C_TEST_FIXTURE)) { throw "input path mismatch" }
if ($null -eq $result.cache_context) { throw "cache context was not constructed" }
"""
    env = {
        **__import__("os").environ,
        "OBJC3C_TEST_COMMAND_MODULE": str(command_module),
        "OBJC3C_TEST_ROOT": str(ROOT),
        "OBJC3C_TEST_FIXTURE": str(fixture),
        "OBJC3C_TEST_COMPILER_IDENTITY": str(compiler_identity_file),
    }
    completed = subprocess.run(
        [shell, "-NoProfile", "-Command", script],
        cwd=ROOT,
        env=env,
        capture_output=True,
        text=True,
        check=False,
    )
    assert completed.returncode == 0, completed.stdout + completed.stderr
