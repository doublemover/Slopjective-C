from __future__ import annotations

from typing import Any

from ..assertions import expect


def validate_command_surfaces(command_surfaces: object) -> None:
    expect(isinstance(command_surfaces, dict), "package manifest missing command surfaces")
    assert isinstance(command_surfaces, dict)
    expect(
        command_surfaces.get("build_stdlib")
        == "npm run objc3c -- materialize-stdlib-workspace",
        "package manifest missing build_stdlib command surface",
    )
    expect(
        command_surfaces.get("check_stdlib_surface")
        == "npm run objc3c -- check-stdlib-surface",
        "package manifest missing check_stdlib_surface command surface",
    )
    expect(
        command_surfaces.get("stdlib") == "npm run objc3c -- validate-stdlib-foundation",
        "package manifest missing stdlib command surface",
    )
    expect(
        command_surfaces.get("stdlib_e2e")
        == "npm run objc3c -- validate-runnable-stdlib-foundation",
        "package manifest missing stdlib_e2e command surface",
    )


def validate_manifest_surfaces(
    *,
    manifest: dict[str, Any],
    stdlib_surface: object,
    stdlib_program_surface: object,
    stdlib_modules: object,
) -> None:
    expect(isinstance(stdlib_surface, dict), "package manifest missing stdlib_foundation_surface")
    expect(isinstance(stdlib_program_surface, dict), "package manifest missing stdlib_program_surface")
    assert isinstance(stdlib_surface, dict)
    assert isinstance(stdlib_program_surface, dict)
    expect(
        "validate-runnable-stdlib-foundation" in stdlib_surface.get("public_actions", []),
        "package manifest stdlib surface missing validate-runnable-stdlib-foundation",
    )
    expect(
        "package-runnable-toolchain" in stdlib_program_surface.get("public_actions", []),
        "package manifest stdlib program surface missing package-runnable-toolchain",
    )
    expect(
        isinstance(stdlib_modules, list) and stdlib_modules,
        "package manifest missing stdlib_modules",
    )


def validate_surface_payloads(
    *,
    manifest: dict[str, Any],
    stdlib_surface: object,
    package_surface_payload: dict[str, Any],
    stdlib_program_surface_payload: dict[str, Any],
    lowering_import_surface_payload: dict[str, Any],
    advanced_helper_package_surface_payload: dict[str, Any],
) -> None:
    expect(
        package_surface_payload.get("workspace_contract") == "stdlib/workspace.json",
        "packaged stdlib package surface drifted from the checked-in workspace contract",
    )
    expect(
        package_surface_payload.get("lowering_import_surface")
        == "stdlib/lowering_import_surface.json",
        "packaged stdlib package surface drifted from the lowering/import surface contract",
    )
    expect(
        package_surface_payload.get("advanced_helper_package_surface")
        == "stdlib/advanced_helper_package_surface.json",
        "packaged stdlib package surface drifted from the advanced helper package contract",
    )
    expect(
        manifest.get("stdlib_program_command_surfaces")
        == stdlib_program_surface_payload.get("command_surfaces"),
        "package manifest stdlib program command surfaces drifted",
    )
    expect(
        manifest.get("stdlib_program_publish_inputs")
        == stdlib_program_surface_payload.get("publish_inputs"),
        "package manifest stdlib program publish inputs drifted",
    )
    expect(
        manifest.get("stdlib_program_examples")
        == stdlib_program_surface_payload.get("capability_demo_examples"),
        "package manifest stdlib program examples drifted",
    )
    expect(
        manifest.get("stdlib_lowering_artifact_filenames")
        == lowering_import_surface_payload.get("artifact_filenames"),
        "package manifest stdlib lowering artifact inventory drifted",
    )
    expect(
        manifest.get("stdlib_import_surface")
        == lowering_import_surface_payload.get("import_surface"),
        "package manifest stdlib import surface drifted",
    )
    assert isinstance(stdlib_surface, dict)
    expect(
        stdlib_surface.get("advanced_helper_package_surface")
        == "stdlib/advanced_helper_package_surface.json",
        "package manifest stdlib surface missing advanced helper package surface",
    )
    expect(
        manifest.get("advanced_helper_modules")
        == advanced_helper_package_surface_payload.get("advanced_helper_modules"),
        "package manifest advanced helper module inventory drifted",
    )
    expect(
        manifest.get("advanced_helper_command_surfaces")
        == advanced_helper_package_surface_payload.get("advanced_helper_command_surfaces"),
        "package manifest advanced helper command surfaces drifted",
    )
    expect(
        manifest.get("advanced_helper_profile_gates")
        == advanced_helper_package_surface_payload.get("advanced_helper_profile_gates"),
        "package manifest advanced helper profile gates drifted",
    )
