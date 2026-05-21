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
    source = source_index()
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
        "source_index": source,
        "source_declaration_count": source["declaration_count"],
        "source_reference_count": source["reference_count"],
        "source_import_count": source["import_count"],
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


def source_index() -> dict[str, Any]:
    source_path = "tests/tooling/fixtures/native/hello.objc3"
    return {
        "contract_id": "objc3c.developer.tooling.source.index.v1",
        "source_path": source_path,
        "module_name": "Demo",
        "available": True,
        "source_truth_model": "source-text-plus-compile-manifest-diagnostics-and-summary-paths",
        "manifest_path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.manifest.json",
        "declaration_count": 1,
        "reference_count": 1,
        "import_count": 0,
        "diagnostic_anchor_count": 0,
        "emitted_artifact_count": 1,
        "declarations": [
            {
                "symbol": "main",
                "kind": "function",
                "module": "Demo",
                "location": location(source_path),
                "definition": definition(source_path),
                "hover": {
                    "contents": "function main",
                    "module": "Demo",
                    "source": "compile-manifest-declaration-coordinate",
                },
                "origin": "compile-manifest",
            }
        ],
        "references": [
            {
                "symbol": "main",
                "module": "Demo",
                "location": location(source_path),
                "reference_kind": "definition",
                "definition_reference": True,
                "origin": "source-lexical-index",
            }
        ],
        "imports": [],
        "diagnostic_anchors": [],
        "emitted_artifacts": [
            {
                "kind": "manifest",
                "path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.manifest.json",
                "origin": "compile-summary-paths",
            }
        ],
        "source_index_digest": "e" * 64,
        "deterministic_ordering": "source-location-then-kind-then-symbol",
        "retired_route_reason": "",
    }


