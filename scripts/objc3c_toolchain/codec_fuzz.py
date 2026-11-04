"""Bounded paired-sidecar discovery with actual implementation feedback and replay."""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
import os
from pathlib import Path
import re
import resource
import shutil
import signal
import subprocess
import tempfile
import time

from .binary_packages import digest, write_json
from .compiler_instrumentation import loaded_graph
from .implementation_analysis import dependencies as ninja_dependencies
from .llvm_ir_execution import command
from .llvm_tools import foundation
from .sanitizer_policy import require_clean_diagnostic, strict_environment
from .source_coverage import compile_inventory, raw_profiles

OWNED = ('interface_codec.cpp', 'interface_contract.cpp', 'module_names.cpp', 'runtime_interface_types.cpp')


def target_coverage(text):
    """Harness/LLVM-header counters cannot stand in for production decoder reach."""
    lines = [line for line in text.splitlines() if line.startswith('COVERED_FUNC:') and
             '/native/objc3c/src/clang_foundation/interface_codec.cpp:' in line and
             re.search(r'hits:\s*[1-9]\d* edges:\s*[1-9]\d*/', line)]
    if not lines or not any('DecodeSemanticInterfaceSidecar' in line for line in lines):
        raise ValueError('fuzzer feedback did not reach the actual production decoder')
    return lines


def verify_feedback(symbols):
    if not all(marker in symbols for marker in ('__sanitizer_cov_8bit_counters_init', '__sanitizer_cov_pcs_init', '__asan_init', '__ubsan_handle_')):
        raise ValueError('fuzzer implementation object lacks engine/ASan/UBSan instrumentation')


def replay_result(text, *, count, expected_inputs=None):
    value = json.loads(text)
    if not isinstance(value, dict):
        raise ValueError('codec replay result is not an object')
    rows = value.get('observations', [])
    if (len(rows) != count or any(type(value.get(key)) is not int or value[key] < 0 for key in
            ('accepted', 'semantic_rejected', 'framing_rejected')) or
            sum(value[key] for key in ('accepted', 'semantic_rejected', 'framing_rejected')) != count or
            any(type(value.get(key)) is not int or value[key] < 1 for key in
                ('descriptors_before', 'descriptors_after', 'threads_before', 'threads_after')) or
            value.get('descriptors_before') != value.get('descriptors_after') or
            value.get('threads_before') != value.get('threads_after') or
            any(not isinstance(row, dict) or row.get('outcome') not in (0, 1, 2) or type(row.get('outcome')) is not int or
                type(row.get('bytes')) is not int or not 0 <= row['bytes'] <= 131072 or
                not re.fullmatch('[0-9a-f]{64}', row.get('input_sha256', '')) or
                (row['outcome'] == 2 and not re.fullmatch('[0-9a-f]{64}', row.get('canonical_sha256', ''))) or
                (row['outcome'] != 2 and row.get('canonical_sha256') != '')
                for row in rows)):
        raise ValueError('codec replay lost input/result/resource-reset observations')
    if expected_inputs is not None:
        expected = [(path.stat().st_size, hashlib.sha256(path.read_bytes()).hexdigest())
                    for path in map(Path, expected_inputs)]
        if len(expected) != count or [(row['bytes'], row['input_sha256']) for row in rows] != expected:
            raise ValueError('codec replay did not observe the exact ordered input bytes')
    counts = Counter(row['outcome'] for row in rows)
    if any(value[key] != counts[index] for index, key in enumerate(('framing_rejected', 'semantic_rejected', 'accepted'))):
        raise ValueError('codec replay summary disagrees with actual per-input observations')
    return value


def tree_bytes(root):
    """Bound retained discovery data without following external links."""
    root = Path(root)
    total = 0
    for path in root.rglob('*'):
        if path.is_symlink():
            raise ValueError('codec discovery data contains a symbolic link')
        if path.is_file():
            total += path.stat().st_size
    return total


