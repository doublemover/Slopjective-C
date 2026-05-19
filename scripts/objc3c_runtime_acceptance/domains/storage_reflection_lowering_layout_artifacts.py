"""Shared compile artifacts for storage/reflection lowering layout cases."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs

from ..paths import ROOT

PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL = (
    "tests/tooling/fixtures/native/synthesized_accessor_property_lowering_positive.objc3"
)
PROPERTY_ACCESSOR_LAYOUT_FIXTURE = ROOT / PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL
REGISTRATION_MANIFEST_ARTIFACT = "module.runtime-registration-manifest.json"


@dataclass(frozen=True)
class LoweringLayoutArtifacts:
    ll_path: Path
    manifest_path: Path
    registration_manifest_path: Path


def compile_property_accessor_layout_fixture(case_dir: Path) -> LoweringLayoutArtifacts:
    _, ll_path, manifest_path = compile_fixture_outputs(
        PROPERTY_ACCESSOR_LAYOUT_FIXTURE,
        case_dir / "compile",
    )
    registration_manifest_path = case_dir / "compile" / REGISTRATION_MANIFEST_ARTIFACT
    if not registration_manifest_path.is_file():
        raise RuntimeError(
            f"compiled fixture did not publish {registration_manifest_path}"
        )
    return LoweringLayoutArtifacts(
        ll_path=ll_path,
        manifest_path=manifest_path,
        registration_manifest_path=registration_manifest_path,
    )


__all__ = [
    "LoweringLayoutArtifacts",
    "PROPERTY_ACCESSOR_LAYOUT_FIXTURE",
    "PROPERTY_ACCESSOR_LAYOUT_FIXTURE_LABEL",
    "REGISTRATION_MANIFEST_ARTIFACT",
    "compile_property_accessor_layout_fixture",
]