def language_server() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.developer.tooling.language.server.capability.surface.v1",
        "summary_status_name": "ok",
        "manifest_backed_navigation": True,
        "workspace_index_backed_navigation": True,
        "source_index_backed_hover": True,
        "source_index_digest": "e" * 64,
        "source_graph_backed_references": False,
        "source_graph_digest": "f" * 64,
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
            "hover": [
                "compile-manifest-declaration-coordinates",
                "source-derived-editor-index",
            ],
            "codeAction": ["diagnostics-json-fixits"],
        },
        "publication_boundary": "only diagnostics, compile-owned declaration coordinates, source-index hover, workspace guardrails, and diagnostic fix-its publish positive LSP rows",
        "supported_capability_ids": [
            "publishDiagnostics",
            "documentSymbol",
            "workspaceSymbol",
            "definition",
            "hover",
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
            "hover": {
                "supported": True,
                "support_class": "source-index-backed",
                "evidence_ids": [
                    "compile-manifest-declaration-coordinates",
                    "source-derived-editor-index",
                ],
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


def source_graph() -> dict[str, Any]:
    source_path = "tests/tooling/fixtures/native/hello.objc3"
    loc = location(source_path)
    return {
        "contract_id": "objc3c.developer.tooling.source.graph.v1",
        "source_path": source_path,
        "module_name": "Demo",
        "available": True,
        "fail_closed": True,
        "support_class": "compiler-declaration-graph-with-fail-closed-reference-candidates",
        "graph_truth_model": "compiler-owned declarations plus checked package provenance; lexical candidates are non-authoritative until native semantic reference closure is emitted",
        "evidence": {
            "source_truth_inputs": [
                "compile-manifest-declaration-coordinates",
                "workspace-semantic-index-guardrails",
                "source-index-unverified-reference-candidates",
            ],
            "manifest_path": "tmp/artifacts/developer-tooling/editor-surface/hello/module.manifest.json",
            "compiler_graph_fields": [],
            "source_index_digest": "e" * 64,
            "workspace_index_digest": "a" * 64,
            "compiler_declaration_coordinates": True,
            "native_compiler_source_graph_present": False,
            "semantic_reference_closure": False,
            "lexical_candidates_authoritative": False,
            "reference_authority": "compiler-owned semantic reference closure required",
            "package_authority": "workspace-index package guardrails",
            "unsupported_authoritative_claims": [
                "regex-only reference truth",
                "rename without semantic reference closure",
                "semantic tokens from lexical tokenization alone",
            ],
        },
        "node_count": 2,
        "edge_count": 2,
        "declaration_node_count": 1,
        "reference_candidate_count": 0,
        "semantic_reference_count": 1,
        "package_node_count": 0,
        "package_dependency_edge_count": 0,
        "semantic_token_count": 1,
        "nodes": [
            {
                "node_id": "sgn:module",
                "symbol_kind": "module",
                "display_name": "Demo",
                "location": loc,
            },
            {
                "node_id": "sgn:main",
                "symbol_kind": "function",
                "display_name": "main",
                "location": loc,
            },
        ],
        "edges": [
            {"edge_id": "sge:module-main", "edge_kind": "module-declaration"},
            {"edge_id": "sge:main-self", "edge_kind": "declaration-to-reference"},
        ],
        "declarations": [
            {
                "node_id": "sgn:main",
                "symbol": "main",
                "kind": "function",
                "definition_target": definition(source_path),
            }
        ],
        "reference_candidates": [],
        "navigation_consumers": {
            "definition_targets": [],
            "implementation_targets": [],
            "generated_accessor_targets": [],
            "semantic_token_source": "source_graph.semantic_tokens.tokens",
        },
        "references": {
            "supported": False,
            "support_class": "source-graph-fail-closed",
            "evidence_ids": [],
            "fail_closed": True,
            "unpublished_reason": "native compiler semantic reference closure is missing",
            "query_model": "node-id-to-semantic-reference-edges",
            "candidate_count": 0,
            "queries": [],
            "diagnostics": [],
        },
        "rename": {
            "supported": False,
            "support_class": "source-graph-fail-closed",
            "evidence_ids": [],
            "fail_closed": True,
            "unpublished_reason": "safe rename remains disabled",
            "edit_model": "workspace-edit-from-semantic-reference-closure",
            "diagnostics": [],
            "negative_case_policy": [],
        },
        "semantic_tokens": {
            "supported": False,
            "support_class": "source-graph-fail-closed",
            "evidence_ids": [],
            "fail_closed": True,
            "unpublished_reason": "full semantic token publication requires compiler-owned reference classification",
            "legend": {"token_types": ["function"], "token_modifiers": ["declaration"]},
            "tokens": [],
        },
        "package_provenance": {
            "local_package_id": f"source:{source_path}",
            "package_count": 2,
            "workspace_index_digest": "a" * 64,
            "package_dependencies": [],
        },
        "consumer_capabilities": {
            "references": {
                "supported": False,
                "support_class": "source-graph-fail-closed",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "native compiler semantic reference closure is missing",
            },
            "rename": {
                "supported": False,
                "support_class": "source-graph-fail-closed",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "safe rename remains disabled",
            },
            "semanticTokens": {
                "supported": False,
                "support_class": "source-graph-fail-closed",
                "evidence_ids": [],
                "fail_closed": True,
                "unpublished_reason": "full semantic token publication requires compiler-owned reference classification",
            },
        },
        "diagnostics": [],
        "source_graph_digest": "f" * 64,
        "deterministic_ordering": "node-kind-canonical-name-range-then-edge-kind-source-target-range",
        "retired_route_reason": "",
        "remaining_native_compiler_work": [
            "emit structured declaration-to-reference semantic edges with source spans from sema"
        ],
    }


def artifact_inspector(contract: dict[str, Any]) -> dict[str, Any]:
    source = source_index()
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
            "object_format": "coff",
            "expected_sha256": "c" * 64,
            "digest_matches": True,
            "inventory_source_model": "emitted-object-tool-symbol-and-section-inventory",
            "tool_inventory_available": True,
            "inventory_available": True,
            "inventory_digest": "f" * 64,
            "symbol_count": 2,
            "symbols": [
                {"name": "_main", "binding": "exported"},
                {"name": "objc3_runtime_bootstrap", "binding": "exported"},
            ],
            "section_count": 1,
            "sections": [{"name": ".text"}],
            "exported_runtime_helper_count": 1,
            "exported_runtime_helpers": [
                {"name": "objc3_runtime_bootstrap", "binding": "exported"}
            ],
            "imported_runtime_helper_count": 0,
            "imported_runtime_helpers": [],
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
        "runtime_inventory": {
            "available": True,
            "reflection_abi_version": "objc3-runtime-reflection-v1",
            "runtime_metadata_link": "tmp/artifacts/developer-tooling/editor-surface/hello/runtime.bin",
            "inventory_digest": "f" * 64,
            "class_record_count": 1,
            "class_records": [{"name": "Demo"}],
            "selector_record_count": 1,
            "selector_records": [{"name": "main"}],
            "method_record_count": 1,
            "method_records": [{"selector": "main"}],
            "property_record_count": 0,
            "property_records": [],
            "protocol_record_count": 0,
            "protocol_records": [],
            "category_record_count": 0,
            "category_records": [],
            "stdlib_helper_references": ["objc3_runtime_bootstrap"],
            "runtime_import_package_records": [],
            "retired_route_reason": "",
        },
        "package_inventory": {
            "available": True,
            "module_identity": "Demo",
            "package_identity": "source:tests/tooling/fixtures/native/hello.objc3",
            "abi_identity": "objc3-abi-v1",
            "package_identity_source": "manifest",
            "abi_identity_source": "manifest",
            "manifest_package_identity": "source:tests/tooling/fixtures/native/hello.objc3",
            "registry_package_identity": "source:tests/tooling/fixtures/native/hello.objc3",
            "receipt_package_identities": ["source:tests/tooling/fixtures/native/hello.objc3"],
            "identity_mismatch": False,
            "registry_identity": "local-registry-fixture",
            "manifest_trust_status": "trusted",
            "registry_trust_status": "trusted",
            "package_operation_receipts": [
                {
                    "path": "tmp/artifacts/package-ecosystem/install-validation/objc3c-install-receipt.json",
                    "available": True,
                    "operation": "install",
                    "package_id": "source:tests/tooling/fixtures/native/hello.objc3",
                    "sha256": "d" * 64,
                    "expected_sha256": "d" * 64,
                    "digest_matches": True,
                    "trust_status": "trusted",
                    "trusted": True,
                    "retired_route_reason": "",
                }
            ],
            "package_operation_receipt_count": 1,
            "untrusted_receipt_count": 0,
            "retired_route_reason": "",
        },
        "source_graph": {
            "available": True,
            "graph_inputs": [
                "manifest-declarations",
                "workspace-index-packages",
                "source-derived-editor-index",
            ],
            "declaration_node_count": 1,
            "workspace_package_count": 2,
            "workspace_index_digest": "a" * 64,
            "source_declaration_count": 1,
            "source_reference_count": 1,
            "source_index_digest": "e" * 64,
            "source_graph_digest": "d" * 64,
            "retired_route_reason": "",
        },
        "artifact_links": {
            "manifest_link": "tmp/artifacts/developer-tooling/editor-surface/hello/manifest.json",
            "ir_link": "tmp/artifacts/developer-tooling/editor-surface/hello/module.ll",
            "diagnostics_link": "tmp/artifacts/developer-tooling/editor-surface/hello/diagnostics.json",
            "runtime_metadata_link": "tmp/artifacts/developer-tooling/editor-surface/hello/runtime.bin",
            "source_graph_link": "tmp/reports/developer-tooling/editor-surface/hello/source-graph.json",
            "source_graph_digest": "d" * 64,
            "debug_map_link": "tmp/reports/developer-tooling/editor-surface/hello/debug-map.json",
            "optimization_trace_link": "tmp/artifacts/developer-tooling/editor-surface/hello/optimization-trace.json",
        },
        "provenance": {
            "available": True,
            "generated": True,
            "source_truth_inputs": ["tests/tooling/fixtures/native/hello.objc3"],
            "generated_artifacts": [],
            "retired_route_reason": "",
        },
        "inventory_validation": {
            "inventory_ready": True,
            "fail_closed": False,
            "fail_closed_reasons": [],
            "unsupported_inventory_notes": [],
        },
        "source_index": source,
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
    source = source_index()
    graph = source_graph()
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
        "hover_targets": [
            {
                "name": "main",
                "kind": "function",
                "contents": "function main",
                "target_uri": "tests/tooling/fixtures/native/hello.objc3",
                "target_range": location("tests/tooling/fixtures/native/hello.objc3")["range"],
                "target_compiler_range": location("tests/tooling/fixtures/native/hello.objc3")["compiler_range"],
            }
        ],
        "source_index": source,
        "source_graph_digest": graph["source_graph_digest"],
        "source_graph_navigation": graph["navigation_consumers"],
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
        "source_index": source,
        "source_graph": graph,
        "workspace_index": workspace,
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
            "source_graph_path": "tmp/reports/developer-tooling/editor-surface/hello/source-graph.json",
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
            "source_graph": payload["source_graph"],
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