def bounded_command(argv, *, cwd, environment, log, timeout, watched_root, byte_limit):
    """Run one discovery process with a live wall-clock and retained-byte cap."""
    args = list(map(str, argv));started = time.monotonic();timed_out = budget_exceeded = False
    def no_core():
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
    with Path(log).open('w') as stream:
        try:
            with subprocess.Popen(args, cwd=cwd, env=environment, stdout=stream, stderr=subprocess.STDOUT,
                                  stdin=subprocess.DEVNULL, start_new_session=True, preexec_fn=no_core) as process:
                while process.poll() is None:
                    elapsed = time.monotonic()-started
                    try: retained = tree_bytes(watched_root)
                    except FileNotFoundError: retained = 0
                    if retained > byte_limit or elapsed > timeout:
                        budget_exceeded = retained > byte_limit;timed_out = not budget_exceeded
                        os.killpg(process.pid, signal.SIGKILL);process.wait()
                        stream.write('\nDiscovery stopped by '+('retained-byte' if budget_exceeded else 'wall-clock')+' budget.\n')
                        break
                    time.sleep(.05)
                code = 125 if budget_exceeded else (124 if timed_out else process.returncode)
        except OSError as error:
            stream.write(str(error)+'\n');code = 127
    return {'argv': args, 'cwd': str(cwd), 'exit_code': code, 'timed_out': timed_out,
            'budget_exceeded': budget_exceeded, 'retained_bytes': tree_bytes(watched_root),
            'seconds': round(time.monotonic()-started, 6), 'log': str(log), 'log_sha256': digest(Path(log))}


