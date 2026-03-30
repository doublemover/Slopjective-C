#!/usr/bin/env python3
"""Validate the checked-in stdlib boundary and canonical module inventory."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WORKSPACE_PATH = ROOT / "stdlib" / "workspace.json"
MODULE_INVENTORY_PATH = ROOT / "stdlib" / "module_inventory.json"
STABILITY_POLICY_PATH = ROOT / "stdlib" / "stability_policy.json"
PACKAGE_SURFACE_PATH = ROOT / "stdlib" / "package_surface.json"
CORE_ARCHITECTURE_PATH = ROOT / "stdlib" / "core_architecture.json"
SEMANTIC_POLICY_PATH = ROOT / "stdlib" / "semantic_policy.json"
SPEC_CONTRACT_PATH = ROOT / "spec" / "STANDARD_LIBRARY_CONTRACT.md"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stdlib" / "surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stdlib.surface.summary.v1"
TABLE_ROW_RE = re.compile(
    r"^\|\s*`(?P<module>[^`]+)`\s*\|\s*`(?P<capability>[^`]+)`\s*\|\s*(?P<profile>[^|]+?)\s*\|$"
)


def fail(message: str) -> int:
    print(f"stdlib-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, object]:
    return json.loads(path.read_text(encoding="utf-8"))


def parse_spec_canonical_modules(spec_text: str) -> list[dict[str, str]]:
    rows: list[dict[str, str]] = []
    for raw_line in spec_text.splitlines():
        match = TABLE_ROW_RE.match(raw_line.strip())
        if not match:
            continue
        module = match.group("module")
        capability = match.group("capability")
        if not module.startswith("objc3."):
            continue
        rows.append(
            {
                "module": module,
                "capability_id": capability,
                "required_profile": match.group("profile").strip(),
            }
        )
    return rows


def main() -> int:
    if not WORKSPACE_PATH.is_file():
        return fail(f"missing workspace contract: {repo_rel(WORKSPACE_PATH)}")
    if not MODULE_INVENTORY_PATH.is_file():
        return fail(f"missing module inventory: {repo_rel(MODULE_INVENTORY_PATH)}")
    if not STABILITY_POLICY_PATH.is_file():
        return fail(f"missing stability policy: {repo_rel(STABILITY_POLICY_PATH)}")
    if not PACKAGE_SURFACE_PATH.is_file():
        return fail(f"missing package surface: {repo_rel(PACKAGE_SURFACE_PATH)}")
    if not CORE_ARCHITECTURE_PATH.is_file():
        return fail(f"missing core architecture contract: {repo_rel(CORE_ARCHITECTURE_PATH)}")
    if not SEMANTIC_POLICY_PATH.is_file():
        return fail(f"missing semantic policy contract: {repo_rel(SEMANTIC_POLICY_PATH)}")
    if not SPEC_CONTRACT_PATH.is_file():
        return fail(f"missing spec contract: {repo_rel(SPEC_CONTRACT_PATH)}")

    workspace = load_json(WORKSPACE_PATH)
    inventory = load_json(MODULE_INVENTORY_PATH)
    stability_policy = load_json(STABILITY_POLICY_PATH)
    package_surface = load_json(PACKAGE_SURFACE_PATH)
    core_architecture = load_json(CORE_ARCHITECTURE_PATH)
    semantic_policy = load_json(SEMANTIC_POLICY_PATH)
    spec_text = SPEC_CONTRACT_PATH.read_text(encoding="utf-8")

    if workspace.get("contract_id") != "objc3c.stdlib.workspace.v1":
        return fail("workspace contract_id drifted")
    if workspace.get("schema_version") != 1:
        return fail("workspace schema_version drifted")
    if workspace.get("module_inventory") != "stdlib/module_inventory.json":
        return fail("workspace module_inventory path drifted")
    if workspace.get("stability_policy") != "stdlib/stability_policy.json":
        return fail("workspace stability_policy path drifted")
    if workspace.get("package_surface") != "stdlib/package_surface.json":
        return fail("workspace package_surface path drifted")
    if workspace.get("core_architecture") != "stdlib/core_architecture.json":
        return fail("workspace core_architecture path drifted")
    if workspace.get("semantic_policy") != "stdlib/semantic_policy.json":
        return fail("workspace semantic_policy path drifted")
    if workspace.get("core_runbook") != "docs/runbooks/objc3c_stdlib_core.md":
        return fail("workspace core_runbook path drifted")
    if inventory.get("contract_id") != "objc3c.stdlib.module_inventory.v1":
        return fail("module inventory contract_id drifted")
    if inventory.get("schema_version") != 1:
        return fail("module inventory schema_version drifted")
    if inventory.get("spec_contract") != "spec/STANDARD_LIBRARY_CONTRACT.md":
        return fail("module inventory spec_contract drifted")
    if stability_policy.get("contract_id") != "objc3c.stdlib.stability_policy.v1":
        return fail("stability policy contract_id drifted")
    if stability_policy.get("schema_version") != 1:
        return fail("stability policy schema_version drifted")
    if package_surface.get("contract_id") != "objc3c.stdlib.package_surface.v1":
        return fail("package surface contract_id drifted")
    if package_surface.get("schema_version") != 1:
        return fail("package surface schema_version drifted")
    if package_surface.get("workspace_contract") != "stdlib/workspace.json":
        return fail("package surface workspace_contract drifted")
    if package_surface.get("module_inventory") != "stdlib/module_inventory.json":
        return fail("package surface module_inventory drifted")
    if package_surface.get("stability_policy") != "stdlib/stability_policy.json":
        return fail("package surface stability_policy drifted")
    if package_surface.get("core_architecture") != "stdlib/core_architecture.json":
        return fail("package surface core_architecture drifted")
    if package_surface.get("semantic_policy") != "stdlib/semantic_policy.json":
        return fail("package surface semantic_policy drifted")
    if package_surface.get("machine_output_root") != "tmp/artifacts/stdlib":
        return fail("package surface machine_output_root drifted")
    if package_surface.get("machine_report_root") != "tmp/reports/stdlib":
        return fail("package surface machine_report_root drifted")
    if package_surface.get("package_stage_root") != "tmp/pkg/objc3c-native-runnable-toolchain":
        return fail("package surface package_stage_root drifted")
    if package_surface.get("import_model") != "implementation-alias-module-declarations-map-to-canonical-spec-module-ids":
        return fail("package surface import_model drifted")
    if core_architecture.get("contract_id") != "objc3c.stdlib.core_architecture.v1":
        return fail("core architecture contract_id drifted")
    if core_architecture.get("schema_version") != 1:
        return fail("core architecture schema_version drifted")
    if core_architecture.get("workspace_contract") != "stdlib/workspace.json":
        return fail("core architecture workspace_contract drifted")
    if core_architecture.get("runbook") != "docs/runbooks/objc3c_stdlib_core.md":
        return fail("core architecture runbook drifted")
    if core_architecture.get("scope") != "foundation-utility-text-data-collections-option-result-surface":
        return fail("core architecture scope drifted")
    if semantic_policy.get("contract_id") != "objc3c.stdlib.semantic_policy.v1":
        return fail("semantic policy contract_id drifted")
    if semantic_policy.get("schema_version") != 1:
        return fail("semantic policy schema_version drifted")
    if semantic_policy.get("workspace_contract") != "stdlib/workspace.json":
        return fail("semantic policy workspace_contract drifted")
    if semantic_policy.get("core_architecture") != "stdlib/core_architecture.json":
        return fail("semantic policy core_architecture drifted")

    canonical_modules = inventory.get("canonical_modules")
    if not isinstance(canonical_modules, list) or not canonical_modules:
        return fail("module inventory missing canonical_modules")

    inventory_rows: list[dict[str, str]] = []
    required_extra_fields = (
        "implementation_module",
        "workspace_root",
        "source",
        "smoke_source",
        "manifest",
    )
    for entry in canonical_modules:
        if not isinstance(entry, dict):
            return fail("module inventory entry must be an object")
        module = entry.get("module")
        capability_id = entry.get("capability_id")
        required_profile = entry.get("required_profile")
        if not all(isinstance(value, str) and value for value in (module, capability_id, required_profile)):
            return fail("module inventory entry is missing module/capability_id/required_profile")
        for field in required_extra_fields:
            value = entry.get(field)
            if not isinstance(value, str) or not value:
                return fail(f"module inventory entry {module} is missing {field}")
        inventory_rows.append(
            {
                "module": module,
                "implementation_module": str(entry["implementation_module"]),
                "capability_id": capability_id,
                "required_profile": required_profile,
                "workspace_root": str(entry["workspace_root"]),
                "source": str(entry["source"]),
                "smoke_source": str(entry["smoke_source"]),
                "manifest": str(entry["manifest"]),
            }
        )

    spec_rows = parse_spec_canonical_modules(spec_text)
    spec_comparison_rows = [
        {
            "module": entry["module"],
            "capability_id": entry["capability_id"],
            "required_profile": entry["required_profile"],
        }
        for entry in inventory_rows
    ]
    if spec_rows != spec_comparison_rows:
        return fail("module inventory drifted from spec canonical module table")

    layers = stability_policy.get("layers")
    if not isinstance(layers, list) or not layers:
        return fail("stability policy missing layers")
    inventory_module_names = {entry["module"] for entry in inventory_rows}
    covered_modules: set[str] = set()
    for layer in layers:
        if not isinstance(layer, dict):
            return fail("stability policy layer must be an object")
        layer_name = layer.get("name")
        modules = layer.get("modules")
        allowed_dependencies = layer.get("allowed_dependencies")
        if not isinstance(layer_name, str) or not layer_name:
            return fail("stability policy layer missing name")
        if not isinstance(modules, list) or not modules:
            return fail(f"stability policy layer {layer_name} missing modules")
        if not isinstance(allowed_dependencies, list):
            return fail(f"stability policy layer {layer_name} missing allowed_dependencies")
        for module_name in modules:
            if not isinstance(module_name, str) or not module_name:
                return fail(f"stability policy layer {layer_name} published an invalid module")
            if module_name not in inventory_module_names:
                return fail(f"stability policy layer {layer_name} referenced unknown module {module_name}")
            covered_modules.add(module_name)
        for dependency in allowed_dependencies:
            if not isinstance(dependency, str) or not dependency:
                return fail(f"stability policy layer {layer_name} published an invalid dependency")
            if dependency not in inventory_module_names:
                return fail(
                    f"stability policy layer {layer_name} referenced unknown dependency {dependency}"
                )
    if covered_modules != inventory_module_names:
        return fail("stability policy module coverage drifted from module inventory")

    module_imports = package_surface.get("module_imports")
    if not isinstance(module_imports, list) or not module_imports:
        return fail("package surface missing module_imports")
    package_imports_by_module: dict[str, dict[str, str]] = {}
    for entry in module_imports:
        if not isinstance(entry, dict):
            return fail("package surface module_import entry must be an object")
        canonical_module = entry.get("canonical_module")
        implementation_module = entry.get("implementation_module")
        source_declaration = entry.get("source_declaration")
        if not all(
            isinstance(value, str) and value
            for value in (canonical_module, implementation_module, source_declaration)
        ):
            return fail("package surface module_import entry is malformed")
        package_imports_by_module[str(canonical_module)] = {
            "implementation_module": str(implementation_module),
            "source_declaration": str(source_declaration),
        }

    for entry in inventory_rows:
        for path_key in ("workspace_root", "source", "smoke_source", "manifest"):
            path = ROOT / entry[path_key]
            if path_key == "workspace_root":
                if not path.is_dir():
                    return fail(f"missing module workspace root: {entry[path_key]}")
            else:
                if not path.is_file():
                    return fail(f"missing module artifact: {entry[path_key]}")
        manifest_payload = load_json(ROOT / entry["manifest"])
        if manifest_payload.get("contract_id") != "objc3c.stdlib.module.surface.v1":
            return fail(f"module manifest contract_id drifted for {entry['module']}")
        if manifest_payload.get("canonical_module") != entry["module"]:
            return fail(f"module manifest canonical_module drifted for {entry['module']}")
        if manifest_payload.get("implementation_module") != entry["implementation_module"]:
            return fail(f"module manifest implementation_module drifted for {entry['module']}")
        if manifest_payload.get("capability_id") != entry["capability_id"]:
            return fail(f"module manifest capability_id drifted for {entry['module']}")
        if manifest_payload.get("workspace_root") != entry["workspace_root"]:
            return fail(f"module manifest workspace_root drifted for {entry['module']}")
        if manifest_payload.get("module_semver") != semantic_policy.get("module_semver", {}).get(entry["module"]):
            return fail(f"module manifest module_semver drifted for {entry['module']}")
        if manifest_payload.get("source") != entry["source"]:
            return fail(f"module manifest source drifted for {entry['module']}")
        if manifest_payload.get("smoke_source") != entry["smoke_source"]:
            return fail(f"module manifest smoke_source drifted for {entry['module']}")
        source_text = (ROOT / entry["source"]).read_text(encoding="utf-8")
        expected_decl = f"module {entry['implementation_module']};"
        if expected_decl not in source_text:
            return fail(f"module source declaration drifted for {entry['module']}")
        package_import = package_imports_by_module.get(entry["module"])
        if package_import is None:
            return fail(f"package surface missing canonical module {entry['module']}")
        if package_import["implementation_module"] != entry["implementation_module"]:
            return fail(f"package surface implementation_module drifted for {entry['module']}")
        if package_import["source_declaration"] != expected_decl:
            return fail(f"package surface source_declaration drifted for {entry['module']}")
        manifest_api_families = manifest_payload.get("api_families")
        if not isinstance(manifest_api_families, list) or not all(
            isinstance(value, str) and value for value in manifest_api_families
        ):
            return fail(f"module manifest api_families malformed for {entry['module']}")
        manifest_exports = manifest_payload.get("exports")
        if not isinstance(manifest_exports, list) or not all(
            isinstance(value, str) and value for value in manifest_exports
        ):
            return fail(f"module manifest exports malformed for {entry['module']}")
        for export_name in manifest_exports:
            if export_name not in source_text:
                return fail(
                    f"module source missing exported symbol spelling {export_name} for {entry['module']}"
                )

    architecture_live_paths = core_architecture.get("live_paths")
    if not isinstance(architecture_live_paths, list) or not architecture_live_paths:
        return fail("core architecture missing live_paths")
    for raw_path in architecture_live_paths:
        if not isinstance(raw_path, str) or not raw_path:
            return fail("core architecture live_paths entry malformed")
        path = ROOT / raw_path
        if not path.exists():
            return fail(f"core architecture live path missing: {raw_path}")

    architecture_api_families = core_architecture.get("api_families")
    if not isinstance(architecture_api_families, dict) or not architecture_api_families:
        return fail("core architecture missing api_families")
    inventory_modules_by_name = {entry["module"]: entry for entry in inventory_rows}
    for module_name, families in architecture_api_families.items():
        if not isinstance(module_name, str) or module_name not in inventory_modules_by_name:
            return fail(f"core architecture referenced unknown module {module_name}")
        if not isinstance(families, list) or not families:
            return fail(f"core architecture api_families malformed for {module_name}")
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name]["manifest"])
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
        manifest_payload = load_json(ROOT / inventory_modules_by_name[module_name]["manifest"])
        if manifest_payload.get("exports") != required_exports:
            return fail(f"module manifest exports drifted for {module_name}")

    semantic_module_semver = semantic_policy.get("module_semver")
    if not isinstance(semantic_module_semver, dict) or not semantic_module_semver:
        return fail("semantic policy missing module_semver")
    for module_name, version_payload in semantic_module_semver.items():
        if module_name not in inventory_modules_by_name:
            return fail(f"semantic policy referenced unknown module {module_name}")
        if not isinstance(version_payload, dict):
            return fail(f"semantic policy module_semver malformed for {module_name}")
        if version_payload != {"major": 1, "minor": 0, "patch": 0}:
            return fail(f"semantic policy module_semver drifted for {module_name}")

    error_semantics = semantic_policy.get("error_semantics")
    if not isinstance(error_semantics, dict):
        return fail("semantic policy missing error_semantics")
    if error_semantics.get("result_ok_tag") != 1 or error_semantics.get("result_err_tag") != 2:
        return fail("semantic policy result tag values drifted")
    if error_semantics.get("result_bridge_diagnostic") != (
        "returns 0 when option presence matches the provided result tag, otherwise 30601"
    ):
        return fail("semantic policy result_bridge_diagnostic drifted")
    if error_semantics.get("text_data_compatibility_diagnostic") != (
        "returns 0 when unit_count equals byte_count, otherwise 30602"
    ):
        return fail("semantic policy text_data_compatibility_diagnostic drifted")

    keypath_semantics = semantic_policy.get("keypath_semantics")
    if not isinstance(keypath_semantics, dict):
        return fail("semantic policy missing keypath_semantics")
    if keypath_semantics.get("text_compatibility_diagnostic") != (
        "returns 0 when text_units is at least component_count, otherwise 30603"
    ):
        return fail("semantic policy text_compatibility_diagnostic drifted")

    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(
        json.dumps(
            {
                "contract_id": SUMMARY_CONTRACT_ID,
                "schema_version": 1,
                "status": "PASS",
                "workspace_contract": repo_rel(WORKSPACE_PATH),
                "module_inventory": repo_rel(MODULE_INVENTORY_PATH),
                "stability_policy": repo_rel(STABILITY_POLICY_PATH),
                "package_surface": repo_rel(PACKAGE_SURFACE_PATH),
                "core_architecture": repo_rel(CORE_ARCHITECTURE_PATH),
                "semantic_policy": repo_rel(SEMANTIC_POLICY_PATH),
                "spec_contract": repo_rel(SPEC_CONTRACT_PATH),
                "canonical_modules": inventory_rows,
                "layers": layers,
                "module_imports": module_imports,
                "api_families": architecture_api_families,
                "required_exports": architecture_required_exports,
                "module_semver": semantic_module_semver,
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
