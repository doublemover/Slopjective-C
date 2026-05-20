"""Block/ARC storage automation fixture catalog."""

from __future__ import annotations

from ...fixture_compilation import NegativeDiagnosticExpectation
from ...paths import ROOT
from .constants import ARC_ARG
from .models import FixtureArtifactSpec


CASE_ID = "block-storage-arc-automation-semantics"
NATIVE_FIXTURE_DIR = ROOT / "tests" / "tooling" / "fixtures" / "native"

OWNED_CAPTURE_FIXTURE = FixtureArtifactSpec(
    key="owned-capture",
    fixture=NATIVE_FIXTURE_DIR / "owned_object_capture_helper_positive.objc3",
    output_dir_name="owned-positive",
)
NONOWNING_CAPTURE_FIXTURE = FixtureArtifactSpec(
    key="nonowning-capture",
    fixture=(
        NATIVE_FIXTURE_DIR
        / "nonowning_object_capture_helper_elided_positive.objc3"
    ),
    output_dir_name="nonowning-positive",
)
ARC_MODE_FIXTURE = FixtureArtifactSpec(
    key="arc-mode",
    fixture=NATIVE_FIXTURE_DIR / "arc_mode_handling_positive.objc3",
    output_dir_name="arc-mode-positive",
    extra_args=(ARC_ARG,),
)
ARC_INFERENCE_FIXTURE = FixtureArtifactSpec(
    key="arc-inference",
    fixture=NATIVE_FIXTURE_DIR / "arc_inference_lifetime_positive.objc3",
    output_dir_name="arc-inference-positive",
    extra_args=(ARC_ARG,),
)
ARC_CLEANUP_SCOPE_FIXTURE = FixtureArtifactSpec(
    key="arc-cleanup-scope",
    fixture=NATIVE_FIXTURE_DIR / "arc_cleanup_scope_positive.objc3",
    output_dir_name="arc-cleanup-scope-positive",
    extra_args=(ARC_ARG,),
)
ARC_IMPLICIT_CLEANUP_FIXTURE = FixtureArtifactSpec(
    key="arc-implicit-cleanup",
    fixture=NATIVE_FIXTURE_DIR / "arc_implicit_cleanup_void_positive.objc3",
    output_dir_name="arc-implicit-cleanup-positive",
    extra_args=(ARC_ARG,),
)
ARC_AUTORELEASE_RETURN_FIXTURE = FixtureArtifactSpec(
    key="arc-autorelease-return",
    fixture=NATIVE_FIXTURE_DIR / "arc_autorelease_return_positive.objc3",
    output_dir_name="arc-autorelease-return-positive",
    extra_args=(ARC_ARG,),
)
ARC_METHOD_FAMILY_FIXTURE = FixtureArtifactSpec(
    key="arc-method-family-retained-message-cleanup",
    fixture=NATIVE_FIXTURE_DIR / "arc_method_family_retained_message_cleanup.objc3",
    output_dir_name="arc-method-family-retained-message-cleanup-positive",
    extra_args=(ARC_ARG,),
)

NEGATIVE_EXPECTATIONS = (
    NegativeDiagnosticExpectation(
        key="weak-mutation-negative",
        fixture=NATIVE_FIXTURE_DIR / "weak_object_capture_mutation_negative.objc3",
        expected_snippets=[
            "type mismatch: block mutated capture 'weakValue' requires owned runtime-backed storage"
        ],
        expected_codes=["O3S206"],
    ),
    NegativeDiagnosticExpectation(
        key="unowned-mutation-negative",
        fixture=NATIVE_FIXTURE_DIR / "unowned_object_capture_mutation_negative.objc3",
        expected_snippets=[
            "type mismatch: block mutated capture 'borrowedValue' requires owned runtime-backed storage"
        ],
        expected_codes=["O3S206"],
    ),
)


__all__ = [
    "ARC_AUTORELEASE_RETURN_FIXTURE",
    "ARC_CLEANUP_SCOPE_FIXTURE",
    "ARC_IMPLICIT_CLEANUP_FIXTURE",
    "ARC_INFERENCE_FIXTURE",
    "ARC_METHOD_FAMILY_FIXTURE",
    "ARC_MODE_FIXTURE",
    "CASE_ID",
    "NATIVE_FIXTURE_DIR",
    "NEGATIVE_EXPECTATIONS",
    "NONOWNING_CAPTURE_FIXTURE",
    "OWNED_CAPTURE_FIXTURE",
]
