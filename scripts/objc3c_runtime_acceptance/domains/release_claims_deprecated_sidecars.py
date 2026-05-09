"""Deprecated release-claim sidecar helpers."""

from __future__ import annotations

from pathlib import Path

from ..runtime_contract_release import DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES


def write_deprecated_claim_sidecars(directory: Path) -> None:
    for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES:
        (directory / filename).write_text("{}", encoding="utf-8")


def deprecated_claim_sidecars_absent(directory: Path) -> bool:
    return all(
        not (directory / filename).exists()
        for filename in DEPRECATED_CLAIM_COMPATIBILITY_SIDECAR_FILENAMES
    )
