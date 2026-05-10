from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = (
    ROOT
    / "reports"
    / "claimability"
    / "type-semantic-model-closure"
    / "type_semantic_model_closure_summary.json"
)


def assert_type_semantic_model_summary_artifact_exists() -> None:
    assert SUMMARY.is_file()
