import json

from behavior_fixture_boundary_support import (
    FIXTURE_ROOT,
    NATIVE_ROOT,
    ROOT,
    generated_manifest_entries,
    load_json,
)


def test_generated_fixture_manifest_is_provenance_only() -> None:
    generated_manifest, generated_entries = generated_manifest_entries()
    generated_boundary = generated_manifest["boundary"]

    assert generated_manifest["fixtures"] == generated_entries
    assert generated_boundary["kind"] == "generated-contract-artifacts"
    assert generated_boundary["source_of_truth"] == "generator-output"
    assert generated_boundary["canonical_behavior_source"] is False
    assert generated_boundary["allowed_path_roots"] == ["tests/tooling/fixtures/objc3c"]
    assert "not canonical behavior expectations" in generated_boundary["hand_edit_policy"]
    assert generated_boundary["positive_fixture_policy"] == (
        "generated artifacts never define positive native behavior"
    )
    assert generated_boundary["boundary_contract_index"] == (
        "tests/conformance/hard_cutover_fixture_boundary_contracts.json"
    )
    assert generated_boundary["phase_owner_contract_index"] == (
        "tests/conformance/hard_cutover_behavior_phase_owner_contracts.json"
    )
    assert generated_boundary["phase_support_claim_authority"] is False
    generated_allowed_roots = tuple(ROOT / root for root in generated_boundary["allowed_path_roots"])

    for entry in generated_entries:
        path = ROOT / entry["path"]
        assert path.exists(), entry["path"]
        assert entry["origin"] == "generated"
        assert entry["generator"]
        assert entry["provenance"]
        assert entry["boundary"] == "generated-contract-artifact"
        assert entry["behavior_boundary"] == "generated-provenance-only"
        assert entry["canonical_behavior_source"] is False
        assert entry["phase_support_claim_authority"] is False
        assert not path.is_relative_to(NATIVE_ROOT)
        assert any(path.is_relative_to(root) for root in generated_allowed_roots)


def test_generated_manifest_cannot_reference_native_behavior_or_retired_support() -> None:
    generated_manifest = load_json(FIXTURE_ROOT / "generated" / "manifest.json")
    forbidden_support_tokens = (
        "old-mode",
        "compatibility",
        "migration",
        "retired-route",
        "gate",
        "runtime_dispatch",
        "runtime-dispatch",
    )

    assert generated_manifest["boundary"]["canonical_behavior_source"] is False
    assert generated_manifest["boundary"]["positive_fixture_policy"] == (
        "generated artifacts never define positive native behavior"
    )
    for entry in generated_manifest["fixtures"]:
        path = ROOT / entry["path"]
        serialized = json.dumps(entry, sort_keys=True).lower()
        assert path.is_file(), entry["path"]
        assert not path.is_relative_to(NATIVE_ROOT)
        assert entry["behavior_boundary"] == "generated-provenance-only"
        assert entry["canonical_behavior_source"] is False
        assert entry["phase_support_claim_authority"] is False
        for token in forbidden_support_tokens:
            assert token not in serialized
