"""Advisory Linux ObjC3 frontend and sustained runtime workload measurements."""
from __future__ import annotations

import argparse
import json
import math
import os
from pathlib import Path
import platform
import statistics
import subprocess
import time

from .binary_packages import digest, find_installed_shared_library, write_json
from .package import SCHEMA, _digest, inventory, verify_sdk
import shutil
from .compiler_instrumentation import loaded_graph
from .semantic_runtime_oracles import (CURRENT_WORKLOAD_VERSION, FAMILIES, OWNERSHIP_FAMILIES,
    expected_work, ownership_flags, runtime_workload_result)
from .llvm_ir_execution import _kill_session
from .sanitizer_policy import require_clean_diagnostic

SCALES = (32, 256, 2048)
ROUNDS, SEED = 16, 17


def workload_names(version=1):
    if type(version) is not int or version not in (1, CURRENT_WORKLOAD_VERSION):
        raise ValueError('unreviewed semantic workload version')
    return {phase+'-'+str(scale) for phase in ('syntax', 'object', 'runtime') for scale in SCALES} | (
        {'runtime-'+family+'-'+str(scale) for family in FAMILIES for scale in SCALES}
        if version == CURRENT_WORKLOAD_VERSION else set())


def semantic_result(text, workload):
    name, scale = workload.rsplit('-', 1)
    return (runtime_result(text, int(scale)) if name == 'runtime' else
            runtime_workload_result(text, name.removeprefix('runtime-'), int(scale)))


def workload_inventory(version=CURRENT_WORKLOAD_VERSION):
    rows = []
    for name in sorted(workload_names(version)):
        phase, scale = name.rsplit('-', 1);scale = int(scale)
        runtime = phase.startswith('runtime')
        family = phase.removeprefix('runtime-')
        rows.append({'id': name, 'scale': scale, 'phase': 'native-execute' if runtime else phase,
            'source': ('tests/benchmarks/collections.cpp' if phase == 'runtime' else
                       'tests/benchmarks/ownership_workloads.mm' if family in OWNERSHIP_FAMILIES else
                       'tests/benchmarks/runtime_workloads.cpp' if runtime else 'frontend_source(scale)'),
            'base_compiler_flags': ['--no-default-config', '-std=c++20', '-O2'], 'timeout_seconds': 120,
            'frontend_flags': ownership_flags(family) if family in OWNERSHIP_FAMILIES else [] if runtime else ['-x', 'objective-c++', '-fobjc-version=3',
                '-fobjc-runtime=gnustep-2.2', '-fobjc-arc', '-fblocks'],
            'ownership': family.split('-', 1)[0] if family in OWNERSHIP_FAMILIES else None,
            'command_identity': 'Each sample retains complete argv, working directory and SDK/resource/loader identities.',
            'cache_state': 'fresh output artifacts; warm shared filesystem cache; no compiler cache launcher',
            'measurement': ('steady-state shared-pool tasks including frame teardown; canonical pool initialization/probe outside interval'
                if family == 'shared-pool-tasks' else 'steady-clock semantic work including teardown'
                if runtime else 'compiler command wall time'),
            'runtime_threads': 5 if family in ('worker-tasks', 'shared-pool-tasks') else 1 if runtime else None,
            'worker_threads': 4 if family in ('worker-tasks', 'shared-pool-tasks') else 0,
            'rounds': ROUNDS if runtime else None,
            'seed': SEED if runtime and family != 'text' else None,
            'maximum_admitted_task_batch': 256 if family in (
                'task-group-actor', 'task-group-cancel', 'worker-tasks', 'shared-pool-tasks') else None,
            'throughput': 'completed task callbacks per second including allocation, join and frame teardown'
                if family in ('task-group-actor', 'task-group-cancel', 'worker-tasks', 'shared-pool-tasks') else None,
            'expected': (expected_runtime(scale) if phase == 'runtime' else
                         expected_work(family, scale) if runtime else {'compiler_exit_code': 0}),
            'oracle': 'independent checksum, completed-work and lifetime accounting' if runtime else
                      'successful syntax check and fresh nonempty object for object phase'})
    return rows


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
            try:
                while True:
                    child, status, usage = os.wait4(process.pid, os.WNOHANG)
                    if child:
                        process.returncode = os.waitstatus_to_exitcode(status)
                        break
                    if time.monotonic()-started >= timeout:
                        timed_out = True
                        _kill_session(process.pid)
                        _, status, usage = os.wait4(process.pid, 0)
                        process.returncode = os.waitstatus_to_exitcode(status)
                        break
                    time.sleep(0.002)
            except BaseException:
                _kill_session(process.pid)
                process.wait()
                raise
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
    if any(type(value) not in (int, float) or not math.isfinite(value) or value <= 0 for value in values):
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
        'container': Path('/.dockerenv').exists(),
        'runtime_threads': 'Per-workload inventory: one caller and zero or four scheduling workers.',
        'noise_limits': 'Shared runner; frequency/contention uncontrolled. Timing remains advisory.'}


