"""Fixture and source builders for live metaprogramming host-cache cases."""

from __future__ import annotations

from datetime import datetime
from pathlib import Path

from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_helpers import (
    build_metaprogramming_cache_seed_macros,
)

from .config import (
    PROVIDER_FIXTURE_PATH,
    PROVIDER_MATERIALIZED_FIXTURE_NAME,
    PROVIDER_MODULE_DECLARATION,
    PROVIDER_MODULE_NAME_PREFIX,
    cache_root_args,
    cache_root_for_case,
    cache_root_override_for_case,
)
from .models import LiveMetaprogrammingCacheProvider, ProviderSource


def build_provider_source() -> ProviderSource:
    unique_suffix = datetime.now().strftime("%H%M%S%f")
    module_name = f"{PROVIDER_MODULE_NAME_PREFIX}{unique_suffix}"
    source = PROVIDER_FIXTURE_PATH.read_text(encoding="utf-8").replace(
        PROVIDER_MODULE_DECLARATION,
        f"module {module_name};",
        1,
    )
    return ProviderSource(
        source=source,
        module_name=module_name,
        unique_suffix=unique_suffix,
    )


def build_provider(case_dir: Path) -> LiveMetaprogrammingCacheProvider:
    source = build_provider_source()
    cache_root_override = cache_root_override_for_case(case_dir)
    return LiveMetaprogrammingCacheProvider(
        fixture=case_dir / PROVIDER_MATERIALIZED_FIXTURE_NAME,
        source=source.source,
        module_name=source.module_name,
        unique_suffix=source.unique_suffix,
        cache_root=cache_root_for_case(case_dir),
        cache_root_override=cache_root_override,
        cache_root_args=cache_root_args(cache_root_override),
    )


def provider_source_for_materialization_attempt(
    provider: LiveMetaprogrammingCacheProvider,
    materialization_attempt: int,
) -> str:
    return provider.source + build_metaprogramming_cache_seed_macros(
        materialization_attempt,
        provider.unique_suffix,
    )


def write_provider_fixture(
    provider: LiveMetaprogrammingCacheProvider,
    source: str,
) -> None:
    provider.fixture.write_text(source, encoding="utf-8")


def provider_materialization_compile_dir(
    case_dir: Path,
    materialization_attempt: int,
    *,
    after_cache_prune: bool,
) -> Path:
    suffix = "-materialize" if after_cache_prune else ""
    return case_dir / f"provider-first-{materialization_attempt:02d}{suffix}"


def provider_replay_compile_dir(case_dir: Path) -> Path:
    return case_dir / "provider-second"


def consumer_compile_dir(case_dir: Path) -> Path:
    return case_dir / "consumer"


def probe_executable_path(case_dir: Path) -> Path:
    return case_dir / "macro_host_process_cache_integration_probe.exe"


__all__ = [
    "build_provider",
    "build_provider_source",
    "consumer_compile_dir",
    "probe_executable_path",
    "provider_materialization_compile_dir",
    "provider_replay_compile_dir",
    "provider_source_for_materialization_attempt",
    "write_provider_fixture",
]
