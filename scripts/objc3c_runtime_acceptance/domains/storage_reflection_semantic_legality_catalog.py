"""Storage/reflection semantic legality case catalog data."""

from __future__ import annotations

from objc3c_runtime_acceptance.fixture_compilation import NegativeDiagnosticExpectation
from objc3c_runtime_acceptance.paths import ROOT


STORAGE_LEGALITY_SEMANTICS_CASE_ID = "storage-legality-semantics"
STORAGE_LEGALITY_REGISTRATION_MANIFEST_NAME = (
    "module.runtime-registration-manifest.json"
)
STORAGE_LEGALITY_POSITIVE_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "runtime_backed_storage_ownership_legality_positive.objc3"
)
STORAGE_LEGALITY_POSITIVE_FIXTURE_LABEL = (
    "tests/tooling/fixtures/native/runtime_backed_storage_ownership_legality_positive.objc3"
)

EXPECTED_STORAGE_LEGALITY_PROPERTY_DESCRIPTOR_COUNT = 10
EXPECTED_STORAGE_LEGALITY_IVAR_DESCRIPTOR_COUNT = 5

STORAGE_LEGALITY_IR_EVIDENCE = (
    (
        "runtime_backed_storage_ownership_legality",
        "runtime-backed storage ownership legality",
    ),
    ("property_attribute_profiles=10", "ten property-attribute profiles"),
    ("accessor_ownership_profiles=10", "ten accessor ownership profiles"),
)


def storage_legality_negative_expectations() -> list[NegativeDiagnosticExpectation]:
    return [
        NegativeDiagnosticExpectation(
            key="negative-atomic-ownership",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_atomic_ownership_negative.objc3",
            expected_snippets=[
                "atomic ownership-aware property 'value' in interface 'Widget' is unsupported until executable accessor storage semantics land"
            ],
            expected_codes=["O3S206"],
        ),
        NegativeDiagnosticExpectation(
            key="negative-weak-mismatch",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "runtime_backed_storage_ownership_weak_mismatch_negative.objc3",
            expected_snippets=[
                "property ownership qualifier '__weak' conflicts with @property ownership modifier 'assign'"
            ],
            expected_codes=["O3S206"],
        ),
        NegativeDiagnosticExpectation(
            key="negative-unowned-mismatch",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "runtime_backed_storage_ownership_unowned_mismatch_negative.objc3",
            expected_snippets=[
                "property ownership qualifier '__unsafe_unretained' conflicts with @property ownership modifier 'unowned'"
            ],
            expected_codes=["O3S206"],
        ),
        NegativeDiagnosticExpectation(
            key="negative-scalar-ownership",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_scalar_ownership_negative.objc3",
            expected_snippets=[
                "@property ownership modifier 'strong' requires an Objective-C object property"
            ],
            expected_codes=["O3S206"],
        ),
        NegativeDiagnosticExpectation(
            key="negative-duplicate-getter",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "accessor_duplicate_getter_negative.objc3",
            expected_snippets=[
                "duplicate effective getter selector 'value' for properties 'token' and 'alias'"
            ],
            expected_codes=["O3S206"],
        ),
        NegativeDiagnosticExpectation(
            key="negative-duplicate-setter",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "accessor_duplicate_setter_negative.objc3",
            expected_snippets=[
                "duplicate effective setter selector 'setValue:' for properties 'token' and 'alias'"
            ],
            expected_codes=["O3S206"],
        ),
        NegativeDiagnosticExpectation(
            key="negative-readonly-setter",
            fixture=ROOT
            / "tests"
            / "tooling"
            / "fixtures"
            / "native"
            / "property_readonly_setter_negative.objc3",
            expected_snippets=[
                "readonly property 'value' in interface 'Widget' must not declare a setter modifier"
            ],
            expected_codes=["O3S206"],
        ),
    ]
