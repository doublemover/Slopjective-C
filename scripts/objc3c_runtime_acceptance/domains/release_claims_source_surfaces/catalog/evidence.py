"""Evidence and path helpers for release-claims source surfaces."""

from __future__ import annotations

from typing import Any

from .models import PayloadField, PayloadFields, SurfacePaths


def payload_field(name: str, value: Any) -> PayloadField:
    return (name, value)


def payload_fields(*fields: PayloadField) -> PayloadFields:
    return tuple(fields)


def compile_artifact_set(*artifacts: str) -> PayloadField:
    return payload_field("compile_artifact_set", tuple(artifacts))


def source_contract_ids(*contract_ids: str) -> PayloadField:
    return payload_field("source_contract_ids", tuple(contract_ids))


def authoritative_code_paths(*paths: str) -> PayloadField:
    return payload_field("authoritative_code_paths", tuple(paths))


def authoritative_source_fields(*fields: str) -> PayloadField:
    return payload_field("authoritative_source_fields", tuple(fields))


def fixture_paths(*paths: Any) -> SurfacePaths:
    return tuple(paths)


def release_model(name: str, value: str) -> PayloadField:
    return payload_field(name, value)


def explicit_non_goals(*non_goals: str) -> PayloadField:
    return payload_field("explicit_non_goals", tuple(non_goals))


def required_truth(name: str) -> PayloadField:
    return payload_field(name, True)


__all__ = [
    "authoritative_code_paths",
    "authoritative_source_fields",
    "compile_artifact_set",
    "explicit_non_goals",
    "fixture_paths",
    "payload_field",
    "payload_fields",
    "release_model",
    "required_truth",
    "source_contract_ids",
]
