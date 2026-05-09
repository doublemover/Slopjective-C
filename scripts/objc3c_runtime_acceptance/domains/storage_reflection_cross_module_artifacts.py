"""Artifact capture for cross-module storage/reflection preservation cases."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_with_args
from objc3c_runtime_acceptance.paths import ROOT

from ..runtime_contract_storage_reflection import (
    STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE,
    STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE,
)


@dataclass(frozen=True)
class CrossModuleStorageReflectionArtifacts:
    provider_compile_ms: int
    consumer_compile_ms: int
    provider_import_payload: dict[str, Any]
    provider_registration_manifest: dict[str, Any]
    provider_storage_surface: Any
    consumer_registration_manifest: dict[str, Any]
    link_plan: dict[str, Any]


def capture_cross_module_storage_reflection_artifacts(
    run_dir: Path,
) -> CrossModuleStorageReflectionArtifacts:
    case_dir = run_dir / "cross-module-storage-reflection-artifact-preservation"
    provider_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(STORAGE_REFLECTION_PRESERVATION_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)

    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    provider_storage_surface = provider_import_payload.get(
        "objc_runtime_storage_reflection_artifact_preservation", {}
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    compile_fixture_with_args(
        consumer_fixture,
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = json.loads(
        (consumer_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )
    link_plan = json.loads(
        (consumer_compile_dir / "module.cross-module-runtime-link-plan.json").read_text(
            encoding="utf-8"
        )
    )

    return CrossModuleStorageReflectionArtifacts(
        provider_compile_ms=provider_compile_ms,
        consumer_compile_ms=consumer_compile_ms,
        provider_import_payload=provider_import_payload,
        provider_registration_manifest=provider_registration_manifest,
        provider_storage_surface=provider_storage_surface,
        consumer_registration_manifest=consumer_registration_manifest,
        link_plan=link_plan,
    )


__all__ = [
    "CrossModuleStorageReflectionArtifacts",
    "capture_cross_module_storage_reflection_artifacts",
]
