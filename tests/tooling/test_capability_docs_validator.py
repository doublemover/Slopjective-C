from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from typing import Any

import pytest

ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = ROOT / "scripts" / "validate_capability_docs.py"

PUBLIC_CAPABILITY_DOCS = (
    ROOT / "README.md",
    ROOT / "docs" / "support" / "README.md",
    ROOT / "docs" / "support" / "capability_matrix.md",
    ROOT / "docs" / "support" / "hard_cutover_capability_truth.md",
    ROOT / "docs" / "support" / "evidence_map.md",
    ROOT / "docs" / "workflows" / "commands.md",
    ROOT / "docs" / "workflows" / "validation.md",
    ROOT / "docs" / "workflows" / "ci.md",
    ROOT / "site" / "src" / "README.md",
    ROOT / "site" / "src" / "OWNERSHIP.md",
    ROOT / "site" / "src" / "index.body.md",
    ROOT / "site" / "index.md",
)

FORBIDDEN_PUBLIC_DOC_SNIPPETS = (
    "lint" + "-default",
    "python -m scripts." + "objc3c_workflow",
    "python scripts/" + "objc3c_workflow",
    "npm run lint",
    "npm run build",
    "supported through a retired adapter",
    "accepted by an alternate parser path",
    "available through a retired mode label",
    "retired-source lane accepts old syntax",
    "implemented because a roadmap says it is planned",
    "complete because a generated report says so without a matching implemented row",
    "future support rows",
    "future-spec surface",
    "projected completion claim",
)

PARSER_CLAIM = {
    "claim_id": "objc3c.behavior.parser.canonical-syntax",
    "owner_phase": "parser",
    "behavior_fixture": "tests/native/parser/positive/canonical_module_main.objc3",
    "executable_command": "npm run objc3c -- test-behavior-matrix",
}

RUNTIME_CLAIM = {
    "claim_id": "objc3c.behavior.runtime.strict-dispatch-error",
    "owner_phase": "runtime",
    "behavior_fixture": "tests/native/runtime/dispatch/message_send_runtime_dispatch_strict_error.objc3",
    "executable_command": "npm run objc3c -- test-behavior-matrix",
}


def _load_validator():
    spec = importlib.util.spec_from_file_location("validate_capability_docs", VALIDATOR_PATH)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _manifest(*claims: dict[str, str]) -> dict[str, Any]:
    return {
        "schema_version": 1,
        "support_claims": list(claims),
        "fixtures": [
            {
                "path": claim["behavior_fixture"],
                "origin": "hand-authored",
                "owner_phase": claim["owner_phase"],
                "behavior_family": "validator-test",
                "fixture_kind": "positive",
                "expected_diagnostic_code": "",
            }
            for claim in claims
        ],
    }


def test_public_capability_docs_reject_retired_public_surface_claims() -> None:
    for path in PUBLIC_CAPABILITY_DOCS:
        text = path.read_text(encoding="utf-8")
        for snippet in FORBIDDEN_PUBLIC_DOC_SNIPPETS:
            assert snippet not in text, f"{path.relative_to(ROOT)} contains {snippet!r}"


def _parser_row() -> dict[str, Any]:
    return {
        "id": "compiler.parser.core-declarations",
        "title": "Canonical parser syntax",
        "state": "implemented",
        "summary": "Parser claim used by validator tests.",
        "support_claims": [PARSER_CLAIM["claim_id"]],
        "evidence": [
            {
                "kind": "test",
                "path": PARSER_CLAIM["behavior_fixture"],
                "command": PARSER_CLAIM["executable_command"],
            }
        ],
    }


def test_support_claim_links_accept_manifest_fixture_evidence() -> None:
    validator = _load_validator()

    validator._validate_support_claim_links([_parser_row()], _manifest(PARSER_CLAIM))


def test_manifest_support_claims_reject_evidence_log_commands() -> None:
    validator = _load_validator()
    evidence_log_claim = {
        **PARSER_CLAIM,
        "executable_command": "python scripts/render_behavior_report.py",
    }

    with pytest.raises(validator.CapabilityDocsError, match="must use executable command"):
        validator._manifest_support_claims(_manifest(evidence_log_claim))


