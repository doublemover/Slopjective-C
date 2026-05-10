from behavior_fixture_boundary_support import (
    FIXTURE_BOUNDARY_CONTRACTS,
    FIXTURE_ROOT,
    NATIVE_ROOT,
    PHASE_ORDER,
    PHASE_OWNER_CONTRACTS,
    POSITIVE_RESIDUE_AUDIT,
    ROOT,
    load_behavior_fixture_catalog,
    load_json,
    load_manifest_fixture_entries,
)


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
