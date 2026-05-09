from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from typing import Any

import pytest

from capability_docs_validator.cli import build_parser
from capability_docs_validator.constants import (
    CAPABILITY_DOCS_SURFACE_OWNER,
    CAPABILITY_EVIDENCE_OWNER,
    CAPABILITY_MANIFEST_OWNER,
    CAPABILITY_MATRIX_OWNER,
    CAPABILITY_TRUTH_OWNER,
    capability_truth_owner_contract,
)
from capability_docs_validator.docs import _validate_docs_reference_rows
from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.manifest import _manifest_support_claims
from capability_docs_validator.matrix import _require_matrix_shape
from capability_docs_validator.support_links import (
    _row_support_claims,
    _validate_support_claim_links,
)
from scripts.check_objc3c_public_claim_drift import (
    PUBLIC_CLAIM_DRIFT_OWNER,
    public_claim_drift_owner_contract,
)

ROOT = Path(__file__).resolve().parents[2]
LEGACY_ENTRYPOINT = ROOT / "scripts" / "validate_capability_docs.py"
BEHAVIOR_MATRIX_COMMAND = "npm run objc3c -- test-behavior-matrix"


def _claim() -> dict[str, str]:
    return {
        "claim_id": "objc3c.behavior.parser.canonical-syntax",
        "owner_phase": "parser",
        "behavior_fixture": "tests/native/parser/positive/canonical_module_main.objc3",
        "executable_command": BEHAVIOR_MATRIX_COMMAND,
    }


def _manifest(*claims: dict[str, str]) -> dict[str, Any]:
    return {
        "support_claims": list(claims),
        "fixtures": [
            {
                "path": claim["behavior_fixture"],
                "origin": "hand-authored",
                "owner_phase": claim["owner_phase"],
            }
            for claim in claims
        ],
    }


def _row() -> dict[str, Any]:
    claim = _claim()
    return {
        "id": "compiler.parser.core-declarations",
        "state": "implemented",
        "support_claims": [claim["claim_id"]],
        "evidence": [
            {
                "kind": "test",
                "path": claim["behavior_fixture"],
                "command": claim["executable_command"],
            }
        ],
    }


def test_legacy_entrypoint_reexports_validator_owner_modules() -> None:
    spec = importlib.util.spec_from_file_location("validate_capability_docs", LEGACY_ENTRYPOINT)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)

    assert module.build_parser is build_parser
    assert module._manifest_support_claims is _manifest_support_claims
    assert module._validate_support_claim_links is _validate_support_claim_links
    assert module._validate_docs_reference_rows is _validate_docs_reference_rows


def test_matrix_shape_claim_links_and_manifest_validation_are_separate_owners() -> None:
    rows = _require_matrix_shape({"capabilities": [_row()]})

    assert _row_support_claims(rows[0]) == [_claim()["claim_id"]]
    assert _manifest_support_claims(_manifest(_claim()))[_claim()["claim_id"]][
        "owner_phase"
    ] == "parser"
    _validate_support_claim_links(rows, _manifest(_claim()))


def test_capability_truth_contract_declares_matrix_evidence_and_blocker_owners() -> None:
    owner_contract = capability_truth_owner_contract()

    assert owner_contract["truth_owner"]["owner_id"] == CAPABILITY_TRUTH_OWNER
    assert owner_contract["matrix_owner"]["owner_id"] == CAPABILITY_MATRIX_OWNER
    assert owner_contract["evidence_owner"]["owner_id"] == CAPABILITY_EVIDENCE_OWNER
    assert owner_contract["manifest_owner"]["owner_id"] == CAPABILITY_MANIFEST_OWNER
    assert (
        owner_contract["docs_surface_owner"]["owner_id"]
        == CAPABILITY_DOCS_SURFACE_OWNER
    )
    assert owner_contract["blocker_metadata"]["blocker_contract"] == (
        "hard-cutover-capability-truth-fail-closed"
    )


def test_public_claim_drift_contract_declares_surface_owner_and_blockers() -> None:
    owner_contract = public_claim_drift_owner_contract(
        public_surfaces={"README.md"},
        scan_paths=["README.md", "docs/support/capability_matrix.md"],
    )

    assert owner_contract["owner_id"] == PUBLIC_CLAIM_DRIFT_OWNER
    assert owner_contract["public_claim_surface_count"] == 1
    assert owner_contract["scan_path_count"] == 2
    assert owner_contract["blocker_metadata"]["blocker_contract"] == (
        "hard-cutover-public-claim-drift-fail-closed"
    )


def test_claim_link_owner_rejects_duplicate_matrix_claim_links() -> None:
    rows = [_row(), {**_row(), "id": "compiler.parser.duplicate"}]

    with pytest.raises(CapabilityDocsError, match="linked by both"):
        _validate_support_claim_links(rows, _manifest(_claim()))
