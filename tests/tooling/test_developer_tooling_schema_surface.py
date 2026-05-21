from __future__ import annotations

import copy
import sys
from pathlib import Path
from typing import Any

import pytest


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import JsonSchemaValidationError, load_json_object, validate_json_schema  # noqa: E402
from scripts.check_objc3c_developer_tooling_schema_surface import (  # noqa: E402
    build_summary,
    validate_artifact_inspector_surface,
    validate_capability_boundaries,
    validate_debug_surface,
    validate_embedded_payload_consistency,
    validate_workspace_surface,
)


FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "developer_tooling"
CONTRACT_PATH = FIXTURE_ROOT / "tooling_schema_surface_contract.json"
EDITOR_SCHEMA_PATH = ROOT / "schemas" / "objc3c-developer-tooling-editor-surface-v1.schema.json"
SUMMARY_SCHEMA_PATH = ROOT / "schemas" / "objc3c-developer-tooling-schema-surface-summary-v1.schema.json"


def location(source_path: str) -> dict[str, Any]:
    return {
        "uri": source_path,
        "source_path": source_path,
        "compiler_range": {
            "line": 1,
            "column": 1,
            "end_line": 1,
            "end_column": 5,
        },
        "range": {
            "start": {"line": 0, "character": 0},
            "end": {"line": 0, "character": 4},
        },
        "range_model": "compiler-1-based-plus-lsp-zero-based",
    }


def definition(source_path: str) -> dict[str, Any]:
    loc = location(source_path)
    return {
        "target_uri": source_path,
        "target_range": loc["range"],
        "target_compiler_range": loc["compiler_range"],
    }


def symbol_record(
    *,
    name: str = "main",
    kind: str = "function",
    package_id: str | None = None,
    source_path: str = "tests/tooling/fixtures/native/hello.objc3",
    origin: str = "compile-manifest",
) -> dict[str, Any]:
    record = {
        "name": name,
        "kind": kind,
        "container_name": package_id or "Demo",
        "source_path": source_path,
        "location": location(source_path),
        "definition": definition(source_path),
        "origin": origin,
    }
    if package_id is not None:
        record["package_id"] = package_id
        record["cross_package"] = package_id.startswith(("stdlib:", "showcase:"))
    return record


def workspace_index(contract: dict[str, Any]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.workspace.semantic.index.v1",
        "available": True,
        "index_model": "compile-manifest-plus-checked-in-package-workspace-surfaces",
        "source_truth_model": "checked-in-workspace-and-package-surfaces-plus-live-frontend-manifest",
        "evidence_roots": [
            *contract["expected_workspace_source_truth_inputs"],
            "tmp/artifacts/developer-tooling/editor-surface/hello/module.manifest.json",
        ],
        "unsupported_surfaces": list(contract["expected_workspace_unsupported_surfaces"]),
        "fail_closed": False,
        "package_count": 2,
        "cross_package_edge_count": 1,
        "packages": [
            {
                "package_id": "source:tests/tooling/fixtures/native/hello.objc3",
                "package_kind": "primary-source",
                "module_name": "Demo",
                "source_path": "tests/tooling/fixtures/native/hello.objc3",
                "source_authority": "live-frontend-compile",
                "definition": location("tests/tooling/fixtures/native/hello.objc3"),
            },
            {
                "package_id": "stdlib:objc3.core",
                "package_kind": "stdlib-module",
                "module_name": "objc3.core",
                "source_path": "stdlib/modules/objc3.core/module.objc3",
                "source_authority": "stdlib/module_inventory.json",
                "definition": location("stdlib/modules/objc3.core/module.objc3"),
            },
        ],
        "cross_package_edges": [
            {
                "from_package_id": "showcase:auroraBoard",
                "to_package_id": "stdlib:objc3.core",
                "edge_kind": "stdlib-followup-module",
                "source_authority": "showcase/portfolio.json",
            }
        ],
        "package_symbols": [
            symbol_record(
                name="objc3.core",
                kind="stdlib-module",
                package_id="stdlib:objc3.core",
                source_path="stdlib/modules/objc3.core/module.objc3",
                origin="stdlib/module_inventory.json",
            )
        ],
        "guardrails": {
            "contract_id": "objc3c.developer.tooling.workspace.package.guardrails.v1",
            "source_contracts": contract["expected_workspace_source_truth_inputs"][3:],
            "checks": {
                "lockfile_deterministic": True,
                "offline_mirror_lock_derived": True,
                "offline_mirror_no_network": True,
                "tamper_rejection_diagnostic_owned": True,
                "mixed_version_policy_owned": True,
                "tmp_not_source_authority": True,
            },
            "ok": True,
        },
        "workspace_index_digest": "a" * 64,
        "deterministic_ordering": "package-id-then-source-path",
        "retired_route_reason": "",
    }


