from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m208_lowering_runtime_whole_module_optimization_controls_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M208 lowering/runtime whole-module optimization controls",
        "tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/",
        "tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/",
        "tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m208/lowering-runtime-wmo-controls/module.manifest.json",
        "tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/wmo-control-source-anchors.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "max_message_send_args",
        "semantic_surface",
        "declared_functions",
        "resolved_function_symbols",
        "runtime_dispatch_arg_slots",
        "selector_global_ordering",
        "manifest_functions.reserve(program.functions.size())",
        "std::unordered_set<std::string> manifest_function_names",
        "if (manifest_function_names.insert(fn.name).second)",
        'manifest << "    \\"max_message_send_args\\":" << options.lowering.max_message_send_args << ",\\n";',
        'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
        '<< ",\\"declared_functions\\":" << manifest_functions.size()',
        '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
        "if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {",
        'error = "invalid lowering contract max_message_send_args: "',
        "boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;",
        "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;",
        "if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {",
        'lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");',
        'call << "  " << dispatch_value << " = call i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32 "',
        'rg -n "manifest_functions\\\\.reserve\\\\(program\\\\.functions\\\\.size\\\\(\\\\)\\\\)|manifest_function_names|max_message_send_args|semantic_surface|declared_functions|resolved_function_symbols|runtime_dispatch_arg_slots|selector_global_ordering" native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp native/objc3c/src/lower/objc3_lowering_contract.cpp native/objc3c/src/ir/objc3_ir_emitter.cpp > tmp/reports/objc3c-native/m208/lowering-runtime-wmo-controls/wmo-control-source-anchors.txt',
        "python -m pytest tests/tooling/test_objc3c_m208_lowering_wmo_contract.py -q",
    ):
        assert text in fragment


def test_m208_lowering_runtime_wmo_source_anchors_align_to_real_surfaces() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)

    mapped_source_anchors = (
        (
            "manifest_functions.reserve(program.functions.size())",
            artifacts_source,
            "manifest_functions.reserve(program.functions.size());",
        ),
        (
            "std::unordered_set<std::string> manifest_function_names",
            artifacts_source,
            "std::unordered_set<std::string> manifest_function_names;",
        ),
        (
            "if (manifest_function_names.insert(fn.name).second)",
            artifacts_source,
            "if (manifest_function_names.insert(fn.name).second) {",
        ),
        (
            'manifest << "    \\"max_message_send_args\\":" << options.lowering.max_message_send_args << ",\\n";',
            artifacts_source,
            'manifest << "    \\"max_message_send_args\\":" << options.lowering.max_message_send_args << ",\\n";',
        ),
        (
            'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
            artifacts_source,
            'manifest << "      \\"semantic_surface\\": {\\"declared_globals\\":" << program.globals.size()',
        ),
        (
            '<< ",\\"declared_functions\\":" << manifest_functions.size()',
            artifacts_source,
            '<< ",\\"declared_functions\\":" << manifest_functions.size()',
        ),
        (
            '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
            artifacts_source,
            '<< ",\\"resolved_function_symbols\\":" << pipeline_result.integration_surface.functions.size()',
        ),
        (
            "if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {",
            lowering_contract_source,
            "if (input.max_message_send_args > kObjc3RuntimeDispatchMaxArgs) {",
        ),
        (
            'error = "invalid lowering contract max_message_send_args: "',
            lowering_contract_source,
            'error = "invalid lowering contract max_message_send_args: " + std::to_string(input.max_message_send_args) +',
        ),
        (
            "boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;",
            lowering_contract_source,
            "boundary.runtime_dispatch_arg_slots = normalized.max_message_send_args;",
        ),
        (
            "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;",
            lowering_contract_source,
            "boundary.selector_global_ordering = kObjc3SelectorGlobalOrdering;",
        ),
        (
            "if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {",
            emitter_source,
            "if (expr->args.size() > lowering_ir_boundary_.runtime_dispatch_arg_slots) {",
        ),
        (
            'lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");',
            emitter_source,
            'lowered.args.assign(lowering_ir_boundary_.runtime_dispatch_arg_slots, "0");',
        ),
        (
            'call << "  " << dispatch_value << " = call i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32 "',
            emitter_source,
            'call << "  " << dispatch_value << " = call i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32 "',
        ),
    )
    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text

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
