"""Command execution adapters for live metaprogramming host-cache cases."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture_with_args
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe
from objc3c_runtime_acceptance.probes import parse_key_value_output
from objc3c_runtime_acceptance.probes import run_probe

from .artifacts import load_consumer_link_plan, load_provider_compile_artifacts
from .config import (
    CACHE_ROOT_ENVIRONMENT_VARIABLE,
    CONSUMER_FIXTURE_PATH,
    HOST_CACHE_PROBE_LABEL,
    HOST_CACHE_PROBE_PATH,
    IMPORT_RUNTIME_SURFACE_FLAG,
    REGISTRATION_ORDINAL_FLAG,
)
from .fixtures import probe_executable_path
from .models import (
    LiveMetaprogrammingCacheCompile,
    LiveMetaprogrammingCacheConsumerLink,
    LiveMetaprogrammingCacheProbeRun,
)


@dataclass(frozen=True)
class LiveMetaprogrammingCacheCommandRunner:
    """Narrow adapter around compile/probe commands used by the case."""

    def compile_provider(
        self,
        fixture: Path,
        compile_dir: Path,
        cache_root_args: tuple[str, ...],
    ) -> LiveMetaprogrammingCacheCompile:
        compile_fixture_with_args(
            fixture,
            compile_dir,
            [
                REGISTRATION_ORDINAL_FLAG,
                "1",
                *cache_root_args,
            ],
        )
        return load_provider_compile_artifacts(compile_dir)

    def compile_consumer(
        self,
        compile_dir: Path,
        runtime_import_path: Path,
        cache_root_args: tuple[str, ...],
    ) -> LiveMetaprogrammingCacheConsumerLink:
        compile_fixture_with_args(
            CONSUMER_FIXTURE_PATH,
            compile_dir,
            [
                REGISTRATION_ORDINAL_FLAG,
                "2",
                IMPORT_RUNTIME_SURFACE_FLAG,
                str(runtime_import_path),
                *cache_root_args,
            ],
        )
        return load_consumer_link_plan(compile_dir)

    def run_probe(
        self,
        clangxx: str,
        case_dir: Path,
        cache_root_override: str,
    ) -> LiveMetaprogrammingCacheProbeRun:
        executable_path = probe_executable_path(case_dir)
        compile_probe(clangxx, ROOT / HOST_CACHE_PROBE_PATH, executable_path, [])
        payload = parse_key_value_output(
            run_probe(
                executable_path,
                env={CACHE_ROOT_ENVIRONMENT_VARIABLE: cache_root_override},
            ),
            HOST_CACHE_PROBE_LABEL,
        )
        return LiveMetaprogrammingCacheProbeRun(
            executable_path=executable_path,
            payload=payload,
        )


__all__ = ["LiveMetaprogrammingCacheCommandRunner"]
