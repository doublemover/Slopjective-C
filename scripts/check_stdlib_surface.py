#!/usr/bin/env python3
"""Validate the checked-in stdlib boundary and canonical module inventory."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_tooling.paths import repo_rel
from stdlib_surface.artifacts import validate_module_artifacts
from stdlib_surface.commands import validate_command_surfaces
from stdlib_surface.contracts import validate_document_headers
from stdlib_surface.inventory import validate_inventory_and_policy
from stdlib_surface.semantics import validate_semantic_policy

from check_stdlib_surface_model import (
    StdlibSurfaceDocuments,
    StdlibSurfacePaths,
    StdlibSurfaceReport,
    write_stdlib_surface_report,
)


PATHS = StdlibSurfacePaths.from_script(Path(__file__))
ROOT = PATHS.root


def fail(message: str) -> int:
    print(f"stdlib-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def main() -> int:
    for surface_input in PATHS.required_inputs():
        if not surface_input.path.is_file():
            return fail(f"{surface_input.missing_message}: {repo_rel(surface_input.path)}")

    documents = StdlibSurfaceDocuments.load(PATHS)
    core_architecture = documents.core_architecture
    advanced_architecture = documents.advanced_architecture
    semantic_policy = documents.semantic_policy
    lowering_import_surface = documents.lowering_import_surface
    advanced_helper_package_surface = documents.advanced_helper_package_surface
    program_surface = documents.program_surface

    document_header_error = validate_document_headers(documents)
    if document_header_error is not None:
        return fail(document_header_error)

    inventory_error, inventory_validation = validate_inventory_and_policy(documents)
    if inventory_error is not None:
        return fail(inventory_error)
    if inventory_validation is None:
        raise RuntimeError("stdlib surface inventory validation did not return a payload")
    module_surfaces = inventory_validation.module_surfaces
    layers = inventory_validation.layers
    module_imports = inventory_validation.module_imports
    inventory_module_names = inventory_validation.inventory_module_names
    advanced_api_families = inventory_validation.advanced_api_families

    artifact_error = validate_module_artifacts(
        root=ROOT,
        module_surfaces=module_surfaces,
        package_imports_by_module=inventory_validation.package_imports_by_module,
        semantic_policy=semantic_policy,
    )
    if artifact_error is not None:
        return fail(artifact_error)

    architecture_live_paths = core_architecture.get("live_paths")
    if not isinstance(architecture_live_paths, list) or not architecture_live_paths:
        return fail("core architecture missing live_paths")
    for raw_path in architecture_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return fail("core architecture live_paths entry malformed")
        path = ROOT / raw_path
        if not path.exists():
            return fail(f"core architecture live path missing: {raw_path}")

    advanced_live_paths = advanced_architecture.get("live_paths")
    if not isinstance(advanced_live_paths, list) or not advanced_live_paths:
        return fail("advanced architecture missing live_paths")
    for raw_path in advanced_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return fail("advanced architecture live_paths entry malformed")
        path = ROOT / raw_path
        if not path.exists():
            return fail(f"advanced architecture live path missing: {raw_path}")

    architecture_api_families = core_architecture.get("api_families")
    if not isinstance(architecture_api_families, dict) or not architecture_api_families:
        return fail("core architecture missing api_families")
    inventory_modules_by_name = {module_surface.module: module_surface for module_surface in module_surfaces}
    for module_name, families in architecture_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"core architecture referenced unknown module {module_name}")
        if not isinstance(families, list) or not families:
            return fail(f"core architecture api_families malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("api_families") != families:
            return fail(f"module manifest api_families drifted for {module_name}")
    architecture_required_exports = core_architecture.get("required_exports")
    if not isinstance(architecture_required_exports, dict) or not architecture_required_exports:
        return fail("core architecture missing required_exports")
    for module_name, required_exports in architecture_required_exports.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"core architecture required_exports referenced unknown module {module_name}")
        if not isinstance(required_exports, list) or not required_exports:
            return fail(f"core architecture required_exports malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("exports") != required_exports:
            return fail(f"module manifest exports drifted for {module_name}")

    if not isinstance(advanced_api_families, dict) or not advanced_api_families:
        return fail("advanced architecture missing api_families")
    for module_name, families in advanced_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"advanced architecture referenced unknown module {module_name}")
        if not isinstance(families, list) or not families:
            return fail(f"advanced architecture api_families malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("api_families") != families:
            return fail(f"module manifest advanced api_families drifted for {module_name}")

    advanced_required_exports = advanced_architecture.get("required_exports")
    if not isinstance(advanced_required_exports, dict) or not advanced_required_exports:
        return fail("advanced architecture missing required_exports")
    for module_name, required_exports in advanced_required_exports.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"advanced architecture required_exports referenced unknown module {module_name}")
        if not isinstance(required_exports, list) or not required_exports:
            return fail(f"advanced architecture required_exports malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name].manifest)
        if manifest_payload.get("exports") != required_exports:
            return fail(f"module manifest advanced exports drifted for {module_name}")

    semantic_policy_error, semantic_policy_validation = validate_semantic_policy(
        semantic_policy=semantic_policy,
        inventory_module_names=inventory_module_names,
    )
    if semantic_policy_error is not None:
        return fail(semantic_policy_error)
    if semantic_policy_validation is None:
        raise RuntimeError("stdlib surface semantic policy validation did not return a payload")
    semantic_module_semver = semantic_policy_validation.module_semver

    command_surface_error, command_surface_validation = validate_command_surfaces(
        root=ROOT,
        lowering_import_surface=lowering_import_surface,
        advanced_helper_package_surface=advanced_helper_package_surface,
        program_surface=program_surface,
        inventory_module_names=inventory_module_names,
    )
    if command_surface_error is not None:
        return fail(command_surface_error)
    if command_surface_validation is None:
        raise RuntimeError("stdlib surface command validation did not return a payload")
    artifact_filenames = command_surface_validation.artifact_filenames
    advanced_helper_modules = command_surface_validation.advanced_helper_modules
    capability_demo_examples = command_surface_validation.capability_demo_examples

    write_stdlib_surface_report(
        StdlibSurfaceReport(
            paths=PATHS,
            canonical_modules=module_surfaces,
            layers=layers,
            module_imports=module_imports,
            api_families=architecture_api_families,
            advanced_api_families=advanced_api_families,
            required_exports=architecture_required_exports,
            advanced_required_exports=advanced_required_exports,
            module_semver=semantic_module_semver,
            artifact_filenames=artifact_filenames,
            advanced_helper_modules=advanced_helper_modules,
            capability_demo_examples=capability_demo_examples,
        )
    )
    print(f"summary_path: {repo_rel(PATHS.summary)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
