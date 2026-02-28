from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m209_lowering_runtime_pgo_hooks_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M209 lowering/runtime profile-guided optimization hooks",
        "tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/",
        "tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/",
        "tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m209/lowering-runtime-pgo-hooks/module.manifest.json",
        "tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m209/lowering-runtime-pgo-hooks/pgo-hook-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        '!0 = !{i32 <language_version>, !"compatibility_mode", i1 <migration_assist>, i64 <legacy_yes>, i64 <legacy_no>, i64 <legacy_null>, i64 <legacy_total>}',
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "Objc3IRFrontendMetadata ir_frontend_metadata;",
        "ir_frontend_metadata.language_version = options.language_version;",
        "ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);",
        "ir_frontend_metadata.migration_assist = options.migration_assist;",
        "ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;",
        "ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;",
        "ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;",
        'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";',
        'out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)',
        'out << "!objc3.frontend = !{!0}\\n";',
        'out << "!0 = !{i32 " << static_cast<unsigned>(frontend_metadata_.language_version) << ", !\\""',
        'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "invalid lowering contract runtime_dispatch_symbol",
        'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        "python -m pytest tests/tooling/test_objc3c_m209_lowering_pgo_contract.py -q",
    ):
        assert text in fragment


def test_m209_lowering_runtime_pgo_source_anchors_align_to_real_surfaces() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

    mapped_source_anchors = (
        ("Objc3IRFrontendMetadata ir_frontend_metadata;", artifacts_source, "Objc3IRFrontendMetadata ir_frontend_metadata;"),
        (
            "ir_frontend_metadata.language_version = options.language_version;",
            artifacts_source,
            "ir_frontend_metadata.language_version = options.language_version;",
        ),
        (
            "ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);",
            artifacts_source,
            "ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);",
        ),
        (
            "ir_frontend_metadata.migration_assist = options.migration_assist;",
            artifacts_source,
            "ir_frontend_metadata.migration_assist = options.migration_assist;",
        ),
        (
            "ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;",
            artifacts_source,
            "ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;",
        ),
        (
            "ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;",
            artifacts_source,
            "ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;",
        ),
        (
            "ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;",
            artifacts_source,
            "ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;",
        ),
        ("Objc3LoweringIRBoundaryReplayKey(...)", lowering_contract_source, "Objc3LoweringIRBoundaryReplayKey("),
        (
            "invalid lowering contract runtime_dispatch_symbol",
            lowering_contract_source,
            "invalid lowering contract runtime_dispatch_symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): ",
        ),
        (
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
            lowering_contract_source,
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        ),
        (
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
        (
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
            artifacts_source,
            '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        ),
        (
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
            artifacts_source,
            '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        ),
    )
    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text

    assert 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";' in (
        emitter_source
    )
    assert 'out << "; frontend_profile = language_version=" << static_cast<unsigned>(frontend_metadata_.language_version)' in (
        emitter_source
    )
    assert 'out << "!objc3.frontend = !{!0}\\n";' in emitter_source
    assert 'out << "!0 = !{i32 " << static_cast<unsigned>(frontend_metadata_.language_version) << ", !\\""' in (
        emitter_source
    )
    assert 'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";' in emitter_source
