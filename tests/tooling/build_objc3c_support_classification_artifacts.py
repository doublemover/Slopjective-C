from __future__ import annotations

from pathlib import Path


def assert_support_classification_artifacts_exist(
    json_out: Path,
    md_out: Path,
) -> None:
    assert json_out.is_file()
    assert md_out.is_file()


def assert_support_classification_artifacts_absent(
    json_out: Path,
    md_out: Path,
) -> None:
    assert not json_out.exists()
    assert not md_out.exists()
