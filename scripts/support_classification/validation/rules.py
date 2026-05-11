"""Rule definitions for support classification validation."""

from __future__ import annotations

from typing import Iterable
from typing import cast

from .contracts import CANONICAL_SUPPORT_CLASSES
from .contracts import SupportClassPolicy

TMP_SOURCE_TRUTH_PREFIXES = ("tmp/", "tmp\\")


def canonical_support_class_names() -> set[str]:
    return set(CANONICAL_SUPPORT_CLASSES)


def missing_support_classes(classes: set[str]) -> list[str]:
    return sorted(canonical_support_class_names() - classes)


def support_class_policy(support_class: str) -> SupportClassPolicy:
    return cast(SupportClassPolicy, CANONICAL_SUPPORT_CLASSES[support_class])


def unknown_evidence_families(
    required_families: Iterable[str],
    evidence_families: set[str],
) -> list[str]:
    return sorted(set(required_families) - evidence_families)


def uses_tmp_source_truth(path_text: str) -> bool:
    return path_text.startswith(TMP_SOURCE_TRUTH_PREFIXES)


__all__ = [
    "TMP_SOURCE_TRUTH_PREFIXES",
    "canonical_support_class_names",
    "missing_support_classes",
    "support_class_policy",
    "unknown_evidence_families",
    "uses_tmp_source_truth",
]