def prepare_prefix(stage, prefix, *, source_build_id, target, imports=False):
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
    if imports:
        link(stage / 'bin/objc3c', prefix / 'bin/objc3c')
        shutil.copytree(stage / 'include/objc', prefix / 'include/objc', copy_function=os.link)
        verifier = prefix / 'share/objc3c/verify-sdk.py'
        verifier.parent.mkdir(parents=True)
        shutil.copyfile(Path(__file__).with_name('package.py'), verifier)
    record = {'schema': SCHEMA, 'source_build_id': source_build_id, 'target': target,
        'purpose': 'Development-only semantic workload measurement; no SDK archive qualification', 'files': inventory(prefix)}
    record['seal_sha256'] = _digest(record)
    write_json(prefix / 'objc3-sdk.json', record);(prefix / 'objc3-sdk.json').chmod(0o644)
    return verify_sdk(prefix)


def measure(repo, output, *, sdk, baseline_sdk=None, repetitions=3, extended_runtime=False):
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
    fixtures = [Path(__file__), fixture]
    if extended_runtime:
        fixtures += [repo / 'tests/benchmarks/runtime_workloads.cpp',
                     repo / 'tests/benchmarks/ownership_workloads.mm',
                     Path(__file__).with_name('semantic_runtime_oracles.py')]
    frozen = {str(path): digest(path) for path in fixtures}
    report = {'schema': 'objc3c.semantic-benchmarks.v1', 'status': 'running', 'host': host_context(),
        'workload_version': CURRENT_WORKLOAD_VERSION if extended_runtime else 1, 'inputs': frozen,
        'workloads': workload_inventory(CURRENT_WORKLOAD_VERSION if extended_runtime else 1),
        'repetitions': repetitions, 'samples': [], 'order': [],
        'scope': 'Warm frontend syntax/object generation and sustained concrete i32 map creation/insertion/lookup/count/teardown. No cold build/filesystem, archive or full language claim.',
        'timing_decision': 'advisory-inconclusive-uncalibrated', 'correctness': 'running',
        'cache_state': 'One unmeasured warmup per SDK/row; fresh output artifacts, warm shared OS caches; compiler cache launchers absent.',
        'sdks': [{'path': str(prefix), 'seal_sha256': value['seal_sha256'], 'source_build_id': value['source_build_id'],
            'purpose': value.get('purpose', 'sealed SDK')} for prefix, value in zip(prefixes, manifests)]}
    if extended_runtime:
        report['scope'] += (' Extended runtime: UTF-8 builder/scalar/byte/slice/concat/search and teardown; '
            'generic-array bounded mutation and copy/destruction accounting; '
            'manual-executor actor suspension and dependency cancellation through group/frame quiescence; '
            'four private worker executors running suspended tasks through joined completion. '
            'Private-worker timings include executor creation/shutdown; canonical shared-pool startup is outside its work interval. '
            'ARC/MRC object allocation, explicit retained aliases, copied Block capture, invocation, destruction and weak zeroing. '
            'No compiler-generated async frame claim.')
    states = []
    def run(label, args, state):
        log = output / (label+'.log')
        row = sample(args, cwd=state['root'], environment=state['environment'], log=log)
        row = {'name': label, 'sdk_index': state['index'], **row}
        report['samples'].append(row)
        write_json(output / 'qualification.json', report)
        if row['timed_out'] or row['exit_code']:
            raise ValueError('semantic benchmark command failed: '+label)
        require_clean_diagnostic(log.read_text(errors='replace'))
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
                                ('libobjc3_runtime_services.a', prefix / 'lib/libobjc3_runtime_services.a'),
                                ('libobjc.so', prefix / 'lib/libobjc.so')):
                probe = run(f'sdk-{index}-instrumentation-'+name.replace('.', '-'),
                    ['/usr/bin/nm', *(['-D'] if name in ('libclang-cpp.so', 'libobjc.so') and probe != compiler else []),
                        '--undefined-only', probe], state)
                text = Path(probe['log']).read_text()
                markers[name] = sorted({kind for kind in ('asan', 'ubsan', 'tsan', 'llvm_profile') if '__'+kind in text})
            state['instrumentation'] = markers
            report['sdks'][index].update(instrumentation=markers, environment=environment,
                                         compiler_instrumentation_probe=str(compiler_probe))
            if markers['libobjc3_runtime_services.a'] or markers['libobjc.so']:
                raise ValueError('ordinary runtime timing cannot use an instrumented service archive or Objective-C runtime')
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
            if extended_runtime:
                extra_source = root / 'runtime_workloads.cpp'
                extra_source.write_bytes((repo / 'tests/benchmarks/runtime_workloads.cpp').read_bytes())
                extra_obj, extra_binary = root / 'runtime_workloads.o', root / 'runtime_workloads'
                run(f'sdk-{index}-extended-runtime-object', [compiler, *flags, '-I'+str(prefix / 'include'),
                    '-c', extra_source, '-o', extra_obj], state)
                run(f'sdk-{index}-extended-runtime-link', [compiler, '--no-default-config', '-fuse-ld=lld',
                    extra_obj, prefix / 'lib/libobjc3_runtime_services.a', prefix / 'lib/libobjc.so',
                    '-pthread', '-Wl,-rpath,'+str(prefix / 'lib'), '-o', extra_binary], state)
                report['sdks'][index]['extended_runtime_loader'] = loaded_graph(extra_binary,
                    Path('/lib64/ld-linux-x86-64.so.2').resolve(), [],
                    run=lambda name, argv: Path(run('sdk-'+str(index)+'-extended-runtime-'+name,
                                                   argv, state)['log']).read_text())
                frozen.update(report['sdks'][index]['extended_runtime_loader']['libraries'])
                ownership_source = root / 'ownership_workloads.mm'
                ownership_source.write_bytes((repo / 'tests/benchmarks/ownership_workloads.mm').read_bytes())
                ownership_binaries = {}
                report['sdks'][index]['ownership_loaders'] = {}
                for family in OWNERSHIP_FAMILIES:
                    obj, owned_binary = root / (family+'.o'), root / family
                    run(f'sdk-{index}-{family}-object', [compiler, *flags, *ownership_flags(family),
                        '-I'+str(prefix / 'include'), '-c', ownership_source, '-o', obj], state)
                    run(f'sdk-{index}-{family}-link', [compiler, '--no-default-config', '-fuse-ld=lld',
                        obj, prefix / 'lib/libobjc.so', '-pthread', '-Wl,-rpath,'+str(prefix / 'lib'),
                        '-o', owned_binary], state)
                    graph = loaded_graph(owned_binary, Path('/lib64/ld-linux-x86-64.so.2').resolve(), [],
                        required=((prefix / 'lib/libobjc.so').resolve(strict=True),),
                        run=lambda name, argv, family=family: Path(run(f'sdk-{index}-{family}-'+name, argv, state)['log']).read_text())
                    report['sdks'][index]['ownership_loaders'][family] = graph
                    frozen.update(graph['libraries']);ownership_binaries[family] = owned_binary
                for family in FAMILIES:
                    for scale in SCALES:
                        state['rows']['runtime-'+family+'-'+str(scale)] = (
                            [ownership_binaries[family], str(scale), str(ROUNDS), str(SEED)]
                            if family in OWNERSHIP_FAMILIES else
                            [extra_binary, family, str(scale), str(ROUNDS), str(SEED)])
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
                    if name.startswith('object-'):
                        # A zero-exit compiler that writes nothing cannot reuse
                        # the previous repetition's object as completed work.
                        Path(command[-1]).unlink(missing_ok=True)
                    row = run(f'{iteration}-sdk-{index}-'+name, command, state)
                    row.update(workload=name, warmup=iteration == 0)
                    if name.startswith('runtime-'):
                        row['semantic_result'] = semantic_result(Path(row['log']).read_text(), name)
                        row['work_seconds'] = row['semantic_result']['nanoseconds']/1e9
                        if row['semantic_result'].get('callbacks', 0):
                            row['task_completions_per_second'] = row['semantic_result']['callbacks']/row['work_seconds']
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
        if extended_runtime:
            report['runtime_delay_controls'] = []
            for family in FAMILIES:
                workload = 'runtime-'+family+'-32'
                delayed = []
                for repetition in range(repetitions):
                    row = run(f'delay-control-{family}-{repetition}',
                              [*state['rows'][workload], '1'], state)
                    delayed.append(semantic_result(Path(row['log']).read_text(), workload)['nanoseconds']/1e9)
                ordinary = [row['work_seconds'] for row in report['samples']
                    if row.get('workload') == workload and row['sdk_index'] == state['index'] and
                    row.get('warmup') is False]
                effect = statistics.median(delayed)-statistics.median(ordinary)
                if effect < ROUNDS*0.001/2:
                    raise ValueError('extended runtime delay control was not detected: '+family)
                report['runtime_delay_controls'].append({'family': family, 'status': 'PASS',
                    'samples_seconds': delayed, 'median_effect_seconds': effect,
                    'semantic_output_unchanged': True, 'minimum_control_effect_seconds': ROUNDS*0.001/2})
        report['summaries'] = [{
            'sdk_index': state['index'], 'workload': name,
            'wall': summarize([row for row in report['samples'] if row.get('workload') == name and
                row['sdk_index'] == state['index'] and not row.get('warmup')]),
            **({'work': summarize([row for row in report['samples'] if row.get('workload') == name and
                row['sdk_index'] == state['index'] and not row.get('warmup')], 'work_seconds')} if name.startswith('runtime-') else {}),
            **({'task_completions_per_second': summarize([row for row in report['samples']
                if row.get('workload') == name and row['sdk_index'] == state['index'] and
                row.get('warmup') is False], 'task_completions_per_second')}
                if name.startswith(('runtime-task-', 'runtime-worker-', 'runtime-shared-')) else {})
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
    parser.add_argument('--extended-runtime', action='store_true')
    result = measure(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'timing_decision': result['timing_decision']}))


if __name__ == '__main__':
    main()
