"""Registration replay fixture and source artifact builders."""

from __future__ import annotations

import json
from pathlib import Path
from time import perf_counter

from ...fixture_compilation import compile_fixture_with_args
from ...paths import ROOT
from .catalog import (
    CONSUMER_FIXTURE,
    CONSUMER_OUTPUT_DIR,
    IMPORT_SURFACE_FILE,
    LINK_PLAN_FILE,
    PROVIDER_FIXTURE,
    PROVIDER_OUTPUT_DIR,
    REGISTRATION_MANIFEST_FILE,
)
from .models import JsonObject, RegistrationReplayArtifacts


def compile_registration_replay_fixture_pair(
    case_dir: Path,
) -> RegistrationReplayArtifacts:
    provider_compile_dir = case_dir / PROVIDER_OUTPUT_DIR
    provider_compile_started = perf_counter()
    provider_obj = compile_fixture_with_args(
        ROOT / Path(PROVIDER_FIXTURE),
        provider_compile_dir,
        ["--objc3-bootstrap-registration-order-ordinal", "1"],
    )
    provider_compile_ms = int((perf_counter() - provider_compile_started) * 1000)
    provider_import_surface = provider_compile_dir / IMPORT_SURFACE_FILE
    provider_registration_manifest = _read_json(
        provider_compile_dir / REGISTRATION_MANIFEST_FILE
    )

    consumer_compile_dir = case_dir / CONSUMER_OUTPUT_DIR
    consumer_compile_started = perf_counter()
    consumer_obj = compile_fixture_with_args(
        ROOT / Path(CONSUMER_FIXTURE),
        consumer_compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(provider_import_surface),
        ],
    )
    consumer_compile_ms = int((perf_counter() - consumer_compile_started) * 1000)
    consumer_registration_manifest = _read_json(
        consumer_compile_dir / REGISTRATION_MANIFEST_FILE
    )
    link_plan_path = consumer_compile_dir / LINK_PLAN_FILE

    return RegistrationReplayArtifacts(
        provider_obj=provider_obj,
        consumer_obj=consumer_obj,
        provider_import_surface=provider_import_surface,
        link_plan_path=link_plan_path,
        provider_registration_manifest=provider_registration_manifest,
        consumer_registration_manifest=consumer_registration_manifest,
        link_plan=_read_json(link_plan_path),
        provider_compile_ms=provider_compile_ms,
        consumer_compile_ms=consumer_compile_ms,
    )


def _read_json(path: Path) -> JsonObject:
    return json.loads(path.read_text(encoding="utf-8"))


__all__ = ["compile_registration_replay_fixture_pair"]
