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
            "action_check_documentation_surface",
            "action_check_public_command_budget",
            "action_validate_documentation_surface",
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
}

FORBIDDEN_FACADE_IMPLEMENTATION_IMPORTS = (
    "from ..commands import run",
    "from ..commands import pwsh_file",
    "from ..environment import",
    "import sys",
)
RETIRED_REGISTRY_IMPORTS = (
    "from ..registry import",
    "from scripts.objc3c_workflow.registry import",
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
