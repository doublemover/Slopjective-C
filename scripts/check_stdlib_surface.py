#!/usr/bin/env python3
"""Validate the checked-in stdlib boundary and canonical module inventory."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_tooling.paths import repo_rel
from stdlib_surface.architecture import validate_architecture_surfaces
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

    architecture_error, architecture_validation = validate_architecture_surfaces(
        root=ROOT,
        core_architecture=core_architecture,
        advanced_architecture=advanced_architecture,
        module_surfaces=module_surfaces,
        advanced_api_families=advanced_api_families,
    )
    if architecture_error is not None:
        return fail(architecture_error)
    if architecture_validation is None:
        raise RuntimeError("stdlib surface architecture validation did not return a payload")
    architecture_api_families = architecture_validation.api_families
    architecture_required_exports = architecture_validation.required_exports
    advanced_required_exports = architecture_validation.advanced_required_exports

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
