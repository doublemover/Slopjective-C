"""Block/ARC capture legality fixture catalog."""

from __future__ import annotations

from ...fixture_compilation import NegativeDiagnosticExpectation
from ...paths import ROOT
from .data import CaptureFixtureSpec, SurfaceProfile


CASE_ID = "escaping-block-capture-legality"
PROBE = "compile-manifest-diagnostics-and-llvm-ir"
CLAIM_CLASS = "compile-coupled-inspection"
FIXTURE = (
    "tests/tooling/fixtures/native/"
    "escaping_block_runtime_hook_argument_positive.objc3"
)

NATIVE_FIXTURE_DIR = ROOT / "tests" / "tooling" / "fixtures" / "native"

SEMANTIC_SURFACE_PATH = ("frontend", "pipeline", "semantic_surface")
ESCAPE_SURFACE_KEY = "objc_block_storage_escape_lowering_surface"
COPY_DISPOSE_SURFACE_KEY = "objc_block_copy_dispose_lowering_surface"

ARGUMENT_FIXTURE = CaptureFixtureSpec(
    key="argument",
    fixture=NATIVE_FIXTURE_DIR / "escaping_block_runtime_hook_argument_positive.objc3",
    output_dir_name="argument-positive",
    expected_profile=SurfaceProfile(
        escape_to_heap_sites=1,
        requires_byref_cells_sites=0,
        copy_helper_required_sites=0,
        dispose_helper_required_sites=0,
    ),
    escape_message=(
        "expected escaping argument fixture to publish one heap-promotion "
        "candidate without byref cells"
    ),
    copy_dispose_message=(
        "expected escaping argument fixture to keep copy/dispose helpers elided"
    ),
)

RETURN_FIXTURE = CaptureFixtureSpec(
    key="return",
    fixture=NATIVE_FIXTURE_DIR / "escaping_block_runtime_hook_return_positive.objc3",
    output_dir_name="return-positive",
    expected_profile=SurfaceProfile(
        escape_to_heap_sites=1,
        requires_byref_cells_sites=0,
        copy_helper_required_sites=0,
        dispose_helper_required_sites=0,
    ),
    escape_message=(
        "expected escaping return fixture to publish one heap-promotion "
        "candidate without byref cells"
    ),
    copy_dispose_message=(
        "expected escaping return fixture to keep copy/dispose helpers elided"
    ),
)

BYREF_ESCAPE_FIXTURE = CaptureFixtureSpec(
    key="byref",
    fixture=NATIVE_FIXTURE_DIR / "escaping_block_runtime_hook_byref_positive.objc3",
    output_dir_name="byref-escape-positive",
    expected_profile=SurfaceProfile(
        escape_to_heap_sites=1,
        requires_byref_cells_sites=1,
        copy_helper_required_sites=1,
        dispose_helper_required_sites=1,
    ),
    escape_message=(
        "expected escaping byref fixture to publish one heap-promotion "
        "candidate with one byref-cell site"
    ),
    copy_dispose_message=(
        "expected escaping byref fixture to require copy/dispose helpers"
    ),
)

OWNED_ESCAPE_FIXTURE = CaptureFixtureSpec(
    key="owned",
    fixture=(
        NATIVE_FIXTURE_DIR
        / "escaping_block_runtime_hook_owned_capture_positive.objc3"
    ),
    output_dir_name="owned-escape-positive",
    expected_profile=SurfaceProfile(
        escape_to_heap_sites=1,
        requires_byref_cells_sites=0,
        copy_helper_required_sites=1,
        dispose_helper_required_sites=1,
    ),
    escape_message=(
        "expected escaping owned-capture fixture to publish one heap-promotion "
        "candidate without byref cells"
    ),
    copy_dispose_message=(
        "expected escaping owned-capture fixture to require copy/dispose helpers"
    ),
)

PRE_DIAGNOSTIC_FIXTURES = (ARGUMENT_FIXTURE, RETURN_FIXTURE)
POST_DIAGNOSTIC_FIXTURES = (BYREF_ESCAPE_FIXTURE, OWNED_ESCAPE_FIXTURE)
SUMMARY_FIXTURES = (
    ARGUMENT_FIXTURE,
    RETURN_FIXTURE,
    BYREF_ESCAPE_FIXTURE,
    OWNED_ESCAPE_FIXTURE,
)

NEGATIVE_EXPECTATIONS = (
    NegativeDiagnosticExpectation(
        key="bad-call-negative",
        fixture=NATIVE_FIXTURE_DIR / "capture_legality_escape_invocation_bad_call.objc3",
        expected_snippets=[
            "type mismatch: expected 'i32' argument for parameter 0 of callable "
            "'closure', got 'bool'"
        ],
        expected_codes=["O3S206"],
    ),
    NegativeDiagnosticExpectation(
        key="missing-capture-negative",
        fixture=(
            NATIVE_FIXTURE_DIR
            / "capture_legality_escape_invocation_missing_capture.objc3"
        ),
        expected_snippets=["undefined capture 'seed' in block literal"],
        expected_codes=["O3S202"],
    ),
)


__all__ = [
    "CASE_ID",
    "CLAIM_CLASS",
    "COPY_DISPOSE_SURFACE_KEY",
    "ESCAPE_SURFACE_KEY",
    "FIXTURE",
    "NEGATIVE_EXPECTATIONS",
    "POST_DIAGNOSTIC_FIXTURES",
    "PRE_DIAGNOSTIC_FIXTURES",
    "PROBE",
    "SEMANTIC_SURFACE_PATH",
    "SUMMARY_FIXTURES",
]
