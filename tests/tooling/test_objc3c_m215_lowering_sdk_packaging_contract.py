from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
DIAGNOSTICS_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
MANIFEST_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_manifest_artifacts.cpp"
OBJC3_PATH_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_objc3_path.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m215_lowering_runtime_sdk_packaging_profile_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M215 lowering/runtime SDK packaging profile",
        "tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/",
        "tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/",
        "tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.diagnostics.json",
        "tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.obj",
        "tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.object-backend.txt",
        "tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/ide-consumable-artifact-markers.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        '"schema_version": "1.0.0"',
        '"diagnostics": [',
        '"severity":',
        '"line":',
        '"column":',
        '"code":',
        '"message":',
        '"raw":',
        '"module":',
        '"frontend":',
        '"lowering":',
        '"globals":',
        '"functions":',
        '"runtime_dispatch_symbol":',
        '"runtime_dispatch_arg_slots":',
        "clang",
        "llvm-direct",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "invalid lowering contract runtime_dispatch_symbol",
        'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());',
        'WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest_json);',
        'const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");',
        "byte-identical `module.ll`, `module.manifest.json`, `module.diagnostics.json`, and `module.object-backend.txt`.",
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.ll tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json > tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/abi-ir-anchors.txt',
        'rg -n "\\"schema_version\\":|\\"diagnostics\\":|\\"severity\\":|\\"line\\":|\\"column\\":|\\"code\\":|\\"message\\":|\\"raw\\":|\\"module\\":|\\"frontend\\":|\\"lowering\\":|\\"globals\\":|\\"functions\\":|\\"runtime_dispatch_symbol\\":|\\"runtime_dispatch_arg_slots\\":|clang|llvm-direct" tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.diagnostics.json tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.manifest.json tmp/artifacts/compilation/objc3c-native/m215/lowering-runtime-sdk-packaging/module.object-backend.txt > tmp/reports/objc3c-native/m215/lowering-runtime-sdk-packaging/ide-consumable-artifact-markers.txt',
        "python -m pytest tests/tooling/test_objc3c_m215_lowering_sdk_packaging_contract.py -q",
    ):
        assert text in fragment


def test_m215_lowering_runtime_sdk_packaging_markers_align_to_source_surfaces() -> None:
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    diagnostics_source = _read(DIAGNOSTICS_ARTIFACTS_SOURCE)
    manifest_source = _read(MANIFEST_ARTIFACTS_SOURCE)
    objc3_path_source = _read(OBJC3_PATH_SOURCE)

    assert 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";' in (
        emitter_source
    )
    assert 'out << "; frontend_profile = language_version="' in emitter_source
    assert 'out << "!objc3.frontend = !{!0}\\n";' in emitter_source
    assert 'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";' in emitter_source

    assert 'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol' in (
        artifacts_source
    )
    assert '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args' in artifacts_source
    assert '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";' in artifacts_source
    assert 'manifest << "  \\"module\\": \\"" << program.module_name << "\\",\\n";' in artifacts_source
    assert 'manifest << "  \\"frontend\\": {\\n";' in artifacts_source
    assert 'manifest << "  \\"globals\\": [\\n";' in artifacts_source
    assert 'manifest << "  \\"functions\\": [\\n";' in artifacts_source

    assert "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " in (
        lowering_contract_source
    )
    assert "Objc3LoweringIRBoundaryReplayKey" in lowering_contract_source
    assert 'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +' in lowering_contract_source

    assert 'WriteText(out_dir / (emit_prefix + ".diagnostics.json"), out.str());' in diagnostics_source
    assert 'out << "  \\"schema_version\\": \\"1.0.0\\",\\n";' in diagnostics_source
    assert '\\"diagnostics\\": [' in diagnostics_source
    assert '\\"severity\\":\\"' in diagnostics_source
    assert '\\"line\\":' in diagnostics_source
    assert '\\"column\\":' in diagnostics_source
    assert '\\"code\\":\\"' in diagnostics_source
    assert '\\"message\\":\\"' in diagnostics_source
    assert '\\"raw\\":\\"' in diagnostics_source

    assert 'WriteText(out_dir / (emit_prefix + ".manifest.json"), manifest_json);' in manifest_source

    assert 'const fs::path backend_out = cli_options.out_dir / (cli_options.emit_prefix + ".object-backend.txt");' in (
        objc3_path_source
    )
    assert "clang\\n" in objc3_path_source
    assert "llvm-direct\\n" in objc3_path_source
    assert "WriteText(backend_out, backend_text);" in objc3_path_source
