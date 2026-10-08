"""Assertions for metaprogramming macro-safety/cache diagnostics."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect
from objc3c_runtime_acceptance.paths import FRONTEND_RUNNER_RELATIVE_PATH


def expect_macro_safety_surface(surface: dict[str, Any]) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1",
        "expected macro host process provider fixture to preserve the macro safety semantic contract",
    )
    expect(
        surface.get("macro_marker_sites") == 1
        and surface.get("macro_package_sites") == 1
        and surface.get("macro_provenance_sites") == 1
        and surface.get("macro_cache_key_sites") == 1
        and surface.get("macro_sandbox_policy_sites") == 1
        and surface.get("expansion_visible_macro_sites") == 1,
        "expected macro host process provider fixture to preserve macro metadata counts",
    )
    expect(
        surface.get("safe_macro_callable_sites") == 1
        and surface.get("incomplete_macro_metadata_sites") == 0
        and surface.get("orphan_macro_metadata_sites") == 0
        and surface.get("invalid_package_sites") == 0
        and surface.get("invalid_provenance_sites") == 0
        and surface.get("missing_cache_key_sites") == 0
        and surface.get("invalid_cache_key_sites") == 0
        and surface.get("missing_sandbox_policy_sites") == 0
        and surface.get("invalid_sandbox_policy_sites") == 0
        and surface.get("nondeterministic_callable_sites") == 0
        and surface.get("unsupported_callable_topology_sites") == 0,
        "expected macro host process provider fixture to preserve fail-closed macro safety counts",
    )
    expect(
        surface.get("metadata_completeness_enforced") is True
        and surface.get("sandbox_namespace_enforced") is True
        and surface.get("provenance_determinism_enforced") is True
        and surface.get("cache_key_invalidation_enforced") is True
        and surface.get("sandbox_policy_deny_by_default_enforced") is True
        and surface.get("callable_determinism_enforced") is True
        and surface.get("deterministic") is True
        and surface.get("ready_for_lowering_and_runtime") is True,
        "expected macro host process provider fixture to preserve deterministic fail-closed enforcement flags",
    )


def expect_macro_host_cache_surface(surface: dict[str, Any]) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected macro host process provider fixture to publish the metaprogramming host-cache integration contract",
    )
    expect(
        surface.get("source_contract_id")
        == "objc3c.metaprogramming.expansion.host.runtime.boundary.v1",
        "expected macro host process provider fixture to preserve the host runtime boundary source contract",
    )
    expect(
        surface.get("host_executable_relative_path")
        == FRONTEND_RUNNER_RELATIVE_PATH
        and surface.get("cache_root_relative_path")
        == "tmp/artifacts/objc3c-native/cache/metaprogramming",
        "expected macro host process provider fixture to preserve host executable and cache root compatibility paths",
    )
    expect(
        surface.get("deterministic") is True
        and surface.get("host_process_exit_code") == 0
        and isinstance(surface.get("cache_hit"), bool),
        "expected macro host process provider fixture to preserve deterministic host-cache readiness",
    )
    expect(
        surface.get("cache_model")
        == "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-explicit-macro-cache-keys-and-policy-version-and-reused-on-subsequent-runs",
        "expected macro host process provider fixture to publish the explicit cache key model",
    )
    expect(
        surface.get("invalidation_model")
        == "metaprogramming-replay-key-explicit-macro-cache-key-or-sandbox-policy-drift-invalidates-the-entry-while-corrupt-or-incomplete-cache-artifacts-fail-closed"
        and surface.get("sandbox_policy_model")
        == "macro-host-materialization-is-deny-by-default-and-only-admits-pure-free-functions-with-objc_macro_sandbox-named-deterministic"
        and surface.get("diagnostics_model")
        == "stable-O3S331-and-O3S332-diagnostics-gate-missing-or-invalid-macro-cache-key-and-sandbox-policy-metadata",
        "expected macro host process provider fixture to publish cache invalidation, sandbox, and diagnostic models",
    )
    expect(
        surface.get("cache_artifact_contract_checked") is True
        and surface.get("provenance_integrity_checked") is True
        and surface.get("sandbox_policy_checked") is True
        and surface.get("diagnostic_contract_checked") is True
        and isinstance(surface.get("cache_key_material_digest"), str)
        and len(surface.get("cache_key_material_digest")) == 64,
        "expected macro host process provider fixture to publish checked cache provenance and material digest evidence",
    )


def expect_macro_runtime_import_surface(surface: dict[str, Any]) -> None:
    expect(
        surface.get("contract_id")
        == "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1",
        "expected runtime import surface to preserve the metaprogramming host-cache integration contract",
    )
    expect(
        surface.get("runtime_import_artifact_ready") is True
        and surface.get("separate_compilation_ready") is True
        and surface.get("deterministic") is True,
        "expected runtime import surface to preserve host-cache compatibility readiness",
    )
    expect(
        surface.get("invalidation_model")
        == "metaprogramming-replay-key-explicit-macro-cache-key-or-sandbox-policy-drift-invalidates-the-entry-while-corrupt-or-incomplete-cache-artifacts-fail-closed"
        and surface.get("sandbox_policy_model")
        == "macro-host-materialization-is-deny-by-default-and-only-admits-pure-free-functions-with-objc_macro_sandbox-named-deterministic"
        and surface.get("diagnostics_model")
        == "stable-O3S331-and-O3S332-diagnostics-gate-missing-or-invalid-macro-cache-key-and-sandbox-policy-metadata",
        "expected runtime import surface to preserve cache invalidation, sandbox, and diagnostic models",
    )
