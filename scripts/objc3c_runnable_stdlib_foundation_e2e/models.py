from __future__ import annotations

from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class StdlibPackageSurface:
    manifest: dict[str, Any]
    command_surfaces: dict[str, Any]
    stdlib_surface: dict[str, Any]
    stdlib_program_surface: dict[str, Any]
    stdlib_modules: list[Any]
    compile_wrapper: Path
    runtime_library: Path
    stdlib_program_contract: Path
    stdlib_program_runbook: Path
    stdlib_program_site_entry: Path
    workspace_manifest: Path
    module_inventory: Path
    stability_policy: Path
    package_surface: Path
    lowering_import_surface: Path
    advanced_helper_package_surface: Path
    package_surface_payload: dict[str, Any]
    stdlib_program_surface_payload: dict[str, Any]
    lowering_import_surface_payload: dict[str, Any]
    advanced_helper_package_surface_payload: dict[str, Any]
    packaged_publish_inputs: list[str]


@dataclass(frozen=True)
class StdlibCompileResult:
    canonical_module: str
    implementation_module: str
    smoke_source: str
    artifact_root: str
    object: str
    compile_manifest: str
    registration_manifest: str

    def payload(self) -> dict[str, Any]:
        return asdict(self)
