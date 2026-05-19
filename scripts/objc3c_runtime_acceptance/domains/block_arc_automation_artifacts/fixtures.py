"""Block/ARC storage automation source fixture materialization."""

from __future__ import annotations

from pathlib import Path

from ...fixture_compilation import (
    compile_fixture_outputs,
    compile_fixture_with_args,
    compile_negative_diagnostic_batch,
)
from .catalog import CASE_ID, NEGATIVE_EXPECTATIONS
from .constants import MANIFEST_FILE_NAME
from .models import (
    FixtureArtifactPayload,
    FixtureArtifactSpec,
    ManifestSurface,
    NegativeBatch,
)
from .payloads import (
    artifact_payload_from_paths,
    fixture_artifact_payload,
    read_manifest,
)


def compile_fixture_artifact(
    spec: FixtureArtifactSpec,
    case_dir: Path,
) -> FixtureArtifactPayload:
    out_dir = case_dir / spec.output_dir_name
    if spec.extra_args:
        compile_fixture_with_args(
            spec.fixture,
            out_dir,
            extra_args=list(spec.extra_args),
        )
        return fixture_artifact_payload(out_dir)

    _, ll_path, manifest_path = compile_fixture_outputs(spec.fixture, out_dir)
    return artifact_payload_from_paths(manifest_path=manifest_path, ll_path=ll_path)


def compile_manifest_only_fixture(
    spec: FixtureArtifactSpec,
    case_dir: Path,
) -> ManifestSurface:
    out_dir = case_dir / spec.output_dir_name
    compile_fixture_with_args(spec.fixture, out_dir, extra_args=list(spec.extra_args))
    return read_manifest(out_dir / MANIFEST_FILE_NAME)


def compile_negative_diagnostics(case_dir: Path) -> NegativeBatch:
    return compile_negative_diagnostic_batch(
        case_id=CASE_ID,
        out_dir=case_dir / "negative-diagnostics-batch",
        expectations=list(NEGATIVE_EXPECTATIONS),
    )


__all__ = [
    "compile_fixture_artifact",
    "compile_manifest_only_fixture",
    "compile_negative_diagnostics",
]
