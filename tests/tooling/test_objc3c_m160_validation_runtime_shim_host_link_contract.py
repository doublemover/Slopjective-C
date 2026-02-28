import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
FIXTURE_ROOT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "objc3c"
    / "m160_validation_runtime_shim_host_link_contract"
)
RUN1_MANIFEST = FIXTURE_ROOT / "replay_run_1" / "module.manifest.json"
RUN1_IR = FIXTURE_ROOT / "replay_run_1" / "module.ll"
RUN2_MANIFEST = FIXTURE_ROOT / "replay_run_2" / "module.manifest.json"
RUN2_IR = FIXTURE_ROOT / "replay_run_2" / "module.ll"

LANE_CONTRACT = "m160-runtime-shim-host-link-v1"
LOWERING_PREFIX = "; runtime_shim_host_link_lowering = "
PROFILE_PREFIX = "; frontend_objc_runtime_shim_host_link_profile = "
PROFILE_RE = re.compile(
    r"^; frontend_objc_runtime_shim_host_link_profile = "
    r"message_send_sites=(\d+), runtime_shim_required_sites=(\d+), runtime_shim_elided_sites=(\d+), "
    r"runtime_dispatch_arg_slots=(\d+), runtime_dispatch_declaration_parameter_count=(\d+), "
    r"runtime_dispatch_symbol=([^,]+), default_runtime_dispatch_symbol_binding=(true|false), "
    r"contract_violation_sites=(\d+), deterministic_runtime_shim_host_link_handoff=(true|false)$"
)
METADATA_RE = re.compile(
    r'^!13 = !{i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), i64 (\d+), !"([^"]+)", i1 ([01]), i64 (\d+), i1 ([01])}$'
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _load_json(path: Path) -> dict:
    return json.loads(_read(path))


def _expected_replay_key(
    message_send_sites: int,
    runtime_shim_required_sites: int,
    runtime_shim_elided_sites: int,
    runtime_dispatch_arg_slots: int,
    runtime_dispatch_declaration_parameter_count: int,
    runtime_dispatch_symbol: str,
    default_runtime_dispatch_symbol_binding: bool,
    contract_violation_sites: int,
    deterministic: bool,
) -> str:
    deterministic_token = "true" if deterministic else "false"
    default_symbol_binding_token = "true" if default_runtime_dispatch_symbol_binding else "false"
    return (
        f"message_send_sites={message_send_sites};"
        f"runtime_shim_required_sites={runtime_shim_required_sites};"
        f"runtime_shim_elided_sites={runtime_shim_elided_sites};"
        f"runtime_dispatch_arg_slots={runtime_dispatch_arg_slots};"
        f"runtime_dispatch_declaration_parameter_count={runtime_dispatch_declaration_parameter_count};"
        f"runtime_dispatch_symbol={runtime_dispatch_symbol};"
        f"default_runtime_dispatch_symbol_binding={default_symbol_binding_token};"
        f"contract_violation_sites={contract_violation_sites};"
        f"deterministic={deterministic_token};"
        f"lane_contract={LANE_CONTRACT}"
    )


def _manifest_packet(manifest: dict) -> tuple[tuple[int, ...], str, bool, int, bool]:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    return (
        (
            sema["runtime_shim_host_link_message_send_sites"],
            sema["runtime_shim_host_link_required_runtime_shim_sites"],
            sema["runtime_shim_host_link_elided_runtime_shim_sites"],
            sema["runtime_shim_host_link_runtime_dispatch_arg_slots"],
            sema["runtime_shim_host_link_runtime_dispatch_declaration_parameter_count"],
        ),
        sema["runtime_shim_host_link_runtime_dispatch_symbol"],
        sema["runtime_shim_host_link_default_runtime_dispatch_symbol_binding"],
        sema["runtime_shim_host_link_contract_violation_sites"],
        sema["deterministic_runtime_shim_host_link_handoff"],
    )


def _extract_lowering_replay_key(ir_text: str) -> str:
    for line in ir_text.splitlines():
        if line.startswith(LOWERING_PREFIX):
            return line.removeprefix(LOWERING_PREFIX)
    raise AssertionError(f"missing IR replay-key marker: {LOWERING_PREFIX}")


def _extract_profile_packet(ir_text: str) -> tuple[tuple[int, ...], str, bool, int, bool]:
    for line in ir_text.splitlines():
        match = PROFILE_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:5])
            symbol = match.group(6)
            default_binding = match.group(7) == "true"
            contract_violation_sites = int(match.group(8))
            deterministic = match.group(9) == "true"
            return counts, symbol, default_binding, contract_violation_sites, deterministic
    raise AssertionError(f"missing IR profile marker: {PROFILE_PREFIX}")


