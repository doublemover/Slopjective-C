"""Dispatch fast-path runtime probe assertions."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


def assert_dispatch_fast_path_probe_payload(payload: dict[str, Any]) -> None:
    expect(payload.get("baseline_status") == 0, "expected baseline method-cache snapshot to succeed")
    expect(payload.get("dynamic_entry_status") == 0, "expected dynamic fast-path entry lookup to succeed")
    expect(payload.get("explicit_entry_status") == 0, "expected explicit fast-path entry lookup to succeed")
    expect(payload.get("strict_error_entry_status") == 0, "expected strict error method-cache entry lookup to succeed")
    expect(payload.get("implicit_value") == 3, "expected implicit direct call to remain direct")
    expect(payload.get("explicit_value") == 5, "expected explicit direct call to remain direct")
    expect(payload.get("mixed_first") == 12 and payload.get("mixed_second") == 12,
           "expected mixed dispatch fixture to execute through the live runtime")
    expect(payload.get("strict_error_first") == payload.get("strict_error_expected") == payload.get("strict_error_second"),
           "expected strict dispatch error to stay deterministic across cache miss/hit")
    expect(payload.get("baseline_cache_entry_count") == 4,
           "expected realized dispatch runtime to seed four method-cache entries")
    expect(payload.get("baseline_fast_path_seed_count") == 4,
           "expected realized dispatch runtime to publish seeded fast-path entries")
    expect(payload.get("dynamic_entry_found") == 1 and payload.get("dynamic_entry_resolved") == 1,
           "expected dynamicEscape entry to resolve live")
    expect(payload.get("dynamic_entry_fast_path_seeded") == 1,
           "expected dynamicEscape entry to be seeded for fast-path dispatch")
    expect(payload.get("dynamic_entry_effective_direct_dispatch") == 0,
           "expected dynamicEscape entry to stay runtime-dispatched")
    expect(payload.get("dynamic_entry_fast_path_reason") == "class-final",
           "expected dynamicEscape fast-path reason to remain class-final")
    expect(payload.get("explicit_entry_found") == 1 and payload.get("explicit_entry_resolved") == 1,
           "expected explicitDirect entry to resolve live")
    expect(payload.get("explicit_entry_fast_path_seeded") == 1,
           "expected explicitDirect entry to be seeded for direct dispatch")
    expect(payload.get("explicit_entry_effective_direct_dispatch") == 1,
           "expected explicitDirect entry to preserve direct dispatch semantics")
    expect(payload.get("explicit_entry_fast_path_reason") == "direct",
           "expected explicitDirect fast-path reason to remain direct")
    expect(payload.get("mixed_first_state_last_dispatch_used_cache") == 1,
           "expected first mixed dispatch runtime call to hit the seeded cache")
    expect(payload.get("mixed_first_state_last_dispatch_used_fast_path") == 1,
           "expected first mixed dispatch runtime call to use the seeded fast path")
    expect(payload.get("mixed_first_state_last_dispatch_resolved_live_method") == 1,
           "expected first mixed dispatch runtime call to resolve a live method")
    expect(payload.get("mixed_first_state_last_dispatch_strict_error") == 0,
           "did not expect first mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_first_state_last_selector") == "dynamicEscape",
           "expected first mixed dispatch runtime call to target dynamicEscape")
    expect(payload.get("mixed_first_dispatch_state_status") == 0,
           "expected first mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_first_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected first mixed dispatch runtime call to report the cache-hit fast path")
    expect(payload.get("mixed_first_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected first mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_first_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected first mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_first_dispatch_state_last_used_builtin") == 0,
           "expected first mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("mixed_second_state_last_dispatch_used_cache") == 1,
           "expected repeated mixed dispatch runtime call to remain cached")
    expect(payload.get("mixed_second_state_last_dispatch_used_fast_path") == 1,
           "expected repeated mixed dispatch runtime call to remain on the fast path")
    expect(payload.get("mixed_second_state_last_dispatch_strict_error") == 0,
           "did not expect repeated mixed dispatch runtime call to fall back")
    expect(payload.get("mixed_second_dispatch_state_status") == 0,
           "expected repeated mixed dispatch runtime call to publish dispatch state")
    expect(payload.get("mixed_second_dispatch_state_last_dispatch_path") == "cache-hit-fast-path",
           "expected repeated mixed dispatch runtime call to stay on the cache-hit fast path")
    expect(payload.get("mixed_second_dispatch_state_last_implementation_kind") == "emitted-method-body",
           "expected repeated mixed dispatch runtime call to execute an emitted method body")
    expect(payload.get("mixed_second_dispatch_state_last_effective_direct_dispatch") == 0,
           "expected repeated mixed dispatch runtime call to remain runtime-dispatched")
    expect(payload.get("mixed_second_dispatch_state_last_used_builtin") == 0,
           "expected repeated mixed dispatch runtime call to avoid builtin dispatch")
    expect(payload.get("strict_error_first_state_last_dispatch_used_cache") == 0,
           "expected first missingDispatch: call to miss the cache")
    expect(payload.get("strict_error_first_state_last_dispatch_used_fast_path") == 0,
           "expected first missingDispatch: call to avoid the fast path")
    expect(payload.get("strict_error_first_state_last_dispatch_resolved_live_method") == 0,
           "did not expect first missingDispatch: call to resolve live")
    expect(payload.get("strict_error_first_state_last_dispatch_strict_error") == 1,
           "expected first missingDispatch: call to fall back")
    expect(payload.get("strict_error_first_dispatch_state_status") == 0,
           "expected first missingDispatch: call to publish dispatch state")
    expect(payload.get("strict_error_first_dispatch_state_last_dispatch_path") == "slow-path-error",
           "expected first missingDispatch: call to report slow-path strict dispatch error")
    expect(payload.get("strict_error_first_dispatch_state_last_implementation_kind") == "strict-dispatch-error",
           "expected first missingDispatch: call to report strict dispatch error status")
    expect(payload.get("strict_error_second_state_last_dispatch_used_cache") == 1,
           "expected repeated missingDispatch: call to hit the strict error cache entry")
    expect(payload.get("strict_error_second_state_last_dispatch_used_fast_path") == 0,
           "expected repeated missingDispatch: call to stay off the fast path")
    expect(payload.get("strict_error_second_state_last_dispatch_strict_error") == 1,
           "expected repeated missingDispatch: call to remain a strict dispatch error")
    expect(payload.get("strict_error_second_dispatch_state_status") == 0,
           "expected repeated missingDispatch: call to publish dispatch state")
    expect(payload.get("strict_error_second_dispatch_state_last_dispatch_path") == "cache-hit-error",
           "expected repeated missingDispatch: call to report cached strict dispatch error")
    expect(payload.get("strict_error_second_dispatch_state_last_implementation_kind") == "strict-dispatch-error",
           "expected repeated missingDispatch: call to report cached strict dispatch error status")


__all__ = ["assert_dispatch_fast_path_probe_payload"]
