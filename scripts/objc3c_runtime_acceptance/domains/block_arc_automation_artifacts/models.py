"""Block/ARC storage automation artifact data contracts."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


ManifestSurface = dict[str, Any]
NegativeBatch = dict[str, Any]


@dataclass(frozen=True)
class FixtureArtifactSpec:
    key: str
    fixture: Path
    output_dir_name: str
    extra_args: tuple[str, ...] = ()


@dataclass(frozen=True)
class FixtureArtifactPayload:
    manifest: ManifestSurface
    llvm_ir: str


@dataclass(frozen=True)
class BlockArcAutomationArtifacts:
    owned_manifest: ManifestSurface
    owned_copy_dispose_surface: ManifestSurface
    owned_escape_surface: ManifestSurface
    owned_ll: str
    nonowning_copy_dispose_surface: ManifestSurface
    nonowning_arc_diagnostics_surface: ManifestSurface
    nonowning_ll: str
    arc_mode_sema: ManifestSurface
    arc_mode_block_copy_dispose_surface: ManifestSurface
    arc_mode_ll: str
    arc_inference_sema: ManifestSurface
    arc_inference_ll: str
    arc_cleanup_scope_sema: ManifestSurface
    arc_implicit_cleanup_sema: ManifestSurface
    arc_autorelease_return_sema: ManifestSurface
    arc_autorelease_return_ll: str
    arc_autoreleasepool_order_sema: ManifestSurface
    arc_autoreleasepool_order_ll: str
    arc_weak_autoreleasepool_sema: ManifestSurface
    arc_weak_autoreleasepool_ll: str
    arc_method_family_sema: ManifestSurface
    arc_method_family_ll: str
    negative_batch: NegativeBatch


__all__ = [
    "BlockArcAutomationArtifacts",
    "FixtureArtifactPayload",
    "FixtureArtifactSpec",
    "ManifestSurface",
    "NegativeBatch",
]
