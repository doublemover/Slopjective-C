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
    / "public_runtime_reflection_api_contract.json"
)
HEADER_PATH = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "runtime"
    / "public"
    / "objc3_runtime_reflection.h"
)
UMBRELLA_PATH = HEADER_PATH.with_name("objc3_runtime_api.h")
IMPLEMENTATION_PATH = HEADER_PATH.with_suffix(".cpp")
CMAKE_PATH = ROOT / "native" / "objc3c" / "src" / "runtime" / "CMakeLists.txt"
PROBE_PATH = ROOT / "tests" / "tooling" / "runtime" / "public_runtime_reflection_api_probe.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _contract() -> dict[str, object]:
    return json.loads(_read(CONTRACT_PATH))


def test_public_runtime_reflection_contract_fixture_matches_sources() -> None:
    contract = _contract()
    header = _read(HEADER_PATH)
    umbrella = _read(UMBRELLA_PATH)
    implementation = _read(IMPLEMENTATION_PATH)
    cmake = _read(CMAKE_PATH)
    probe = _read(PROBE_PATH)

    assert contract["contract_id"] == "objc3c.runtime.public.reflection.api.v1"
    assert contract["issue"] == 8174
    assert contract["support_claim"] == "objc3c.behavior.runtime.public-reflection-api"
    assert '#include "runtime/public/objc3_runtime_reflection.h"' in umbrella
    assert "public/objc3_runtime_reflection.cpp" in cmake
    assert "public/objc3_runtime_reflection.h" in cmake
    assert "OBJC3_RUNTIME_REFLECTION_ABI_VERSION" in header

    for symbol in contract["entrypoints"]:
        assert f"{symbol}(" in header
        assert f"{symbol}(" in implementation
        assert f"{symbol}(" in probe

    for snapshot_type in contract["snapshot_types"]:
        assert f"typedef struct {snapshot_type}" in header

    for status_code in contract["status_codes"]:
        assert status_code in header
        assert status_code in implementation

    for forbidden_symbol in contract["forbidden_public_symbols"]:
        assert forbidden_symbol not in header


def test_public_runtime_reflection_uses_realized_state_and_fail_closed_statuses() -> None:
    implementation = _read(IMPLEMENTATION_PATH)

    assert "ProcessRuntimeState()" in implementation
    assert "std::lock_guard<std::mutex> lock(state.mutex)" in implementation
    assert "state.realized_class_nodes" in implementation
    assert "node.attached_category_records" in implementation
    assert "FindRuntimePropertyAccessorByNameUnlocked" in implementation
    assert "ProtocolExistsByNameUnlocked" in implementation
    assert "QueryRealizedClassProtocolConformanceUnlocked" in implementation
    assert "FindSelectorSlotByCanonicalSpellingUnlocked" in implementation
    assert "AppendDynamicSelectorSlotUnlocked" not in implementation
    assert "LookupSelectorUnlocked(selector)" not in implementation
    assert "PublicRuntimeReflectionSurfaceRecord" in implementation
    assert "snapshot.issue_ref = 8174" in implementation
    assert "snapshot.creates_dynamic_runtime_state = 0" in implementation
    assert "snapshot.exposes_private_testing_snapshot = 0" in implementation

    assert "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT" in implementation
    assert "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY" in implementation
    assert "OBJC3_RUNTIME_REFLECTION_STATUS_MALFORMED_METADATA" in implementation
    assert "kUnsupportedMetadataPolicy" in implementation
    assert "owner_identity;" not in _read(HEADER_PATH)


def test_public_runtime_reflection_probe_uses_public_surface() -> None:
    probe = _read(PROBE_PATH)

    assert '#include "runtime/public/objc3_runtime_api.h"' in probe
    assert "objc3_runtime_copy_reflection_class" in probe
    assert "objc3_runtime_copy_reflection_property" in probe
    assert "objc3_runtime_copy_reflection_method" in probe
    assert "objc3_runtime_copy_reflection_protocol_conformance" in probe
    assert "objc3_runtime_copy_reflection_selector" in probe
    assert "objc3_runtime_copy_reflection_surface_by_kind" in probe
    assert "surface.creates_dynamic_runtime_state" in probe
    assert "surface.exposes_private_testing_snapshot" in probe
    assert "value_property.property_behavior_name" in probe
    assert "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_OUTPUT" in probe
    for private_snapshot_entrypoint in (
        "objc3_runtime_copy_realized_class_entry_for_testing",
        "objc3_runtime_copy_property_entry_for_testing",
        "objc3_runtime_copy_protocol_conformance_query_for_testing",
        "objc3_runtime_copy_selector_lookup_entry_for_testing",
    ):
        assert private_snapshot_entrypoint not in probe
    assert "OBJC3_RUNTIME_REFLECTION_STATUS_INVALID_QUERY" in _read(HEADER_PATH)


def test_public_runtime_reflection_header_compiles_from_c_when_available(
    tmp_path: Path,
) -> None:
    c_compiler = find_compiler(["cc", "clang", "gcc"])
    if c_compiler is None:
        pytest.skip("no C compiler available in PATH")

    c_source = tmp_path / "runtime_reflection_header_smoke.c"
    c_object = tmp_path / "runtime_reflection_header_smoke.o"
    c_source.write_text(
        "\n".join(
            [
                '#include "runtime/public/objc3_runtime_api.h"',
                "static int smoke(void) {",
                "  objc3_runtime_reflection_state_snapshot state = {0};",
                "  objc3_runtime_reflection_surface_snapshot surface = {0};",
                "  objc3_runtime_reflection_class_snapshot cls = {0};",
                "  objc3_runtime_reflection_property_snapshot property = {0};",
                "  objc3_runtime_reflection_method_snapshot method = {0};",
                "  objc3_runtime_reflection_protocol_snapshot protocol = {0};",
                "  objc3_runtime_reflection_protocol_conformance_snapshot conf = {0};",
                "  objc3_runtime_reflection_category_snapshot category = {0};",
                "  objc3_runtime_reflection_selector_snapshot selector = {0};",
                "  if (OBJC3_RUNTIME_REFLECTION_ABI_VERSION == 0u) return 1;",
                "  if (objc3_runtime_reflection_api_abi_version() == 0u) return 2;",
                "  if (objc3_runtime_reflection_surface_count() == 0u) return 3;",
                "  (void)objc3_runtime_copy_reflection_surface(0u, &surface);",
                "  (void)objc3_runtime_copy_reflection_surface_by_kind(",
                "      OBJC3_RUNTIME_REFLECTION_SURFACE_PROPERTY, &surface);",
                "  (void)objc3_runtime_copy_reflection_state(&state);",
                '  (void)objc3_runtime_copy_reflection_class("Widget", &cls);',
                '  (void)objc3_runtime_copy_reflection_property("Widget", "count", &property);',
                "  (void)objc3_runtime_copy_reflection_method(",
                '      "Widget", "count", OBJC3_RUNTIME_REFLECTION_METHOD_FAMILY_INSTANCE, &method);',
                '  (void)objc3_runtime_copy_reflection_protocol("Tracer", &protocol);',
                '  (void)objc3_runtime_copy_reflection_protocol_conformance("Widget", "Tracer", &conf);',
                '  (void)objc3_runtime_copy_reflection_category("Widget", "Tracing", &category);',
                '  (void)objc3_runtime_copy_reflection_selector("count", &selector);',
                "  return state.status + cls.status + property.status + method.status +",
                "         protocol.status + conf.status + category.status + selector.status;",
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
