from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DIAGNOSTICS_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m212_lowering_runtime_code_action_profile_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M212 lowering/runtime code-action profile",
        "tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/",
        "tmp/reports/objc3c-native/m212/lowering-runtime-code-action/",
        "tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.diagnostics.json",
        "tmp/reports/objc3c-native/m212/lowering-runtime-code-action/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "@@ rewrite_scope:module",
        "runtime_dispatch_symbol=",
        "selector_global_ordering=lexicographic",
        '"source":',
        '"line":',
        '"column":',
        '"code":',
        '"message":',
        '"raw":',
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "invalid lowering contract runtime_dispatch_symbol",
        'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        'manifest << "  \\"source\\": \\"" << input_path.generic_string() << "\\",\\n";',
        'manifest << "    {\\"name\\":\\"" << program.globals[i].name << "\\",\\"value\\":" << resolved_global_values[i]',
        '<< ",\\"line\\":" << program.globals[i].line << ",\\"column\\":" << program.globals[i].column << "}";',
        '<< ",\\"line\\":" << fn.line << ",\\"column\\":" << fn.column << "}";',
        'out << "    {\\"severity\\":\\"" << EscapeJsonString(ToLower(key.severity)) << "\\",\\"line\\":" << line',
        '<< ",\\"column\\":" << column << ",\\"code\\":\\"" << EscapeJsonString(key.code) << "\\",\\"message\\":\\""',
        "byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.",
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.ll tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json > tmp/reports/objc3c-native/m212/lowering-runtime-code-action/abi-ir-anchors.txt',
        '@("@@ rewrite_scope:module") | Set-Content tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt; rg -n "runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp >> tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt; rg -n "\\"source\\":|\\"line\\":|\\"column\\":|\\"code\\":|\\"message\\":|\\"raw\\":" tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.manifest.json tmp/artifacts/compilation/objc3c-native/m212/lowering-runtime-code-action/module.diagnostics.json >> tmp/reports/objc3c-native/m212/lowering-runtime-code-action/rewrite-markers.txt',
        "python -m pytest tests/tooling/test_objc3c_m212_lowering_code_action_contract.py -q",
    ):
        assert text in fragment


def test_m212_lowering_runtime_code_action_markers_align_to_source_surfaces() -> None:
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    diagnostics_source = _read(DIAGNOSTICS_ARTIFACTS_SOURCE)

    assert 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";' in (
        emitter_source
    )
    assert 'out << "; frontend_profile = language_version="' in emitter_source
    assert 'out << "!objc3.frontend = !{!0}\\n";' in emitter_source
    assert 'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";' in emitter_source

    assert 'manifest << "  \\"source\\": \\"" << input_path.generic_string() << "\\",\\n";' in artifacts_source
    assert 'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol' in (
        artifacts_source
    )
    assert '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args' in artifacts_source
    assert '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";' in artifacts_source
    assert 'manifest << "    {\\"name\\":\\"" << program.globals[i].name << "\\",\\"value\\":" << resolved_global_values[i]' in (
        artifacts_source
    )
    assert '<< ",\\"line\\":" << program.globals[i].line << ",\\"column\\":" << program.globals[i].column << "}";' in (
        artifacts_source
    )
    assert '<< ",\\"line\\":" << fn.line << ",\\"column\\":" << fn.column << "}";' in artifacts_source

    assert "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " in (
        lowering_contract_source
    )
    assert 'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +' in lowering_contract_source

    assert 'out << "  \\"schema_version\\": \\"1.0.0\\",\\n";' in diagnostics_source
    assert 'out << "  \\"diagnostics\\": [\\n";' in diagnostics_source
    assert 'out << "    {\\"severity\\":\\"" << EscapeJsonString(ToLower(key.severity)) << "\\",\\"line\\":" << line' in (
        diagnostics_source
    )
    assert '<< ",\\"column\\":" << column << ",\\"code\\":\\"" << EscapeJsonString(key.code) << "\\",\\"message\\":\\"' in (
        diagnostics_source
    )
    assert '<< EscapeJsonString(key.message) << "\\",\\"raw\\":\\"" << EscapeJsonString(diagnostics[i]) << "\\"}";' in (
        diagnostics_source
    )
