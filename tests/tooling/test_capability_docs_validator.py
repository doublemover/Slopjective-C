from __future__ import annotations

import importlib.util
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
    "lint-default",
    "python -m scripts.objc3c_workflow",
    "python scripts/objc3c_workflow",
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


def test_manifest_support_claims_reject_report_only_commands() -> None:
    validator = _load_validator()
    report_only_claim = {
        **PARSER_CLAIM,
        "executable_command": "python scripts/render_behavior_report.py",
    }

    with pytest.raises(validator.CapabilityDocsError, match="must use executable command"):
        validator._manifest_support_claims(_manifest(report_only_claim))


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
