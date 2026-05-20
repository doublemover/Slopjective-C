from __future__ import annotations

import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

import check_release_evidence as release_evidence

FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "release_evidence_contract"


def test_release_evidence_gate_contract_pins_pairs_empty_mode_and_attestation() -> None:
    contract = release_evidence.load_release_evidence_contract()

    assert release_evidence.release_label_from_contract(contract) == "v0.11"
    assert release_evidence.schema_data_pairs_from_contract(contract) == (
        (
            "schemas/objc3-abi-2025Q4.schema.json",
            "conformance/manifests/objc3-abi-2025Q4.example.json",
        ),
        (
            "schemas/objc3-conformance-evidence-bundle-v1.schema.json",
            "conformance/bundles/objc3-conformance-evidence-bundle-v0.11.example.json",
        ),
        (
            "schemas/objc3-runtime-2025Q4.manifest.schema.json",
            "conformance/manifests/objc3-runtime-2025Q4.manifest.json",
        ),
    )

    empty_input_mode = release_evidence.empty_input_mode_from_contract(contract)
    assert empty_input_mode["blocks_public_claims"] is True
    assert empty_input_mode["blocking_issue_refs"] == ["#8058", "#8059"]

    generated_index = release_evidence.generated_index_contract(contract)
    assert generated_index["output_name"] == "evidence-index.json"
    expected_output_path = "/".join(("tmp", "release_evidence", "evidence-index.json"))
    assert generated_index["output_path"] == expected_output_path
    assert (
        generated_index["artifact_authenticity"]["provenance_mode"]
        == "generator_replayable"
    )
    assert (
        generated_index["artifact_authenticity"]["content_role"]
        == "conformance_evidence_index"
    )


def test_release_evidence_contract_rejects_invalid_contract_json() -> None:
    with pytest.raises(release_evidence.ReleaseEvidenceContractError, match="invalid JSON"):
        release_evidence.load_release_evidence_contract(
            FIXTURE_ROOT / "hard_fail_invalid_contract_json.json"
        )


def test_release_evidence_contract_rejects_empty_schema_pairs() -> None:
    contract = release_evidence.load_release_evidence_contract(
        FIXTURE_ROOT / "invalid_empty_pairs.json"
    )

    with pytest.raises(
        release_evidence.ReleaseEvidenceContractError,
        match="schema_data_pairs must be a non-empty list",
    ):
        release_evidence.schema_data_pairs_from_contract(contract)


def test_release_evidence_contract_rejects_unsorted_schema_pairs() -> None:
    contract = release_evidence.load_release_evidence_contract(
        FIXTURE_ROOT / "drift_multi_pair_unsorted.json"
    )

    with pytest.raises(
        release_evidence.ReleaseEvidenceContractError,
        match="schema_data_pairs must be sorted by id",
    ):
        release_evidence.schema_data_pairs_from_contract(contract)


def test_release_evidence_contract_checks_required_pair_files() -> None:
    contract = release_evidence.load_release_evidence_contract(
        FIXTURE_ROOT / "missing_schema_pair.json"
    )
    pairs = release_evidence.schema_data_pairs_from_contract(contract)

    with pytest.raises(
        release_evidence.ReleaseEvidenceContractError,
        match=(
            "missing required file "
            "tests/tooling/fixtures/release_evidence_contract/does_not_exist.schema.json"
        ),
    ):
        release_evidence.validate_schema_data_pair_files(pairs)
