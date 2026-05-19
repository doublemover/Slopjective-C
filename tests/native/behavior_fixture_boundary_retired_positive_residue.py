import re
from pathlib import Path

from behavior_fixture_boundary_support import (
    POSITIVE_RESIDUE_AUDIT,
    ROOT,
    STRICT_KINDS,
    load_behavior_fixture_catalog,
    load_json,
)


TOOLING_NATIVE_FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native"
TOOLING_POSITIVE_LEXICAL_AUDIT_EXTRAS = {
    TOOLING_NATIVE_FIXTURE_ROOT / "return_paths_ok.objc3",
}
POSITIVE_NAME_MARKERS = ("_positive", "_accept", "_success")
POSITIVE_RESIDUE_PATTERNS = (
    re.compile(r"\bold[-_\s]+mode\b"),
    re.compile(r"\bretired[-_\s]+route"),
    re.compile(r"\bretired[-_\s]+adapter\b"),
    re.compile(r"\bcompat(?:ibility)?(?:[-_\s]+gate)?\b"),
    re.compile(r"\blegacy\b"),
    re.compile(r"\bmigration(?:[-_\s]+lane)?\b"),
    re.compile(r"\bshims?\b"),
    re.compile(r"\bfallbacks?\b"),
)


def normalize_residue_token(value: str) -> str:
    return re.sub(r"[-_\s]+", "-", value.lower())


def is_positive_fixture_path(path: Path) -> bool:
    relative_parts = [part.lower() for part in path.relative_to(TOOLING_NATIVE_FIXTURE_ROOT).parts]
    stem = path.stem.lower()
    return "positive" in relative_parts or any(marker in stem for marker in POSITIVE_NAME_MARKERS)


def positive_tooling_fixtures() -> set[Path]:
    positive_paths = {
        path
        for path in TOOLING_NATIVE_FIXTURE_ROOT.rglob("*.objc3")
        if path.is_file() and is_positive_fixture_path(path)
    }
    return positive_paths | {path for path in TOOLING_POSITIVE_LEXICAL_AUDIT_EXTRAS if path.is_file()}


def has_positive_residue_token(value: str) -> bool:
    normalized = value.lower()
    return any(pattern.search(normalized) for pattern in POSITIVE_RESIDUE_PATTERNS)


def test_positive_residue_audit_policy_is_hard_cutover_only() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)

    result = audit["result"]
    assert result.startswith("no remaining")
    for retired_surface in (
        "retired mode",
        "retired adapter",
        "alternate acceptance path",
        "retired-source lane",
    ):
        assert retired_surface in result
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
        assert normalize_residue_token(hit["token"]) in normalize_residue_token(text)
        assert hit["classification"]
        assert "not " in hit["disposition"]


def test_positive_fixture_lexical_residue_hits_are_documented() -> None:
    audit = load_json(POSITIVE_RESIDUE_AUDIT)
    documented_paths = {hit["path"] for hit in audit["documented_lexical_positive_hits"]}
    lexical_hit_paths = {
        path.relative_to(ROOT).as_posix()
        for path in positive_tooling_fixtures()
        if has_positive_residue_token(path.read_text(encoding="utf-8"))
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
