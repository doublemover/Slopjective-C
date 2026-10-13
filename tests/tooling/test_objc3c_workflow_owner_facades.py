from __future__ import annotations

import ast
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
            "action_check_native_docs",
        ),
        "handlers": (
            WORKFLOW_ROOT / "action_handlers_core_site_docs.py",
            WORKFLOW_ROOT / "action_handlers_core_markdown.py",
        ),
    },
    "hygiene": {
        "owners": ("hygiene_composites", "hygiene_paths", "hygiene_source_policy"),
        "exports": (
            "action_lint",
            "action_check_source_hygiene_hard_cutover",
            "action_validate_repo_superclean",
        ),
        "handlers": (
            WORKFLOW_ROOT / "action_handlers_core_linting.py",
            WORKFLOW_ROOT / "action_handlers_core_source_hygiene.py",
            WORKFLOW_ROOT / "action_handlers_core_superclean.py",
            WORKFLOW_ROOT / "action_handlers_tooling_hygiene.py",
        ),
    },
    "native_build": {
        "owners": ("native_build_binaries", "native_build_package", "native_build_paths"),
        "exports": (
            "action_build_native_binaries",
            "action_build_native_contracts",
            "action_compile_objc3c",
            "action_package_runnable_toolchain",
        ),
        "handlers": (
            WORKFLOW_ROOT / "action_handlers_core_build.py",
            WORKFLOW_ROOT / "action_handlers_defaults.py",
        ),
    },
    "schema_surfaces": {
        "owners": ("schema_surfaces_checks", "schema_surfaces_paths"),
        "exports": (
            "action_check_public_conformance_schema_surface",
            "action_check_release_foundation_schema_surface",
            "action_check_security_hardening_schema_surface",
        ),
        "handlers": (
            WORKFLOW_ROOT / "action_handlers_reporting_public_performance.py",
            WORKFLOW_ROOT / "action_handlers_release_foundation.py",
            WORKFLOW_ROOT / "action_handlers_packaging_channels.py",
            WORKFLOW_ROOT / "action_handlers_distribution_credibility.py",
        ),
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
        "handlers": (WORKFLOW_ROOT / "action_handlers_ecosystem_publication.py",),
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
            handler_tree = ast.parse(handler_text)
            assert any(
                isinstance(node, ast.ImportFrom)
                and node.module == "scripts.objc3c_workflow.actions"
                and any(alias.name == facade_name for alias in node.names)
                for node in ast.walk(handler_tree)
            ), handler_path.relative_to(ROOT).as_posix()
            assert any(
                isinstance(node, ast.Attribute)
                and isinstance(node.value, ast.Name)
                and node.value.id == facade_name
                and node.attr.startswith("action_")
                for node in ast.walk(handler_tree)
            ), handler_path.relative_to(ROOT).as_posix()
            for owner in contract["owners"]:
                owner_module = f"scripts.objc3c_workflow.actions.{owner}"
                assert not any(
                    (isinstance(node, ast.ImportFrom)
                     and (node.module == owner_module
                          or (node.module == "scripts.objc3c_workflow.actions"
                              and any(alias.name == owner for alias in node.names))))
                    or (isinstance(node, ast.Import)
                        and any(alias.name == owner_module for alias in node.names))
                    for node in ast.walk(handler_tree)
                ), handler_path.relative_to(ROOT).as_posix()


def test_workflow_facades_preserve_public_export_surface() -> None:
    for facade_name, contract in OWNER_FACADES.items():
        module = importlib.import_module(f"scripts.objc3c_workflow.actions.{facade_name}")
        exported = set(getattr(module, "__all__"))
        for public_name in contract["exports"]:
            assert public_name in exported
            assert hasattr(module, public_name)


def test_umbrella_validation_public_route_preserves_direct_documentation_owner() -> None:
    owner = importlib.import_module("scripts.objc3c_workflow.actions.docs_documentation")
    handlers = importlib.import_module(
        "scripts.objc3c_workflow.action_handlers_core_documentation_validation"
    )
    core = importlib.import_module("scripts.objc3c_workflow.action_handlers_core")
    action = owner.VALIDATE_UMBRELLA_READINESS_ACTION
    assert handlers.CORE_DOCUMENTATION_VALIDATION_ACTION_HANDLERS[action] is (
        owner.action_validate_umbrella_readiness
    )
    assert core.CORE_ACTION_HANDLERS[action] is owner.action_validate_umbrella_readiness
