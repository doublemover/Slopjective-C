"""The paired-codec discovery owner fails closed on weak replay and feedback."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain.codec_fuzz import (accepted_exit_codes, bounded_command, command_completion,
    compact_coverage_log, diagnostic_stream, frontend_replay_result, frontend_target_coverage,
    patched_frontend_coverage, replay_result, target_coverage, tree_bytes, verify_feedback,
    verify_foundation_feedback)
from scripts.objc3c_toolchain.sanitizer_policy import strict_environment

BOOTSTRAP = Path(os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT', '/missing-objc3-bootstrap'))
FUZZ_COMPILER = BOOTSTRAP / 'bin/clang++'
FUZZ_SYMBOLIZER = BOOTSTRAP / 'bin/llvm-symbolizer'
FUZZ_READY = FUZZ_COMPILER.is_file() and FUZZ_SYMBOLIZER.is_file()


def replay(path):
    payload = path.read_bytes()
    return {'framing_rejected': 0, 'semantic_rejected': 0, 'accepted': 1,
        'descriptors_before': 4, 'descriptors_after': 4, 'threads_before': 1, 'threads_after': 1,
        'observations': [{'outcome': 2, 'canonical_sha256': 'a'*64,
            'input_sha256': hashlib.sha256(payload).hexdigest(), 'bytes': len(payload)}]}


@pytest.mark.parametrize('mutation', ['bytes', 'digest', 'order', 'resource', 'classification', 'canonical'])
def test_exact_replay_rejects_byte_resource_and_result_substitution(tmp_path, mutation):
    first = tmp_path / 'first';first.write_bytes(b'first exact bytes')
    second = tmp_path / 'second';second.write_bytes(b'second exact bytes')
    value = replay(first)
    if mutation == 'bytes': value['observations'][0]['bytes'] += 1
    elif mutation == 'digest': value['observations'][0]['input_sha256'] = 'b'*64
    elif mutation == 'order': value['observations'] *= 2;value['accepted'] = 2
    elif mutation == 'resource': value['descriptors_after'] = 5
    elif mutation == 'classification': value['observations'][0]['outcome'] = True
    else: value['observations'][0]['canonical_sha256'] = ''
    with pytest.raises(ValueError):
        replay_result(json.dumps(value), count=2 if mutation == 'order' else 1,
                      expected_inputs=[first, second] if mutation == 'order' else [first])
    assert replay_result(json.dumps(replay(first)), count=1, expected_inputs=[first])['accepted'] == 1


def test_only_actual_decoder_coverage_and_full_instrumentation_pass():
    actual = ('COVERED_FUNC: hits: 4 edges: 7/9 DecodeSemanticInterfaceSidecar '
              '/repo/native/objc3c/src/clang_foundation/interface_codec.cpp:42')
    assert target_coverage(actual) == [actual]
    for impostor in (actual.replace('interface_codec.cpp', 'CodecTarget.cpp'),
                     actual.replace('hits: 4', 'hits: 0'), 'COVERED_FUNC: harness only'):
        with pytest.raises(ValueError): target_coverage(impostor)
    symbols = '__sanitizer_cov_8bit_counters_init __sanitizer_cov_pcs_init __asan_init __ubsan_handle_add_overflow'
    verify_feedback(symbols)
    with pytest.raises(ValueError): verify_feedback(symbols.replace('__sanitizer_cov_pcs_init', ''))


def test_frontend_replay_requires_exact_ordered_bytes_and_resource_reset(tmp_path):
    first = tmp_path / 'first.m';first.write_bytes(b'@interface A @end')
    second = tmp_path / 'second.m';second.write_bytes(b'@implementation A @end')
    rows = [{'disposition': index, 'diagnostics': index, 'bytes': path.stat().st_size,
             'input_sha256': hashlib.sha256(path.read_bytes()).hexdigest(), 'result_sha256': str(index)*64}
            for index, path in enumerate((first, second))]
    value = {'descriptors_before': 4, 'descriptors_after': 4, 'threads_before': 1,
             'threads_after': 1, 'observations': rows}
    assert len(frontend_replay_result(json.dumps(value), count=2, expected_inputs=[first, second])['observations']) == 2
    for mutation in ('order', 'digest', 'resource', 'disposition'):
        changed = json.loads(json.dumps(value))
        if mutation == 'order': changed['observations'].reverse()
        elif mutation == 'digest': changed['observations'][0]['input_sha256'] = 'f'*64
        elif mutation == 'resource': changed['threads_after'] += 1
        else: changed['observations'][0]['disposition'] = 4
        with pytest.raises(ValueError):
            frontend_replay_result(json.dumps(changed), count=2, expected_inputs=[first, second])


def test_patched_parser_and_sema_feedback_cannot_be_substituted_by_the_facade():
    facade = ('COVERED_FUNC: hits: 4 edges: 7/9 Compile '
              '/repo/native/objc3c/src/clang_foundation/compile_session.cpp:42')
    parser = 'COVERED_FUNC: hits: 2 edges: 3/5 ParseDeclaration /repo/out/src/clang/lib/Parse/ParseDecl.cpp:90'
    sema = 'COVERED_FUNC: hits: 3 edges: 4/8 ActOnDeclaration /repo/out/src/clang/lib/Sema/SemaDecl.cpp:120'
    assert frontend_target_coverage(facade) == [facade]
    assert patched_frontend_coverage('\n'.join((facade, parser, sema))) == {'Parse': [parser], 'Sema': [sema]}
    for omitted in ((facade, parser), (facade, sema), (facade, parser.replace('hits: 2', 'hits: 0'), sema)):
        with pytest.raises(ValueError): patched_frontend_coverage('\n'.join(omitted))
    symbols = '__sanitizer_cov_8bit_counters_init __sanitizer_cov_pcs_init __asan_init __ubsan_handle_type_mismatch'
    assert len(verify_foundation_feedback(symbols)) == 4
    with pytest.raises(ValueError): verify_foundation_feedback(symbols.replace('__asan_init', ''))


def test_live_discovery_budget_stops_artifact_growth(tmp_path):
    watched = tmp_path / 'watched';watched.mkdir();log = tmp_path / 'run.log'
    script = ('from pathlib import Path; import time; p=Path('+repr(str(watched / 'growth'))+'); '
              'p.write_bytes(b"x"*8192); time.sleep(5)')
    row = bounded_command([sys.executable, '-c', script], cwd=tmp_path, environment=dict(__import__('os').environ),
                          log=log, timeout=3, watched_root=watched, byte_limit=1024)
    assert row['budget_exceeded'] and not row['timed_out'] and row['exit_code'] == 125
    assert tree_bytes(watched) == 8192


def test_only_minimization_admits_natural_or_bounded_completion():
    natural = {'exit_code': 0, 'timed_out': False, 'budget_exceeded': False}
    fault = {'exit_code': 77, 'timed_out': False, 'budget_exceeded': False}
    bounded = {'exit_code': 124, 'timed_out': True, 'budget_exceeded': False}
    assert accepted_exit_codes((0, 77)) == (0, 77)
    assert command_completion(natural, (0, 77), allow_bounded_timeout=True) == 'natural-process-exit'
    assert command_completion(fault, (0, 77), allow_bounded_timeout=True) == 'expected-fault-exit'
    assert command_completion(bounded, (0, 77), allow_bounded_timeout=True) == 'bounded-wall-clock'
    for row in (
            {**natural, 'exit_code': 1},
            {**natural, 'exit_code': 124},
            {**natural, 'exit_code': 125, 'budget_exceeded': True},
            {**natural, 'exit_code': -9}):
        with pytest.raises(ValueError, match='unreviewed disposition'):
            command_completion(row, (0, 77), allow_bounded_timeout=True)
    with pytest.raises(ValueError, match='unreviewed disposition'):
        command_completion(natural, 77, allow_bounded_timeout=True)
    for invalid in ((), (0, 0), (0, True), ('0',)):
        with pytest.raises(ValueError, match='unique integer'):
            accepted_exit_codes(invalid)


@pytest.mark.skipif(not FUZZ_READY, reason='requires the pinned libFuzzer compiler and symbolizer')
def test_real_libfuzzer_natural_minimization_completion_is_admitted(tmp_path):
    source = tmp_path / 'natural-minimizer.cpp'
    source.write_text(r'''
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t *data, std::size_t size) {
  if (size >= 4 && data[0] == '0') {
    std::fputs("OBJ3_MINIMIZER_CONTROL: injected fault\n", stderr);
    std::abort();
  }
  return 0;
}
''')
    binary = tmp_path / 'natural-minimizer'
    subprocess.run([FUZZ_COMPILER, '--no-default-config', '-std=c++20', '-g', '-O1',
        '-fsanitize=fuzzer,address,undefined', '-fno-sanitize-recover=undefined', source,
        '-o', binary], check=True, capture_output=True, timeout=60)
    original = tmp_path / 'original';original.write_bytes(b'0' * 16)
    reduced = tmp_path / 'reduced'
    environment = strict_environment(FUZZ_COMPILER, FUZZ_SYMBOLIZER)
    environment['ASAN_OPTIONS'] = ('detect_leaks=1:detect_stack_use_after_return=1:'
        'halt_on_error=1:abort_on_error=1:exitcode=77:symbolize=1')
    environment['UBSAN_OPTIONS'] = 'halt_on_error=1:abort_on_error=1:print_stacktrace=1:exitcode=77'
    log = tmp_path / 'minimize.log'
    row = bounded_command([binary, '-minimize_crash=1', '-runs=100000', '-max_total_time=10',
        '-error_exitcode=77', '-timeout_exitcode=78', '-exact_artifact_path='+str(reduced), original],
        cwd=tmp_path, environment=environment, log=log, timeout=12, watched_root=tmp_path,
        byte_limit=64 * 1024**2)
    assert row['exit_code'] == 0 and not row['timed_out']
    assert command_completion(row, (0, 77), allow_bounded_timeout=True) == 'natural-process-exit'
    text = log.read_text(errors='replace')
    assert 'OBJ3_MINIMIZER_CONTROL: injected fault' in text
    assert 'Done MinimizeCrashInputInternalStep, no crashes found' in text
    assert reduced.is_file() and 0 < reduced.stat().st_size <= original.stat().st_size


def test_successful_coverage_log_compaction_retains_covered_rows_and_full_identity(tmp_path):
    log = tmp_path / 'coverage.log'
    text = ('INFO: campaign complete\n'
            'COVERED_FUNC: hits: 3 edges: 2/4 ParseThing /src/clang/lib/Parse/Parse.cpp:4\n'
            'UNCOVERED_FUNC: hits: 0 edges: 0/8 Other /src/clang/lib/Sema/Other.cpp:9\n'
            'stat::number_of_executed_units: 42\n')
    log.write_text(text)
    original = hashlib.sha256(text.encode()).hexdigest()

    result = compact_coverage_log(log, text, minimum_bytes=1)

    assert result['full_log_bytes'] == len(text)
    assert result['full_log_sha256'] == original
    assert result['log_bytes'] < result['full_log_bytes']
    assert result['log_sha256'] == hashlib.sha256(log.read_bytes()).hexdigest()
    assert 'COVERED_FUNC:' in log.read_text()
    assert 'UNCOVERED_FUNC:' not in log.read_text()
    assert 'stat::number_of_executed_units: 42' in log.read_text()


def test_coverage_inventory_names_cannot_spoof_a_sanitizer_finding():
    metadata = ('UNCOVERED_FUNC: hits: 0 edges: 0/8 '
                'llvm::AddressSanitizer::run() /src/AddressSanitizer.cpp:4\n')
    assert diagnostic_stream(metadata, coverage_report=True) == ''
    finding = '==42==ERROR: AddressSanitizer: heap-buffer-overflow\n'
    assert finding in diagnostic_stream(metadata + finding, coverage_report=True)
