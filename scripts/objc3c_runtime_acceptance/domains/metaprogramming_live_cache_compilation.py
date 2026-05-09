"""Compile and probe operations for live metaprogramming host-cache cases."""

from __future__ import annotations

import json
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any

from objc3c_runtime_acceptance.assertions import expect
from objc3c_runtime_acceptance.native_build import compile_fixture_with_args
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_helpers import (
    build_metaprogramming_cache_seed_macros,
    compile_live_metaprogramming_cache_candidate,
    remove_metaprogramming_cache_entry_from_artifact,
)

from ..core import ROOT
from ..progress import repo_display_path

HOST_CACHE_PROBE_PATH = "tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp"
HOST_CACHE_PROBE_LABEL = "live metaprogramming host-cache runtime integration probe"


@dataclass(frozen=True)
class LiveMetaprogrammingCacheProvider:
    fixture: Path
    source: str
    module_name: str
    unique_suffix: str
    cache_root: Path
    cache_root_override: str
    cache_root_args: list[str]


@dataclass(frozen=True)
class LiveMetaprogrammingCacheCompile:
    compile_dir: Path
    host_cache_artifact_path: Path
    runtime_import_path: Path
    host_cache_artifact: dict[str, Any]
    runtime_import_surface: dict[str, Any]
    host_cache_import_surface: dict[str, Any]


@dataclass(frozen=True)
class LiveMetaprogrammingCacheConsumerLink:
    compile_dir: Path
    link_plan_path: Path
    link_plan: dict[str, Any]


@dataclass(frozen=True)
class LiveMetaprogrammingCacheProbeRun:
    executable_path: Path
    payload: dict[str, Any]


def live_metaprogramming_cache_case_dir(run_dir: Path) -> Path:
    return run_dir / "live-metaprogramming-cache-runtime-integration"


def prepare_live_metaprogramming_cache_provider(
    case_dir: Path,
) -> LiveMetaprogrammingCacheProvider:
    provider_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_provider.objc3"
    )
    provider_source = provider_fixture.read_text(encoding="utf-8")
    unique_suffix = datetime.now().strftime("%H%M%S%f")
    unique_module_name = f"MetaprogrammingHostProcessProvider{unique_suffix}"
    provider_source = provider_source.replace(
        "module MetaprogrammingHostProcessProvider;",
        f"module {unique_module_name};",
        1,
    )

    cache_root = case_dir / "metaprogramming-cache-root"
    cache_root_override = repo_display_path(cache_root)
    return LiveMetaprogrammingCacheProvider(
        fixture=case_dir / "metaprogramming_cache_provider_materialize.objc3",
        source=provider_source,
        module_name=unique_module_name,
        unique_suffix=unique_suffix,
        cache_root=cache_root,
        cache_root_override=cache_root_override,
        cache_root_args=[
            "--objc3-metaprogramming-cache-root",
            cache_root_override,
        ],
    )


def materialize_live_metaprogramming_cache_provider(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
) -> LiveMetaprogrammingCacheCompile:
    for materialization_attempt in range(0, 16):
        extra_macros = build_metaprogramming_cache_seed_macros(
            materialization_attempt,
            provider.unique_suffix,
        )
        provider.fixture.write_text(provider.source + extra_macros, encoding="utf-8")
        compile_dir = case_dir / f"provider-first-{materialization_attempt:02d}"
        candidate = _compile_cache_provider_candidate(
            provider.fixture,
            compile_dir,
            provider.cache_root_args,
        )
        if (
            candidate.host_cache_artifact.get("launch_attempted") is not True
            and remove_metaprogramming_cache_entry_from_artifact(
                candidate.host_cache_artifact
            )
        ):
            compile_dir = (
                case_dir / f"provider-first-{materialization_attempt:02d}-materialize"
            )
            candidate = _compile_cache_provider_candidate(
                provider.fixture,
                compile_dir,
                provider.cache_root_args,
            )
        if candidate.host_cache_artifact.get("launch_attempted") is True:
            return candidate

    expect(
        False,
        "expected live metaprogramming host-cache implementation case to force a materializing cache miss before the cache-hit replay check",
    )
    raise AssertionError("unreachable")


def compile_live_metaprogramming_cache_replay(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
) -> LiveMetaprogrammingCacheCompile:
    return _compile_cache_provider_candidate(
        provider.fixture,
        case_dir / "provider-second",
        provider.cache_root_args,
    )


def compile_live_metaprogramming_cache_consumer(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
    runtime_import_path: Path,
) -> LiveMetaprogrammingCacheConsumerLink:
    consumer_fixture = (
        ROOT
        / "tests"
        / "tooling"
        / "fixtures"
        / "native"
        / "macro_host_process_consumer.objc3"
    )
    compile_dir = case_dir / "consumer"
    compile_fixture_with_args(
        consumer_fixture,
        compile_dir,
        [
            "--objc3-bootstrap-registration-order-ordinal",
            "2",
            "--objc3-import-runtime-surface",
            str(runtime_import_path),
            *provider.cache_root_args,
        ],
    )
    link_plan_path = compile_dir / "module.cross-module-runtime-link-plan.json"
    return LiveMetaprogrammingCacheConsumerLink(
        compile_dir=compile_dir,
        link_plan_path=link_plan_path,
        link_plan=json.loads(link_plan_path.read_text(encoding="utf-8")),
    )


def run_live_metaprogramming_cache_probe(
    clangxx: str,
    case_dir: Path,
    cache_root_override: str,
) -> LiveMetaprogrammingCacheProbeRun:
    host_cache_probe = ROOT / HOST_CACHE_PROBE_PATH
    host_cache_exe = case_dir / "macro_host_process_cache_integration_probe.exe"
    compile_probe(clangxx, host_cache_probe, host_cache_exe, [])
    payload = parse_key_value_output(
        run_probe(
            host_cache_exe,
            env={"OBJC3C_METAPROGRAMMING_CACHE_ROOT": cache_root_override},
        ),
        HOST_CACHE_PROBE_LABEL,
    )
    return LiveMetaprogrammingCacheProbeRun(
        executable_path=host_cache_exe,
        payload=payload,
    )


def _compile_cache_provider_candidate(
    temp_provider_fixture: Path,
    compile_dir: Path,
    cache_root_args: list[str],
) -> LiveMetaprogrammingCacheCompile:
    (
        host_cache_artifact_path,
        runtime_import_path,
        host_cache_artifact,
        runtime_import_surface,
        host_cache_import_surface,
    ) = compile_live_metaprogramming_cache_candidate(
        temp_provider_fixture,
        compile_dir,
        cache_root_args,
    )
    return LiveMetaprogrammingCacheCompile(
        compile_dir=compile_dir,
        host_cache_artifact_path=host_cache_artifact_path,
        runtime_import_path=runtime_import_path,
        host_cache_artifact=host_cache_artifact,
        runtime_import_surface=runtime_import_surface,
        host_cache_import_surface=host_cache_import_surface,
    )


__all__ = [
    "HOST_CACHE_PROBE_PATH",
    "LiveMetaprogrammingCacheCompile",
    "LiveMetaprogrammingCacheConsumerLink",
    "LiveMetaprogrammingCacheProbeRun",
    "LiveMetaprogrammingCacheProvider",
    "compile_live_metaprogramming_cache_consumer",
    "compile_live_metaprogramming_cache_replay",
    "live_metaprogramming_cache_case_dir",
    "materialize_live_metaprogramming_cache_provider",
    "prepare_live_metaprogramming_cache_provider",
    "run_live_metaprogramming_cache_probe",
]
