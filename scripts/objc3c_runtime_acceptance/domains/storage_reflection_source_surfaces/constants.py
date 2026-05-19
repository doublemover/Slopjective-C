"""Storage/reflection source-surface constants."""

from __future__ import annotations


COMPILE_ARTIFACT_SET = (
    "<emit-prefix>.obj",
    "<emit-prefix>.ll",
    "<emit-prefix>.manifest.json",
    "<emit-prefix>.runtime-registration-manifest.json",
    "<emit-prefix>.runtime-registration-descriptor.json",
)

REQUIRES_COUPLED_REGISTRATION_MANIFEST = True
REQUIRES_REAL_COMPILE_OUTPUT = True
REQUIRES_LINKED_RUNTIME_PROBE = True


__all__ = [
    "COMPILE_ARTIFACT_SET",
    "REQUIRES_COUPLED_REGISTRATION_MANIFEST",
    "REQUIRES_LINKED_RUNTIME_PROBE",
    "REQUIRES_REAL_COMPILE_OUTPUT",
]
