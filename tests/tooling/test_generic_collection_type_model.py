from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "stdlib_collections"
    / "generic_collection_type_model_contract.json"
)
MATRIX_PATH = ROOT / "docs" / "support" / "capability_matrix.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _read_text(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def test_generic_collection_type_model_has_source_backed_shapes() -> None:
    contract = _read_json(CONTRACT_PATH)
    model_header = _read_text("native/objc3c/src/sema/model/generic_collection_type_model.h")
    model_source = _read_text(
        "native/objc3c/src/sema/objc3_semantic_generic_collection_type_model.cpp"
    )
    factory_source = _read_text("native/objc3c/src/sema/objc3_semantic_type_factory.cpp")
    relations_source = _read_text("native/objc3c/src/sema/objc3_semantic_type_relations.cpp")
    lowering_contract = _read_text(
        "native/objc3c/src/sema/objc3_semantic_type_lowering_contract.h"
    )

    for path in contract["source_truth"]:
        assert (ROOT / path).is_file(), path

    for shape in contract["collection_shapes"]:
        assert shape["kind"] in model_header
        assert f'type_name == "{shape["kind"]}"' in model_source
        if shape["arity"] == 1:
            assert "argument_count == 1u" in model_source
        if shape["arity"] == 2:
            assert "argument_count == 2u" in model_source

    required_model_tokens = {
        "element_type_identity",
        "key_type_identity",
        "value_type_identity",
        "iterator_element_type_identity",
        "runtime_descriptor_identity",
        "abi_identity",
        "Objc3GenericCollectionVariance::Invariant",
        "Objc3GenericCollectionMutability::Mutable",
        "nested generic collection shape is not supported",
        "unsupported generic collection key type",
        "unsupported generic collection element type",
    }
    for token in required_model_tokens:
        assert token in model_header + model_source

    assert "MakeGenericCollectionSemanticType" in factory_source
    assert "AreSameObjc3GenericCollectionTypeModel" in relations_source
    assert "AreObjc3GenericCollectionTypeModelsAssignmentCompatible" in model_source
    assert "BuildObjc3GenericCollectionTypeModel(type)" in lowering_contract


def test_generic_collection_type_model_evidence_rows_are_claimable_without_literals() -> None:
    contract = _read_json(CONTRACT_PATH)
    matrix = _read_json(MATRIX_PATH)
    catalog = _read_json(CATALOG_PATH)
    manifest = _read_json(MANIFEST_PATH)

    matrix_rows = {row["id"]: row for row in matrix["capabilities"]}
    catalog_rows = {row["support_claim"]: row for row in catalog["rows"]}
    manifest_claims = {row["claim_id"]: row for row in manifest["support_claims"]}
    fixture_paths = {row["path"] for row in manifest["fixtures"]}

    for claim in contract["support_claims"]:
        capability = matrix_rows[claim["capability_id"]]
        catalog_row = catalog_rows[claim["support_claim"]]
        manifest_row = manifest_claims[claim["support_claim"]]

        assert capability["state"] == "implemented"
        assert capability["support_claims"] == [claim["support_claim"]]
        assert manifest_row["owner_phase"] == claim["owner_phase"]
        assert catalog_row["owner_phase"] == claim["owner_phase"]
        assert catalog_row["conformance_fixture"] == str(
            CONTRACT_PATH.relative_to(ROOT).as_posix()
        )
        assert catalog_row["traceability_fixture"] in contract["source_truth"]
        assert catalog_row["positive_evidence"] == contract["source_truth"]
        assert sorted(catalog_row["required_diagnostic_codes"]) == ["O3S206"]

    assert str(CONTRACT_PATH.relative_to(ROOT).as_posix()) in fixture_paths

    forbidden_claim_fragments = ["literal", "for-in", "interpolation", "foundation"]
    for claim in contract["support_claims"]:
        claim_text = claim["support_claim"].lower()
        assert not any(fragment in claim_text for fragment in forbidden_claim_fragments)
