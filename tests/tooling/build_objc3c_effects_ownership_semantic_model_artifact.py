from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SUMMARY = (
    ROOT
    / "tmp"
    / "reports"
    / "claimability"
    / "effects-ownership-semantic-model"
    / "effects_ownership_semantic_model_summary.json"
)


def assert_effects_ownership_semantic_model_summary_artifact_exists() -> None:
    assert SUMMARY.is_file()
