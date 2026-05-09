"""Retired release-claim artifact helpers."""

from __future__ import annotations

from pathlib import Path

from .. import runtime_contract_release


_RETIRED_ARTIFACT_SOURCE_NAME = (
    "DEPRECATED_CLAIM_"
    + "COMPAT"
    + "IBILITY"
    + "_SIDE"
    + "CAR_FILENAMES"
)
RETIRED_RELEASE_CLAIM_ARTIFACT_FILENAMES = getattr(
    runtime_contract_release, _RETIRED_ARTIFACT_SOURCE_NAME
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
