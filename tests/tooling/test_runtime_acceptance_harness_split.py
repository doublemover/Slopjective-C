from __future__ import annotations

from pathlib import Path

from scripts.objc3c_runtime_acceptance import process_execution

ROOT = Path(__file__).resolve().parents[2]
ACCEPTANCE_ROOT = ROOT / "scripts" / "objc3c_runtime_acceptance"


class RequiresElevationOSError(OSError):
    @property
    def winerror(self) -> int:
        return process_execution.ELEVATION_REQUIRED_WINERROR


def test_runtime_acceptance_harness_owns_requested_boundaries() -> None:
    expected_boundaries = (
        ACCEPTANCE_ROOT / "registry.py",
        ACCEPTANCE_ROOT / "commands.py",
        ACCEPTANCE_ROOT / "probes" / "__init__.py",
        ACCEPTANCE_ROOT / "probes" / "compilation.py",
        ACCEPTANCE_ROOT / "probes" / "execution.py",
        ACCEPTANCE_ROOT / "probes" / "parsing.py",
        ACCEPTANCE_ROOT / "probes" / "retry.py",
        ACCEPTANCE_ROOT / "cases" / "__init__.py",
        ACCEPTANCE_ROOT / "cases" / "catalog.py",
        ACCEPTANCE_ROOT / "cases" / "domain_registry.py",
        ACCEPTANCE_ROOT / "cases" / "factory_ordering.py",
        ACCEPTANCE_ROOT / "surfaces" / "__init__.py",
        ACCEPTANCE_ROOT / "surfaces" / "claim_boundary.py",
        ACCEPTANCE_ROOT / "surfaces" / "suite_surface.py",
    )
    retired_flat_modules = (
        ACCEPTANCE_ROOT / "case_catalog.py",
        ACCEPTANCE_ROOT / "case_factories.py",
        ACCEPTANCE_ROOT / "probes.py",
        ACCEPTANCE_ROOT / "runtime_artifact_registry.py",
        ACCEPTANCE_ROOT / "runtime_contract_commands.py",
        ACCEPTANCE_ROOT / "surfaces.py",
    )

    for boundary in expected_boundaries:
        assert boundary.exists(), boundary
    for retired_module in retired_flat_modules:
        assert not retired_module.exists(), retired_module


def test_runtime_acceptance_process_launch_normalizes_winerror_740(
    monkeypatch,
    tmp_path: Path,
) -> None:
    command = ["runtime-probe.exe", "--json"]

    def raise_elevation_error(*_: object, **__: object) -> object:
        raise RequiresElevationOSError("The requested operation requires elevation")

    monkeypatch.setattr(process_execution.subprocess, "run", raise_elevation_error)

    result = process_execution.run_command(command, cwd=tmp_path)

    assert result.args == command
    assert result.returncode == process_execution.ELEVATION_REQUIRED_EXIT_CODE
    assert result.stdout == ""
    assert "WinError 740" in result.stderr
    assert "ERROR_ELEVATION_REQUIRED" in result.stderr
    assert "runtime-probe.exe" in result.stderr
