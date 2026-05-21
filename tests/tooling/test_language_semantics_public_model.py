from __future__ import annotations

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_language_semantics_public_model import (  # noqa: E402
    CONTRACT_ID,
    CONTRACT_PATH,
    MODEL_HEADER_PATH,
    REQUIRED_ISSUES,
    REQUIRED_SUPPORT_CLAIMS,
    REQUIRED_SURFACES,
    SCHEMA_PATH,
    validate_language_semantics_public_model,
)


def test_language_semantics_public_model_fixture_validates_source_truth() -> None:
    result = validate_language_semantics_public_model(CONTRACT_PATH)

    assert result.passed, result.failures
    assert result.payload["contract_id"] == (
        "objc3c.language.semantics.public-model.validation.v1"
    )
    assert set(result.payload["issue_mapping"]["issues"]) == REQUIRED_ISSUES
    assert set(result.payload["issue_mapping"]["support_claims"]) >= REQUIRED_SUPPORT_CLAIMS
    assert set(result.payload["surfaces"]["surface_names"]) == REQUIRED_SURFACES
    assert result.payload["source_truth"]["source_anchor_count"] >= 7
    assert result.payload["handoff_count"] >= 6


def test_language_semantics_public_model_schema_and_contract_are_stable() -> None:
    schema_text = SCHEMA_PATH.read_text(encoding="utf-8")

    assert CONTRACT_ID == "objc3c.language.semantics.public-model.v1"
    assert '"workflow_action": {' in schema_text
    assert '"const": "validate-language-semantics-public-model"' in schema_text
    assert '"fallback_or_compatibility_shim_allowed": { "const": false }' in schema_text
    assert '"support_claim": { "const": "objc3c.behavior.language.generics.public-type-parameters" }' in schema_text


def test_language_semantics_public_model_header_exposes_typed_surfaces() -> None:
    header_text = MODEL_HEADER_PATH.read_text(encoding="utf-8")
    umbrella_text = (
        ROOT / "native" / "objc3c" / "src" / "sema" / "model" / "semantic_type.h"
    ).read_text(encoding="utf-8")

    for token in (
        "Objc3GenericTypeParameterSurface",
        "Objc3GenericSpecializationSurface",
        "Objc3ProtocolExistentialWitnessSurface",
        "Objc3OwnershipFlowSurface",
        "Objc3ConcurrencyEffectSurface",
        "Objc3ModuleVisibilitySurface",
        "Objc3InteropLaneSurface",
        "fallback_or_compatibility_shim_allowed",
        "IsReadyObjc3PublicLanguageSemanticsModelSummary",
    ):
        assert token in header_text
    assert "sema/model/language_semantics_public_model.h" in umbrella_text


def test_language_semantics_public_model_keeps_unsupported_claims_false() -> None:
    result = validate_language_semantics_public_model(CONTRACT_PATH)

    unsupported_policy = result.payload["unsupported_policy"]
    assert unsupported_policy["higher_kinded_types_claimed"] is False
    assert unsupported_policy["generic_collection_abi_claimed"] is False
    assert unsupported_policy["swift_protocol_bridge_claimed"] is False
    assert unsupported_policy["distributed_actors_claimed"] is False
    assert unsupported_policy["swift_abi_import_claimed"] is False
    assert unsupported_policy["cpp_template_import_claimed"] is False
