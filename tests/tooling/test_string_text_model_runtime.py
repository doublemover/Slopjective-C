from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "scripts"
if str(SCRIPTS) not in sys.path:
    sys.path.insert(0, str(SCRIPTS))

from objc3c_runtime_acceptance.paths import RUNTIME_LIB  # noqa: E402
from objc3c_runtime_acceptance.probes import compile_probe  # noqa: E402
from objc3c_runtime_acceptance.probes import parse_json_output  # noqa: E402
from objc3c_runtime_acceptance.probes import run_probe  # noqa: E402
from objc3c_tooling.probe_compile import find_clangxx  # noqa: E402


PROBE = ROOT / "tests" / "tooling" / "runtime" / "string_text_model_runtime_probe.cpp"


def test_string_text_model_runtime_owns_and_validates_utf8_storage(
    tmp_path: Path,
) -> None:
    assert RUNTIME_LIB.is_file(), (
        "native runtime library is required; run "
        "$env:OBJC3C_NATIVE_BUILD_PARALLELISM='4'; "
        "powershell -File scripts/build_objc3c_native.ps1 -ExecutionMode binaries-only"
    )
    exe_path = tmp_path / "string_text_model_runtime_probe.exe"
    compile_probe(find_clangxx(), PROBE, exe_path, [])
    payload = parse_json_output(run_probe(exe_path), "string text model runtime probe")

    assert payload["storage_create_call_count"] == 5
    assert payload["storage_query_call_count"] == 6
    assert payload["scalar_query_call_count"] == 8
    assert payload["scalar_iterator_call_count"] == 5
    assert payload["builder_create_call_count"] == 1
    assert payload["builder_finalize_call_count"] == 1
    assert payload["interpolation_call_count"] == 2
    assert payload["builder_append_call_count"] == 1
    assert payload["equality_call_count"] == 2
    assert payload["compare_call_count"] == 4
    assert payload["format_call_count"] == 1
    assert payload["text_record_count"] == 8
    assert payload["owned_storage_record_count"] == 5
    assert payload["owned_storage_byte_count"] == 40
    assert payload["literal_record_count"] == 1
    assert payload["builder_record_count"] == 1
    assert payload["scalar_iterator_record_count"] == 1
    assert payload["last_malformed_offset"] == 0
    assert payload["last_status"] == 30646
