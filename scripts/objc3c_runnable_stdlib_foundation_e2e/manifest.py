from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import normalize_rel_path, repo_rel

from .assertions import expect
from .constants import PACKAGE_CONTRACT_ID
from .models import StdlibPackageSurface


def package_path(package_root: Path, manifest_value: object) -> Path:
    return package_root / normalize_rel_path(str(manifest_value))


def require_packaged_file(path: Path) -> None:
    expect(path.is_file(), f"packaged runnable toolchain missing required stdlib file {path}")


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


def collect_packaged_publish_inputs(
    *,
    package_root: Path,
    publish_inputs: object,
) -> list[str]:
    expect(
        isinstance(publish_inputs, list) and publish_inputs,
        "package manifest missing stdlib program publish inputs",
    )
    assert isinstance(publish_inputs, list)
    packaged_publish_inputs: list[str] = []
    for raw_path in publish_inputs:
        expect(
            isinstance(raw_path, str) and raw_path,
            "package manifest published malformed stdlib program input",
        )
        packaged_path = package_root / normalize_rel_path(raw_path)
        expect(
            packaged_path.is_file(),
            f"packaged runnable toolchain missing stdlib program publish input {packaged_path}",
        )
        packaged_publish_inputs.append(repo_rel(packaged_path))
    return packaged_publish_inputs


def load_stdlib_package_surface(
    *,
    package_root: Path,
    manifest_path: Path,
) -> StdlibPackageSurface:
    manifest = load_json(manifest_path)
    expect(manifest.get("contract_id") == PACKAGE_CONTRACT_ID, "unexpected package contract id")

    command_surfaces = manifest.get("command_surfaces", {})
    stdlib_surface = manifest.get("stdlib_foundation_surface", {})
    stdlib_program_surface = manifest.get("stdlib_program_surface", {})
    stdlib_modules = manifest.get("stdlib_modules", [])
    validate_command_surfaces(command_surfaces)
    validate_manifest_surfaces(
        manifest=manifest,
        stdlib_surface=stdlib_surface,
        stdlib_program_surface=stdlib_program_surface,
        stdlib_modules=stdlib_modules,
    )

    compile_wrapper = package_path(package_root, manifest["compile_wrapper"])
    runtime_library = package_path(package_root, manifest["runtime_library"])
    stdlib_program_contract = package_path(package_root, manifest["stdlib_program_contract"])
    stdlib_program_runbook = package_path(package_root, manifest["stdlib_program_runbook"])
    stdlib_program_site_entry = package_path(
        package_root,
        manifest["stdlib_program_site_entry"],
    )
    workspace_manifest = package_path(package_root, manifest["stdlib_workspace_manifest"])
    module_inventory = package_path(package_root, manifest["stdlib_module_inventory"])
    stability_policy = package_path(package_root, manifest["stdlib_stability_policy"])
    package_surface = package_path(package_root, manifest["stdlib_package_surface"])
    lowering_import_surface = package_path(
        package_root,
        manifest["stdlib_lowering_import_surface"],
    )
    advanced_helper_package_surface = package_path(
        package_root,
        manifest["stdlib_advanced_helper_package_surface"],
    )
    for path in (
        compile_wrapper,
        runtime_library,
        stdlib_program_contract,
        stdlib_program_runbook,
        stdlib_program_site_entry,
        workspace_manifest,
        module_inventory,
        stability_policy,
        package_surface,
        lowering_import_surface,
        advanced_helper_package_surface,
    ):
        require_packaged_file(path)

    package_surface_payload = load_json(package_surface)
    stdlib_program_surface_payload = load_json(stdlib_program_contract)
    lowering_import_surface_payload = load_json(lowering_import_surface)
    advanced_helper_package_surface_payload = load_json(advanced_helper_package_surface)
    validate_surface_payloads(
        manifest=manifest,
        stdlib_surface=stdlib_surface,
        package_surface_payload=package_surface_payload,
        stdlib_program_surface_payload=stdlib_program_surface_payload,
        lowering_import_surface_payload=lowering_import_surface_payload,
        advanced_helper_package_surface_payload=advanced_helper_package_surface_payload,
    )
    packaged_publish_inputs = collect_packaged_publish_inputs(
        package_root=package_root,
        publish_inputs=manifest.get("stdlib_program_publish_inputs"),
    )

    assert isinstance(stdlib_modules, list)
    return StdlibPackageSurface(
        manifest=manifest,
        command_surfaces=command_surfaces,
        stdlib_surface=stdlib_surface,
        stdlib_program_surface=stdlib_program_surface,
        stdlib_modules=stdlib_modules,
        compile_wrapper=compile_wrapper,
        runtime_library=runtime_library,
        stdlib_program_contract=stdlib_program_contract,
        stdlib_program_runbook=stdlib_program_runbook,
        stdlib_program_site_entry=stdlib_program_site_entry,
        workspace_manifest=workspace_manifest,
        module_inventory=module_inventory,
        stability_policy=stability_policy,
        package_surface=package_surface,
        lowering_import_surface=lowering_import_surface,
        advanced_helper_package_surface=advanced_helper_package_surface,
        package_surface_payload=package_surface_payload,
        stdlib_program_surface_payload=stdlib_program_surface_payload,
        lowering_import_surface_payload=lowering_import_surface_payload,
        advanced_helper_package_surface_payload=advanced_helper_package_surface_payload,
        packaged_publish_inputs=packaged_publish_inputs,
    )
