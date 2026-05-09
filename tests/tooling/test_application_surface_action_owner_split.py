from __future__ import annotations

import importlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ACTION_ROOT = ROOT / "scripts" / "objc3c_workflow" / "actions"


OWNER_FACADES = {
    "application_showcase": {
        "owners": ("application_getting_started", "application_showcase_examples"),
        "exports": (
            "action_check_showcase_surface",
            "action_validate_getting_started",
            "action_validate_runnable_showcase",
            "action_validate_showcase",
            "action_validate_showcase_runtime",
        ),
    },
    "application_stdlib": {
        "owners": (
            "application_stdlib_integrations",
            "application_stdlib_runnable",
            "application_stdlib_surface",
            "application_stdlib_workspace",
        ),
        "exports": (
            "action_check_stdlib_surface",
            "action_materialize_stdlib_workspace",
            "action_validate_runnable_stdlib_advanced",
            "action_validate_runnable_stdlib_foundation",
            "action_validate_runnable_stdlib_program",
            "action_validate_stdlib_advanced",
            "action_validate_stdlib_foundation",
            "action_validate_stdlib_program",
        ),
    },
}

FORBIDDEN_FACADE_IMPORTS = (
    "from ..commands import",
    "from .application_surface_paths import",
    "import sys",
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_application_surface_facades_delegate_to_owner_modules() -> None:
    for facade_name, contract in OWNER_FACADES.items():
        facade_text = _read(ACTION_ROOT / f"{facade_name}.py")
        for owner in contract["owners"]:
            assert (ACTION_ROOT / f"{owner}.py").is_file()
            assert f"from .{owner} import" in facade_text
        for forbidden in FORBIDDEN_FACADE_IMPORTS:
            assert forbidden not in facade_text


def test_application_surface_facades_preserve_public_exports() -> None:
    for facade_name, contract in OWNER_FACADES.items():
        module = importlib.import_module(f"scripts.objc3c_workflow.actions.{facade_name}")
        exported = set(module.__all__)
        for public_name in contract["exports"]:
            assert public_name in exported
            assert hasattr(module, public_name)
