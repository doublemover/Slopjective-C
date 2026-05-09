"""Imported-runtime packaging replay compile artifacts."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from time import perf_counter
from typing import Any

from ..fixture_compilation import compile_fixture_with_args
from ..paths import ROOT
from ..runtime_contract_interop import (
    IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE,
    IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE,
)


@dataclass(frozen=True)
class ImportedRuntimePackagingReplayArtifacts:
    provider_obj: Path
    consumer_obj: Path
    provider_import_surface: Path
    link_plan_path: Path
    provider_import_payload: dict[str, Any]
    provider_registration_manifest: dict[str, Any]
    consumer_registration_manifest: dict[str, Any]
    link_plan: dict[str, Any]
    provider_compile_ms: int
    consumer_compile_ms: int


def compile_imported_runtime_packaging_fixture_pair(
    case_dir: Path,
) -> ImportedRuntimePackagingReplayArtifacts:
    provider_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_PROVIDER_FIXTURE)
    consumer_fixture = ROOT / Path(IMPORTED_RUNTIME_PACKAGING_CONSUMER_FIXTURE)

    provider_compile_dir = case_dir / "provider"
    provider_compile_started = perf_counter()
    provider_obj = compile_fixture_with_args(
        provider_fixture,
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / "module.runtime-import-surface.json"
    if not provider_import_surface.is_file():
        raise RuntimeError(
            f"imported runtime provider did not publish {provider_import_surface}"
        )
    provider_import_payload = json.loads(
        provider_import_surface.read_text(encoding="utf-8")
    )
    provider_registration_manifest = json.loads(
        (provider_compile_dir / "module.runtime-registration-manifest.json").read_text(
            encoding="utf-8"
        )
    )

    consumer_compile_dir = case_dir / "consumer"
    consumer_compile_started = perf_counter()
    consumer_obj = compile_fixture_with_args(
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
    link_plan_path = consumer_compile_dir / "module.cross-module-runtime-link-plan.json"
    if not link_plan_path.is_file():
        raise RuntimeError(
            f"imported runtime consumer did not publish {link_plan_path}"
        )
    link_plan = json.loads(link_plan_path.read_text(encoding="utf-8"))

    return ImportedRuntimePackagingReplayArtifacts(
        provider_obj=provider_obj,
        consumer_obj=consumer_obj,
        provider_import_surface=provider_import_surface,
        link_plan_path=link_plan_path,
        provider_import_payload=provider_import_payload,
        provider_registration_manifest=provider_registration_manifest,
        consumer_registration_manifest=consumer_registration_manifest,
        link_plan=link_plan,
        provider_compile_ms=provider_compile_ms,
        consumer_compile_ms=consumer_compile_ms,
    )


__all__ = [
    "ImportedRuntimePackagingReplayArtifacts",
    "compile_imported_runtime_packaging_fixture_pair",
]
