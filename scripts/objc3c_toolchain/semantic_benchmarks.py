"""Advisory Linux ObjC3 frontend and sustained runtime workload measurements."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import platform
import signal
import statistics
import subprocess
import time

from .binary_packages import digest, find_installed_shared_library, write_json
from .package import SCHEMA, _digest, inventory, verify_sdk
import shutil
from .compiler_instrumentation import loaded_graph

SCALES = (32, 256, 2048)
ROUNDS, SEED = 16, 17


def frontend_source(scale):
    if scale not in SCALES:
        raise ValueError('unreviewed frontend benchmark scale')
    return '\n'.join('''int function_INDEX(int input) {
  Optional(int) optional = Some(int, input);
  guard let value = optional else { return 0; }
  Result(int, int) result = Ok(int, int, value);
  int output = 0;
  defer { output = 0; }
  match(result) {
  case .Ok(let success): { output = success; }
  case .Err(let failure): { output = failure; }
  }
  return output;
}'''.replace('INDEX', str(index)) for index in range(scale))+'\n'


def expected_runtime(scale):
    if scale not in SCALES:
        raise ValueError('unreviewed runtime benchmark scale')
    return {'checksum': ROUNDS*(scale*SEED + scale*(scale-1)//2),
            'completed_operations': ROUNDS*(2*scale+3)}


def runtime_result(text, scale):
    try:
        value = json.loads(text)
    except ValueError as error:
        raise ValueError('benchmark omitted its semantic/work-count oracle') from error
    expected = expected_runtime(scale)
    if (not isinstance(value, dict) or set(value) != {*expected, 'nanoseconds'} or
            any(type(value[key]) is not int or value[key] != expected[key] for key in expected) or
            type(value['nanoseconds']) is not int or value['nanoseconds'] <= 0):
        raise ValueError('benchmark semantic output, completed work or timer is invalid')
    return value


def sample(argv, *, cwd, environment, log, timeout=120):
    """Per-command wait4 rusage; never reuse a global descendant RSS maximum."""
    started, timed_out = time.monotonic(), False
    with log.open('w') as stream:
        with subprocess.Popen(list(map(str, argv)), cwd=cwd, env=environment,
                stdin=subprocess.DEVNULL, stdout=stream, stderr=subprocess.STDOUT,
                start_new_session=True) as process:
            while True:
                child, status, usage = os.wait4(process.pid, os.WNOHANG)
                if child:
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                if time.monotonic()-started >= timeout:
                    timed_out = True
                    os.killpg(process.pid, signal.SIGKILL)
                    _, status, usage = os.wait4(process.pid, 0)
                    process.returncode = os.waitstatus_to_exitcode(status)
                    break
                time.sleep(0.002)
    row = {'argv': list(map(str, argv)), 'cwd': str(cwd), 'exit_code': process.returncode,
        'timed_out': timed_out, 'wall_seconds': time.monotonic()-started,
        'cpu_seconds': usage.ru_utime+usage.ru_stime, 'peak_process_rss_kib': usage.ru_maxrss,
        'rss_scope': 'Largest individual process in this command and its waited descendants; not aggregate tree memory.',
        'log': str(log), 'log_sha256': digest(log)}
    return row


def summarize(rows, field='wall_seconds'):
    if len(rows) < 3:
        raise ValueError('benchmark requires at least three retained samples')
    values = [row[field] for row in rows]
    if any(type(value) not in (int, float) or value <= 0 for value in values):
        raise ValueError('invalid or missing benchmark sample')
    center = statistics.median(values)
    return {'median': center, 'minimum': min(values), 'maximum': max(values),
        'median_absolute_deviation': statistics.median(abs(value-center) for value in values),
        'samples': len(values)}


def host_context():
    def read(path):
        return Path(path).read_text().strip()
    return {'system': platform.platform(), 'kernel': platform.release(),
        'cpu_model': next(line.split(':', 1)[1].strip() for line in read('/proc/cpuinfo').splitlines() if line.startswith('model name')),
        'cpu_quota': read('/sys/fs/cgroup/cpu.max'), 'cpuset': read('/sys/fs/cgroup/cpuset.cpus.effective'),
        'memory_limit': read('/sys/fs/cgroup/memory.max'), 'affinity': sorted(os.sched_getaffinity(0)),
        'container': Path('/.dockerenv').exists(), 'runtime_threads': 1,
        'noise_limits': 'Shared runner; frequency/contention uncontrolled. Timing remains advisory.'}


def prepare_prefix(stage, prefix, *, source_build_id, target):
    """Seal current compiler/runtime build products for the existing CI lane."""
    stage, prefix = stage.resolve(strict=True), prefix.resolve()
    if prefix.exists() or prefix.is_relative_to(stage):
        raise ValueError('benchmark preparation requires a fresh independent development prefix')
    prefix.mkdir(parents=True)
    def link(source, destination):
        destination.parent.mkdir(parents=True, exist_ok=True)
        os.link(source.resolve(strict=True), destination)
    for name in ('clang', 'clang++', 'ld.lld'):
        link(stage / 'bin' / name, prefix / 'bin' / name)
    for name in ('libLLVM.so', 'libclang-cpp.so', 'libobjc.so', 'libobjc3_runtime_services.a'):
        source = (find_installed_shared_library(stage / 'lib', name)
                  if name in ('libLLVM.so', 'libclang-cpp.so')
                  else (stage / 'lib' / name).resolve(strict=True))
        if source is None:
            continue
        link(source, prefix / 'lib' / source.name)
        if source.name != name: (prefix / 'lib' / name).symlink_to(source.name)
    for name in ('lib/clang/23/include', 'include/objc3c/runtime'):
        shutil.copytree(stage / name, prefix / name, copy_function=os.link)
    record = {'schema': SCHEMA, 'source_build_id': source_build_id, 'target': target,
        'purpose': 'Development-only semantic workload measurement; no SDK archive qualification', 'files': inventory(prefix)}
    record['seal_sha256'] = _digest(record)
    write_json(prefix / 'objc3-sdk.json', record);(prefix / 'objc3-sdk.json').chmod(0o644)
    return verify_sdk(prefix)


def measure(repo, output, *, sdk, baseline_sdk=None, repetitions=3):
    repo, output = repo.resolve(strict=True), output.resolve()
    if (platform.system() != 'Linux' or platform.machine() != 'x86_64' or
            type(repetitions) is not int or not 3 <= repetitions <= 15 or
            output.exists() or not output.is_relative_to(repo / 'out')):
        raise ValueError('benchmark needs native Linux x86-64, fresh owned output and 3..15 samples')
    prefixes = [Path(value).resolve(strict=True) for value in ([baseline_sdk] if baseline_sdk else [])+[sdk]]
    manifests = [verify_sdk(prefix) for prefix in prefixes]
    if {value['target'] for value in manifests} != {'x86_64-unknown-linux-gnu'}:
        raise ValueError('benchmark SDK target contexts differ')
    output.mkdir(parents=True)
    fixture = repo / 'tests/benchmarks/collections.cpp'
    frozen = {str(path): digest(path) for path in (Path(__file__), fixture)}
    report = {'schema': 'objc3c.semantic-benchmarks.v1', 'status': 'running', 'host': host_context(),
        'workload_version': 1, 'inputs': frozen, 'repetitions': repetitions, 'samples': [], 'order': [],
        'scope': 'Warm frontend syntax/object generation and sustained concrete i32 map creation/insertion/lookup/count/teardown. No cold build or filesystem, archive, actor, ARC lifetime or full language claim.',
        'timing_decision': 'advisory-inconclusive-uncalibrated', 'correctness': 'running',
        'cache_state': 'One unmeasured warmup per SDK/row; fresh output artifacts, warm shared OS caches; compiler cache launchers absent.',
        'sdks': [{'path': str(prefix), 'seal_sha256': value['seal_sha256'], 'source_build_id': value['source_build_id'],
            'purpose': value.get('purpose', 'sealed SDK')} for prefix, value in zip(prefixes, manifests)]}
    states = []
    def run(label, args, state):
        log = output / (label+'.log')
        row = sample(args, cwd=state['root'], environment=state['environment'], log=log)
        row = {'name': label, 'sdk_index': state['index'], **row}
        report['samples'].append(row)
        write_json(output / 'qualification.json', report)
        if row['timed_out'] or row['exit_code']:
            raise ValueError('semantic benchmark command failed: '+label)
        return row
    try:
        for index, prefix in enumerate(prefixes):
            root = output / ('sdk-'+str(index));root.mkdir()
            environment = {key: os.environ[key] for key in ('HOME', 'TMPDIR', 'LANG') if key in os.environ}
            environment.update(PATH=str(prefix / 'bin')+':'+os.defpath, LD_LIBRARY_PATH=str(prefix / 'lib'),
                               LC_ALL='C', SOURCE_DATE_EPOCH='0')
            state = {'index': index, 'prefix': prefix, 'root': root, 'environment': environment, 'rows': {}}
            states.append(state)
            resources = [path for path in (prefix / 'lib/clang').iterdir() if path.is_dir()]
            if len(resources) != 1:
                raise ValueError('benchmark requires one explicit compiler resource root')
            compiler = prefix / 'bin/clang++'
            flags = ['--no-default-config', '-resource-dir', resources[0], '-std=c++20', '-O2']
            # Record sanitizer markers in both implementation and runtime inputs.
            # These distinguish diagnostic compiler timings from ordinary builds.
            markers = {}
            compiler_libraries = [library for library in
                (find_installed_shared_library(prefix / 'lib', candidate)
                 for candidate in ('libLLVM.so', 'libclang-cpp.so')) if library is not None]
            compiler_probe = (find_installed_shared_library(prefix / 'lib', 'libclang-cpp.so')
                              or compiler)
            for name, probe in (('libclang-cpp.so', compiler_probe),
                                ('libobjc3_runtime_services.a', prefix / 'lib/libobjc3_runtime_services.a')):
                probe = run(f'sdk-{index}-instrumentation-'+name.replace('.', '-'),
                    ['/usr/bin/nm', *(['-D'] if name == 'libclang-cpp.so' and probe != compiler else []),
                        '--undefined-only', probe], state)
                text = Path(probe['log']).read_text()
                markers[name] = sorted({kind for kind in ('asan', 'ubsan', 'tsan', 'llvm_profile') if '__'+kind in text})
            state['instrumentation'] = markers
            report['sdks'][index].update(instrumentation=markers, environment=environment,
                                         compiler_instrumentation_probe=str(compiler_probe))
            if markers['libobjc3_runtime_services.a']:
                raise ValueError('ordinary runtime timing cannot use an instrumented service archive')
            report['sdks'][index]['compiler_loader'] = loaded_graph(compiler,
                Path('/lib64/ld-linux-x86-64.so.2').resolve(),
                compiler_libraries,
                run=lambda name, argv: Path(run('sdk-'+str(index)+'-compiler-'+name, argv, state)['log']).read_text())
            runtime_source = root / 'collections.cpp';runtime_source.write_bytes(fixture.read_bytes())
            obj, binary = root / 'collections.o', root / 'collections'
            run(f'sdk-{index}-runtime-object', [compiler, *flags, '-I'+str(prefix / 'include'),
                '-c', runtime_source, '-o', obj], state)
            run(f'sdk-{index}-runtime-link', [compiler, '--no-default-config', '-fuse-ld=lld', obj,
                prefix / 'lib/libobjc3_runtime_services.a', prefix / 'lib/libobjc.so',
                '-pthread', '-Wl,-rpath,'+str(prefix / 'lib'), '-o', binary], state)
            report['sdks'][index]['runtime_loader'] = loaded_graph(binary,
                Path('/lib64/ld-linux-x86-64.so.2').resolve(), [],
                run=lambda name, argv: Path(run('sdk-'+str(index)+'-runtime-'+name, argv, state)['log']).read_text())
            for graph in ('compiler_loader', 'runtime_loader'):
                frozen.update(report['sdks'][index][graph]['libraries'])
            for scale in SCALES:
                source = root / ('frontend-'+str(scale)+'.mm');source.write_text(frontend_source(scale))
                common = [compiler, *flags, '-x', 'objective-c++', '-fobjc-version=3',
                    '-fobjc-runtime=gnustep-2.2', '-fobjc-arc', '-fblocks', source]
                state['rows']['syntax-'+str(scale)] = [*common, '-fsyntax-only']
                state['rows']['object-'+str(scale)] = [*common, '-c', '-o', root / ('frontend-'+str(scale)+'.o')]
                state['rows']['runtime-'+str(scale)] = [binary, str(scale), str(ROUNDS), str(SEED), '0']
        if any(state['instrumentation'] != states[0]['instrumentation'] for state in states):
            raise ValueError('baseline/candidate compiler instrumentation contexts differ')
        for iteration in range(repetitions+1):
            order = list(range(len(states)))[::1 if iteration % 2 == 0 else -1]
            report['order'].append({'iteration': iteration, 'warmup': iteration == 0, 'sdk_indices': order})
            for index in order:
                state = states[index]
                for name, command in state['rows'].items():
                    row = run(f'{iteration}-sdk-{index}-'+name, command, state)
                    row.update(workload=name, warmup=iteration == 0)
                    if name.startswith('runtime-'):
                        row['semantic_result'] = runtime_result(Path(row['log']).read_text(), int(name.split('-')[1]))
                        row['work_seconds'] = row['semantic_result']['nanoseconds']/1e9
                    elif name.startswith('object-'):
                        artifact = Path(command[-1])
                        if not artifact.is_file() or not artifact.stat().st_size:
                            raise ValueError('frontend benchmark omitted its object')
                        row['object_sha256'] = digest(artifact)
        # A bounded same-input delay must preserve semantics and produce a
        # measurable minimum effect. This calibrates a control, not product limits.
        state, delayed = states[-1], []
        for repetition in range(repetitions):
            row = run('delay-control-'+str(repetition), [*state['rows']['runtime-32'][:-1], '1'], state)
            delayed.append(runtime_result(Path(row['log']).read_text(), 32)['nanoseconds']/1e9)
        ordinary = [row['work_seconds'] for row in report['samples'] if row.get('workload') == 'runtime-32'
                    and row['sdk_index'] == state['index'] and not row.get('warmup')]
        effect = statistics.median(delayed)-statistics.median(ordinary)
        if effect < ROUNDS*0.001/2:
            raise ValueError('bounded-delay regression control was not detected')
        report['delay_control'] = {'status': 'PASS', 'samples_seconds': delayed, 'median_effect_seconds': effect,
            'semantic_output_unchanged': True, 'minimum_control_effect_seconds': ROUNDS*0.001/2}
        report['summaries'] = [{
            'sdk_index': state['index'], 'workload': name,
            'wall': summarize([row for row in report['samples'] if row.get('workload') == name and
                row['sdk_index'] == state['index'] and not row.get('warmup')]),
            **({'work': summarize([row for row in report['samples'] if row.get('workload') == name and
                row['sdk_index'] == state['index'] and not row.get('warmup')], 'work_seconds')} if name.startswith('runtime-') else {})
        } for state in states for name in state['rows']]
        report['comparisons'] = []
        if len(states) == 2:
            for name in states[0]['rows']:
                rows = [next(row for row in report['summaries'] if row['sdk_index'] == index and row['workload'] == name)
                        for index in range(2)]
                field = 'work' if name.startswith('runtime-') else 'wall'
                before, after = [row[field]['median'] for row in rows]
                report['comparisons'].append({'workload': name, 'measurement': field,
                    'absolute_effect_seconds': after-before, 'candidate_over_baseline': after/before,
                    'decision': 'advisory-inconclusive-uncalibrated'})
        if host_context() != report['host'] or any(digest(Path(path)) != value for path, value in frozen.items()):
            raise ValueError('benchmark host/workload context changed')
        for prefix, prior in zip(prefixes, manifests):
            if verify_sdk(prefix)['seal_sha256'] != prior['seal_sha256']:
                raise ValueError('benchmark SDK changed during measurement')
        report.update(status='PASS', correctness='PASS')
    except (OSError, ValueError) as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        write_json(output / 'qualification.json', report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('output', 'sdk'):
        parser.add_argument('--'+name, type=Path, required=True)
    parser.add_argument('--baseline-sdk', type=Path)
    parser.add_argument('--repetitions', type=int, default=3)
    result = measure(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'timing_decision': result['timing_decision']}))


if __name__ == '__main__':
    main()
