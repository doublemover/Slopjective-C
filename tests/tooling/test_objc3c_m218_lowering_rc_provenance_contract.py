from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m218_lowering_rc_provenance_profile_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M218 lowering/runtime RC provenance profile",
        "tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/",
        "tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.manifest.json",
        "tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt",
        "tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/attestation-markers.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "invalid lowering contract runtime_dispatch_symbol",
        "runtime_dispatch_symbol=",
        "selector_global_ordering=lexicographic",
        "rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.",
        "replay and attestation marker reports stay stable across reruns (no added/removed markers).",
        "closure remains open if any required packet artifact, ABI/IR anchor, replay marker, or attestation marker is missing.",
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.ll > tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt',
        'rg -n "\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m218/lowering-runtime-rc-provenance/module.manifest.json >> tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/replay-markers.txt',
        'rg -n "Objc3LoweringIRBoundaryReplayKey|invalid lowering contract runtime_dispatch_symbol|runtime_dispatch_symbol=|selector_global_ordering=lexicographic" native/objc3c/src/lower/objc3_lowering_contract.cpp > tmp/reports/objc3c-native/m218/lowering-runtime-rc-provenance/attestation-markers.txt',
        "python -m pytest tests/tooling/test_objc3c_m218_lowering_rc_provenance_contract.py -q",
    ):
        assert text in fragment


def test_m218_lowering_rc_provenance_markers_align_to_source_surfaces() -> None:
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

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

    assert "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " in (
        lowering_contract_source
    )
    assert "Objc3LoweringIRBoundaryReplayKey" in lowering_contract_source
    assert 'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +' in lowering_contract_source
    assert '";selector_global_ordering=" + boundary.selector_global_ordering;' in lowering_contract_source
