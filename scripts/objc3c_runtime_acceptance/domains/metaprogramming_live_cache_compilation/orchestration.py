"""Case orchestration for live metaprogramming host-cache compilation."""

from __future__ import annotations

from pathlib import Path
from subprocess import CompletedProcess

from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_helpers import (
    remove_metaprogramming_cache_entry_from_artifact,
)

from .assertions import expect_materializing_cache_miss_was_found
from .config import MATERIALIZATION_ATTEMPT_LIMIT, case_dir_for_run
from .fixtures import (
    build_provider,
    consumer_compile_dir,
    provider_materialization_compile_dir,
    provider_replay_compile_dir,
    provider_source_for_materialization_attempt,
    write_provider_fixture,
)
from .models import (
    LiveMetaprogrammingCacheCompile,
    LiveMetaprogrammingCacheConsumerLink,
    LiveMetaprogrammingCacheProbeRun,
    LiveMetaprogrammingCacheProvider,
)
from .runner import LiveMetaprogrammingCacheCommandRunner


def live_metaprogramming_cache_case_dir(run_dir: Path) -> Path:
    return case_dir_for_run(run_dir)


def prepare_live_metaprogramming_cache_provider(
    case_dir: Path,
) -> LiveMetaprogrammingCacheProvider:
    return build_provider(case_dir)


def materialize_live_metaprogramming_cache_provider(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
) -> LiveMetaprogrammingCacheCompile:
    runner = LiveMetaprogrammingCacheCommandRunner()
    for materialization_attempt in range(0, MATERIALIZATION_ATTEMPT_LIMIT):
        write_provider_fixture(
            provider,
            provider_source_for_materialization_attempt(
                provider,
                materialization_attempt,
            ),
        )
        candidate = runner.compile_provider(
            provider.fixture,
            provider_materialization_compile_dir(
                case_dir,
                materialization_attempt,
                after_cache_prune=False,
            ),
            provider.cache_root_args,
        )
        if _needs_forced_materialization_retry(candidate):
            candidate = runner.compile_provider(
                provider.fixture,
                provider_materialization_compile_dir(
                    case_dir,
                    materialization_attempt,
                    after_cache_prune=True,
                ),
                provider.cache_root_args,
            )
        if _launch_attempted(candidate):
            return candidate

    expect_materializing_cache_miss_was_found()


def compile_live_metaprogramming_cache_replay(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
) -> LiveMetaprogrammingCacheCompile:
    return LiveMetaprogrammingCacheCommandRunner().compile_provider(
        provider.fixture,
        provider_replay_compile_dir(case_dir),
        provider.cache_root_args,
    )


def compile_live_metaprogramming_cache_tampered_replay_expect_failure(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
) -> CompletedProcess[str]:
    return LiveMetaprogrammingCacheCommandRunner().compile_provider_expect_failure(
        provider.fixture,
        case_dir / "provider-tampered-cache-replay",
        provider.cache_root_args,
    )


def compile_live_metaprogramming_cache_consumer(
    provider: LiveMetaprogrammingCacheProvider,
    case_dir: Path,
    runtime_import_path: Path,
) -> LiveMetaprogrammingCacheConsumerLink:
    return LiveMetaprogrammingCacheCommandRunner().compile_consumer(
        consumer_compile_dir(case_dir),
        runtime_import_path,
        provider.cache_root_args,
    )


def run_live_metaprogramming_cache_probe(
    clangxx: str,
    case_dir: Path,
    cache_root_override: str,
) -> LiveMetaprogrammingCacheProbeRun:
    return LiveMetaprogrammingCacheCommandRunner().run_probe(
        clangxx,
        case_dir,
        cache_root_override,
    )


def _needs_forced_materialization_retry(
    candidate: LiveMetaprogrammingCacheCompile,
) -> bool:
    return (
        not _launch_attempted(candidate)
        and remove_metaprogramming_cache_entry_from_artifact(
            candidate.host_cache_artifact
        )
    )


def _launch_attempted(candidate: LiveMetaprogrammingCacheCompile) -> bool:
    return candidate.host_cache_artifact.get("launch_attempted") is True


__all__ = [
    "compile_live_metaprogramming_cache_consumer",
    "compile_live_metaprogramming_cache_replay",
    "compile_live_metaprogramming_cache_tampered_replay_expect_failure",
    "live_metaprogramming_cache_case_dir",
    "materialize_live_metaprogramming_cache_provider",
    "prepare_live_metaprogramming_cache_provider",
    "run_live_metaprogramming_cache_probe",
]
