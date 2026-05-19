"""Retired release-claim artifact helpers."""

from __future__ import annotations

from pathlib import Path

from ..runtime_contract_release import (
    RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES,
)


def write_retired_release_claim_artifacts(directory: Path) -> None:
    for filename in RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES:
        (directory / filename).write_text("{}", encoding="utf-8")


def retired_release_claim_artifacts_absent(directory: Path) -> bool:
    return all(
        not (directory / filename).exists()
        for filename in RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES
    )


__all__ = [
    "RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES",
    "retired_release_claim_artifacts_absent",
    "write_retired_release_claim_artifacts",
]
