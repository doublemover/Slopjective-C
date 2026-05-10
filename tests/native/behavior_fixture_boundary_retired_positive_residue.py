from behavior_fixture_boundary_support import (
    POSITIVE_RESIDUE_AUDIT,
    ROOT,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_json,
)


def test_positive_residue_audit_policy_is_hard_cutover_only() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    assert audit["result"].startswith("no remaining old-mode")
    assert audit["validation"] == "not run"
    assert set(audit["conversion_policy"]) == {
        "true_retired_positive",
        "lexical_false_positive",
        "negative_fixture",
    }


def test_positive_residue_confirmed_rejections_are_strict() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    behavior_by_path = load_behavior_fixture_catalog().by_relative_source()

    for relative_path in audit["confirmed_rejections"]:
        path = ROOT / relative_path
        assert path.is_file(), relative_path
        if relative_path in behavior_by_path:
            fixture = behavior_by_path[relative_path]
            assert fixture.fixture_kind in STRICT_KINDS
        else:
            assert any(marker in path.stem.lower() for marker in ("negative", "rejected"))


def test_positive_residue_absent_retired_positive_paths_remain_absent() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    for relative_path in audit["absent_retired_positive_paths"]:
        assert not (ROOT / relative_path).exists(), relative_path


def test_positive_residue_documents_lexical_false_positive_surfaces() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    for hit in audit["documented_lexical_positive_hits"]:
        path = ROOT / hit["path"]
        assert path.is_file(), hit["path"]
        text = path.read_text(encoding="utf-8")
        assert hit["token"] in text
        assert hit["classification"]
        assert "not " in hit["disposition"]


def test_positive_fixture_lexical_residue_hits_are_documented() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    documented_paths = {hit["path"] for hit in audit["documented_lexical_positive_hits"]}
    tooling_root = ROOT / "tests" / "tooling" / "fixtures" / "native"
    positive_fixture_paths = {
        *tooling_root.glob("*positive*.objc3"),
        *(tooling_root / "execution" / "positive").glob("*.objc3"),
        *(tooling_root / "recovery" / "positive").glob("*.objc3"),
    }
    retired_terms = (
        "old-mode",
        "old_mode",
        "gate",
        "retired-route",
        "compatibility",
        "migration",
        "legacy",
    )
    lexical_hit_paths = {
        path.relative_to(ROOT).as_posix()
        for path in positive_fixture_paths
        if any(term in path.read_text(encoding="utf-8").lower() for term in retired_terms)
    }

    assert lexical_hit_paths <= documented_paths


def test_positive_residue_outcome_index_uses_documented_false_positive_paths() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    outcome_index = load_json(
        ROOT / "tests" / "conformance" / "hard_cutover_behavior_outcome_owner_index.json"
    )
    documented_paths = {hit["path"] for hit in audit["documented_lexical_positive_hits"]}

    residue_outcome = next(
        entry
        for entry in outcome_index["outcomes"]
        if entry["outcome"] == "positive_residue_false_positive"
    )
    assert set(residue_outcome["evidence"]) == documented_paths
