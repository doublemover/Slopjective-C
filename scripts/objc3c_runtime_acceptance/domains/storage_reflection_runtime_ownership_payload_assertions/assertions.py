"""Storage ownership reflection payload and compile-surface assertions."""

from __future__ import annotations

from objc3c_runtime_acceptance.domains.storage_reflection_runtime_ownership_sources import (
    StorageOwnershipReflectionArtifacts,
)
from objc3c_runtime_acceptance.expectation_matching import expect

from .catalog import BOX_ACCESSOR_COUNT_MESSAGE
from .catalog import BOX_INSTANCE_SIZE_MESSAGE
from .catalog import BOX_REALIZED_MESSAGE
from .catalog import COMPILE_OWNERSHIP_LL_EXPECTATIONS
from .catalog import LIVE_IMPLEMENTATION_SURFACE_EXPECTATIONS
from .catalog import MANIFEST_IMPLEMENTATION_REQUIREMENTS_EXPECTATIONS
from .catalog import MANIFEST_IMPLEMENTATION_SURFACE_EXPECTATIONS
from .catalog import REGISTRATION_MANIFEST_EXPECTATIONS
from .data import StorageOwnershipReflectionFacts
from .predicates import box_instance_layout_satisfies
from .predicates import box_reflection_payload_found
from .predicates import box_runtime_accessor_count_satisfies
from .predicates import ll_text_contains_expectation
from .predicates import surface_field_matches


def assert_storage_ownership_runtime_payload(
    facts: StorageOwnershipReflectionFacts,
    artifacts: StorageOwnershipReflectionArtifacts,
) -> None:
    _assert_box_reflection_payload(facts)
    _assert_registration_manifest(artifacts)
    _assert_compile_ownership_surface(artifacts)
    _assert_manifest_implementation_surface(facts)
    _assert_live_implementation_surface(facts)


def _assert_box_reflection_payload(facts: StorageOwnershipReflectionFacts) -> None:
    expect(box_reflection_payload_found(facts), BOX_REALIZED_MESSAGE)
    expect(box_runtime_accessor_count_satisfies(facts), BOX_ACCESSOR_COUNT_MESSAGE)
    expect(box_instance_layout_satisfies(facts), BOX_INSTANCE_SIZE_MESSAGE)


def _assert_registration_manifest(
    artifacts: StorageOwnershipReflectionArtifacts,
) -> None:
    for expectation in REGISTRATION_MANIFEST_EXPECTATIONS:
        expect(
            surface_field_matches(artifacts.registration_manifest, expectation),
            expectation.message,
        )


def _assert_compile_ownership_surface(
    artifacts: StorageOwnershipReflectionArtifacts,
) -> None:
    for expectation in COMPILE_OWNERSHIP_LL_EXPECTATIONS:
        expect(
            ll_text_contains_expectation(artifacts.ll_text, expectation),
            expectation.message,
        )


def _assert_manifest_implementation_surface(
    facts: StorageOwnershipReflectionFacts,
) -> None:
    manifest_surface = facts.manifest_implementation_surface
    for expectation in MANIFEST_IMPLEMENTATION_SURFACE_EXPECTATIONS:
        expect(
            surface_field_matches(manifest_surface, expectation),
            expectation.message,
        )
    for expectation in MANIFEST_IMPLEMENTATION_REQUIREMENTS_EXPECTATIONS:
        expect(
            surface_field_matches(manifest_surface, expectation),
            expectation.message,
        )


def _assert_live_implementation_surface(
    facts: StorageOwnershipReflectionFacts,
) -> None:
    implementation_surface = facts.implementation_surface
    for expectation in LIVE_IMPLEMENTATION_SURFACE_EXPECTATIONS:
        expect(
            surface_field_matches(implementation_surface, expectation),
            expectation.message,
        )


__all__ = ["assert_storage_ownership_runtime_payload"]