def language_server() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.language.server.capability.surface.v1",
        "summary_status_name": "ok",
        "manifest_backed_navigation": True,
        "workspace_index_backed_navigation": True,
        "diagnostic_transport": {
            "contract_id": "objc3c.developer.tooling.lsp.diagnostic.transport.v1",
            "source_path": "tests/tooling/fixtures/native/hello.objc3",
            "publish_method": "textDocument/publishDiagnostics",
            "diagnostic_count": 0,
            "severity_counts": {},
            "diagnostics": [],
            "machine_applicable_fixit_count": 0,
            "code_action_count": 0,
            "code_actions": [],
            "recovery_boundary_count": 0,
            "recovery_boundaries": [],
            "recovery_acceptance_policy": "recovery metadata is diagnostic context only and never turns an invalid program into a success",
        },
        "capability_evidence_roots": {
            "publishDiagnostics": ["diagnostics-json"],
            "documentSymbol": ["compile-manifest-declaration-coordinates"],
            "workspaceSymbol": [
                "compile-manifest-declaration-coordinates",
                "workspace-semantic-index-guardrails",
            ],
            "definition": ["compile-manifest-declaration-coordinates"],
            "codeAction": ["diagnostics-json-fixits"],
        },
        "publication_boundary": "only diagnostics, compile-owned declaration coordinates, workspace guardrails, and diagnostic fix-its publish positive LSP rows",
        "supported_capability_ids": [
            "publishDiagnostics",
            "documentSymbol",
            "workspaceSymbol",
            "definition",
        ],
        "unpublished_capability_ids": [
            "references",
            "rename",
            "semanticTokens",
            "codeAction",
            "statementLevelStepping",
        ],
        "capability_statuses": {
            "publishDiagnostics": {
                "supported": True,
                "support_class": "authoritative",
                "evidence": "diagnostics-json",
                "evidence_ids": ["diagnostics-json"],
                "fail_closed": False,
            },
            "documentSymbol": {
                "supported": True,
                "support_class": "manifest-backed",
                "evidence_ids": ["compile-manifest-declaration-coordinates"],
                "fail_closed": False,
                "unpublished_reason": "",
            },
            "workspaceSymbol": {
                "supported": True,
                "support_class": "workspace-index-backed",
                "evidence_ids": [
                    "compile-manifest-declaration-coordinates",
                    "workspace-semantic-index-guardrails",
                ],
                "fail_closed": False,
                "unpublished_reason": "",
            },
            "definition": {
                "supported": True,
                "support_class": "manifest-backed",
                "evidence_ids": ["compile-manifest-declaration-coordinates"],
                "fail_closed": False,
                "unpublished_reason": "",
            },
            "references": {
                "supported": False,
                "support_class": "fail-closed-unpublished",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "not published",
            },
            "rename": {
                "supported": False,
                "support_class": "fail-closed-unpublished",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "not published",
            },
            "semanticTokens": {
                "supported": False,
                "support_class": "fail-closed-unpublished",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "not published",
            },
            "codeAction": {
                "supported": False,
                "support_class": "fail-closed",
                "evidence": "",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "disabled until diagnostics emit machine-applicable fix-its",
            },
            "statementLevelStepping": {
                "supported": False,
                "support_class": "fail-closed-unpublished",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "not published",
            },
        },
    }


