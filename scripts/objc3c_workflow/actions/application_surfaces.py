"""Showcase, stdlib, application, and corpus workflow actions."""

from __future__ import annotations

import sys

from ..commands import pwsh_file, run
from ..environment import ROOT

SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
GETTING_STARTED_INTEGRATION_PY = ROOT / "scripts" / "check_getting_started_integration.py"
CONFORMANCE_CORPUS_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_conformance_corpus_integration.py"
CONFORMANCE_MINIMA_PS1 = ROOT / "scripts" / "check_conformance_suite.ps1"
RUNNABLE_CONFORMANCE_CORPUS_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_conformance_corpus_end_to_end.py"
)
STDLIB_SURFACE_PY = ROOT / "scripts" / "check_stdlib_surface.py"
MATERIALIZE_STDLIB_PY = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
STDLIB_FOUNDATION_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_foundation_integration.py"
STDLIB_ADVANCED_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_advanced_integration.py"
STDLIB_PROGRAM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
RUNNABLE_STDLIB_FOUNDATION_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_stdlib_foundation_end_to_end.py"
)
RUNNABLE_STDLIB_ADVANCED_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_stdlib_advanced_end_to_end.py"
)
RUNNABLE_STDLIB_PROGRAM_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_stdlib_program_end_to_end.py"
)
CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY = (
    ROOT / "scripts" / "materialize_objc3c_canonical_application_workspace.py"
)
APPLICATION_ARCHITECTURE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_application_architecture_integration.py"
)
RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_application_architecture_end_to_end.py"
)


def action_check_showcase_surface(rest: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_SURFACE_PY), *rest])


def action_check_stdlib_surface(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_SURFACE_PY)])


def action_materialize_stdlib_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(MATERIALIZE_STDLIB_PY), *rest])


def action_materialize_canonical_application_workspace(rest: list[str]) -> int:
    return run([sys.executable, str(CANONICAL_APPLICATION_WORKSPACE_MATERIALIZER_PY), *rest])


def action_validate_showcase_runtime(rest: list[str]) -> int:
    return pwsh_file(SHOWCASE_RUNTIME_PS1, *rest)


def action_validate_showcase(_: list[str]) -> int:
    return run([sys.executable, str(SHOWCASE_INTEGRATION_PY)])


def action_validate_runnable_showcase(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_SHOWCASE_E2E_PY)])


def action_validate_getting_started(_: list[str]) -> int:
    return run([sys.executable, str(GETTING_STARTED_INTEGRATION_PY)])


def action_validate_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(CONFORMANCE_CORPUS_INTEGRATION_PY)])


def action_check_conformance_minima(_: list[str]) -> int:
    return pwsh_file(CONFORMANCE_MINIMA_PS1)


def action_validate_runnable_conformance_corpus(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_CONFORMANCE_CORPUS_E2E_PY)])


def action_validate_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_FOUNDATION_INTEGRATION_PY)])


def action_validate_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_ADVANCED_INTEGRATION_PY)])


def action_validate_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(STDLIB_PROGRAM_INTEGRATION_PY)])


def action_validate_runnable_stdlib_foundation(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_FOUNDATION_E2E_PY)])


def action_validate_runnable_stdlib_advanced(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_ADVANCED_E2E_PY)])


def action_validate_runnable_stdlib_program(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_STDLIB_PROGRAM_E2E_PY)])


def action_validate_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(APPLICATION_ARCHITECTURE_INTEGRATION_PY)])


def action_validate_runnable_application_architecture(_: list[str]) -> int:
    return run([sys.executable, str(RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY)])
