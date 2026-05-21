from __future__ import annotations

import json
import subprocess
from pathlib import Path

import pytest

from c_api_smoke_support import SRC_ROOT, find_compiler


ROOT = Path(__file__).resolve().parents[2]
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "language_semantics_runtime_api_contract.json"
)
HEADER_PATH = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "runtime"
    / "public"
    / "objc3_runtime_language_semantics.h"
)
UMBRELLA_PATH = HEADER_PATH.with_name("objc3_runtime_api.h")
IMPLEMENTATION_PATH = HEADER_PATH.with_suffix(".cpp")
PROTOCOL_CONFORMANCE_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "runtime" / "classes" / "protocol_conformance.h"
)
CMAKE_PATH = ROOT / "native" / "objc3c" / "src" / "runtime" / "CMakeLists.txt"
PROBE_PATH = ROOT / "tests" / "tooling" / "runtime" / "language_semantics_runtime_api_probe.cpp"
SEMA_CONTRACT_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "sema"
    / "model"
    / "language_semantics_concrete_contracts.h"
)
LOWERING_HANDOFF_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "lower"
    / "model"
    / "language_semantics_lowering_handoff.h"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _contract() -> dict[str, object]:
    return json.loads(_read(CONTRACT_PATH))


def test_language_semantics_runtime_api_contract_matches_sources() -> None:
    contract = _contract()
    header = _read(HEADER_PATH)
    umbrella = _read(UMBRELLA_PATH)
    implementation = _read(IMPLEMENTATION_PATH)
    cmake = _read(CMAKE_PATH)
    probe = _read(PROBE_PATH)

    assert contract["contract_id"] == "objc3c.runtime.language-semantics.api.v1"
    assert set(contract["issues"]) == {8160, 8164, 8166, 8167}
    assert '#include "runtime/public/objc3_runtime_language_semantics.h"' in umbrella
    assert "public/objc3_runtime_language_semantics.cpp" in cmake
    assert "public/objc3_runtime_language_semantics.h" in cmake
    assert "OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION" in header
    assert "typedef struct objc3_runtime_language_semantics_surface_snapshot" in header
    assert "associated_type_support" in header
    assert "dynamic_existential_dispatch_support" in header
    assert "unsupported_associated_type_diagnostic" in header
    assert "unsupported_dynamic_dispatch_diagnostic" in header

    for symbol in contract["entrypoints"]:
        assert f"{symbol}(" in header
        assert f"{symbol}(" in implementation
        assert f"{symbol}(" in probe

    for status_code in contract["status_codes"]:
        assert status_code in header
        assert status_code in implementation

    for surface_kind in contract["surface_kinds"]:
        assert surface_kind in header
        assert surface_kind in probe

    for forbidden_symbol in contract["forbidden_public_symbols"]:
        assert forbidden_symbol not in header


def test_language_semantics_runtime_api_rows_bind_real_runtime_anchors() -> None:
    contract = _contract()
    implementation = _read(IMPLEMENTATION_PATH)
    runtime_evidence = implementation + "\n" + _read(PROTOCOL_CONFORMANCE_HEADER)
    probe = _read(PROBE_PATH)

    assert "kRuntimeLanguageSemanticsSurfaces" in implementation
    assert "objc3_runtime_language_semantics_surface_count() == 4u" in probe
    for row in contract["surface_rows"]:
        assert str(row["issue"]) in implementation
        assert row["support_claim"] in implementation
        assert row["semantic_surface"] in implementation
        assert row["metadata_key"] in runtime_evidence
        assert row["runtime_anchor"] in implementation
        assert row["positive_fixture"] in implementation
        assert row["negative_fixture"] in implementation
        assert row["diagnostic_code"] in implementation
        for optional_runtime_field in (
            "witness_metadata_key",
            "conformance_metadata_key",
            "unsupported_associated_type_diagnostic",
            "unsupported_dynamic_dispatch_diagnostic",
        ):
            if optional_runtime_field in row:
                assert row[optional_runtime_field] in runtime_evidence

    assert "EmittedKeyPathDescriptor::generic_metadata_replay_key" in implementation
    assert "BuildRuntimeProtocolExistentialWitnessMetadata" in implementation
    assert "kObjc3ProtocolExistentialWitnessMetadataKey" in implementation
    assert "ProtocolConformanceMatch" in implementation
    assert "QueryRealizedClassProtocolConformanceUnlocked" in implementation
    assert "RuntimeResultFailClosedOwnershipModel" in implementation
    assert "objc3_runtime_spawn_task_i32+objc3_runtime_executor_hop_i32+" in implementation


def test_language_semantics_runtime_api_is_bound_to_km_handoff_contracts() -> None:
    sema_contract = _read(SEMA_CONTRACT_HEADER)
    lowering_handoff = _read(LOWERING_HANDOFF_HEADER)

    for token in (
        "generic-runtime-metadata-api",
        "protocol-witness-runtime-api",
        "ownership-runtime-api",
        "concurrency-runtime-api-surface",
        "tests/tooling/runtime/language_semantics_runtime_api_probe.cpp",
    ):
        assert token in sema_contract

    for token in (
        "runtime-api-facing-language-semantics",
        "public-runtime-language-semantics-snapshot",
        "objc3_runtime_copy_language_semantics_surface",
        "runtime_api_snapshots_publish_handoff",
    ):
        assert token in lowering_handoff


def test_language_semantics_runtime_api_header_compiles_from_c_when_available(
    tmp_path: Path,
) -> None:
    c_compiler = find_compiler(["cc", "clang", "gcc"])
    if c_compiler is None:
        pytest.skip("no C compiler available in PATH")

    c_source = tmp_path / "language_semantics_runtime_api_smoke.c"
    c_object = tmp_path / "language_semantics_runtime_api_smoke.o"
    c_source.write_text(
        "\n".join(
            [
                '#include "runtime/public/objc3_runtime_api.h"',
                "static int smoke(void) {",
                "  objc3_runtime_language_semantics_surface_snapshot row = {0};",
                "  if (OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION == 0u) return 1;",
                "  if (objc3_runtime_language_semantics_api_abi_version() == 0u) return 2;",
                "  if (objc3_runtime_language_semantics_surface_count() == 0u) return 3;",
                "  return objc3_runtime_copy_language_semantics_surface_by_kind(",
                "      OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY, &row);",
                "}",
                "",
            ]
        ),
        encoding="utf-8",
    )

    result = subprocess.run(
        [
            c_compiler,
            "-std=c11",
            "-c",
            str(c_source),
            "-I",
            str(SRC_ROOT),
            "-o",
            str(c_object),
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0:
        diagnostics = (result.stderr + "\n" + result.stdout).strip()
        if not diagnostics:
            pytest.skip("C compiler invocation is unavailable in this environment")
        pytest.fail(diagnostics)
    assert c_object.exists()