def artifact_inspector(contract: dict[str, Any]) -> dict[str, Any]:
    records = [
        {
            "kind": kind,
            "path": f"tmp/artifacts/developer-tooling/editor-surface/hello/{kind}.json",
            "available": True,
            "size_bytes": 1,
            "sha256": "b" * 64,
            "explanation": f"{kind} artifact",
            "retired_route_reason": "",
        }
        for kind in contract["expected_artifact_record_kinds"]
    ]
    return {
        "contract_id": "objc3c.developer.tooling.artifact.inspector.v1",
        "source_path": "tests/tooling/fixtures/native/hello.objc3",
        "supported": True,
        "support_class": "compile-artifact-inspector",
        "artifact_records": records,
        "inspected_artifact_kinds": list(contract["expected_artifact_record_kinds"]),
        "unsupported_artifact_kinds": [],
        "diagnostics": {
            "available": True,
            "path": "tmp/artifacts/developer-tooling/editor-surface/hello/diagnostics.json",
            "diagnostic_count": 0,
            "severity_counts": {},
            "diagnostic_codes": [],
            "retired_route_reason": "",
        },
        "manifest": {
            "available": True,
            "path": "tmp/artifacts/developer-tooling/editor-surface/hello/manifest.json",
            "module": "Demo",
            "declaration_count": 1,
            "symbol_kind_counts": {"function": 1},
            "source_graph_field_count": 0,
            "source_graph_fields": [],
            "retired_route_reason": "",
        },
        "ir": {
            "available": True,
            "path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.ll",
            "line_count": 1,
            "function_definition_count": 1,
            "external_declaration_count": 0,
            "global_record_count": 0,
            "retired_route_reason": "",
        },
        "object": {
            "available": True,
            "path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.obj",
            "size_bytes": 1,
            "sha256": "c" * 64,
            "object_symbol_inventory_command": "llvm-objdump --syms module.obj",
            "object_section_inventory_command": "llvm-readobj --sections module.obj",
            "inspection_ready": True,
            "retired_route_reason": "",
        },
        "runtime_imports": {
            "available": True,
            "runtime_metadata_binary_path": "tmp/artifacts/developer-tooling/editor-surface/hello/runtime.bin",
            "runtime_metadata_binary_present": True,
            "runtime_import_count": 0,
            "runtime_imports": [],
            "runtime_inspector_available": True,
            "runtime_inspector_contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
            "runtime_dump_commands": {
                "object_sections": "llvm-readobj --sections module.obj",
                "object_symbols": "llvm-objdump --syms module.obj",
            },
            "retired_route_reason": "",
        },
        "source_graph": {
            "available": True,
            "graph_inputs": ["manifest-declarations", "workspace-index-packages"],
            "declaration_node_count": 1,
            "workspace_package_count": 2,
            "workspace_index_digest": "a" * 64,
            "source_graph_digest": "d" * 64,
            "retired_route_reason": "",
        },
        "inspection_commands": {
            key: f"inspect {key}" for key in contract["expected_inspection_command_keys"]
        },
        "retired_route_reason": "",
    }


def debug_surface(contract: dict[str, Any]) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.debug.map.surface.v1",
        "supported": True,
        "support_class": "declaration-breakpoint-preview",
        "debugger_model": "declaration-breakpoint-and-object-symbol-inspection",
        "source_map_supported": False,
        "source_map_model": "declaration-coordinate-only",
        "statement_level_stepping": False,
        "stepping_retired_route_reason": "statement-level stepping remains fail-closed until emitted line-table evidence exists on the canonical toolchain path",
        "object_artifact_present": True,
        "object_path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.obj",
        "declaration_breakpoint_anchor_count": 1,
        "declaration_breakpoints": [
            {"symbol": "main", "kind": "function", "line": 1, "column": 1}
        ],
        "object_section_inventory_command": "llvm-readobj --sections module.obj",
        "object_symbol_inventory_command": "llvm-objdump --syms module.obj",
        "runtime_debug_trace_command": "npm run objc3c -- trace-runtime-debug",
        "runtime_debug_trace_path": "tmp/reports/objc3c-public-workflow/runtime-debug-trace.json",
        "runtime_debug_trace_schema": "schemas/objc3c-runtime-debug-trace-v1.schema.json",
        "runtime_debug_trace_model": "deterministic-runtime-inspector-and-editor-debug-artifact-trace",
        "runtime_inspector_contract_id": "objc3c.runtime.metadata.object.inspection.harness.v1",
        "artifact_inspection_ready": True,
        "evidence_roots": [
            "compile-manifest-declaration-coordinates",
            "runtime-inspector-object-symbol-inventory",
        ],
        "reserved_capability_rows": [
            {
                "capability_id": capability_id,
                "status": "reserved",
                "fail_closed": True,
                "unpublished_reason": "not emitted on the canonical toolchain path",
            }
            for capability_id in contract["reserved_debug_capabilities"]
        ],
        "retired_route_reason": "",
    }


