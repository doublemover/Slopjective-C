from __future__ import annotations

import importlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WORKFLOW_ROOT = ROOT / "scripts" / "objc3c_workflow"
ACTION_ROOT = WORKFLOW_ROOT / "actions"

OWNER_FACADES = {
    "docs": {
        "owners": ("docs_documentation", "docs_paths", "docs_public_commands"),
        "exports": (
            "action_build_site",
            "action_check_public_command_budget",
            "action_validate_umbrella_readiness",
        ),
        "handlers": (WORKFLOW_ROOT / "action_handlers_core.py",),
    },
    "hygiene": {
        "owners": ("hygiene_composites", "hygiene_paths", "hygiene_source_policy"),
        "exports": (
            "action_lint",
            "action_check_source_hygiene_hard_cutover",
            "action_validate_repo_superclean",
        ),
        "handlers": (WORKFLOW_ROOT / "action_handlers_core.py",),
    },
    "native_build": {
        "owners": ("native_build_binaries", "native_build_package", "native_build_paths"),
        "exports": (
            "action_build_native_binaries",
            "action_build_native_contracts",
            "action_compile_objc3c",
            "action_package_runnable_toolchain",
        ),
        "handlers": (WORKFLOW_ROOT / "action_handlers_core.py",),
    },
    "schema_surfaces": {
        "owners": ("schema_surfaces_checks", "schema_surfaces_paths"),
        "exports": (
            "action_check_public_conformance_schema_surface",
            "action_check_release_foundation_schema_surface",
            "action_check_security_hardening_schema_surface",
        ),
        "handlers": (WORKFLOW_ROOT / "action_handlers_reporting_release.py",),
    },
    "ecosystem_publication": {
        "owners": (
            "ecosystem_publication_adoption",
            "ecosystem_publication_contracts",
            "ecosystem_publication_governance",
            "ecosystem_publication_metadata",
            "ecosystem_publication_operations",
            "ecosystem_publication_package",
            "ecosystem_publication_planning",
        ),
        "exports": (
            "action_build_package_lock",
            "action_validate_package_authoring",
            "action_validate_package_mirror",
            "action_validate_package_ecosystem",
            "action_validate_runnable_package_ecosystem",
            "action_validate_long_horizon_operations",
            "action_publish_long_horizon_operations",
            "action_validate_adoption_legibility",
            "action_publish_adoption_legibility",
            "action_validate_governance_sustainability",
            "action_publish_governance_sustainability",
            "action_publish_planning_issues",
            "action_check_planning_publication_drift",
        ),
        "handlers": (WORKFLOW_ROOT / "action_handlers_tooling_developer.py",),
    },
    "performance": {
        "owners": (
            "performance_artifacts",
            "performance_compiler_throughput",
            "performance_foundation",
            "performance_governance_policy",
            "performance_governance_workflow",
            "performance_metrics",
            "performance_runnable",
            "performance_runtime",
            "performance_scenarios",
        ),
        "exports": (
            "action_benchmark_comparative_baselines",
            "action_benchmark_compiler_throughput",
            "action_benchmark_performance",
            "action_benchmark_runtime_inspector",
            "action_benchmark_runtime_performance",
            "action_build_performance_dashboard",
            "action_check_performance_governance_schema_surface",
            "action_check_performance_governance_surface",
            "action_publish_performance_report",
            "action_validate_compiler_throughput",
            "action_validate_performance_foundation",
            "action_validate_performance_governance",
            "action_validate_performance_governance_end_to_end",
            "action_validate_performance_governance_integration",
            "action_validate_runnable_compiler_throughput",
            "action_validate_runnable_performance",
            "action_validate_runnable_runtime_performance",
            "action_validate_runtime_performance",
        ),
        "handlers": (
            WORKFLOW_ROOT / "action_handlers_performance.py",
            WORKFLOW_ROOT / "action_handlers_reporting_public_performance.py",
        ),
    },
}

FORBIDDEN_FACADE_IMPLEMENTATION_IMPORTS = (
    "from ..commands import run",
    "from ..commands import pwsh_file",
    "from ..environment import",
    "import sys",
)
RETIRED_REGISTRY_IMPORTS = (
    "from ..registry import",
    "from scripts.objc3c_workflow."
    "registry import",
    "ACTION_SPECS",
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_workflow_action_facades_delegate_to_owner_modules() -> None:
    for facade_name, contract in OWNER_FACADES.items():
        facade_path = ACTION_ROOT / f"{facade_name}.py"
        facade_text = _read(facade_path)

        for owner in contract["owners"]:
            owner_path = ACTION_ROOT / f"{owner}.py"
            assert owner_path.is_file(), owner_path.relative_to(ROOT).as_posix()
            assert f"from .{owner} import" in facade_text

        for forbidden in FORBIDDEN_FACADE_IMPLEMENTATION_IMPORTS:
            assert forbidden not in facade_text
        for retired in RETIRED_REGISTRY_IMPORTS:
            assert retired not in facade_text


def test_workflow_handlers_route_through_thin_facades() -> None:
    for facade_name, contract in OWNER_FACADES.items():
        for handler_path in contract["handlers"]:
            handler_text = _read(handler_path)
            assert f"{facade_name}," in handler_text
            for owner in contract["owners"]:
                assert owner not in handler_text


def test_workflow_facades_preserve_public_export_surface() -> None:
    for facade_name, contract in OWNER_FACADES.items():
        module = importlib.import_module(f"scripts.objc3c_workflow.actions.{facade_name}")
        exported = set(getattr(module, "__all__"))
        for public_name in contract["exports"]:
            assert public_name in exported
            assert hasattr(module, public_name)
