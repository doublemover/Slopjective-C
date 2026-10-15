"""Developer tooling validation action specs."""

from __future__ import annotations

from .action_spec import ActionSpec

TOOLING_VALIDATION_ACTION_SPECS: dict[str, ActionSpec] = {
    "validate-developer-tooling": ActionSpec(
        "validate-developer-tooling",
        "run the integrated developer-tooling inspect and trace validation flow",
        "python:scripts/check_objc3c_developer_tooling_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "developer-facing inspect and trace commands stay executable, artifact-backed, "
            "and tied to the live frontend runner"
        ),
    ),
    "validate-language-service": ActionSpec(
        "validate-language-service",
        "replay checked Objective-C 3 language-service requests and validate source-graph-backed responses",
        "python:scripts/check_objc3c_language_service.py",
        validation_tier="repo",
        guarantee_owner=(
            "language-service request dispatch, document lifecycle, workspace indexing, "
            "cache invalidation, and unsupported request boundaries stay fixture-backed"
        ),
    ),
    "validate-debug-source-maps": ActionSpec(
        "validate-debug-source-maps",
        "validate Objective-C 3 source maps, debug maps, provenance links, native line-table rows, and inline frames",
        "python:scripts/check_objc3c_debug_source_maps.py",
        validation_tier="repo",
        guarantee_owner=(
            "debug/source-map artifacts fail closed when source graph nodes, source digests, "
            "provenance links, optimization preservation claims, package identity, native "
            "line-table rows, or inline-frame ranges drift"
        ),
    ),
    "validate-debugger-integration": ActionSpec(
        "validate-debugger-integration",
        "validate replayable LLDB debugger commands, value inspection, and source-map-backed stepping records",
        "python:scripts/check_objc3c_debugger_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "debugger stepping and LLDB command claims stay replayable and fail closed unless "
            "source maps, debug-map entries, native line-table rows, object/debug anchors, and "
            "debug-preserved build settings all agree"
        ),
        pass_through_args=True,
    ),
    "validate-object-model-debugger-proof": ActionSpec(
        "validate-object-model-debugger-proof",
        "validate object-model debugger source maps, native line tables, debug anchors, and value inspection links",
        "python:scripts/check_objc3c_object_model_debugger_proof.py",
        validation_tier="repo",
        guarantee_owner=(
            "object-model debugger proof stays fail-closed unless source-map entries, "
            "debug-map records, native line-table rows, runtime reflection debug anchors, "
            "and replayable value-inspection commands all point at the same runtime "
            "reflection identities"
        ),
        pass_through_args=True,
    ),
    "validate-bonus-experiences": ActionSpec(
        "validate-bonus-experiences",
        "run the integrated bonus-experience validation flow across the live showcase tutorial and template surfaces",
        "python:scripts/check_objc3c_bonus_experience_integration.py",
        validation_tier="repo",
        guarantee_owner=(
            "bonus-experience workflows stay executable, template-derived, and tied to "
            "the live showcase tutorial and developer-tooling surfaces"
        ),
    ),
    "validate-runnable-bonus-experiences": ActionSpec(
        "validate-runnable-bonus-experiences",
        "validate template-derived bonus experiences end to end from the staged runnable toolchain bundle",
        "python:scripts/check_objc3c_runnable_bonus_experience_end_to_end.py",
        validation_tier="full",
        guarantee_owner=(
            "staged runnable toolchain bundles preserve bonus-experience template "
            "compilation runtime execution and capability-probe semantics"
        ),
    ),
}
