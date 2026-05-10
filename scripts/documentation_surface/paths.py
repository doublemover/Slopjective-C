from __future__ import annotations

from pathlib import Path

CHECKER_NAME = "documentation-surface"
ROOT = Path(__file__).resolve().parents[2]
DOCUMENTATION_SURFACE_OWNER = "documentation-surface.owner"
DOCUMENTATION_SURFACE_OWNER_SURFACE = "scripts/check_documentation_surface_model.py"
DOCUMENTATION_SURFACE_BLOCKER_METADATA = {
    "blocker_contract": "hard-cutover-docs-surface-fail-closed",
    "blocker_scope": "reader-facing-docs-and-machine-appendix-boundary",
    "blocker_owner": DOCUMENTATION_SURFACE_OWNER,
    "blocker_owner_surface": DOCUMENTATION_SURFACE_OWNER_SURFACE,
}

README_PATH = ROOT / "README.md"
CONTRIBUTING_PATH = ROOT / "CONTRIBUTING.md"
SHOWCASE_README_PATH = ROOT / "showcase" / "README.md"
SITE_BODY_PATH = ROOT / "site" / "src" / "index.body.md"
SITE_POLICY_PATH = ROOT / "site" / "src" / "README.md"
NATIVE_OWNERSHIP_PATH = ROOT / "docs" / "objc3c-native" / "src" / "OWNERSHIP.md"
NATIVE_FRAGMENT_README_PATH = ROOT / "docs" / "objc3c-native" / "src" / "README.md"
MAINTAINER_WORKFLOW_PATH = ROOT / "docs" / "runbooks" / "objc3c_maintainer_workflows.md"
DEVELOPER_TOOLING_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_developer_tooling.md"
BONUS_EXPERIENCES_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_bonus_experiences.md"
PERFORMANCE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_performance.md"
RUNTIME_PERFORMANCE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_runtime_performance.md"
COMPILER_THROUGHPUT_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_compiler_throughput.md"
STDLIB_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_foundation.md"
STDLIB_CORE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_core.md"
STDLIB_ADVANCED_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_advanced.md"
STDLIB_README_PATH = ROOT / "stdlib" / "README.md"
STDLIB_PROGRAM_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_program.md"
PUBLIC_COMMAND_SURFACE_PATH = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
TUTORIAL_README_PATH = ROOT / "docs" / "tutorials" / "README.md"
GETTING_STARTED_PATH = ROOT / "docs" / "tutorials" / "getting_started.md"
BUILD_RUN_VERIFY_PATH = ROOT / "docs" / "tutorials" / "build_run_verify.md"
GUIDED_WALKTHROUGH_PATH = ROOT / "docs" / "tutorials" / "guided_walkthrough.md"
MIGRATION_GUIDE_PATH = ROOT / "docs" / "tutorials" / "objc2_to_objc3_migration.md"
COMPARISON_README_PATH = ROOT / "docs" / "tutorials" / "objc2_swift_cpp_comparison.md"
