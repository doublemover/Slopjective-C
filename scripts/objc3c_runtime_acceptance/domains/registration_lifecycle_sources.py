"""Fixture and probe source helpers for registration lifecycle acceptance."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from time import perf_counter
from typing import Any

from objc3c_runtime_acceptance.fixture_compilation import compile_fixture
from objc3c_runtime_acceptance.paths import ROOT
from objc3c_runtime_acceptance.probes import compile_probe_with_args
from objc3c_runtime_acceptance.probes import parse_json_output
from objc3c_runtime_acceptance.probes import run_probe

from ..runtime_contract_registration import (
    INSTALLATION_LIFECYCLE_FIXTURE,
    INSTALLATION_LIFECYCLE_PROBE,
)


@dataclass(frozen=True)
class RegistrationLifecycleFixtureBuild:
    object_path: Path
    registration_descriptor: dict[str, Any]
    fixture_compile_ms: int


@dataclass(frozen=True)
class RegistrationLifecycleProbeLink:
    executable_path: Path
    probe_link_ms: int


@dataclass(frozen=True)
class RegistrationLifecycleProbeRun:
    payload: dict[str, Any]
    probe_run_ms: int


def installation_lifecycle_case_dir(run_dir: Path) -> Path:
    return run_dir / "installation-lifecycle"


def compile_registration_lifecycle_fixture(
    case_dir: Path,
) -> RegistrationLifecycleFixtureBuild:
    fixture = ROOT / Path(INSTALLATION_LIFECYCLE_FIXTURE)
    compile_started = perf_counter()
    object_path = compile_fixture(fixture, case_dir / "compile")
    fixture_compile_ms = int((perf_counter() - compile_started) * 1000)
    descriptor_path = (
        case_dir / "compile" / "module.runtime-registration-descriptor.json"
    )
    registration_descriptor = json.loads(descriptor_path.read_text(encoding="utf-8"))
    return RegistrationLifecycleFixtureBuild(
        object_path=object_path,
        registration_descriptor=registration_descriptor,
        fixture_compile_ms=fixture_compile_ms,
    )


def write_probe_fixture_config(
    case_dir: Path,
    registration_descriptor: dict[str, Any],
) -> Path:
    probe_fixture_config = (
        case_dir / "runtime_installation_loader_lifecycle_probe_fixture_config.h"
    )
    module_name = registration_descriptor[
        "registration_descriptor_identifier"
    ].removesuffix("_registration_descriptor")
    probe_fixture_config.write_text(
        "\n".join(
            [
                "#pragma once",
                f"#define OBJC3_RUNTIME_FIXTURE_MODULE_NAME {json.dumps(module_name)}",
                f"#define OBJC3_RUNTIME_FIXTURE_TRANSLATION_UNIT_IDENTITY_KEY {json.dumps(registration_descriptor['translation_unit_identity_key'])}",
                f"#define OBJC3_RUNTIME_FIXTURE_REGISTRATION_ORDER_ORDINAL {registration_descriptor['translation_unit_registration_order_ordinal']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CLASS_DESCRIPTOR_COUNT {registration_descriptor['class_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROTOCOL_DESCRIPTOR_COUNT {registration_descriptor['protocol_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_CATEGORY_DESCRIPTOR_COUNT {registration_descriptor['category_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_PROPERTY_DESCRIPTOR_COUNT {registration_descriptor['property_descriptor_count']}ULL",
                f"#define OBJC3_RUNTIME_FIXTURE_IVAR_DESCRIPTOR_COUNT {registration_descriptor['ivar_descriptor_count']}ULL",
                "",
            ]
        ),
        encoding="utf-8",
    )
    return probe_fixture_config


def link_registration_lifecycle_probe(
    clangxx: str,
    case_dir: Path,
    object_path: Path,
    probe_fixture_config: Path,
) -> RegistrationLifecycleProbeLink:
    probe = ROOT / Path(INSTALLATION_LIFECYCLE_PROBE)
    executable_path = case_dir / "runtime_installation_loader_lifecycle_probe.exe"
    probe_link_started = perf_counter()
    compile_probe_with_args(
        clangxx,
        probe,
        executable_path,
        [object_path],
        [
            "-include",
            str(probe_fixture_config),
        ],
    )
    probe_link_ms = int((perf_counter() - probe_link_started) * 1000)
    return RegistrationLifecycleProbeLink(
        executable_path=executable_path,
        probe_link_ms=probe_link_ms,
    )


def run_registration_lifecycle_probe(
    executable_path: Path,
) -> RegistrationLifecycleProbeRun:
    probe_run_started = perf_counter()
    payload = parse_json_output(
        run_probe(executable_path), "runtime installation loader lifecycle probe"
    )
    probe_run_ms = int((perf_counter() - probe_run_started) * 1000)
    return RegistrationLifecycleProbeRun(payload=payload, probe_run_ms=probe_run_ms)


__all__ = [
    "RegistrationLifecycleFixtureBuild",
    "RegistrationLifecycleProbeLink",
    "RegistrationLifecycleProbeRun",
    "compile_registration_lifecycle_fixture",
    "installation_lifecycle_case_dir",
    "link_registration_lifecycle_probe",
    "run_registration_lifecycle_probe",
    "write_probe_fixture_config",
]
