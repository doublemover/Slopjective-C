from __future__ import annotations

from pathlib import Path

from objc3c_tooling.json_io import require_json_object as load_json

from ..assertions import expect
from ..constants import PACKAGE_CONTRACT_ID
from ..models import StdlibPackageSurface
from .paths import package_path, require_packaged_file
from .publish import collect_packaged_publish_inputs
from .validation import (
    validate_command_surfaces,
    validate_manifest_surfaces,
    validate_surface_payloads,
)


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
