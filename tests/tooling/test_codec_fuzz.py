"""The paired-codec discovery owner fails closed on weak replay and feedback."""
import hashlib
import json
import sys

import pytest

from scripts.objc3c_toolchain.codec_fuzz import (bounded_command, frontend_replay_result,
    frontend_target_coverage, patched_frontend_coverage, replay_result, target_coverage,
    tree_bytes, verify_feedback, verify_foundation_feedback)


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
