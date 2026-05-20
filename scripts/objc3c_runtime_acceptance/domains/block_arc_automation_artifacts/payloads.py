"""Block/ARC storage automation artifact payload construction."""

from __future__ import annotations

import json
from pathlib import Path

from .constants import (
    ARC_DIAGNOSTICS_FIXIT_SURFACE_KEY,
    COPY_DISPOSE_SURFACE_KEY,
    ESCAPE_SURFACE_KEY,
    LLVM_IR_FILE_NAME,
    MANIFEST_FILE_NAME,
)
from .models import (
    BlockArcAutomationArtifacts,
    FixtureArtifactPayload,
    ManifestSurface,
    NegativeBatch,
)
from .predicates import semantic_surface_entry, sema_surface


def read_manifest(path: Path) -> ManifestSurface:
    return json.loads(path.read_text(encoding="utf-8"))


def fixture_artifact_payload(out_dir: Path) -> FixtureArtifactPayload:
    return artifact_payload_from_paths(
        manifest_path=out_dir / MANIFEST_FILE_NAME,
        ll_path=out_dir / LLVM_IR_FILE_NAME,
    )


def artifact_payload_from_paths(
    manifest_path: Path,
    ll_path: Path,
) -> FixtureArtifactPayload:
    return FixtureArtifactPayload(
        manifest=read_manifest(manifest_path),
        llvm_ir=ll_path.read_text(encoding="utf-8"),
    )


def build_block_arc_automation_artifacts(
    *,
    owned_capture: FixtureArtifactPayload,
    nonowning_capture: FixtureArtifactPayload,
    arc_mode: FixtureArtifactPayload,
    arc_inference: FixtureArtifactPayload,
    arc_cleanup_scope_manifest: ManifestSurface,
    arc_implicit_cleanup_manifest: ManifestSurface,
    arc_autorelease_return: FixtureArtifactPayload,
    arc_method_family: FixtureArtifactPayload,
    negative_batch: NegativeBatch,
) -> BlockArcAutomationArtifacts:
    return BlockArcAutomationArtifacts(
        owned_manifest=owned_capture.manifest,
        owned_copy_dispose_surface=semantic_surface_entry(
            owned_capture.manifest,
            COPY_DISPOSE_SURFACE_KEY,
        ),
        owned_escape_surface=semantic_surface_entry(
            owned_capture.manifest,
            ESCAPE_SURFACE_KEY,
        ),
        owned_ll=owned_capture.llvm_ir,
        nonowning_copy_dispose_surface=semantic_surface_entry(
            nonowning_capture.manifest,
            COPY_DISPOSE_SURFACE_KEY,
        ),
        nonowning_arc_diagnostics_surface=semantic_surface_entry(
            nonowning_capture.manifest,
            ARC_DIAGNOSTICS_FIXIT_SURFACE_KEY,
        ),
        nonowning_ll=nonowning_capture.llvm_ir,
        arc_mode_sema=sema_surface(arc_mode.manifest),
        arc_mode_block_copy_dispose_surface=semantic_surface_entry(
            arc_mode.manifest,
            COPY_DISPOSE_SURFACE_KEY,
        ),
        arc_mode_ll=arc_mode.llvm_ir,
        arc_inference_sema=sema_surface(arc_inference.manifest),
        arc_inference_ll=arc_inference.llvm_ir,
        arc_cleanup_scope_sema=sema_surface(arc_cleanup_scope_manifest),
        arc_implicit_cleanup_sema=sema_surface(arc_implicit_cleanup_manifest),
        arc_autorelease_return_sema=sema_surface(arc_autorelease_return.manifest),
        arc_autorelease_return_ll=arc_autorelease_return.llvm_ir,
        arc_method_family_sema=sema_surface(arc_method_family.manifest),
        arc_method_family_ll=arc_method_family.llvm_ir,
        negative_batch=negative_batch,
    )


__all__ = [
    "artifact_payload_from_paths",
    "build_block_arc_automation_artifacts",
    "fixture_artifact_payload",
    "read_manifest",
]