def qualify(repo, output, *, facade_build, seconds=5, jobs=1):
    repo, output, facade_build = repo.resolve(strict=True), output.resolve(), facade_build.resolve(strict=True)
    if (output.exists() or not output.is_relative_to(repo / 'out') or type(seconds) is not int or
            not 1 <= seconds <= 30 or jobs not in (1, 2)):
        raise ValueError('codec fuzzing requires fresh owned output and bounded time/jobs')
    plan, _ = foundation(repo)
    bootstrap, stage = Path(plan['host_environment']['bootstrap_root']), Path(plan['paths']['stage'])
    generated = facade_build / 'src/clang_foundation'
    native_archive = generated / 'libobjc3c_clang_foundation.a'
    output.mkdir(parents=True);(output / 'logs').mkdir()
    build = Path(tempfile.mkdtemp(prefix='objc3-codec-fuzz-'))
    if shutil.disk_usage(build).free < 1024**3: raise ValueError('codec fuzzing needs 1 GiB temporary build space')
    compile_env = strict_environment(bootstrap / 'bin/clang++', bootstrap / 'bin/llvm-symbolizer')
    compile_env['PATH'] = os.pathsep.join([str(Path(plan['host_environment'][key]).parent) for key in
        ('cmake_path', 'ninja_path')]+[compile_env['PATH']])
    report = {'schema': 'objc3c.paired-codec-fuzz.v1', 'status': 'running', 'commands': [], 'controls': [], 'campaigns': [],
        'source_build_id': plan['source_build_id'], 'build_config_digest': plan['build_config_digest'],
        'scope': 'Four first-party codec/contract/naming/runtime-interface TUs have engine feedback and ASan/UBSan. Matched Clang/LLVM and fallback native archive are sanitizer dependencies, not whole-frontend feedback coverage.',
        'limits': {'input_bytes': 131072, 'part_bytes': 65536, 'input_seconds': 3, 'rss_mb': 1536,
                   'discovery_seconds_per_strategy': seconds, 'retained_campaign_bytes': 32*1024**2},
        'unsupported': ['Full frontend syntax/codegen target', 'AFL++ engine and CMPLOG comparison', 'Whole patched-Clang feedback instrumentation',
                        'Persistent frontend result cleanup', 'Production-size boundary regression expansion', 'Required CI promotion'],
        'temporary_build': str(build), 'inputs': {}}
    frozen = report['inputs']
    for path in [Path(__file__), *sorted((repo / 'native/fuzz').iterdir()), native_archive,
                 generated / 'runtime_interface_header.h', bootstrap / 'bin/clang++', bootstrap / 'bin/llvm-profdata',
                 bootstrap / 'bin/llvm-cov', bootstrap / 'bin/llvm-nm', bootstrap / 'bin/llvm-symbolizer']:
        if path.is_file(): frozen[str(path)] = digest(path)
    started = time.monotonic()
    def save():
        report['seconds'] = round(time.monotonic()-started, 6);write_json(output / 'qualification.json', report)
    def run(name, argv, *, environment=compile_env, expected=0, timeout=120, watched_root=None,
            allow_bounded_timeout=False):
        if any(row['name'] == name for row in report['commands']): raise ValueError('duplicate codec campaign command')
        if watched_root is not None:
            invoke = bounded_command(argv, cwd=output, environment=environment,
                log=output / 'logs' / (name+'.log'), timeout=timeout, watched_root=watched_root,
                byte_limit=report['limits']['retained_campaign_bytes'])
        else:
            invoke = command(argv, cwd=output, environment=environment,
                log=output / 'logs' / (name+'.log'), timeout=timeout)
        row = {'name': name, 'environment': environment, **invoke}
        report['commands'].append(row);save()
        text = Path(row['log']).read_text(errors='replace')
        bounded = allow_bounded_timeout and row['timed_out'] and row['exit_code'] == 124
        if (row.get('budget_exceeded') or (row['timed_out'] and not bounded) or
                (row['exit_code'] != expected and not bounded)):
            raise ValueError('codec campaign command failed: '+name+'; see '+row['log'])
        if expected == 0 and not bounded: require_clean_diagnostic(text)
        if 'WARNING: unrecognized flag' in text: raise ValueError('fuzzer ignored an unreviewed option')
        row['status'] = 'PASS';row['bounded_completion'] = 'wall-clock' if bounded else 'process-exit';save()
        return text
    try:
        run('configure', [plan['host_environment']['cmake_path'], '-S', repo / 'native/fuzz', '-B', build, '-G', 'Ninja',
            '-DCMAKE_MAKE_PROGRAM='+plan['host_environment']['ninja_path'], '-DCMAKE_CXX_COMPILER='+str(bootstrap / 'bin/clang++'),
            '-DOBJC3_FUZZ_FOUNDATION='+str(stage), '-DOBJC3_FUZZ_GENERATED_HEADERS='+str(generated)])
        targets = ['objc3-codec-fuzzer', 'objc3-codec-raw-fuzzer', 'objc3-codec-replay', 'objc3-codec-property-fault', 'objc3-codec-boundary-fault']
        run('build', [plan['host_environment']['cmake_path'], '--build', build, '--target', *targets, '--parallel', str(jobs)], timeout=900)
        graph = run('graph', [plan['host_environment']['ninja_path'], '-C', build, '-t', 'commands', *targets])
        objects = compile_inventory(build, graph, required_flags=('-UNDEBUG', '-fno-sanitize-recover=undefined'))
        eligible = [row for row in objects if Path(row['source']).name in OWNED]
        if len(eligible) != len(OWNED) or {Path(row['source']).name for row in eligible} != set(OWNED):
            raise ValueError('codec campaign omitted an implementation-bearing TU')
        for index, row in enumerate(eligible):
            if '-fsanitize=fuzzer-no-link,address,undefined' not in row['argv']:
                raise ValueError('codec implementation lacks its engine compile flags')
            symbols = run('object-signals-'+str(index), [bootstrap / 'bin/llvm-nm', '--undefined-only', row['object']])
            verify_feedback(symbols)
        report['objects'] = objects
        for row in objects: frozen[row['object']] = row['sha256']
        deps = run('headers', [plan['host_environment']['ninja_path'], '-C', build, '-t', 'deps'])
        closure = ninja_dependencies(deps, build=build, selected_outputs={row['object'] for row in objects})
        for row in objects:
            if not closure.get(row['object']): raise ValueError('codec object lacks actual header closure')
            frozen.update({path: digest(Path(path)) for path in closure[row['object']]})
        binaries = {name: build / name for name in targets}
        env = strict_environment(binaries['objc3-codec-fuzzer'], bootstrap / 'bin/llvm-symbolizer')
        # Discovery disposition is engine-owned, separate from exit-86 strict
        # product qualification. No ambient sanitizer/suppression options enter.
        discovery = {**env, 'ASAN_OPTIONS': 'detect_leaks=1:detect_stack_use_after_return=1:halt_on_error=1:abort_on_error=1:exitcode=77:symbolize=1',
            'UBSAN_OPTIONS': 'halt_on_error=1:abort_on_error=1:print_stacktrace=1:exitcode=77'}
        report['discovery_environment'] = discovery
        for name, binary in binaries.items():
            frozen[str(binary)] = digest(binary)
            graph = loaded_graph(binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(),
                [stage / 'lib/libLLVM.so', stage / 'lib/libclang-cpp.so'],
                run=lambda suffix, argv, name=name: run(name+'-'+suffix, argv, environment=env))
            frozen.update(graph['libraries']);report.setdefault('loader_graphs', {})[name] = graph
        seeds = output / 'mandatory-seeds'
        seed_profiles = output / 'seed-profiles';seed_profiles.mkdir()
        run('generate-seeds', [binaries['objc3-codec-replay'], '--seed', seeds],
            environment={**env, 'LLVM_PROFILE_FILE': str(seed_profiles / '%p-%m.profraw')})
        seed_files = sorted(seeds.iterdir())
        if len(seed_files) != 10 or any(path.stat().st_size > 131072 for path in seed_files):
            raise ValueError('codec seed inventory changed or exceeded its bound')
        report['mandatory_seeds'] = {path.name: digest(path) for path in seed_files}
        for path in seed_files: frozen[str(path)] = digest(path)
        def replay(name, files):
            profiles = output / (name+'-profiles');profiles.mkdir()
            text = run(name, [binaries['objc3-codec-replay'], '--replay', *files],
                environment={**env, 'LLVM_PROFILE_FILE': str(profiles / '%p-%m.profraw')})
            return replay_result(text, count=len(files), expected_inputs=files)
        baseline = replay('mandatory-replay', seed_files)
        if (baseline['accepted'], baseline['semantic_rejected'], baseline['framing_rejected']) != (5, 3, 2):
            raise ValueError('reviewed accepted/rejected codec seeds changed classification')
        repeated = replay('persistent-ABA', [seed_files[0], seed_files[2], seed_files[0]])
        if repeated['observations'][0] != repeated['observations'][2] or repeated['observations'][0] != baseline['observations'][0]:
            raise ValueError('codec persistent A/B/A observations changed')
        repeated_a = replay('persistent-repeat', [seed_files[0]]*16)
        if any(row != baseline['observations'][0] for row in repeated_a['observations']):
            raise ValueError('codec repeated-input observation changed')
        after_large = replay('empty-after-large', [seed_files[8], seed_files[5], seed_files[0], seed_files[2]])
        if after_large['observations'][1] != baseline['observations'][5]:
            raise ValueError('codec empty-source result changed after a maximum-sized input')
        report['controls'].append({'name': 'mandatory-and-persistent-reset-replay', 'status': 'PASS',
            'baseline': baseline, 'ABA': repeated, 'repeated_inputs': len(repeated_a['observations'])})
        flags = ['-seed=1729', '-max_len=131072', '-timeout=3', '-rss_limit_mb=1536', '-malloc_limit_mb=256',
                 '-error_exitcode=77', '-timeout_exitcode=78', '-print_final_stats=1', '-print_coverage=1']
        for strategy, binary in (('raw-dictionary', binaries['objc3-codec-raw-fuzzer']), ('structured-pair', binaries['objc3-codec-fuzzer'])):
            root = output / strategy;root.mkdir();corpus = root / 'corpus';shutil.copytree(seeds, corpus)
            artifacts, profiles = root / 'artifacts', root / 'profiles';artifacts.mkdir();profiles.mkdir()
            mode_env = {**discovery, 'LLVM_PROFILE_FILE': str(profiles / '%p-%m.profraw')}
            text = run(strategy, [binary, *flags, '-max_total_time='+str(seconds),
                '-dict='+str(repo / 'native/fuzz/codec.dict'), '-artifact_prefix='+str(artifacts)+'/', corpus],
                environment=mode_env, timeout=seconds+30, watched_root=root)
            coverage = target_coverage(text)
            units = re.search(r'stat::number_of_executed_units:\s+(\d+)', text)
            peak = re.search(r'stat::peak_rss_mb:\s+(\d+)', text)
            if not units or not peak or int(units[1]) < 5 or list(artifacts.iterdir()):
                raise ValueError('codec discovery is empty, incomplete or produced a finding')
            if tree_bytes(root) > 32*1024**2:
                raise ValueError('codec discovery artifact growth exceeded its budget')
            merged = root / 'merged.profdata'
            run(strategy+'-merge', [bootstrap / 'bin/llvm-profdata', 'merge', '--sparse', *raw_profiles(profiles), '-o', merged])
            exported = run(strategy+'-source-coverage', [bootstrap / 'bin/llvm-cov', 'export', binary,
                '--check-binary-ids', '--instr-profile='+str(merged), '--skip-expansions'])
            document = json.loads(exported)
            owned_coverage = [row for data in document['data'] for row in data['files']
                if Path(row['filename']).name in OWNED and '/native/objc3c/src/clang_foundation/' in row['filename']]
            if not any(Path(row['filename']).name == 'interface_codec.cpp' and row['summary']['branches']['covered'] > 0
                       for row in owned_coverage): raise ValueError('fuzzer did not produce implementation source/branch coverage')
            report['campaigns'].append({'strategy': strategy, 'status': 'PASS', 'executions': int(units[1]),
                'peak_rss_mb': int(peak[1]), 'covered_decoder_functions': coverage,
                'source_coverage': {Path(row['filename']).name: row['summary'] for row in owned_coverage},
                'profile_sha256': digest(merged)})
            save()
        # Test-owned post-digest and borrowed-boundary faults must fail for their
        # exact class; a nonzero parse rejection is not an interesting crash.
        for name, marker in (('property', 'OBJ3_CODEC_PROPERTY: injected post-digest property failure'),
                             ('boundary', 'AddressSanitizer: heap-buffer-overflow')):
            root = output / (name+'-control');root.mkdir()
            fault = binaries['objc3-codec-'+name+'-fault']
            expected_fault_exit = 77 if name == 'property' else -signal.SIGABRT
            def fault_run(suffix, input_path):
                text = run(name+'-'+suffix, [fault, '-runs=1', '-error_exitcode=77',
                    '-timeout_exitcode=78', '-artifact_prefix='+str(root)+'/', input_path],
                    environment=discovery, expected=expected_fault_exit, timeout=20, watched_root=root)
                if marker not in text or 'CodecTarget.cpp' not in text:
                    raise ValueError('codec fault failed for an unrelated reason')
                return text
            fault_run('original-before', seed_files[0])
            reduced = root / 'reduced-pair'
            text = run(name+'-minimize', [fault, '-minimize_crash=1', '-runs=100000', '-max_total_time=10',
                '-error_exitcode=77', '-timeout_exitcode=78',
                '-exact_artifact_path='+str(reduced), seed_files[0]], environment=discovery,
                expected=expected_fault_exit, timeout=12, watched_root=root, allow_bounded_timeout=True)
            if marker not in text or not reduced.is_file() or not 0 < reduced.stat().st_size <= seed_files[0].stat().st_size:
                raise ValueError('codec fault minimization lost its exact failure class or size bound')
            reduced_sha = digest(reduced)
            fault_run('reduced-replay-a', reduced);fault_run('reduced-replay-b', reduced)
            fault_run('original-after', seed_files[0])
            if digest(reduced) != reduced_sha:
                raise ValueError('codec minimized fault bytes changed during exact replay')
            report['controls'].append({'name': name+'-fault-detected-reduced-and-replayed', 'status': 'PASS',
                'diagnostic_class': marker, 'original_bytes': seed_files[0].stat().st_size,
                'reduced_bytes': reduced.stat().st_size, 'reduced_sha256': reduced_sha,
                'stable_replays': 2, 'original_ABA': True, 'reduction_budget_seconds': 12,
                'fully_minimal_claimed': False, 'expected_fault_exit': expected_fault_exit})
        run('omitted-feedback-build', [plan['host_environment']['cmake_path'], '--build', build,
            '--target', 'objc3_codec_no_feedback', '--parallel', '1'], timeout=180)
        omitted = list((build / 'CMakeFiles/objc3_codec_no_feedback.dir').rglob('*.o'))
        if len(omitted) != 1: raise ValueError('omitted-feedback control has no exact object')
        symbols = run('omitted-feedback-signals', [bootstrap / 'bin/llvm-nm', '--undefined-only', omitted[0]])
        try: verify_feedback(symbols)
        except ValueError: report['controls'].append({'name': 'omitted-production-decoder-feedback-rejected', 'status': 'PASS'})
        else: raise ValueError('fuzzer provenance accepted an ordinary decoder object')
        if any(digest(Path(path)) != value for path, value in frozen.items()):
            raise ValueError('codec campaign source/tool/library/object inputs changed')
        report['status'] = 'PASS'
        shutil.rmtree(build);report['temporary_build_reclaimed'] = True
    except Exception as error:
        report.update(status='FAIL', error=str(error));raise
    finally: save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True);parser.add_argument('--facade-build', type=Path, required=True)
    parser.add_argument('--seconds', type=int, default=5);parser.add_argument('--jobs', type=int, default=1)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'campaigns': len(result['campaigns']), 'controls': len(result['controls'])}))


if __name__ == '__main__': main()
