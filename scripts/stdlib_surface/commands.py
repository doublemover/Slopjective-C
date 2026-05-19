from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from stdlib_surface.declarations import validate_program_declarations
from stdlib_surface.program import validate_program_surface


@dataclass(frozen=True)
class CommandSurfaceValidation:
    artifact_filenames: Any
    advanced_helper_modules: Any
    capability_demo_examples: Any


def validate_command_surfaces(
    *,
    root: Path,
    lowering_import_surface: dict[str, Any],
    advanced_helper_package_surface: dict[str, Any],
    program_surface: dict[str, Any],
    inventory_module_names: set[str],
) -> tuple[str | None, CommandSurfaceValidation | None]:
    artifact_filenames = lowering_import_surface.get("artifact_filenames")
    if artifact_filenames != {
        "object": "module.obj",
        "compile_manifest": "module.manifest.json",
        "runtime_registration_manifest": "module.runtime-registration-manifest.json",
    }:
        return "lowering/import surface artifact_filenames drifted", None

    import_surface = lowering_import_surface.get("import_surface")
    if import_surface != {
        "model": "canonical-spec-module-ids-map-to-identifier-safe-implementation-module-declarations",
        "module_imports_source": "stdlib/package_surface.json",
        "identity_fields": ["canonical_module", "implementation_module", "source_declaration"],
    }:
        return "lowering/import surface import_surface drifted", None

    if lowering_import_surface.get("public_actions") != [
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-foundation",
        "validate-runnable-stdlib-foundation",
        "package-runnable-toolchain",
    ]:
        return "lowering/import surface public_actions drifted", None

    if advanced_helper_package_surface.get("contract_id") != "objc3c.stdlib.advanced_helper_package_surface.v1":
        return "advanced helper package surface contract_id drifted", None
    if advanced_helper_package_surface.get("schema_version") != 1:
        return "advanced helper package surface schema_version drifted", None
    if advanced_helper_package_surface.get("workspace_contract") != "stdlib/workspace.json":
        return "advanced helper package surface workspace_contract drifted", None
    if advanced_helper_package_surface.get("package_surface") != "stdlib/package_surface.json":
        return "advanced helper package surface package_surface drifted", None
    if advanced_helper_package_surface.get("advanced_architecture") != "stdlib/advanced_architecture.json":
        return "advanced helper package surface advanced_architecture drifted", None
    if advanced_helper_package_surface.get("module_inventory") != "stdlib/module_inventory.json":
        return "advanced helper package surface module_inventory drifted", None
    if advanced_helper_package_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return "advanced helper package surface machine_output_root drifted", None
    if advanced_helper_package_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return "advanced helper package surface machine_report_root drifted", None
    if advanced_helper_package_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return "advanced helper package surface package_stage_root drifted", None
    if advanced_helper_package_surface.get("staged_manifest_fields") != [
        "stdlib_advanced_architecture",
        "stdlib_advanced_helper_package_surface",
        "advanced_helper_modules",
        "advanced_helper_command_surfaces",
        "advanced_helper_profile_gates",
    ]:
        return "advanced helper package surface staged_manifest_fields drifted", None
    if advanced_helper_package_surface.get("advanced_helper_command_surfaces") != {
        "check_stdlib_surface": "npm run objc3c -- check-stdlib-surface",
        "build_stdlib": "npm run objc3c -- materialize-stdlib-workspace",
        "validate_stdlib_advanced": "npm run objc3c -- validate-stdlib-advanced",
        "validate_runnable_stdlib_advanced": "npm run objc3c -- validate-runnable-stdlib-advanced",
        "package_runnable_toolchain": "npm run objc3c -- package-runnable-toolchain",
    }:
        return "advanced helper package surface command surfaces drifted", None
    if advanced_helper_package_surface.get("public_actions") != [
        "check-stdlib-surface",
        "materialize-stdlib-workspace",
        "validate-stdlib-advanced",
        "validate-runnable-stdlib-advanced",
        "package-runnable-toolchain",
    ]:
        return "advanced helper package surface public_actions drifted", None

    program_surface_error = validate_program_surface(root=root, program_surface=program_surface)
    if program_surface_error is not None:
        return program_surface_error, None

    declarations_error, declarations = validate_program_declarations(
        root=root,
        advanced_helper_package_surface=advanced_helper_package_surface,
        program_surface=program_surface,
        inventory_module_names=inventory_module_names,
    )
    if declarations_error is not None:
        return declarations_error, None
    if declarations is None:
        raise RuntimeError("stdlib surface declaration validation did not return a payload")

    return (
        None,
        CommandSurfaceValidation(
            artifact_filenames=artifact_filenames,
            advanced_helper_modules=declarations.advanced_helper_modules,
            capability_demo_examples=declarations.capability_demo_examples,
        ),
    )
