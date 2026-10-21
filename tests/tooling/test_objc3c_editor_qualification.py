from __future__ import annotations

import pytest

from scripts.objc3c_editor_tooling.lsp_session import probe_positions


ORDINARY_PROBE = {
    "name": "ordinary_parameter",
    "use_line": "return throws(value)",
    "use_name": "value",
    "declaration_line": "int async(int value)",
    "declaration_name": "value",
}


def test_lsp_probe_distinguishes_parameter_declaration_from_macro_argument() -> None:
    source = "int async(int value) { return throws(value); }\n"
    assert probe_positions(source, ORDINARY_PROBE) == (
        {"line": 0, "character": 37},
        {"line": 0, "character": 14},
    )


def test_private_probe_resolves_use_to_its_case_local_binding() -> None:
    source = (
        "case __objc3_case_some(let payload): {\n"
        "  selected = throws(payload);\n"
        "}\n"
    )
    probe = {
        "name": "optional_match_payload",
        "use_line": "selected = throws(payload);",
        "use_name": "payload",
        "declaration_line": "case __objc3_case_some(let payload):",
        "declaration_name": "payload",
    }
    assert probe_positions(source, probe) == (
        {"line": 1, "character": 20},
        {"line": 0, "character": 27},
    )


@pytest.mark.parametrize("separator", ["\n", " "])
def test_lsp_probe_rejects_ambiguous_use_anchor(separator: str) -> None:
    source = "int async(int value) { return throws(value); }" + separator + "return throws(value);"
    with pytest.raises(RuntimeError, match="needs one use_line anchor"):
        probe_positions(source, ORDINARY_PROBE)
