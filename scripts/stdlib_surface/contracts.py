from __future__ import annotations

from typing import Any

from check_stdlib_surface_model import StdlibSurfaceDocuments


FieldCheck = tuple[dict[str, Any], str, object, str]


def _first_drift(checks: tuple[FieldCheck, ...]) -> str | None:
    for payload, field, expected, message in checks:
        if payload.get(field) != expected:
            return message
    return None


def validate_document_headers(documents: StdlibSurfaceDocuments) -> str | None:
    workspace = documents.workspace
    inventory = documents.inventory
    stability_policy = documents.stability_policy
    package_surface = documents.package_surface
    core_architecture = documents.core_architecture
    advanced_architecture = documents.advanced_architecture
    semantic_policy = documents.semantic_policy
    lowering_import_surface = documents.lowering_import_surface

    return _first_drift(
        (
            (workspace, "contract_id", "objc3c.stdlib.workspace.v1", "workspace contract_id drifted"),
            (workspace, "schema_version", 1, "workspace schema_version drifted"),
            (workspace, "module_inventory", "stdlib/module_inventory.json", "workspace module_inventory path drifted"),
            (workspace, "stability_policy", "stdlib/stability_policy.json", "workspace stability_policy path drifted"),
            (workspace, "package_surface", "stdlib/package_surface.json", "workspace package_surface path drifted"),
            (workspace, "core_architecture", "stdlib/core_architecture.json", "workspace core_architecture path drifted"),
            (workspace, "advanced_architecture", "stdlib/advanced_architecture.json", "workspace advanced_architecture path drifted"),
            (workspace, "semantic_policy", "stdlib/semantic_policy.json", "workspace semantic_policy path drifted"),
            (workspace, "lowering_import_surface", "stdlib/lowering_import_surface.json", "workspace lowering_import_surface path drifted"),
            (workspace, "advanced_helper_package_surface", "stdlib/advanced_helper_package_surface.json", "workspace advanced_helper_package_surface path drifted"),
            (workspace, "program_surface", "stdlib/program_surface.json", "workspace program_surface path drifted"),
            (workspace, "core_runbook", "docs/runbooks/objc3c_stdlib_core.md", "workspace core_runbook path drifted"),
            (workspace, "advanced_runbook", "docs/runbooks/objc3c_stdlib_advanced.md", "workspace advanced_runbook path drifted"),
            (workspace, "program_runbook", "docs/runbooks/objc3c_stdlib_program.md", "workspace program_runbook path drifted"),
            (inventory, "contract_id", "objc3c.stdlib.module_inventory.v1", "module inventory contract_id drifted"),
            (inventory, "schema_version", 1, "module inventory schema_version drifted"),
            (inventory, "spec_contract", "spec/STANDARD_LIBRARY_CONTRACT.md", "module inventory spec_contract drifted"),
            (stability_policy, "contract_id", "objc3c.stdlib.stability_policy.v1", "stability policy contract_id drifted"),
            (stability_policy, "schema_version", 1, "stability policy schema_version drifted"),
            (package_surface, "contract_id", "objc3c.stdlib.package_surface.v1", "package surface contract_id drifted"),
            (package_surface, "schema_version", 1, "package surface schema_version drifted"),
            (package_surface, "workspace_contract", "stdlib/workspace.json", "package surface workspace_contract drifted"),
            (package_surface, "module_inventory", "stdlib/module_inventory.json", "package surface module_inventory drifted"),
            (package_surface, "stability_policy", "stdlib/stability_policy.json", "package surface stability_policy drifted"),
            (package_surface, "core_architecture", "stdlib/core_architecture.json", "package surface core_architecture drifted"),
            (package_surface, "advanced_architecture", "stdlib/advanced_architecture.json", "package surface advanced_architecture drifted"),
            (package_surface, "semantic_policy", "stdlib/semantic_policy.json", "package surface semantic_policy drifted"),
            (package_surface, "lowering_import_surface", "stdlib/lowering_import_surface.json", "package surface lowering_import_surface drifted"),
            (package_surface, "advanced_helper_package_surface", "stdlib/advanced_helper_package_surface.json", "package surface advanced_helper_package_surface drifted"),
            (package_surface, "program_surface", "stdlib/program_surface.json", "package surface program_surface drifted"),
            (package_surface, "machine_output_root", "tmp/artifacts/stdlib", "package surface machine_output_root drifted"),
            (package_surface, "machine_report_root", "tmp/reports/stdlib", "package surface machine_report_root drifted"),
            (package_surface, "package_stage_root", "tmp/pkg/objc3c-native-runnable-toolchain", "package surface package_stage_root drifted"),
            (package_surface, "import_model", "compiler-visible-module-declarations-map-to-canonical-spec-module-ids", "package surface import_model drifted"),
            (core_architecture, "contract_id", "objc3c.stdlib.core_architecture.v1", "core architecture contract_id drifted"),
            (core_architecture, "schema_version", 1, "core architecture schema_version drifted"),
            (core_architecture, "workspace_contract", "stdlib/workspace.json", "core architecture workspace_contract drifted"),
            (core_architecture, "runbook", "docs/runbooks/objc3c_stdlib_core.md", "core architecture runbook drifted"),
            (core_architecture, "scope", "foundation-utility-text-data-collections-option-result-surface", "core architecture scope drifted"),
            (advanced_architecture, "contract_id", "objc3c.stdlib.advanced_architecture.v1", "advanced architecture contract_id drifted"),
            (advanced_architecture, "schema_version", 1, "advanced architecture schema_version drifted"),
            (advanced_architecture, "workspace_contract", "stdlib/workspace.json", "advanced architecture workspace_contract drifted"),
            (advanced_architecture, "foundation_contract", "stdlib/core_architecture.json", "advanced architecture foundation_contract drifted"),
            (advanced_architecture, "runbook", "docs/runbooks/objc3c_stdlib_advanced.md", "advanced architecture runbook drifted"),
            (advanced_architecture, "scope", "advanced-helper-concurrency-reflection-interop-runtime-composition-surface", "advanced architecture scope drifted"),
            (semantic_policy, "contract_id", "objc3c.stdlib.semantic_policy.v1", "semantic policy contract_id drifted"),
            (semantic_policy, "schema_version", 1, "semantic policy schema_version drifted"),
            (semantic_policy, "workspace_contract", "stdlib/workspace.json", "semantic policy workspace_contract drifted"),
            (semantic_policy, "core_architecture", "stdlib/core_architecture.json", "semantic policy core_architecture drifted"),
            (semantic_policy, "advanced_architecture", "stdlib/advanced_architecture.json", "semantic policy advanced_architecture drifted"),
            (lowering_import_surface, "contract_id", "objc3c.stdlib.lowering_import_surface.v1", "lowering/import surface contract_id drifted"),
            (lowering_import_surface, "schema_version", 1, "lowering/import surface schema_version drifted"),
            (lowering_import_surface, "workspace_contract", "stdlib/workspace.json", "lowering/import surface workspace_contract drifted"),
            (lowering_import_surface, "package_surface", "stdlib/package_surface.json", "lowering/import surface package_surface drifted"),
            (lowering_import_surface, "module_inventory", "stdlib/module_inventory.json", "lowering/import surface module_inventory drifted"),
            (lowering_import_surface, "smoke_runner", "scripts/run_objc3c_stdlib_workspace_smoke.py", "lowering/import surface smoke_runner drifted"),
            (lowering_import_surface, "machine_output_root", "tmp/artifacts/stdlib", "lowering/import surface machine_output_root drifted"),
            (lowering_import_surface, "machine_report_root", "tmp/reports/stdlib", "lowering/import surface machine_report_root drifted"),
            (lowering_import_surface, "materialized_workspace_root", "tmp/artifacts/stdlib/workspace", "lowering/import surface materialized_workspace_root drifted"),
            (lowering_import_surface, "smoke_artifact_root", "tmp/artifacts/stdlib/smoke", "lowering/import surface smoke_artifact_root drifted"),
        )
    )
