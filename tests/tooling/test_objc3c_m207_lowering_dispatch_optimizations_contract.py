from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m207_lowering_runtime_dispatch_specific_optimization_passes_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M207 lowering/runtime dispatch-specific optimization passes",
        "tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/",
        "tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/",
        "tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.diagnostics.json",
        "tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/dispatch-optimization-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "runtime_dispatch_call_emitted_ = false;",
        "runtime_dispatch_call_emitted_ = true;",
        "receiver_is_compile_time_zero",
        "receiver_is_compile_time_nonzero",
        "msg_nil_",
        "msg_dispatch_",
        "phi i32 [0, %",
        "FunctionMayHaveGlobalSideEffects",
        "call_may_have_global_side_effects",
        "global_proofs_invalidated",
        "runtime_dispatch_symbol",
        "runtime_dispatch_arg_slots",
        "selector_global_ordering",
        "if (runtime_dispatch_call_emitted_) {",
        "lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);",
        "lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);",
        "if (lowered.receiver_is_compile_time_zero) {",
        "if (lowered.receiver_is_compile_time_nonzero) {",
        'const std::string nil_label = NewLabel(ctx, "msg_nil_");',
        'const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");',
        'ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +',
        "const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);",
        "if (call_may_have_global_side_effects) {",
        "ctx.global_proofs_invalidated = true;",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "invalid lowering contract runtime_dispatch_symbol",
        'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '<< "\\",\\"runtime_dispatch_arg_slots\\":" << options.lowering.max_message_send_args',
        '<< ",\\"selector_global_ordering\\":\\"lexicographic\\"},\\n";',
        "byte-identical `module.ll`, `module.manifest.json`, and `module.diagnostics.json`.",
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.ll tmp/artifacts/compilation/objc3c-native/m207/lowering-runtime-dispatch-optimizations/module.manifest.json > tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/abi-ir-anchors.txt',
        'rg -n "runtime_dispatch_call_emitted_|receiver_is_compile_time_zero|receiver_is_compile_time_nonzero|msg_nil_|msg_dispatch_|phi i32 \\[0, %|FunctionMayHaveGlobalSideEffects|call_may_have_global_side_effects|global_proofs_invalidated|Objc3LoweringIRBoundaryReplayKey\\(|runtime_dispatch_symbol|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/ir/objc3_ir_emitter.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp > tmp/reports/objc3c-native/m207/lowering-runtime-dispatch-optimizations/dispatch-optimization-source-anchors.txt',
        "python -m pytest tests/tooling/test_objc3c_m207_lowering_dispatch_optimizations_contract.py -q",
    ):
        assert text in fragment


def test_m207_lowering_runtime_dispatch_optimization_source_anchors_align_to_real_surfaces() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

    mapped_source_anchors = (
        ("if (runtime_dispatch_call_emitted_) {", emitter_source, "if (runtime_dispatch_call_emitted_) {"),
        (
            "lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);",
            emitter_source,
            "lowered.receiver_is_compile_time_zero = IsCompileTimeNilReceiverExprInContext(expr->receiver.get(), ctx);",
        ),
        (
            "lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);",
            emitter_source,
            "lowered.receiver_is_compile_time_nonzero = IsCompileTimeKnownNonNilExprInContext(expr->receiver.get(), ctx);",
        ),
        ("if (lowered.receiver_is_compile_time_zero) {", emitter_source, "if (lowered.receiver_is_compile_time_zero) {"),
        (
            "if (lowered.receiver_is_compile_time_nonzero) {",
            emitter_source,
            "if (lowered.receiver_is_compile_time_nonzero) {",
        ),
        ('const std::string nil_label = NewLabel(ctx, "msg_nil_");', emitter_source, 'const std::string nil_label = NewLabel(ctx, "msg_nil_");'),
        (
            'const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");',
            emitter_source,
            'const std::string dispatch_label = NewLabel(ctx, "msg_dispatch_");',
        ),
        (
            'ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +',
            emitter_source,
            'ctx.code_lines.push_back("  " + out + " = phi i32 [0, %" + nil_label + "], [" + dispatch_value + ", %" +',
        ),
        (
            "const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);",
            emitter_source,
            "const bool call_may_have_global_side_effects = FunctionMayHaveGlobalSideEffects(expr->ident);",
        ),
        ("if (call_may_have_global_side_effects) {", emitter_source, "if (call_may_have_global_side_effects) {"),
        ("ctx.global_proofs_invalidated = true;", emitter_source, "ctx.global_proofs_invalidated = true;"),
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

    assert "runtime_dispatch_call_emitted_ = false;" in emitter_source
    assert "runtime_dispatch_call_emitted_ = true;" in emitter_source
    assert 'out << "; lowering_ir_boundary = " << Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_) << "\\n";' in (
        emitter_source
    )
    assert 'out << "; frontend_profile = language_version="' in emitter_source
    assert 'out << "!objc3.frontend = !{!0}\\n";' in emitter_source
    assert 'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";' in emitter_source
