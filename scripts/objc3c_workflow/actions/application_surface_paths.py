"""Path constants for showcase, stdlib, application, and corpus actions."""

from __future__ import annotations

from ..environment import ROOT

SHOWCASE_SURFACE_PY = ROOT / "scripts" / "check_showcase_surface.py"
SHOWCASE_RUNTIME_PS1 = ROOT / "scripts" / "check_showcase_runtime.ps1"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
RUNNABLE_SHOWCASE_E2E_PY = ROOT / "scripts" / "check_objc3c_runnable_showcase_end_to_end.py"
GETTING_STARTED_INTEGRATION_PY = ROOT / "scripts" / "check_getting_started_integration.py"

CONFORMANCE_CORPUS_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_conformance_corpus_integration.py"
)
PUBLIC_CONFORMANCE_SUITE_MANIFEST_PY = (
    ROOT / "scripts" / "check_objc3c_public_conformance_suite_manifest.py"
)
CONFORMANCE_MINIMA_PS1 = ROOT / "scripts" / "check_conformance_suite.ps1"
RUNNABLE_CONFORMANCE_CORPUS_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_conformance_corpus_end_to_end.py"
)

STDLIB_SURFACE_PY = ROOT / "scripts" / "check_stdlib_surface.py"
MATERIALIZE_STDLIB_PY = ROOT / "scripts" / "materialize_objc3c_stdlib_workspace.py"
STDLIB_FOUNDATION_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_stdlib_foundation_integration.py"
)
STDLIB_ADVANCED_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_stdlib_advanced_integration.py"
)
STDLIB_PROGRAM_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
)
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
APPLICATION_FRAMEWORK_SAMPLES_PY = (
    ROOT / "scripts" / "check_objc3c_application_framework_samples.py"
)
RUNNABLE_APPLICATION_ARCHITECTURE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_application_architecture_end_to_end.py"
)