def _extract_metadata_packet(ir_text: str) -> tuple[tuple[int, ...], str, bool, int, bool]:
    for line in ir_text.splitlines():
        match = METADATA_RE.match(line)
        if match is not None:
            counts = tuple(int(group) for group in match.groups()[:5])
            symbol = match.group(6)
            default_binding = match.group(7) == "1"
            contract_violation_sites = int(match.group(8))
            deterministic = match.group(9) == "1"
            return counts, symbol, default_binding, contract_violation_sites, deterministic
    raise AssertionError('missing LLVM metadata tuple marker: !13 = !{i64 ..., !"symbol", i1 ..., i64 ..., i1 ...}')


def _assert_manifest_packet_contract(manifest: dict) -> None:
    sema = manifest["frontend"]["pipeline"]["sema_pass_manager"]
    surface = manifest["frontend"]["pipeline"]["semantic_surface"]["objc_runtime_shim_host_link_surface"]
    lowering = manifest["lowering_runtime_shim_host_link"]

    message_send_sites = sema["runtime_shim_host_link_message_send_sites"]
    required_sites = sema["runtime_shim_host_link_required_runtime_shim_sites"]
    elided_sites = sema["runtime_shim_host_link_elided_runtime_shim_sites"]
    runtime_dispatch_arg_slots = sema["runtime_shim_host_link_runtime_dispatch_arg_slots"]
    runtime_dispatch_decl_param_count = sema[
        "runtime_shim_host_link_runtime_dispatch_declaration_parameter_count"
    ]
    runtime_dispatch_symbol = sema["runtime_shim_host_link_runtime_dispatch_symbol"]
    default_symbol_binding = sema["runtime_shim_host_link_default_runtime_dispatch_symbol_binding"]
    contract_violation_sites = sema["runtime_shim_host_link_contract_violation_sites"]
    deterministic_handoff = sema["deterministic_runtime_shim_host_link_handoff"]

    expected_replay_key = _expected_replay_key(
        message_send_sites=message_send_sites,
        runtime_shim_required_sites=required_sites,
        runtime_shim_elided_sites=elided_sites,
        runtime_dispatch_arg_slots=runtime_dispatch_arg_slots,
        runtime_dispatch_declaration_parameter_count=runtime_dispatch_decl_param_count,
        runtime_dispatch_symbol=runtime_dispatch_symbol,
        default_runtime_dispatch_symbol_binding=default_symbol_binding,
        contract_violation_sites=contract_violation_sites,
        deterministic=deterministic_handoff,
    )

    assert required_sites + elided_sites == message_send_sites
    assert contract_violation_sites <= message_send_sites
    assert runtime_dispatch_decl_param_count == runtime_dispatch_arg_slots + 2
    assert default_symbol_binding is True
    assert runtime_dispatch_symbol == "objc3_msgsend_i32"

    assert deterministic_handoff is True
    assert sema["lowering_runtime_shim_host_link_replay_key"] == expected_replay_key

    assert surface["message_send_sites"] == message_send_sites
    assert surface["runtime_shim_required_sites"] == required_sites
    assert surface["runtime_shim_elided_sites"] == elided_sites
    assert surface["runtime_dispatch_arg_slots"] == runtime_dispatch_arg_slots
    assert surface["runtime_dispatch_declaration_parameter_count"] == runtime_dispatch_decl_param_count
    assert surface["runtime_dispatch_symbol"] == runtime_dispatch_symbol
    assert surface["default_runtime_dispatch_symbol_binding"] == default_symbol_binding
    assert surface["contract_violation_sites"] == contract_violation_sites
    assert surface["replay_key"] == expected_replay_key
    assert surface["deterministic_handoff"] is True

    assert lowering["replay_key"] == expected_replay_key
    assert lowering["lane_contract"] == LANE_CONTRACT
    assert lowering["deterministic_handoff"] is True


