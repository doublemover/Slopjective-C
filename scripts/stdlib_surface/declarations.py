from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json


@dataclass(frozen=True)
class StdlibSurfaceDeclarations:
    advanced_helper_modules: Any
    capability_demo_examples: Any


def validate_program_declarations(
    *,
    root: Path,
    advanced_helper_package_surface: dict[str, Any],
    program_surface: dict[str, Any],
    inventory_module_names: set[str],
) -> tuple[str | None, StdlibSurfaceDeclarations | None]:
    capability_demo_examples = program_surface.get("capability_demo_examples")
    if not isinstance(capability_demo_examples, list) or len(capability_demo_examples) != 3:
        return "program surface missing capability_demo_examples", None
    showcase_portfolio = load_json(root / str(program_surface["showcase_portfolio"]))
    showcase_examples = showcase_portfolio.get("examples")
    if not isinstance(showcase_examples, list):
        return "showcase portfolio examples are malformed", None
    showcase_examples_by_id = {
        str(entry.get("id")): entry
        for entry in showcase_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    for entry in capability_demo_examples:
        if not isinstance(entry, dict):
            return "program surface published a malformed capability demo entry", None
        demo_id = entry.get("id")
        source = entry.get("source")
        workspace_manifest = entry.get("workspace_manifest")
        stdlib_followup_modules = entry.get("stdlib_followup_modules")
        story_capabilities = entry.get("story_capabilities")
        if not isinstance(demo_id, str) or not demo_id:
            return "program surface capability demo id is malformed", None
        if not isinstance(source, str) or not source:
            return f"program surface source is malformed for {demo_id}", None
        if not isinstance(workspace_manifest, str) or not workspace_manifest:
            return f"program surface workspace_manifest is malformed for {demo_id}", None
        if not isinstance(stdlib_followup_modules, list) or not all(
            isinstance(value, str) and value for value in stdlib_followup_modules
        ):
            return f"program surface stdlib_followup_modules are malformed for {demo_id}", None
        if not isinstance(story_capabilities, list) or not all(
            isinstance(value, str) and value for value in story_capabilities
        ):
            return f"program surface story_capabilities are malformed for {demo_id}", None
        if not (root / source).is_file():
            return f"program surface source path missing for {demo_id}", None
        if not (root / workspace_manifest).is_file():
            return f"program surface workspace manifest missing for {demo_id}", None
        showcase_entry = showcase_examples_by_id.get(demo_id)
        if showcase_entry is None:
            return f"program surface referenced unknown showcase example {demo_id}", None
        if showcase_entry.get("source") != source:
            return f"program surface source drifted for {demo_id}", None
        if showcase_entry.get("workspace_manifest") != workspace_manifest:
            return f"program surface workspace_manifest drifted for {demo_id}", None
        if showcase_entry.get("stdlib_followup_modules") != stdlib_followup_modules:
            return f"program surface stdlib_followup_modules drifted for {demo_id}", None
        if showcase_entry.get("story_capabilities") != story_capabilities:
            return f"program surface story_capabilities drifted for {demo_id}", None
        for module_name in stdlib_followup_modules:
            if module_name not in inventory_module_names:
                return f"program surface referenced unknown stdlib module {module_name} for {demo_id}", None

    advanced_helper_modules = advanced_helper_package_surface.get("advanced_helper_modules")
    if not isinstance(advanced_helper_modules, list) or len(advanced_helper_modules) != 3:
        return "advanced helper package surface missing advanced_helper_modules", None
    for entry in advanced_helper_modules:
        if not isinstance(entry, dict):
            return "advanced helper package surface published a malformed module entry", None
        canonical_module = entry.get("canonical_module")
        implementation_module = entry.get("implementation_module")
        manifest = entry.get("manifest")
        source = entry.get("source")
        smoke_source = entry.get("smoke_source")
        if not all(
            isinstance(value, str) and value
            for value in (canonical_module, implementation_module, manifest, source, smoke_source)
        ):
            return "advanced helper package surface published a malformed module entry", None

    return (
        None,
        StdlibSurfaceDeclarations(
            advanced_helper_modules=advanced_helper_modules,
            capability_demo_examples=capability_demo_examples,
        ),
    )
