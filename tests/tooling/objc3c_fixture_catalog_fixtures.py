from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NATIVE_CATALOG = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "fixture_family_catalog.json"
)
CANONICAL_MANIFEST = ROOT / "tests" / "fixtures" / "canonical" / "manifest.json"
PARSER_CORPUS = ROOT / "tests" / "tooling" / "fixtures" / "parser_conformance_corpus"
PARSER_MANIFEST = PARSER_CORPUS / "manifest.json"
NEGATIVE_EXECUTION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "negative"
)
POSITIVE_EXECUTION = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "execution" / "positive"
)
RECOVERY_README = (
    ROOT / "tests" / "tooling" / "fixtures" / "native" / "recovery" / "README.md"
)
EXPECTED_OWNER_LABELS = {
    "parser",
    "sema",
    "lowering",
    "ir",
    "runtime",
    "e2e",
    "canonical_rejection",
}
EXPECTED_CANONICAL_POSITIVE_OWNER_PHASES = {
    "parser",
    "sema",
    "lowering",
    "ir",
    "runtime",
    "e2e",
}
EXPECTED_OWNER_SPLIT_SURFACES = {
    "tests/tooling/fixtures/native/recovery/positive",
    "tests/tooling/fixtures/native/execution/positive",
    "tests/tooling/fixtures/native/execution/negative",
    "tests/tooling/fixtures/native/*.objc3",
}


def native_fixture_root() -> Path:
    return ROOT / "tests" / "tooling" / "fixtures" / "native"