def representative_surface(contract: dict[str, Any]) -> dict[str, Any]:
    workspace = workspace_index(contract)
    document_symbol = symbol_record()
    navigation = {
        "contract_id": "objc3c.developer.tooling.navigation.index.v1",
        "source_path": "tests/tooling/fixtures/native/hello.objc3",
        "available": True,
        "manifest_path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.manifest.json",
        "symbol_count": 1,
        "supported_symbol_kinds": ["function"],
        "symbols": [
            {
                "name": "main",
                "kind": "function",
                "line": 1,
                "column": 1,
                "end_line": 1,
                "end_column": 5,
            }
        ],
        "document_symbols": [document_symbol],
        "workspace_symbols": [document_symbol, *workspace["package_symbols"]],
        "definition_targets": [
            {
                "name": "main",
                "kind": "function",
                "target_uri": "tests/tooling/fixtures/native/hello.objc3",
                "target_range": location("tests/tooling/fixtures/native/hello.objc3")["range"],
                "target_compiler_range": location("tests/tooling/fixtures/native/hello.objc3")["compiler_range"],
            }
        ],
        "workspace_index": workspace,
        "retired_route_reason": "",
    }
    return {
        "contract_id": "objc3c.developer.tooling.editor.surface.v1",
        "source_path": "tests/tooling/fixtures/native/hello.objc3",
        "summary_path": "tmp/reports/developer-tooling/editor-surface/hello/compile-summary.json",
        "manifest_path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.manifest.json",
        "diagnostics_path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.diagnostics.json",
        "diagnostics": {"status_name": "ok", "total": 0, "entries": []},
        "language_server": language_server(),
        "navigation": navigation,
        "artifact_inspector": artifact_inspector(contract),
        "formatter": {
            "contract_id": "objc3c.developer.tooling.formatter.surface.v1",
            "source_path": "tests/tooling/fixtures/native/hello.objc3",
            "supported": True,
            "support_class": "canonical-objc3-source-formatting",
            "preview_subset_id": "objc3c.format.canonical-objc3-source-subset.v2",
            "formatted_output_path": "tmp/reports/developer-tooling/editor-surface/hello/formatted-source.objc3",
            "changed": False,
            "source_line_count": 1,
            "formatted_line_count": 1,
            "retired_route_reason": "",
            "feature_ids": [],
            "diagnostics": [],
            "max_delimiter_depth": 0,
        },
        "debug": debug_surface(contract),
    }


def test_developer_tooling_editor_schema_validates_representative_payload() -> None:
    contract = load_json_object(CONTRACT_PATH)
    editor_schema = load_json_object(EDITOR_SCHEMA_PATH)
    summary_schema = load_json_object(SUMMARY_SCHEMA_PATH)
    payload = representative_surface(contract)

    validate_json_schema(payload, editor_schema, label="representative editor surface")

    summary = build_summary(
        contract=contract,
        editor_surface_path="tmp/reports/developer-tooling/editor-surface/hello/editor-surface.json",
        checked_payload_paths={
            "capabilities_path": "tmp/reports/developer-tooling/editor-surface/hello/language-server-capabilities.json",
            "navigation_path": "tmp/reports/developer-tooling/editor-surface/hello/navigation-index.json",
            "workspace_index_path": "tmp/reports/developer-tooling/editor-surface/hello/workspace-index.json",
            "artifact_inspector_path": "tmp/reports/developer-tooling/editor-surface/hello/artifact-inspector.json",
            "formatter_path": "tmp/reports/developer-tooling/editor-surface/hello/formatter-output.json",
            "debug_path": "tmp/reports/developer-tooling/editor-surface/hello/debug-map.json",
        },
        failures=[],
    )
    validate_json_schema(summary, summary_schema, label="representative schema summary")


def test_developer_tooling_schema_blocks_missing_contract_sections() -> None:
    contract = load_json_object(CONTRACT_PATH)
    payload = representative_surface(contract)
    payload.pop("artifact_inspector")

    with pytest.raises(JsonSchemaValidationError, match="artifact_inspector"):
        validate_json_schema(payload, load_json_object(EDITOR_SCHEMA_PATH), label="missing artifact inspector")


def test_developer_tooling_schema_checker_accepts_truthful_boundaries() -> None:
    contract = load_json_object(CONTRACT_PATH)
    payload = representative_surface(contract)
    failures: list[str] = []

    validate_embedded_payload_consistency(
        payload,
        {
            "language_server": payload["language_server"],
            "navigation": payload["navigation"],
            "workspace_index": payload["navigation"]["workspace_index"],
            "artifact_inspector": payload["artifact_inspector"],
            "formatter": payload["formatter"],
            "debug": payload["debug"],
        },
        failures,
    )
    validate_capability_boundaries(payload["language_server"], contract, failures)
    validate_workspace_surface(payload["navigation"]["workspace_index"], contract, failures)
    validate_artifact_inspector_surface(payload["artifact_inspector"], contract, failures)
    validate_debug_surface(payload["debug"], contract, failures)

    assert failures == []


def test_developer_tooling_schema_checker_rejects_overpublished_lsp_rows() -> None:
    contract = load_json_object(CONTRACT_PATH)
    payload = representative_surface(contract)
    language_server_payload = copy.deepcopy(payload["language_server"])
    language_server_payload["supported_capability_ids"].append("semanticTokens")
    language_server_payload["unpublished_capability_ids"].remove("semanticTokens")
    language_server_payload["capability_statuses"]["semanticTokens"] = {
        "supported": True,
        "support_class": "semantic-token-preview",
        "evidence_ids": ["unowned-preview"],
        "fail_closed": False,
        "unpublished_reason": "",
    }

    failures: list[str] = []
    validate_capability_boundaries(language_server_payload, contract, failures)

    assert any("semanticTokens" in failure for failure in failures)
