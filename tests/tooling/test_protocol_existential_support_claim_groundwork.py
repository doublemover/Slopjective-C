from __future__ import annotations

import json
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[2]
MANIFEST_PATH = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
CATALOG_PATH = ROOT / "tests" / "conformance" / "support_claim_runnable_evidence_catalog.json"

CLAIM_ID = "objc3c.behavior.language.protocols.protocol-qualified-existential-value-flow"
WITNESS_CLAIM_ID = "objc3c.behavior.language.protocols.existential-witness-model"
CAPABILITY_ID = "language.protocols.protocol-qualified-existential-value-flow"
WITNESS_CAPABILITY_ID = "language.protocols.existential-witness-model"
BEHAVIOR_FIXTURE = (
    "tests/tooling/fixtures/native/protocol_qualified_existential_value_flow.objc3"
)
WITNESS_BEHAVIOR_FIXTURE = (
    "tests/tooling/fixtures/native/execution/positive/id_protocol_qualifier_alias_signature.objc3"
)
PUBLIC_COMMAND = "npm run objc3c -- validate-conformance-corpus"

REQUIRED_POSITIVE_EVIDENCE = {
    BEHAVIOR_FIXTURE,
    "tests/tooling/fixtures/native/protocol_composition_existential_value_flow.objc3",
    "tests/tooling/fixtures/native/type_semantic_model_closure_positive.objc3",
    "tests/conformance/semantic/TYP-8013-01.json",
    "tests/conformance/semantic/TYP-8013-18.json",
    "tests/conformance/semantic/TYP-8013-25.json",
    "native/objc3c/src/runtime/classes/protocol_conformance.h",
    "native/objc3c/src/runtime/classes/protocol_conformance.cpp",
    "native/objc3c/src/runtime/public/objc3_runtime_language_semantics.cpp",
    "native/objc3c/src/pipeline/runtime_import_type_system_preservation_protocol.cpp",
    "scripts/objc3c_type_semantic_model_closure/positive.py",
    "scripts/objc3c_type_semantic_model_closure/summary_checks.py",
}

REQUIRED_NEGATIVE_EVIDENCE = {
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_duplicate_protocol_composition.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_unknown_protocol_composition.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_optional_required_conflict.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_associated_type_rejected.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_dynamic_dispatch_rejected.objc3",
    "tests/conformance/semantic/TYP-8013-02.json",
    "tests/conformance/semantic/TYP-8013-06.json",
    "tests/conformance/semantic/TYP-8013-07.json",
    "tests/conformance/semantic/TYP-8013-19.json",
    "tests/conformance/semantic/TYP-8013-21.json",
    "tests/conformance/semantic/TYP-8013-22.json",
}

REQUIRED_WITNESS_EVIDENCE = {
    WITNESS_BEHAVIOR_FIXTURE,
    "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
    "tests/tooling/runtime/category_attachment_protocol_runtime_probe.cpp",
    "tests/tooling/fixtures/objc3c/language_semantics_runtime_api_contract.json",
    "tests/tooling/fixtures/language_semantics_public_model/public_language_semantics_contract.json",
    "native/objc3c/src/sema/model/language_semantics_public_model.h",
    "native/objc3c/src/runtime/classes/protocol_conformance.h",
    "native/objc3c/src/runtime/classes/protocol_conformance.cpp",
    "native/objc3c/src/runtime/classes/protocol_conformance_snapshots.cpp",
    "native/objc3c/src/runtime/public/objc3_runtime_language_semantics.h",
    "native/objc3c/src/runtime/public/objc3_runtime_language_semantics.cpp",
}

REQUIRED_WITNESS_NEGATIVE_EVIDENCE = {
    "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_associated_type_rejected.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_dynamic_dispatch_rejected.objc3",
    "tests/tooling/fixtures/native/recovery/negative/negative_type_semantic_protocol_qualified_unknown_message.objc3",
    "tests/conformance/semantic/TYP-8013-21.json",
    "tests/conformance/semantic/TYP-8013-22.json",
    "tests/conformance/semantic/TYP-8013-07.json",
}


def _read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def _assert_repo_path_exists(path: str) -> None:
    assert not path.startswith(("tmp/", "tmp\\"))
    assert (ROOT / path).is_file(), path


def test_protocol_existential_fixture_manifest_row_is_narrow_positive_evidence() -> None:
    manifest = _read_json(MANIFEST_PATH)
    fixtures = {
        str(fixture["path"]): fixture
        for fixture in manifest["fixtures"]
        if isinstance(fixture, dict)
    }

    fixture = fixtures[BEHAVIOR_FIXTURE]
    assert fixture["origin"] == "hand-authored"
    assert fixture["owner_phase"] == "sema"
    assert fixture["behavior_family"] == "objc"
    assert fixture["fixture_kind"] == "positive"
    assert fixture["expected_diagnostic_code"] == ""
    _assert_repo_path_exists(BEHAVIOR_FIXTURE)
