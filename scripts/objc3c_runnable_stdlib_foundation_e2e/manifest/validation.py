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
    expect(
        isinstance(manifest.get("stdlib_compatibility_gates"), str)
        and manifest["stdlib_compatibility_gates"] == "stdlib/compatibility_gates.json",
        "package manifest missing stdlib compatibility gate contract",
    )
    expect(
        isinstance(manifest.get("stdlib_compatibility_gate_summary"), dict),
        "package manifest missing stdlib compatibility gate summary",
    )


def validate_surface_payloads(
    *,
    manifest: dict[str, Any],
    stdlib_surface: object,
    package_surface_payload: dict[str, Any],
    compatibility_gates_payload: dict[str, Any],
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
        package_surface_payload.get("compatibility_gates")
        == "stdlib/compatibility_gates.json",
        "packaged stdlib package surface drifted from the compatibility gate contract",
    )
    compatibility_summary = manifest.get("stdlib_compatibility_gate_summary")
    expect(
        isinstance(compatibility_summary, dict),
        "package manifest stdlib compatibility summary malformed",
    )
    assert isinstance(compatibility_summary, dict)
    expect(
        compatibility_summary.get("contract_id")
        == compatibility_gates_payload.get("contract_id"),
        "package manifest stdlib compatibility contract id drifted",
    )
    expect(
        compatibility_summary.get("stdlib_major_version")
        == compatibility_gates_payload.get("stdlib_major_version"),
        "package manifest stdlib compatibility major version drifted",
    )
    expect(
        compatibility_summary.get("abi_gate_mode")
        == compatibility_gates_payload.get("abi_gate", {}).get("mode"),
        "package manifest stdlib ABI gate mode drifted",
    )
    expect(
        compatibility_summary.get("semantic_gate_mode")
        == compatibility_gates_payload.get("semantic_gate", {}).get("mode"),
        "package manifest stdlib semantic gate mode drifted",
    )
    expect(
        compatibility_summary.get("package_gate_manifest_fields")
        == compatibility_gates_payload.get("package_gate", {}).get("required_manifest_fields"),
        "package manifest stdlib compatibility required fields drifted",
    )
    expect(
        compatibility_summary.get("conformance_positive_fixture")
        == compatibility_gates_payload.get("conformance_gate", {}).get("positive_fixture"),
        "package manifest stdlib compatibility positive fixture drifted",
    )
    expect(
        compatibility_summary.get("conformance_negative_fixture")
        == compatibility_gates_payload.get("conformance_gate", {}).get("negative_fixture"),
        "package manifest stdlib compatibility negative fixture drifted",
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
