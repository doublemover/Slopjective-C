"""Path constants for developer-tooling workflow actions."""

from __future__ import annotations

from scripts.repo_superclean_surface.paths import REPO_SUPERCLEAN_SOURCE_OF_TRUTH

from ..environment import ROOT

DEVELOPER_TOOLING_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_developer_tooling_integration.py"
)
RUNNABLE_DEVELOPER_TOOLING_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_developer_tooling_end_to_end.py"
)
EDITOR_TOOLING_SURFACE_PY = ROOT / "scripts" / "build_objc3c_editor_tooling_surface.py"
LANGUAGE_SERVICE_SURFACE_PY = ROOT / "scripts" / "build_objc3c_language_service_surface.py"
CHECK_LANGUAGE_SERVICE_PY = ROOT / "scripts" / "check_objc3c_language_service.py"
FORMAT_OBJC3C_SOURCE_PY = ROOT / "scripts" / "format_objc3c_source.py"
REWRITE_OBJC3C_SOURCE_PY = ROOT / "scripts" / "rewrite_objc3c_source.py"
CHECK_DEVELOPER_TOOLING_DIAGNOSTIC_QUALITY_PY = (
    ROOT / "scripts" / "check_developer_tooling_diagnostic_quality.py"
)
CHECK_DEVELOPER_TOOLING_EDITOR_SOURCE_TRUTH_PY = (
    ROOT / "scripts" / "check_developer_tooling_editor_source_truth.py"
)
RUNTIME_DEBUG_TRACE_PY = ROOT / "scripts" / "build_objc3c_runtime_debug_trace.py"
BONUS_EXPERIENCE_INTEGRATION_PY = (
    ROOT / "scripts" / "check_objc3c_bonus_experience_integration.py"
)
PROJECT_TEMPLATE_MATERIALIZER_PY = ROOT / "scripts" / "materialize_objc3c_project_template.py"
LLVM_CAPABILITIES_PROBE_PY = ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py"
LIBRARY_CLI_PARITY_PY = ROOT / "scripts" / "check_objc3c_library_cli_parity.py"
RUNNABLE_BONUS_EXPERIENCE_E2E_PY = (
    ROOT / "scripts" / "check_objc3c_runnable_bonus_experience_end_to_end.py"
)
FRONTEND_C_API_RUNNER_EXE = (
    ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
)
DEFAULT_DEVELOPER_TOOLING_SOURCE = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
)
DEFAULT_PLAYGROUND_SOURCE = DEFAULT_DEVELOPER_TOOLING_SOURCE
PUBLIC_WORKFLOW_REPORT_ROOT = ROOT / "tmp" / "reports" / "objc3c-public-workflow"
PLAYGROUND_ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "playground"
PLAYGROUND_REPORT_ROOT = ROOT / "tmp" / "reports" / "playground"
PLAYGROUND_WORKSPACE_CONTRACT_ID = "objc3c.playground.workspace.v1"
SHOWCASE_PORTFOLIO_JSON = ROOT / "showcase" / "portfolio.json"
SHOWCASE_TUTORIAL_WALKTHROUGH_JSON = ROOT / "showcase" / "tutorial_walkthrough.json"
HOSTED_LLVM_CAPABILITIES_SUMMARY = (
    ROOT / "tmp" / "artifacts" / "objc3c-native" / "m144" / "llvm_capabilities" / "summary.json"
)
