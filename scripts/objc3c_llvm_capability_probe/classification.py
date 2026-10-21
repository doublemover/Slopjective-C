"""Capability classification surfaces for LLVM probe output."""

from __future__ import annotations

from objc3c_tooling.paths import display_path

from .constants import PROGRAM_SURFACE_PATH, SHOWCASE_PORTFOLIO_PATH


def build_sema_type_system_parity_surface(
    *,
    clang_probe: dict[str, object],
    llc_probe: dict[str, object],
    llc_features: dict[str, object],
) -> dict[str, object]:
    clang_found = bool(clang_probe.get("found"))
    llc_found = bool(llc_probe.get("found"))
    llc_supports_obj = bool(llc_features.get("supports_filetype_obj", False))
    llc_supports_target_object = bool(
        llc_features.get("supports_target_object_emission", False)
    )

    deterministic_semantic_diagnostics = clang_found
    deterministic_type_metadata_handoff = (
        clang_found and llc_found and llc_supports_obj and llc_supports_target_object
    )

    blockers: list[str] = []
    if not clang_found:
        blockers.append("clang executable missing")
    if not llc_found:
        blockers.append("llc executable missing")
    elif not llc_supports_obj:
        blockers.append("llc missing --filetype=obj support")
    elif not llc_supports_target_object:
        target_triple = str(llc_features.get("target_triple", "target"))
        blockers.append(f"llc target object emission failed for {target_triple}")

    parity_ready = deterministic_semantic_diagnostics and deterministic_type_metadata_handoff
    return {
        "deterministic_semantic_diagnostics": deterministic_semantic_diagnostics,
        "deterministic_type_metadata_handoff": deterministic_type_metadata_handoff,
        "parity_ready": parity_ready,
        "blockers": blockers,
    }


def build_capability_demo_compatibility_surface(
    *,
    program_surface: dict[str, object],
    showcase_portfolio: dict[str, object],
    parity_ready: bool,
) -> dict[str, object]:
    failures: list[str] = []
    program_examples = program_surface.get("capability_demo_examples")
    showcase_examples = showcase_portfolio.get("examples")
    onboarding_policy = program_surface.get("onboarding_policy")

    if not isinstance(program_examples, list):
        failures.append("program surface did not publish capability_demo_examples")
        program_examples = []
    if not isinstance(showcase_examples, list):
        failures.append("showcase portfolio did not publish examples")
        showcase_examples = []
    if not isinstance(onboarding_policy, dict):
        failures.append("program surface did not publish onboarding_policy")
        onboarding_policy = {}

    program_examples_by_id = {
        str(entry.get("id")): entry
        for entry in program_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }
    showcase_examples_by_id = {
        str(entry.get("id")): entry
        for entry in showcase_examples
        if isinstance(entry, dict) and isinstance(entry.get("id"), str)
    }

    program_ids = list(program_examples_by_id)
    showcase_ids = list(showcase_examples_by_id)
    examples: list[dict[str, object]] = []

    program_examples_match_showcase_examples = program_ids == showcase_ids
    story_capabilities_match = True
    stdlib_followup_modules_match = True
    actor_claims_are_qualified = True

    if not program_examples_match_showcase_examples:
        failures.append("program surface example ids drifted from showcase portfolio examples")

    for demo_id in program_ids:
        program_entry = program_examples_by_id[demo_id]
        showcase_entry = showcase_examples_by_id.get(demo_id)
        if showcase_entry is None:
            failures.append(f"showcase portfolio missing capability demo entry {demo_id}")
            continue
        story_capabilities = program_entry.get("story_capabilities")
        stdlib_followup_modules = program_entry.get("stdlib_followup_modules")
        if not isinstance(story_capabilities, list) or not all(
            isinstance(value, str) and value for value in story_capabilities
        ):
            failures.append(f"program surface story_capabilities malformed for {demo_id}")
            story_capabilities = []
        if not isinstance(stdlib_followup_modules, list) or not all(
            isinstance(value, str) and value for value in stdlib_followup_modules
        ):
            failures.append(f"program surface stdlib_followup_modules malformed for {demo_id}")
            stdlib_followup_modules = []
        if showcase_entry.get("story_capabilities") != story_capabilities:
            story_capabilities_match = False
            failures.append(f"story capability drift detected for {demo_id}")
        if showcase_entry.get("stdlib_followup_modules") != stdlib_followup_modules:
            stdlib_followup_modules_match = False
            failures.append(f"stdlib follow-up module drift detected for {demo_id}")
        if "actors" in story_capabilities:
            actor_claims_are_qualified = False
            failures.append(f"capability demo {demo_id} published unsupported runnable actor claim")

        claim_class = (
            "comparison-facing"
            if "actor-shaped-messaging" in story_capabilities
            else "runnable-now"
        )
        examples.append(
            {
                "id": demo_id,
                "claim_class": claim_class,
                "story_capabilities": story_capabilities,
                "stdlib_followup_modules": stdlib_followup_modules,
                "compatible": showcase_entry.get("story_capabilities") == story_capabilities
                and showcase_entry.get("stdlib_followup_modules") == stdlib_followup_modules
                and "actors" not in story_capabilities,
            }
        )

    drift_checks = {
        "program_examples_match_showcase_examples": program_examples_match_showcase_examples,
        "story_capabilities_match": story_capabilities_match,
        "stdlib_followup_modules_match": stdlib_followup_modules_match,
        "actor_claims_are_qualified": actor_claims_are_qualified,
        "probe_ready_for_demo_validation": parity_ready,
    }
    if not parity_ready:
        failures.append("capability demo compatibility requires sema/type-system parity to stay ready")

    return {
        "program_surface": display_path(PROGRAM_SURFACE_PATH),
        "showcase_portfolio": display_path(SHOWCASE_PORTFOLIO_PATH),
        "onboarding_entry_order": onboarding_policy.get("entry_order", []),
        "drift_checks": drift_checks,
        "examples": examples,
        "failures": failures,
        "ok": not failures,
    }
