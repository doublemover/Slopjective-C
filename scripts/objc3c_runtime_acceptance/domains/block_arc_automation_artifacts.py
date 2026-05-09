"""Fixture artifact loading for block/ARC storage automation cases."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import (
    NegativeDiagnosticExpectation,
    compile_fixture_outputs,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
)
from objc3c_runtime_acceptance.paths import ROOT

ManifestSurface = dict[str, Any]
NegativeBatch = dict[str, Any]


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
    negative_batch: NegativeBatch


def load_block_arc_automation_artifacts(
    case_dir: Path,
) -> BlockArcAutomationArtifacts:
    owned_manifest, owned_ll = _compile_owned_capture_artifacts(case_dir)
    owned_semantic_surface = _semantic_surface(owned_manifest)

    nonowning_manifest, nonowning_ll = _compile_nonowning_capture_artifacts(case_dir)
    nonowning_semantic_surface = _semantic_surface(nonowning_manifest)

    arc_mode_manifest, arc_mode_ll = _compile_arc_mode_artifacts(case_dir)
    arc_inference_manifest, arc_inference_ll = _compile_arc_inference_artifacts(
        case_dir
    )
    arc_cleanup_scope_manifest = _compile_arc_only_manifest(
        ROOT / "tests" / "tooling" / "fixtures" / "native" / "arc_cleanup_scope_positive.objc3",
        case_dir / "arc-cleanup-scope-positive",
    )
    arc_implicit_cleanup_manifest = _compile_arc_only_manifest(
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_implicit_cleanup_void_positive.objc3",
        case_dir / "arc-implicit-cleanup-positive",
    )
    arc_autorelease_return_manifest, arc_autorelease_return_ll = (
        _compile_arc_autorelease_return_artifacts(case_dir)
    )

    return BlockArcAutomationArtifacts(
        owned_manifest=owned_manifest,
        owned_copy_dispose_surface=owned_semantic_surface.get(
            "objc_block_copy_dispose_lowering_surface", {}
        ),
        owned_escape_surface=owned_semantic_surface.get(
            "objc_block_storage_escape_lowering_surface", {}
        ),
        owned_ll=owned_ll,
        nonowning_copy_dispose_surface=nonowning_semantic_surface.get(
            "objc_block_copy_dispose_lowering_surface", {}
        ),
        nonowning_arc_diagnostics_surface=nonowning_semantic_surface.get(
            "objc_arc_diagnostics_fixit_lowering_surface", {}
        ),
        nonowning_ll=nonowning_ll,
        arc_mode_sema=_sema_surface(arc_mode_manifest),
        arc_mode_block_copy_dispose_surface=_semantic_surface(arc_mode_manifest).get(
            "objc_block_copy_dispose_lowering_surface", {}
        ),
        arc_mode_ll=arc_mode_ll,
        arc_inference_sema=_sema_surface(arc_inference_manifest),
        arc_inference_ll=arc_inference_ll,
        arc_cleanup_scope_sema=_sema_surface(arc_cleanup_scope_manifest),
        arc_implicit_cleanup_sema=_sema_surface(arc_implicit_cleanup_manifest),
        arc_autorelease_return_sema=_sema_surface(arc_autorelease_return_manifest),
        arc_autorelease_return_ll=arc_autorelease_return_ll,
        negative_batch=_compile_negative_diagnostics(case_dir),
    )


def _compile_owned_capture_artifacts(case_dir: Path) -> tuple[ManifestSurface, str]:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "owned_object_capture_helper_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(
        fixture, case_dir / "owned-positive"
    )
    return _read_manifest(manifest_path), ll_path.read_text(encoding="utf-8")


def _compile_nonowning_capture_artifacts(case_dir: Path) -> tuple[ManifestSurface, str]:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "nonowning_object_capture_helper_elided_positive.objc3"
    )
    _, ll_path, manifest_path = compile_fixture_outputs(
        fixture, case_dir / "nonowning-positive"
    )
    return _read_manifest(manifest_path), ll_path.read_text(encoding="utf-8")


def _compile_arc_mode_artifacts(case_dir: Path) -> tuple[ManifestSurface, str]:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_mode_handling_positive.objc3"
    )
    out_dir = case_dir / "arc-mode-positive"
    compile_fixture_with_args(fixture, out_dir, extra_args=["-fobjc-arc"])
    return (
        _read_manifest(out_dir / "module.manifest.json"),
        (out_dir / "module.ll").read_text(encoding="utf-8"),
    )


def _compile_arc_inference_artifacts(case_dir: Path) -> tuple[ManifestSurface, str]:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_inference_lifetime_positive.objc3"
    )
    out_dir = case_dir / "arc-inference-positive"
    compile_fixture_with_args(fixture, out_dir, extra_args=["-fobjc-arc"])
    return (
        _read_manifest(out_dir / "module.manifest.json"),
        (out_dir / "module.ll").read_text(encoding="utf-8"),
    )


def _compile_arc_autorelease_return_artifacts(
    case_dir: Path,
) -> tuple[ManifestSurface, str]:
    fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "arc_autorelease_return_positive.objc3"
    )
    out_dir = case_dir / "arc-autorelease-return-positive"
    compile_fixture_with_args(fixture, out_dir, extra_args=["-fobjc-arc"])
    return (
        _read_manifest(out_dir / "module.manifest.json"),
        (out_dir / "module.ll").read_text(encoding="utf-8"),
    )


def _compile_arc_only_manifest(fixture: Path, out_dir: Path) -> ManifestSurface:
    compile_fixture_with_args(fixture, out_dir, extra_args=["-fobjc-arc"])
    return _read_manifest(out_dir / "module.manifest.json")


def _compile_negative_diagnostics(case_dir: Path) -> NegativeBatch:
    return compile_negative_diagnostic_batch(
        case_id="block-storage-arc-automation-semantics",
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=[
            NegativeDiagnosticExpectation(
                key="weak-mutation-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "weak_object_capture_mutation_negative.objc3",
                expected_snippets=[
                    "type mismatch: block mutated capture 'weakValue' requires owned runtime-backed storage"
                ],
                expected_codes=["O3S206"],
            ),
            NegativeDiagnosticExpectation(
                key="unowned-mutation-negative",
                fixture=ROOT
                / "tests"
                / "tooling"
                / "fixtures"
                / "native"
                / "unowned_object_capture_mutation_negative.objc3",
                expected_snippets=[
                    "type mismatch: block mutated capture 'borrowedValue' requires owned runtime-backed storage"
                ],
                expected_codes=["O3S206"],
            ),
        ],
    )


def _read_manifest(path: Path) -> ManifestSurface:
    return json.loads(path.read_text(encoding="utf-8"))


def _semantic_surface(manifest: ManifestSurface) -> ManifestSurface:
    return manifest.get("frontend", {}).get("pipeline", {}).get("semantic_surface", {})


def _sema_surface(manifest: ManifestSurface) -> ManifestSurface:
    return manifest.get("frontend", {}).get("pipeline", {}).get("sema_pass_manager", {})


__all__ = [
    "BlockArcAutomationArtifacts",
    "ManifestSurface",
    "NegativeBatch",
    "load_block_arc_automation_artifacts",
]
