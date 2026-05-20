from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from check_stdlib_surface_model import CanonicalModuleSurface
from objc3c_tooling.json_io import load_json_any as load_json


def _live_path_exists(root: Path, raw_path: str) -> bool:
    path_text = raw_path.split("#", 1)[0]
    return (root / path_text).exists()


@dataclass(frozen=True)
class ArchitectureValidation:
    api_families: dict[str, Any]
    required_exports: dict[str, Any]
    advanced_required_exports: dict[str, Any]


def validate_architecture_surfaces(
    *,
    root: Path,
    core_architecture: dict[str, Any],
    advanced_architecture: dict[str, Any],
    module_surfaces: list[CanonicalModuleSurface],
    advanced_api_families: object,
) -> tuple[str | None, ArchitectureValidation | None]:
    architecture_live_paths = core_architecture.get("live_paths")
    if not isinstance(architecture_live_paths, list) or not architecture_live_paths:
        return "core architecture missing live_paths", None
    for raw_path in architecture_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return "core architecture live_paths entry malformed", None
        if not _live_path_exists(root, raw_path):
            return f"core architecture live path missing: {raw_path}", None

    advanced_live_paths = advanced_architecture.get("live_paths")
    if not isinstance(advanced_live_paths, list) or not advanced_live_paths:
        return "advanced architecture missing live_paths", None
    for raw_path in advanced_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return "advanced architecture live_paths entry malformed", None
        if not _live_path_exists(root, raw_path):
            return f"advanced architecture live path missing: {raw_path}", None

    architecture_api_families = core_architecture.get("api_families")
    if not isinstance(architecture_api_families, dict) or not architecture_api_families:
        return "core architecture missing api_families", None
    inventory_modules_by_name = {module_surface.module: module_surface for module_surface in module_surfaces}
    for module_name, families in architecture_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return f"core architecture referenced unknown module {module_name}", None
        if not isinstance(families, list) or not families:
            return f"core architecture api_families malformed for {module_name}", None
        manifest_payload = load_json(root / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("api_families") != families:
            return f"module manifest api_families drifted for {module_name}", None
    architecture_required_exports = core_architecture.get("required_exports")
    if not isinstance(architecture_required_exports, dict) or not architecture_required_exports:
        return "core architecture missing required_exports", None
    for module_name, required_exports in architecture_required_exports.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return f"core architecture required_exports referenced unknown module {module_name}", None
        if not isinstance(required_exports, list) or not required_exports:
            return f"core architecture required_exports malformed for {module_name}", None
        manifest_payload = load_json(root / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("exports") != required_exports:
            return f"module manifest exports drifted for {module_name}", None

    if not isinstance(advanced_api_families, dict) or not advanced_api_families:
        return "advanced architecture missing api_families", None
    for module_name, families in advanced_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return f"advanced architecture referenced unknown module {module_name}", None
        if not isinstance(families, list) or not families:
            return f"advanced architecture api_families malformed for {module_name}", None
        manifest_payload = load_json(root / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("api_families") != families:
            return f"module manifest advanced api_families drifted for {module_name}", None

    advanced_required_exports = advanced_architecture.get("required_exports")
    if not isinstance(advanced_required_exports, dict) or not advanced_required_exports:
        return "advanced architecture missing required_exports", None
    for module_name, required_exports in advanced_required_exports.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return f"advanced architecture required_exports referenced unknown module {module_name}", None
        if not isinstance(required_exports, list) or not required_exports:
            return f"advanced architecture required_exports malformed for {module_name}", None
        manifest_payload = load_json(root / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("exports") != required_exports:
            return f"module manifest advanced exports drifted for {module_name}", None

    return (
        None,
        ArchitectureValidation(
            api_families=architecture_api_families,
            required_exports=architecture_required_exports,
            advanced_required_exports=advanced_required_exports,
        ),
    )