def test_support_claim_links_require_behavior_matrix_fixture_evidence() -> None:
    validator = _load_validator()
    row = _parser_row()
    row["evidence"] = [
        {
            "kind": "test",
            "path": "tests/tooling/test_objc3c_parser_extraction.py",
            "command": "python -m pytest tests/tooling/test_objc3c_parser_extraction.py",
        }
    ]

    with pytest.raises(validator.CapabilityDocsError, match="must include executable evidence"):
        validator._validate_support_claim_links([row], _manifest(PARSER_CLAIM))


def test_support_claim_links_require_every_manifest_claim_in_the_matrix() -> None:
    validator = _load_validator()

    with pytest.raises(validator.CapabilityDocsError, match="missing from capability matrix"):
        validator._validate_support_claim_links([_parser_row()], _manifest(PARSER_CLAIM, RUNTIME_CLAIM))


def test_runtime_concurrency_claim_is_implemented_and_probe_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    rows = {row["id"]: row for row in matrix["capabilities"]}
    row = rows["runtime.concurrency.async-actors"]

    assert row["state"] == "implemented"
    assert row["support_claims"] == [
        "objc3c.behavior.runtime.concurrency-async-actors"
    ]
    evidence_paths = {evidence["path"] for evidence in row["evidence"]}
    assert {
        "tests/native/runtime/concurrency/actor_executor_contract.objc3",
        "scripts/objc3c_runtime_acceptance/domains/concurrency_live_runtime_cases.py",
        "tests/tooling/runtime/continuation_runtime_helper_probe.cpp",
        "tests/tooling/runtime/live_task_runtime_and_executor_implementation_probe.cpp",
        "tests/tooling/runtime/live_actor_mailbox_runtime_probe.cpp",
    } <= evidence_paths


def test_runtime_object_model_interface_claim_is_narrow_and_evidence_backed() -> None:
    matrix = json.loads(
        (ROOT / "docs" / "support" / "capability_matrix.json").read_text(
            encoding="utf-8"
        )
    )
    rows = {row["id"]: row for row in matrix["capabilities"]}
    row = rows["runtime.object-model.interface-method-table"]

    assert row["state"] == "implemented"
    assert row["support_claims"] == [
        "objc3c.behavior.runtime.object-model-interface-method-table"
    ]
    evidence_paths = {evidence["path"] for evidence in row["evidence"]}
    assert {
        "tests/native/runtime/object_model/interface_method_table_contract.objc3",
        "scripts/objc3c_runtime_acceptance/domains/object_model_surface_class_cases.py",
        "scripts/objc3c_runtime_acceptance/domains/object_model_surface_query_implementation.py",
        "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
        "native/objc3c/src/runtime/classes/class_graph.cpp",
        "native/objc3c/src/runtime/classes/category_attachment.cpp",
        "native/objc3c/src/runtime/classes/protocol_conformance.cpp",
    } <= evidence_paths
    assert rows["runtime.object-model.full-realization"]["state"] == "reserved"


def test_object_model_implemented_rows_reject_broad_realization_language() -> None:
    validator = _load_validator()
    row = {
        "id": "runtime.object-model.interface-method-table",
        "title": "Full object-model runtime realization",
        "state": "implemented",
        "summary": "Broad object-model behavior over every runtime surface.",
        "support_claims": [
            "objc3c.behavior.runtime.object-model-interface-method-table"
        ],
        "owner_modules": ["native/objc3c/src/runtime/classes/class_graph.cpp"],
        "evidence": [
            {
                "kind": "test",
                "path": "tests/native/runtime/object_model/interface_method_table_contract.objc3",
                "command": "npm run objc3c -- test-behavior-matrix",
            },
            {
                "kind": "source",
                "path": "native/objc3c/src/runtime/classes/class_graph.cpp",
            },
        ],
    }

    with pytest.raises(validator.CapabilityDocsError, match="must stay narrow"):
        validator._validate_object_model_scope([row])
