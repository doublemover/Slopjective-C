import json

from behavior_fixture_boundary_support import (
    FIXTURE_BOUNDARY_CONTRACTS,
    FIXTURE_ROOT,
    NATIVE_ROOT,
    PHASE_ORDER,
    PHASE_OWNER_CONTRACTS,
    POSITIVE_RESIDUE_AUDIT,
    ROOT,
    generated_manifest_entries,
    canonical_manifest_entries,
    load_behavior_fixture_catalog,
    load_json,
    load_manifest_fixture_entries,
)


def test_canonical_fixture_manifest_matches_native_behavior_catalog() -> None:
    canonical_manifest, canonical_entries = canonical_manifest_entries()
    canonical_boundary = canonical_manifest["boundary"]
    canonical_by_path = {entry["path"]: entry for entry in canonical_entries}

    assert canonical_by_path
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    assert set(behavior_by_path) == set(canonical_by_path)

    for relative_source, fixture in behavior_by_path.items():
        entry = canonical_by_path[relative_source]
        path = ROOT / relative_source
        assert path.exists(), entry["path"]
        assert path.is_relative_to(NATIVE_ROOT)
        assert entry == fixture.canonical_manifest_entry()

    assert canonical_manifest["fixtures"] == canonical_entries
    assert canonical_boundary["kind"] == "hand-authored-native-behavior"
    assert canonical_boundary["source_of_truth"] == "tests/native"
    assert canonical_boundary["phase_order"] == list(PHASE_ORDER)
    assert canonical_boundary["positive_fixture_policy"] == (
        "positive fixtures cover canonical behavior only"
    )
    assert canonical_boundary["retired_surface_policy"] == (
        "old-mode, shim, fallback, compatibility, migration-lane, unsupported feature, and runtime-dispatch residues must be rejection, strict-error, or absent-support metadata"
    )
    assert canonical_boundary["boundary_contract_index"] == (
        "tests/conformance/hard_cutover_fixture_boundary_contracts.json"
    )
    assert canonical_boundary["phase_owner_contract_index"] == (
        "tests/conformance/hard_cutover_behavior_phase_owner_contracts.json"
    )


def test_canonical_and_generated_fixture_paths_are_disjoint() -> None:
    _, canonical_entries = canonical_manifest_entries()
    _, generated_entries = generated_manifest_entries()
    canonical_paths = {entry["path"] for entry in canonical_entries}
    generated_paths = {entry["path"] for entry in generated_entries}

    assert canonical_paths
    assert generated_paths
    assert canonical_paths.isdisjoint(generated_paths)


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
        "fallback",
        "shim",
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


def test_canonical_manifest_is_phase_ordered_and_behavior_first() -> None:
    entries = load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    phase_index = {phase: index for index, phase in enumerate(PHASE_ORDER)}
    indexed_phases = [phase_index[entry["owner_phase"]] for entry in entries]

    assert indexed_phases == sorted(indexed_phases)
    for entry in entries:
        path = ROOT / entry["path"]
        relative_parts = path.relative_to(NATIVE_ROOT).parts
        assert relative_parts[0] == entry["owner_phase"]
        assert relative_parts[1] == entry["behavior_family"]


def test_fixture_boundary_contract_index_links_all_boundary_families() -> None:
    contracts = load_json(FIXTURE_BOUNDARY_CONTRACTS)
    catalog = load_json(ROOT / "tests" / "conformance" / "hard_cutover_catalog.json")
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()
    generated_paths = {
        entry["path"] for entry in load_manifest_fixture_entries(FIXTURE_ROOT / "generated" / "manifest.json")
    }
    families = {entry["family"]: entry for entry in contracts["contract_families"]}

    assert contracts["catalog"] == "objc3-hard-cutover-fixture-boundary-contracts"
    assert catalog["policy"]["fixture_boundary_contracts"] == (
        FIXTURE_BOUNDARY_CONTRACTS.relative_to(ROOT).as_posix()
    )
    assert contracts["manifests"]["canonical_behavior"]["path"] == (
        FIXTURE_ROOT / "canonical" / "manifest.json"
    ).relative_to(ROOT).as_posix()
    assert contracts["manifests"]["canonical_behavior"]["phase_owner_contract_index"] == (
        PHASE_OWNER_CONTRACTS.relative_to(ROOT).as_posix()
    )
    assert contracts["manifests"]["canonical_behavior"]["positive_support"] is True
    assert contracts["manifests"]["generated_provenance"]["path"] == (
        FIXTURE_ROOT / "generated" / "manifest.json"
    ).relative_to(ROOT).as_posix()
    assert contracts["manifests"]["generated_provenance"]["phase_owner_contract_index"] == (
        PHASE_OWNER_CONTRACTS.relative_to(ROOT).as_posix()
    )
    assert contracts["manifests"]["generated_provenance"]["phase_support_claim_authority"] is False
    assert contracts["manifests"]["generated_provenance"]["positive_support"] is False

    phase_owner = families["phase_owner_contracts"]
    assert phase_owner["owner_index"] == PHASE_OWNER_CONTRACTS.relative_to(ROOT).as_posix()
    assert phase_owner["phase_order"] == list(PHASE_ORDER)
    assert phase_owner["generated_fixture_authority"] is False
    assert phase_owner["positive_support"] is True

    canonical = families["canonical_positive_behavior"]
    assert canonical["positive_support"] is True
    for relative_path in canonical["evidence"]:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind == "positive"
        assert not fixture.retired_surface_tags

    retired = families["retired_surface_rejection_and_strict_error"]
    assert retired["positive_support"] is False
    for relative_path in retired["evidence"]:
        fixture = behavior_by_path[relative_path]
        assert fixture.fixture_kind in {"negative", "rejection", "strict-error"}
        assert fixture.expected_diagnostic_code

    generated = families["generated_provenance_only"]
    assert generated["positive_support"] is False
    assert set(generated["evidence"]) == generated_paths
    for relative_path in generated["evidence"]:
        path = ROOT / relative_path
        assert path.is_file(), relative_path
        assert not path.is_relative_to(NATIVE_ROOT)

    lexical = families["tooling_positive_lexical_residue_audit"]
    audit_paths = {
        hit["path"] for hit in load_json(POSITIVE_RESIDUE_AUDIT)["documented_lexical_positive_hits"]
    }
    assert set(lexical["evidence"]).issubset(audit_paths)


def test_fixture_boundary_contract_reference_anchors_are_provenance_only() -> None:
    contracts = load_json(FIXTURE_BOUNDARY_CONTRACTS)
    families = {entry["family"]: entry for entry in contracts["contract_families"]}
    reference_family = families["conformance_reference_anchors"]
    anchor_token = reference_family["anchor_token"]
    canonical_paths = {
        entry["path"] for entry in load_manifest_fixture_entries(FIXTURE_ROOT / "canonical" / "manifest.json")
    }
    roots = [ROOT / root for root in reference_family["roots"]]

    assert reference_family["positive_support"] is False
    assert reference_family["contract"] == "reference-provenance-only"
    assert anchor_token == "docs/reference/legacy_spec_anchor_index.md"

    anchored_files = {
        path
        for root in roots
        for path in root.rglob("*.json")
        if anchor_token in path.read_text(encoding="utf-8")
    }
    assert anchored_files
    for path in anchored_files:
        relative_path = path.relative_to(ROOT).as_posix()
        assert relative_path not in canonical_paths
        assert path.is_relative_to(ROOT / "tests" / "conformance")
