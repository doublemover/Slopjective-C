from __future__ import annotations

from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json

from check_stdlib_surface_model import CanonicalModuleSurface, PackageImportSurface


def validate_module_artifacts(
    *,
    root: Path,
    module_surfaces: list[CanonicalModuleSurface],
    package_imports_by_module: dict[str, PackageImportSurface],
    semantic_policy: dict[str, Any],
) -> str | None:
    for module_surface in module_surfaces:
        for path_key in ("workspace_root", "source", "smoke_source", "manifest"):
            raw_path = module_surface.path_for(path_key)
            path = root / raw_path
            if path_key == "workspace_root":
                if not path.is_dir():
                    return f"missing module workspace root: {raw_path}"
            else:
                if not path.is_file():
                    return f"missing module artifact: {raw_path}"
        manifest_payload = load_json(root / module_surface.manifest)
        if manifest_payload.get("contract_id") != "objc3c.stdlib.module.surface.v1":
            return f"module manifest contract_id drifted for {module_surface.module}"
        if manifest_payload.get("canonical_module") != module_surface.module:
            return f"module manifest canonical_module drifted for {module_surface.module}"
        if manifest_payload.get("implementation_module") != module_surface.implementation_module:
            return f"module manifest implementation_module drifted for {module_surface.module}"
        if manifest_payload.get("capability_id") != module_surface.capability_id:
            return f"module manifest capability_id drifted for {module_surface.module}"
        if manifest_payload.get("workspace_root") != module_surface.workspace_root:
            return f"module manifest workspace_root drifted for {module_surface.module}"
        if (
            manifest_payload.get("module_semver")
            != semantic_policy.get("module_semver", {}).get(module_surface.module)
        ):
            return f"module manifest module_semver drifted for {module_surface.module}"
        if manifest_payload.get("source") != module_surface.source:
            return f"module manifest source drifted for {module_surface.module}"
        if manifest_payload.get("smoke_source") != module_surface.smoke_source:
            return f"module manifest smoke_source drifted for {module_surface.module}"
        source_text = (root / module_surface.source).read_text(encoding="utf-8")
        expected_decl = module_surface.expected_source_declaration()
        if expected_decl not in source_text:
            return f"module source declaration drifted for {module_surface.module}"
        package_import = package_imports_by_module.get(module_surface.module)
        if package_import is None:
            return f"package surface missing canonical module {module_surface.module}"
        if package_import.implementation_module != module_surface.implementation_module:
            return f"package surface implementation_module drifted for {module_surface.module}"
        if package_import.source_declaration != expected_decl:
            return f"package surface source_declaration drifted for {module_surface.module}"
        manifest_api_families = manifest_payload.get("api_families")
        if not isinstance(manifest_api_families, list) or not all(
            isinstance(value, str) and value for value in manifest_api_families
        ):
            return f"module manifest api_families malformed for {module_surface.module}"
        manifest_exports = manifest_payload.get("exports")
        if not isinstance(manifest_exports, list) or not all(
            isinstance(value, str) and value for value in manifest_exports
        ):
            return f"module manifest exports malformed for {module_surface.module}"
        for export_name in manifest_exports:
            if export_name not in source_text:
                return f"module source missing exported symbol spelling {export_name} for {module_surface.module}"
        runtime_abi = manifest_payload.get("runtime_abi", [])
        if runtime_abi:
            if not isinstance(runtime_abi, list) or not all(
                isinstance(value, str) and value for value in runtime_abi
            ):
                return f"module manifest runtime_abi malformed for {module_surface.module}"
            for runtime_symbol in runtime_abi:
                if f"extern fn {runtime_symbol}" not in source_text:
                    return (
                        f"module source missing runtime ABI extern {runtime_symbol} "
                        f"for {module_surface.module}"
                    )
    return None