def test_m160_validation_runbook_section_exists() -> None:
    fragment = _read(TESTS_DOC_FRAGMENT)
    for text in (
        "## M160 validation/conformance/perf runtime-shim host-link runbook",
        "python -m pytest tests/tooling/test_objc3c_m160_frontend_runtime_shim_host_link_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m160_sema_runtime_shim_host_link_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m160_lowering_runtime_shim_host_link_contract.py -q",
        "python -m pytest tests/tooling/test_objc3c_m160_validation_runtime_shim_host_link_contract.py -q",
        "lowering_runtime_shim_host_link.replay_key",
        "deterministic_runtime_shim_host_link_handoff",
        "runtime_shim_host_link_lowering",
        "frontend_objc_runtime_shim_host_link_profile",
        "!objc3.objc_runtime_shim_host_link = !{!13}",
    ):
        assert text in fragment


def test_m160_validation_manifest_packets_hold_deterministic_runtime_shim_host_link_contract() -> None:
    for manifest_path in (RUN1_MANIFEST, RUN2_MANIFEST):
        _assert_manifest_packet_contract(_load_json(manifest_path))


def test_m160_validation_ir_markers_match_manifest_runtime_shim_host_link_metadata() -> None:
    for manifest_path, ir_path in ((RUN1_MANIFEST, RUN1_IR), (RUN2_MANIFEST, RUN2_IR)):
        manifest = _load_json(manifest_path)
        ir_text = _read(ir_path)
        manifest_counts, manifest_symbol, manifest_default_binding, manifest_contract_violations, manifest_det = (
            _manifest_packet(manifest)
        )

        assert "!objc3.objc_runtime_shim_host_link = !{!13}" in ir_text
        profile_packet = _extract_profile_packet(ir_text)
        metadata_packet = _extract_metadata_packet(ir_text)

        assert profile_packet[0] == manifest_counts
        assert profile_packet[1] == manifest_symbol
        assert profile_packet[2] == manifest_default_binding
        assert profile_packet[3] == manifest_contract_violations
        assert profile_packet[4] is manifest_det

        assert metadata_packet[0] == manifest_counts
        assert metadata_packet[1] == manifest_symbol
        assert metadata_packet[2] == manifest_default_binding
        assert metadata_packet[3] == manifest_contract_violations
        assert metadata_packet[4] is manifest_det

        assert _extract_lowering_replay_key(ir_text) == manifest["lowering_runtime_shim_host_link"]["replay_key"]


def test_m160_validation_replay_key_and_packets_are_stable_across_replay_runs() -> None:
    run1_manifest = _load_json(RUN1_MANIFEST)
    run2_manifest = _load_json(RUN2_MANIFEST)
    run1_ir = _read(RUN1_IR)
    run2_ir = _read(RUN2_IR)

    assert run1_manifest == run2_manifest
    assert run1_ir == run2_ir

    run1_replay_key = run1_manifest["lowering_runtime_shim_host_link"]["replay_key"]
    run2_replay_key = run2_manifest["lowering_runtime_shim_host_link"]["replay_key"]
    assert run1_replay_key == run2_replay_key
    assert _extract_lowering_replay_key(run1_ir) == run1_replay_key
    assert _extract_lowering_replay_key(run2_ir) == run2_replay_key

    assert hashlib.sha256(RUN1_MANIFEST.read_bytes()).hexdigest() == hashlib.sha256(
        RUN2_MANIFEST.read_bytes()
    ).hexdigest()
    assert hashlib.sha256(RUN1_IR.read_bytes()).hexdigest() == hashlib.sha256(RUN2_IR.read_bytes()).hexdigest()
