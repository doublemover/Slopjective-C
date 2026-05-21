from __future__ import annotations

import importlib.util
import json
import sys
from copy import deepcopy
from pathlib import Path
from typing import Any

import pytest


ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = ROOT / "scripts" / "validate_capability_docs.py"
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"

WITNESS_CLAIM = "objc3c.behavior.language.protocols.existential-witness-model"
CROSS_MODULE_GENERIC_CLAIM = "objc3c.behavior.runtime.generics.cross-module-metadata"


def _load_validator():
    scripts_path = str(ROOT / "scripts")
    if scripts_path not in sys.path:
        sys.path.insert(0, scripts_path)
    spec = importlib.util.spec_from_file_location("validate_capability_docs", VALIDATOR_PATH)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _load_live_rows_and_catalog() -> tuple[list[dict[str, Any]], dict[str, Any]]:
    matrix = json.loads(MATRIX_PATH.read_text(encoding="utf-8"))
    catalog = json.loads(CATALOG_PATH.read_text(encoding="utf-8"))
    rows = matrix["capabilities"]
    assert isinstance(rows, list)
    assert isinstance(catalog, dict)
    return rows, catalog


def _catalog_row(catalog: dict[str, Any], support_claim: str) -> dict[str, Any]:
    rows = catalog["rows"]
    assert isinstance(rows, list)
    for row in rows:
        if isinstance(row, dict) and row.get("support_claim") == support_claim:
            return row
    raise AssertionError(f"missing catalog row {support_claim}")


def _matrix_row(rows: list[dict[str, Any]], capability_id: str) -> dict[str, Any]:
    for row in rows:
        if row.get("id") == capability_id:
            return row
    raise AssertionError(f"missing matrix row {capability_id}")


def test_type_protocol_claim_gate_accepts_live_8160_8164_rows() -> None:
    validator = _load_validator()
    rows, catalog = _load_live_rows_and_catalog()

    validator._validate_type_protocol_capability_rows(rows, catalog)


def test_type_protocol_claim_gate_requires_protocol_fail_closed_negative_evidence() -> None:
    validator = _load_validator()
    rows, catalog = _load_live_rows_and_catalog()
    catalog = deepcopy(catalog)
    row = _catalog_row(catalog, WITNESS_CLAIM)
    row["negative_evidence"] = [
        path
        for path in row["negative_evidence"]
        if path
        != "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_dynamic_dispatch_rejected.objc3"
    ]

    with pytest.raises(validator.CapabilityDocsError, match="negative_evidence.*dynamic_dispatch"):
        validator._validate_type_protocol_capability_rows(rows, catalog)


def test_type_protocol_claim_gate_requires_generic_abi_source_owner() -> None:
    validator = _load_validator()
    rows, catalog = _load_live_rows_and_catalog()
    rows = deepcopy(rows)
    catalog = deepcopy(catalog)
    row = _matrix_row(rows, "runtime.generics.cross-module-metadata")
    row["owner_modules"] = [
        path
        for path in row["owner_modules"]
        if path != "native/objc3c/src/pipeline/objc3_runtime_import_surface.h"
    ]
    row["evidence"] = [
        evidence
        for evidence in row["evidence"]
        if evidence.get("path") != "native/objc3c/src/pipeline/objc3_runtime_import_surface.h"
    ]
    catalog_row = _catalog_row(catalog, CROSS_MODULE_GENERIC_CLAIM)
    catalog_row["positive_evidence"] = [
        path
        for path in catalog_row["positive_evidence"]
        if path != "native/objc3c/src/pipeline/objc3_runtime_import_surface.h"
    ]

    with pytest.raises(validator.CapabilityDocsError, match="source/owner evidence.*objc3_runtime_import_surface"):
        validator._validate_type_protocol_capability_rows(rows, catalog)


def test_type_protocol_claim_gate_requires_bounded_source_truth_text() -> None:
    validator = _load_validator()
    rows, catalog = _load_live_rows_and_catalog()
    catalog = deepcopy(catalog)
    row = _catalog_row(catalog, CROSS_MODULE_GENERIC_CLAIM)
    row["source_truth_requirements"] = [
        "Cross-module generic metadata has replay manifests and semantic fixtures."
    ]

    with pytest.raises(validator.CapabilityDocsError, match="fail-closed evidence"):
        validator._validate_type_protocol_capability_rows(rows, catalog)
