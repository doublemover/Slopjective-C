from __future__ import annotations

from dataclasses import dataclass

from check_stdlib_surface_model import (
    CanonicalModuleSurface,
    PackageImportSurface,
    StdlibSurfaceDocuments,
    parse_spec_canonical_modules,
)


@dataclass(frozen=True)
class InventoryValidation:
    module_surfaces: list[CanonicalModuleSurface]
    layers: list[object]
    module_imports: list[object]
    package_imports_by_module: dict[str, PackageImportSurface]
    inventory_module_names: set[str]
    advanced_api_families: object


def validate_inventory_and_policy(
    documents: StdlibSurfaceDocuments,
) -> tuple[str | None, InventoryValidation | None]:
    inventory = documents.inventory
    stability_policy = documents.stability_policy
    package_surface = documents.package_surface
    advanced_architecture = documents.advanced_architecture
    spec_text = documents.spec_text

    canonical_modules = inventory.get("canonical_modules")
    if not isinstance(canonical_modules, list) or not canonical_modules:
        return "module inventory missing canonical_modules", None

    module_surfaces: list[CanonicalModuleSurface] = []
    required_extra_fields = (
        "implementation_module",
        "workspace_root",
        "source",
        "smoke_source",
        "manifest",
    )
    for entry in canonical_modules:
        if not isinstance(entry, dict):
            return "module inventory entry must be an object", None
        module = entry.get("module")
        capability_id = entry.get("capability_id")
        required_profile = entry.get("required_profile")
        if not all(isinstance(value, str) and value for value in (module, capability_id, required_profile)):
            return "module inventory entry is missing module/capability_id/required_profile", None
        for field in required_extra_fields:
            value = entry.get(field)
            if not isinstance(value, str) or not value:
                return f"module inventory entry {module} is missing {field}", None
        module_surfaces.append(
            CanonicalModuleSurface(
                module=module,
                implementation_module=str(entry["implementation_module"]),
                capability_id=capability_id,
                required_profile=required_profile,
                workspace_root=str(entry["workspace_root"]),
                source=str(entry["source"]),
                smoke_source=str(entry["smoke_source"]),
                manifest=str(entry["manifest"]),
            )
        )

    spec_rows = parse_spec_canonical_modules(spec_text)
    spec_comparison_rows = [module_surface.to_spec_row() for module_surface in module_surfaces]
    if spec_rows != spec_comparison_rows:
        return "module inventory drifted from spec canonical module table", None

    layers = stability_policy.get("layers")
    if not isinstance(layers, list) or not layers:
        return "stability policy missing layers", None
    inventory_module_names = {module_surface.module for module_surface in module_surfaces}
    covered_modules: set[str] = set()
    for layer in layers:
        if not isinstance(layer, dict):
            return "stability policy layer must be an object", None
        layer_name = layer.get("name")
        modules = layer.get("modules")
        allowed_dependencies = layer.get("allowed_dependencies")
        if not isinstance(layer_name, str) or not layer_name:
            return "stability policy layer missing name", None
        if not isinstance(modules, list) or not modules:
            return f"stability policy layer {layer_name} missing modules", None
        if not isinstance(allowed_dependencies, list):
            return f"stability policy layer {layer_name} missing allowed_dependencies", None
        for module_name in modules:
            if not isinstance(module_name, str) or not module_name:
                return f"stability policy layer {layer_name} published an invalid module", None
            if module_name not in inventory_module_names:
                return f"stability policy layer {layer_name} referenced unknown module {module_name}", None
            covered_modules.add(module_name)
        for dependency in allowed_dependencies:
            if not isinstance(dependency, str) or not dependency:
                return f"stability policy layer {layer_name} published an invalid dependency", None
            if dependency not in inventory_module_names:
                return f"stability policy layer {layer_name} referenced unknown dependency {dependency}", None
    if covered_modules != inventory_module_names:
        return "stability policy module coverage drifted from module inventory", None

    family_ownership = stability_policy.get("family_ownership")
    if not isinstance(family_ownership, dict) or not family_ownership:
        return "stability policy missing family_ownership", None
    advanced_api_families = advanced_architecture.get("api_families")
    if family_ownership != advanced_api_families:
        return "stability policy family_ownership drifted from advanced architecture api_families", None

    profile_gates = stability_policy.get("profile_gates")
    if not isinstance(profile_gates, dict) or not profile_gates:
        return "stability policy missing profile_gates", None
    for module_surface in module_surfaces:
        if profile_gates.get(module_surface.module) != module_surface.required_profile:
            return f"stability policy profile_gates drifted for {module_surface.module}", None

    breaking_change_rules = stability_policy.get("breaking_change_rules")
    if not isinstance(breaking_change_rules, list) or len(breaking_change_rules) < 3:
        return "stability policy missing breaking_change_rules", None

    module_imports = package_surface.get("module_imports")
    if not isinstance(module_imports, list) or not module_imports:
        return "package surface missing module_imports", None
    package_imports_by_module: dict[str, PackageImportSurface] = {}
    for entry in module_imports:
        if not isinstance(entry, dict):
            return "package surface module_import entry must be an object", None
        canonical_module = entry.get("canonical_module")
        implementation_module = entry.get("implementation_module")
        source_declaration = entry.get("source_declaration")
        if not all(
            isinstance(value, str) and value
            for value in (canonical_module, implementation_module, source_declaration)
        ):
            return "package surface module_import entry is malformed", None
        package_imports_by_module[str(canonical_module)] = PackageImportSurface(
            canonical_module=str(canonical_module),
            implementation_module=str(implementation_module),
            source_declaration=str(source_declaration),
        )

    return (
        None,
        InventoryValidation(
            module_surfaces=module_surfaces,
            layers=layers,
            module_imports=module_imports,
            package_imports_by_module=package_imports_by_module,
            inventory_module_names=inventory_module_names,
            advanced_api_families=advanced_api_families,
        ),
    )
