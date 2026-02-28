from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
PERF_BUDGET_SCRIPT = ROOT / "scripts" / "check_objc3c_native_perf_budget.ps1"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m210_lowering_runtime_performance_budgets_and_regression_gates_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M210 lowering/runtime performance budgets and regression gates",
        "tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/",
        "tmp/artifacts/objc3c-native/perf-budget/<run_id>/",
        "tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/",
        "tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.manifest.json",
        "tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json",
        "tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/perf-regression-gates.txt",
        "; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic",
        "; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>",
        "!objc3.frontend = !{!0}",
        "declare i32 @<symbol>(i32, ptr, i32, ..., i32)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "tmp/artifacts/objc3c-native/perf-budget",
        "summary.json",
        "defaultMaxElapsedMs",
        "defaultPerFixtureBudgetMs",
        "cache_hit=(true|false)",
        "dispatch_fixture_count",
        "max_elapsed_ms",
        "total_elapsed_ms",
        "budget_breached",
        "cache_proof",
        "status",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "invalid lowering contract runtime_dispatch_symbol",
        'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
        'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        '$perfRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/perf-budget"',
        '$summaryPath = Join-Path $runDir "summary.json"',
        "$defaultMaxElapsedMs = 4000",
        "$defaultPerFixtureBudgetMs = 150",
        '$matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\\s*$")',
        'throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"',
        "dispatch_fixture_count = $dispatchFixtureCount",
        'rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @|\\"lowering\\":{\\"runtime_dispatch_symbol\\"" tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.ll tmp/artifacts/compilation/objc3c-native/m210/lowering-runtime-perf-regression/module.manifest.json > tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/abi-ir-anchors.txt',
        'rg -n "tmp/artifacts/objc3c-native/perf-budget|summary.json|defaultMaxElapsedMs|defaultPerFixtureBudgetMs|cache_hit=|dispatch_fixture_count|max_elapsed_ms|total_elapsed_ms|budget_breached|cache_proof|status" scripts/check_objc3c_native_perf_budget.ps1 tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json > tmp/reports/objc3c-native/m210/lowering-runtime-perf-regression/perf-regression-gates.txt',
        "python -m pytest tests/tooling/test_objc3c_m210_lowering_perf_regression_contract.py -q",
    ):
        assert text in fragment


def test_m210_lowering_runtime_performance_anchors_align_to_source_surfaces() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    emitter_source = _read(IR_EMITTER_SOURCE)
    artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    perf_budget_script = _read(PERF_BUDGET_SCRIPT)

    mapped_source_anchors = (
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
            '$perfRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/perf-budget"',
            perf_budget_script,
            '$perfRoot = Join-Path $repoRoot "tmp/artifacts/objc3c-native/perf-budget"',
        ),
        (
            '$summaryPath = Join-Path $runDir "summary.json"',
            perf_budget_script,
            '$summaryPath = Join-Path $runDir "summary.json"',
        ),
        ("$defaultMaxElapsedMs = 4000", perf_budget_script, "$defaultMaxElapsedMs = 4000"),
        ("$defaultPerFixtureBudgetMs = 150", perf_budget_script, "$defaultPerFixtureBudgetMs = 150"),
        (
            '$matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\\s*$")',
            perf_budget_script,
            '$matches = [regex]::Matches($OutputText, "(?m)^cache_hit=(true|false)\\s*$")',
        ),
        (
            'throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"',
            perf_budget_script,
            'throw "perf-budget FAIL: cache-proof run2 expected cache_hit=true, observed false"',
        ),
        ("dispatch_fixture_count = $dispatchFixtureCount", perf_budget_script, "dispatch_fixture_count = $dispatchFixtureCount"),
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
