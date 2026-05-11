"""Compile orchestration for Object Model metaclass sample fixtures."""

from __future__ import annotations

import json

from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_sources import (
    CanonicalSampleSetSources,
)
from objc3c_runtime_acceptance.domains.object_model_metaclass_sample_sources import (
    MetaclassGraphRootClassSources,
)
from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_outputs

from .data import CanonicalSampleCompileArtifacts
from .data import MetaclassGraphCompileArtifacts


def compile_metaclass_graph_root_class_fixture(
    sources: MetaclassGraphRootClassSources,
) -> MetaclassGraphCompileArtifacts:
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        sources.fixture,
        sources.case_dir / "compile",
    )
    return MetaclassGraphCompileArtifacts(
        obj_path=obj_path,
        ll_path=ll_path,
        manifest_path=manifest_path,
        ll_text=ll_path.read_text(encoding="utf-8"),
        manifest=json.loads(manifest_path.read_text(encoding="utf-8")),
    )


def compile_canonical_sample_set_fixture(
    sources: CanonicalSampleSetSources,
) -> CanonicalSampleCompileArtifacts:
    obj_path, ll_path, manifest_path = compile_fixture_outputs(
        sources.fixture,
        sources.case_dir / "compile",
    )
    registration_manifest_path = (
        sources.case_dir / "compile" / "module.runtime-registration-manifest.json"
    )
    if not registration_manifest_path.is_file():
        raise RuntimeError(f"compiled fixture did not publish {registration_manifest_path}")

    return CanonicalSampleCompileArtifacts(
        obj_path=obj_path,
        ll_path=ll_path,
        manifest_path=manifest_path,
        registration_manifest_path=registration_manifest_path,
        ll_text=ll_path.read_text(encoding="utf-8"),
        manifest=json.loads(manifest_path.read_text(encoding="utf-8")),
        registration_manifest=json.loads(
            registration_manifest_path.read_text(encoding="utf-8")
        ),
    )


__all__ = [
    "compile_canonical_sample_set_fixture",
    "compile_metaclass_graph_root_class_fixture",
]
