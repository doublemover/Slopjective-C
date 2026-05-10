"""Application architecture integration contracts and stable report paths."""

from __future__ import annotations

from objc3c_tooling.paths import ROOT


RUNNER_PATH = "scripts/check_objc3c_application_architecture_integration.py"
TEMPLATE_HARNESS_PY = ROOT / "scripts" / "check_application_architecture_template_harness.py"
CANONICAL_WORKSPACE_PY = ROOT / "scripts" / "materialize_objc3c_canonical_application_workspace.py"
SHOWCASE_INTEGRATION_PY = ROOT / "scripts" / "check_showcase_integration.py"
STDLIB_PROGRAM_INTEGRATION_PY = ROOT / "scripts" / "check_objc3c_stdlib_program_integration.py"
TEMPLATE_SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "application-architecture-testing"
    / "template-harness-summary.json"
)
CANONICAL_SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "application-architecture-testing"
    / "canonical-application-workspace-summary.json"
)
SHOWCASE_SUMMARY_PATH = ROOT / "tmp" / "reports" / "showcase" / "integration-summary.json"
STDLIB_PROGRAM_SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "program-integration-summary.json"
REPORT_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "application-architecture-testing"
    / "runnable-template-canonical-app-summary.json"
)

INTEGRATION_CONTRACT_ID = "objc3c.application.architecture.testing.integration.summary.v1"
WORKFLOW_ACTIONS = (
    "materialize-project-template",
    "materialize-canonical-application-workspace",
    "validate-showcase",
    "validate-stdlib-program",
    "validate-application-architecture",
)
CHILD_SUMMARY_PATHS = (
    TEMPLATE_SUMMARY_PATH,
    CANONICAL_SUMMARY_PATH,
    SHOWCASE_SUMMARY_PATH,
    STDLIB_PROGRAM_SUMMARY_PATH,
)
