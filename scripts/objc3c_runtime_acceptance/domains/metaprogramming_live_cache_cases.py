"""Live metaprogramming host-cache runtime acceptance case registry."""

from __future__ import annotations

from pathlib import Path

from objc3c_runtime_acceptance.case_result import CaseResult
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_compilation import (
    HOST_CACHE_PROBE_PATH,
    compile_live_metaprogramming_cache_consumer,
    compile_live_metaprogramming_cache_replay,
    live_metaprogramming_cache_case_dir,
    materialize_live_metaprogramming_cache_provider,
    prepare_live_metaprogramming_cache_provider,
    run_live_metaprogramming_cache_probe,
)
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_payload_assertions import (
    assert_cache_hit_replay_payload,
    assert_materialized_host_cache_payload,
)
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_runtime_assertions import (
    assert_cache_hit_runtime_surfaces,
    assert_consumer_link_plan,
    assert_materialized_runtime_surfaces,
    assert_runtime_probe_payload,
)
from objc3c_runtime_acceptance.domains.metaprogramming_live_cache_summary import (
    build_live_metaprogramming_cache_runtime_integration_summary,
    live_metaprogramming_cache_provider_fixture_summary_path,
)


def check_live_metaprogramming_cache_runtime_integration_case(
    clangxx: str, run_dir: Path
) -> CaseResult:
    case_dir = live_metaprogramming_cache_case_dir(run_dir)
    case_dir.mkdir(parents=True, exist_ok=True)

    provider = prepare_live_metaprogramming_cache_provider(case_dir)
    materialized = materialize_live_metaprogramming_cache_provider(
        provider,
        case_dir,
    )
    assert_materialized_host_cache_payload(materialized)
    assert_materialized_runtime_surfaces(provider, materialized)

    replay = compile_live_metaprogramming_cache_replay(provider, case_dir)
    assert_cache_hit_replay_payload(materialized, replay)
    assert_cache_hit_runtime_surfaces(materialized, replay)

    consumer_link = compile_live_metaprogramming_cache_consumer(
        provider,
        case_dir,
        materialized.runtime_import_path,
    )
    assert_consumer_link_plan(provider, materialized, consumer_link)

    probe_run = run_live_metaprogramming_cache_probe(
        clangxx,
        case_dir,
        provider.cache_root_override,
    )
    assert_runtime_probe_payload(materialized, probe_run)

    return CaseResult(
        case_id="live-metaprogramming-cache-runtime-integration",
        probe=HOST_CACHE_PROBE_PATH,
        fixture=live_metaprogramming_cache_provider_fixture_summary_path(provider),
        claim_class="compile-linked-runtime-probe",
        passed=True,
        summary=build_live_metaprogramming_cache_runtime_integration_summary(
            provider,
            materialized,
            replay,
            consumer_link,
        ),
    )
