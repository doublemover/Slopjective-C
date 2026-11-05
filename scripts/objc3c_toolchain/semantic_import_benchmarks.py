"""Separate source, PCH, source-free interface and incremental build measurements."""
from __future__ import annotations

import argparse
import hashlib
import json
import math
from pathlib import Path
import platform
import shutil
import sys
import time

from .binary_packages import digest, find_installed_shared_library, write_json
from .compiler_instrumentation import loaded_graph
from .llvm_ir_execution import StageFailure
from .package import verify_sdk
from .sanitizer_policy import require_clean_diagnostic
from .sdk_example import environment
from .semantic_benchmarks import SCALES, host_context, sample, summarize

SCHEMA = 'objc3c.semantic-import-benchmarks.v1'
MODULE, PACKAGE, SEED = 'bench.Provider', 'semantic-import-v1', 17
TRANSPORTS = ('source', 'pch', 'interface')


def sources(scale, *, bias=0):
    if scale not in SCALES or type(scale) is not int or type(bias) is not int or bias not in (0, 1):
        raise ValueError('unreviewed semantic import workload')
    names = ['bench_value_'+str(index) for index in range(scale)]
    header = '#pragma once\n'+''.join('Optional(int) '+name+'(int seed);\n' for name in names)
    provider = '#include "bench.h"\n'+''.join(
        'Optional(int) '+name+'(int seed) { return Some(int, seed + '+str(index+bias)+'); }\n'
        for index, name in enumerate(names))
    body = '#include <stdio.h>\nint main(void) { unsigned long long checksum = 0, completed = 0;\n'+''.join(
        'Optional(int) slot_'+str(index)+' = '+name+'('+str(SEED)+');\n'
        'guard let value_'+str(index)+' = slot_'+str(index)+' else { return 3; }\n'
        'if (value_'+str(index)+' != '+str(SEED+index)+') return 4;\n'
        'checksum += value_'+str(index)+'; ++completed;\n' for index, name in enumerate(names))
    body += ('if (checksum != '+str(expected(scale)['checksum'])+'ULL || completed != '+str(scale)+') return 4;\n'
             'printf("{\\"checksum\\":%llu,\\"completed_operations\\":%llu}\\n", checksum, completed);\n'
             'return 0; }\n')
    return {'header': header, 'provider': provider, 'body': body, 'exports': names}


def expected(scale):
    if type(scale) is not int or scale not in SCALES:
        raise ValueError('unreviewed semantic import scale')
    return {'checksum': sum(SEED+index for index in range(scale)), 'completed_operations': scale}


def semantic_output(text, scale):
    try:
        result = json.loads(text)
    except ValueError as error:
        raise ValueError('import benchmark omitted its completed semantic work') from error
    if (not isinstance(result, dict) or set(result) != set(expected(scale)) or
            any(type(value) is not int for value in result.values()) or result != expected(scale)):
        raise ValueError('import benchmark skipped a provider or returned incorrect semantic work')
    return result


def validate_reuse(report, stage):
    expected_reuse = {'cold': {MODULE: False, '@application': False},
                      'unchanged': {MODULE: True, '@application': True},
                      'changed-leaf': {MODULE: False, '@application': True}}[stage]
    nodes = report.get('nodes', {})
    if (report.get('schema') != 'objc3c.source-graph-build.v4' or report.get('status') != 'PASS' or
            set(nodes) != set(expected_reuse) or
            any(nodes[name].get('reused') is not reused for name, reused in expected_reuse.items())):
        raise ValueError('import benchmark source graph rebuilt/reused the wrong nodes: '+stage)
    return expected_reuse


def verify_report(report, root=None):
    """Recompute every required phase, sample, native oracle and summary."""
    repetitions, scales = report.get('repetitions'), report.get('scales', [])
    if (report.get('schema') != SCHEMA or report.get('status') != 'PASS' or
            report.get('correctness') != 'PASS' or report.get('workload_version') != 1 or
            report.get('timing_decision') != 'advisory-inconclusive-uncalibrated' or
            type(repetitions) is not int or not 3 <= repetitions <= 15 or not scales or
            len(scales) != len(set(scales)) or any(type(n) is not int or n not in SCALES for n in scales) or
            type(report.get('source_graph')) is not bool):
        raise ValueError('incomplete semantic import measurement contract')
    workloads = [{'scale': scale, 'expected': expected(scale),
        'generated_inputs_sha256': hashlib.sha256(json.dumps(sources(scale), sort_keys=True).encode()).hexdigest()}
        for scale in scales]
    if report.get('workloads') != workloads:
        raise ValueError('semantic import workload/generator identity changed')
    keys = {(scale, transport, phase) for scale in scales for transport in TRANSPORTS
            for phase in ('producer', 'consumer-first', 'consumer-repeated', 'link-first',
                          'link-repeated', 'execute-first', 'execute-repeated')}
    keys |= {(scale, 'pch', 'pch-creation') for scale in scales}
    if report['source_graph']:
        keys |= {(scale, 'source-graph', phase) for scale in scales
                 for phase in ('cold', 'unchanged', 'changed-leaf')}
    samples, summaries = report.get('samples', []), report.get('summaries', [])
    if (len(summaries) != len(keys) or
            {(row['scale'], row['transport'], row['phase']) for row in summaries} != keys or
            len({row.get('name') for row in samples}) != len(samples)):
        raise ValueError('semantic import phase/command inventory is incomplete')
    for row in samples:
        if (row.get('timed_out') is not False or type(row.get('exit_code')) is not int or
                type(row.get('expected_exit_code')) is not int or
                row['exit_code'] != row.get('expected_exit_code') or
                any(type(row.get(field)) not in (int, float) or not math.isfinite(row[field]) or row[field] <= 0
                    for field in ('wall_seconds', 'cpu_seconds', 'peak_process_rss_kib'))):
            raise ValueError('semantic import command failed or omitted its actual measurement')
        if root is not None:
            if Path(row['name']).name != row['name'] or row['name'] in ('.', '..'):
                raise ValueError('semantic import command log escaped its owner')
            log = Path(root) / 'logs' / (row['name']+'.log')
            if log.is_symlink() or not log.is_file() or digest(log) != row.get('log_sha256'):
                raise ValueError('semantic import command log changed after measurement')
            require_clean_diagnostic(log.read_text(errors='replace'))
    for scale, transport, phase in sorted(keys):
        rows = [row for row in samples if (row.get('scale'), row.get('transport'), row.get('phase')) ==
                (scale, transport, phase)]
        if (len(rows) != repetitions+1 or {row.get('iteration') for row in rows} != set(range(repetitions+1)) or
                any(type(row.get('iteration')) is not int or row.get('warmup') is not (row['iteration'] == 0) or
                    row['exit_code'] != 0 for row in rows)):
            raise ValueError('semantic import warmup/repetition set is incomplete')
        for row in rows:
            products = row.get('outputs', {})
            count = (0 if transport == 'source-graph' or phase.startswith('execute-') else
                     3 if transport == 'interface' and phase == 'producer' else
                     2 if transport == 'interface' and phase.startswith('consumer-') else 1)
            if (len(products) != count or any(type(value.get('bytes')) is not int or value['bytes'] <= 0 or
                    not isinstance(value.get('sha256'), str) or len(value['sha256']) != 64 or
                    any(char not in '0123456789abcdef' for char in value['sha256']) for value in products.values())):
                raise ValueError('semantic import omitted its fresh product inventory')
            if phase.startswith('execute-'):
                semantic_output(json.dumps(row.get('semantic_result')), scale)
                if root is not None and semantic_output((Path(root) / 'logs' / (row['name']+'.log')).read_text(), scale) != row['semantic_result']:
                    raise ValueError('semantic import native output differs from its raw log')
            if transport == 'interface' and phase.startswith('consumer-'):
                module = row.get('module', {})
                if (row.get('source_free') is not True or module.get('canonical_name') != MODULE or
                        module.get('package_identity') != PACKAGE or not module.get('semantic_fingerprint')):
                    raise ValueError('semantic import consumer did not prove source-free provider admission')
            if transport == 'source-graph':
                validate_reuse({'schema': 'objc3c.source-graph-build.v4', 'status': 'PASS',
                    'nodes': {name: {'reused': value} for name, value in row.get('reused', {}).items()}}, phase)
                if root is not None:
                    snapshot = Path(root) / f'graph-{scale}-{row["iteration"]}' / (phase+'-build-report.json')
                    if snapshot.is_symlink() or digest(snapshot) != row.get('build_report', {}).get('sha256'):
                        raise ValueError('semantic import incremental build report changed')
                    build = json.loads(snapshot.read_text())
                    validate_reuse(build, phase)
                    if (build.get('sdk_seal') != report['sdk']['seal_sha256'] or
                            build.get('modules', {}).get(MODULE, {}).get('semantic_fingerprint') != row.get('semantic_fingerprint')):
                        raise ValueError('semantic import incremental build used another provider or SDK')
        measured = [row for row in rows if row['warmup'] is False]
        summary = next(row for row in summaries if (row['scale'], row['transport'], row['phase']) ==
                       (scale, transport, phase))
        if any(summary.get(field) != summarize(measured, raw) for field, raw in (
                ('wall', 'wall_seconds'), ('cpu', 'cpu_seconds'), ('peak_process_rss_kib', 'peak_process_rss_kib'))):
            raise ValueError('semantic import summary differs from actual retained samples')
    controls = [row for row in samples if row.get('control')]
    expected_controls = {f'{scales[0]}-0-source-'+suffix: (kind, code) for suffix, kind, code in (
        ('wrong-provider-object', 'wrong-provider', 0), ('wrong-provider-link', 'wrong-provider', 0),
        ('wrong-provider-detected', 'wrong-provider', 4), ('missing-provider-detected', 'missing-provider', 1))}
    if (len(controls) != len(expected_controls) or
            {row['name']: (row['control'], row['exit_code']) for row in controls} != expected_controls or
            any('phase' in row and (row.get('scale'), row.get('transport'), row['phase']) not in keys for row in samples)):
        raise ValueError('semantic import wrong/missing-provider controls are incomplete')
    if root is not None:
        for scale in scales:
            retained = Path(root) / ('workload-source-'+str(scale)+'.json')
            if retained.is_symlink() or json.loads(retained.read_text()) != sources(scale):
                raise ValueError('semantic import retained source bytes differ from the generator')
        log = Path(root) / 'logs' / (f'{scales[0]}-0-source-missing-provider-detected.log')
        if 'bench_value_0' not in log.read_text():
            raise ValueError('semantic import missing-provider control failed for another reason')
    for scale in scales:
        for iteration in range(repetitions+1):
            graphs = [row for row in samples if row.get('scale') == scale and row.get('iteration') == iteration
                      and row.get('transport') == 'source-graph']
            if graphs and (len({row.get('semantic_fingerprint') for row in graphs}) != 1 or
                           not graphs[0].get('semantic_fingerprint')):
                raise ValueError('private leaf edit changed the public import identity')
    return summaries


class Measurement:
    def __init__(self, repo, output, sdk, manifest, report):
        self.repo, self.output, self.sdk = repo, output, sdk
        self.manifest, self.report, self.env = manifest, report, environment(sdk)
        self.deadline = time.monotonic() + report.get('budget_seconds', 600)
        self.compiler, self.facade = sdk / 'bin/clang', sdk / 'bin/objc3c'
        self.flags = ['--no-default-config', '--target='+manifest['target'], '-std=c17', '-O2',
            '-fobjc-version=3', '-fobjc-runtime=gnustep-2.2', '-fobjc-arc', '-fblocks',
            '-isystem', str(sdk / 'include')]

    def save(self):
        write_json(self.output / 'qualification.json', self.report)

    def run(self, name, argv, *, outputs=(), expected_exit=0, **context):
        paths = list(map(Path, outputs))
        if any(path.exists() or path.is_symlink() for path in paths):
            raise ValueError('import benchmark requires fresh compiler products')
        log = self.output / 'logs' / (name+'.log')
        if log.exists(): raise ValueError('duplicate import benchmark command')
        remaining = self.deadline-time.monotonic()
        if remaining <= 0: raise StageFailure(name, 'timeout', 'import benchmark total command budget expired')
        row = sample(argv, cwd=self.repo, environment=self.env, log=log, timeout=min(120, remaining))
        row.update(name=name, **context, expected_exit_code=expected_exit)
        self.report['samples'].append(row);self.save()
        if row['timed_out'] or row['exit_code'] != expected_exit:
            raise StageFailure(name, 'timeout' if row['timed_out'] else 'infrastructure', str(log))
        require_clean_diagnostic(log.read_text(errors='replace'))
        if any(not path.is_file() or not path.stat().st_size for path in paths):
            raise ValueError('import benchmark omitted a fresh compiler product')
        row['outputs'] = {str(path): {'sha256': digest(path), 'bytes': path.stat().st_size} for path in paths}
        self.save()
        return row

    def link(self, name, provider, consumer, binary, **context):
        return self.run(name, [self.sdk / 'bin/clang++', '--no-default-config', '-fuse-ld=lld',
            provider, consumer, self.sdk / 'lib/libobjc3_runtime_services.a', self.sdk / 'lib/libobjc.so',
            '-pthread', '-Wl,-rpath,'+str(self.sdk / 'lib'), '-o', binary], outputs=[binary], **context)

    def trial(self, scale, iteration):
        bundle = sources(scale)
        for transport in TRANSPORTS:
            name = f'{scale}-{iteration}-{transport}'
            root = self.output / name;root.mkdir()
            inputs = root / 'producer-inputs';inputs.mkdir()
            header, provider = inputs / 'bench.h', inputs / 'provider.m'
            header.write_text(bundle['header']);provider.write_text(bundle['provider'])
            consumer = root / 'consumer.m'
            consumer.write_text(('' if transport == 'pch' else '@import '+MODULE+';\n'
                                if transport == 'interface' else '#include "producer-inputs/bench.h"\n')+bundle['body'])
            context = dict(scale=scale, transport=transport, iteration=iteration, warmup=iteration == 0)
            po = root / 'provider.o'
            interface = root / (MODULE+'.objc3i')
            if transport == 'interface':
                self.run(name+'-producer', [self.facade, provider, '--objc3', '--arc',
                    '--target', self.manifest['target'], '--runtime', 'gnustep-2.2',
                    '--emit-object', po, '--emit-interface', interface, '--module', MODULE,
                    '--package', PACKAGE, '--module-owned-names', '--module-source-root', inputs,
                    *(part for declaration in bundle['exports'] for part in ('--export-declaration', declaration)),
                    '--', *self.flags], outputs=[po, interface, interface.with_suffix('.objc3i.json')],
                    phase='producer', **context)
                module = json.loads(interface.with_suffix('.objc3i.json').read_text())['module']
                if module['canonical_name'] != MODULE or module['package_identity'] != PACKAGE:
                    raise ValueError('import benchmark emitted another module identity')
                shutil.rmtree(inputs)
                if inputs.exists(): raise ValueError('interface benchmark retained provider sources')
            else:
                self.run(name+'-producer', [self.compiler, *self.flags, '-c', provider, '-o', po],
                         outputs=[po], phase='producer', **context)
                if transport == 'pch':
                    pch = root / 'bench.pch'
                    self.run(name+'-pch', [self.compiler, *self.flags, '-x', 'objective-c-header',
                        header, '-o', pch], outputs=[pch], phase='pch-creation', **context)
            for temperature in ('first', 'repeated'):
                co, binary = root / (temperature+'.o'), root / (temperature+'.bin')
                if transport == 'interface':
                    record = root / (temperature+'-dependencies.json')
                    command = [self.facade, consumer, '--objc3', '--arc', '--target', self.manifest['target'],
                        '--runtime', 'gnustep-2.2', '--source-imports', '--module-search-path', root,
                        '--import-module', MODULE, module['semantic_fingerprint'], PACKAGE,
                        '--emit-object', co, '--emit-dependency-record', record,
                        '--', *self.flags, '-fmodules-cache-path='+str(root / 'module-cache')]
                    products = [co, record]
                else:
                    command = [self.compiler, *self.flags,
                        *(['-include-pch', pch] if transport == 'pch' else []), '-c', consumer, '-o', co]
                    products = [co]
                row = self.run(name+'-'+temperature, command, outputs=products,
                               phase='consumer-'+temperature, **context)
                if transport == 'interface':
                    dependencies = json.loads(record.read_text())
                    modules = dependencies.get('modules', [])
                    if (dependencies.get('schema') != 'objc3c.dependency-record.v2' or len(modules) != 1 or
                            modules[0].get('fingerprint') != module['semantic_fingerprint'] or
                            modules[0].get('package') != PACKAGE or modules[0].get('runtime') != module['runtime_abi']):
                        raise ValueError('import benchmark consumer omitted its admitted provider')
                    row['source_free'] = not inputs.exists()
                    row['module'] = module
                self.link(name+'-'+temperature+'-link', po, co, binary, phase='link-'+temperature, **context)
                if 'runtime_loader' not in self.report['sdk']:
                    self.report['sdk']['runtime_loader'] = loaded_graph(binary,
                        Path('/lib64/ld-linux-x86-64.so.2').resolve(), [],
                        run=lambda suffix, argv: Path(self.run('runtime-'+suffix, argv)['log']).read_text())
                executed = self.run(name+'-'+temperature+'-execute', [binary], phase='execute-'+temperature, **context)
                executed['semantic_result'] = semantic_output(Path(executed['log']).read_text(), scale)
                self.save()
            if scale == self.report['scales'][0] and iteration == 0 and transport == 'source':
                mutant = root / 'wrong-provider.m';mutant.write_text(sources(scale, bias=1)['provider'])
                # The test-only wrong-code control keeps the ordinary declaration
                # contract and changes provider results, without UB or a crash.
                shutil.copyfile(header, root / 'bench.h')
                mutant_object, mutant_binary = root / 'wrong-provider.o', root / 'wrong-provider.bin'
                self.run(name+'-wrong-provider-object', [self.compiler, *self.flags, '-c', mutant, '-o', mutant_object],
                         outputs=[mutant_object], control='wrong-provider')
                self.link(name+'-wrong-provider-link', mutant_object, co, mutant_binary, control='wrong-provider')
                self.run(name+'-wrong-provider-detected', [mutant_binary], expected_exit=4, control='wrong-provider')
                missing = root / 'missing-provider.bin'
                self.run(name+'-missing-provider-detected', [self.sdk / 'bin/clang++', '--no-default-config',
                    '-fuse-ld=lld', co, self.sdk / 'lib/libobjc3_runtime_services.a', self.sdk / 'lib/libobjc.so',
                    '-pthread', '-Wl,-rpath,'+str(self.sdk / 'lib'),
                    '-o', missing], expected_exit=1, control='missing-provider')
                if missing.exists(): raise ValueError('missing provider linked an executable')
                if 'bench_value_0' not in (self.output / 'logs' / (name+'-missing-provider-detected.log')).read_text():
                    raise ValueError('missing-provider control failed for an unrelated link error')

    def source_graph(self, scale, iteration):
        root = self.output / f'graph-{scale}-{iteration}';root.mkdir()
        inputs, app = root / 'provider', root / 'app';inputs.mkdir();app.mkdir()
        bundle = sources(scale)
        (inputs / 'bench.h').write_text(bundle['header'])
        provider = inputs / 'provider.m';provider.write_text(bundle['provider'])
        (app / 'main.m').write_text('@import '+MODULE+';\n'+bundle['body'])
        package = {'schema': 'objc3c.source-package.v4', 'name': MODULE, 'package': PACKAGE,
            'source': 'provider.m', 'language': 'objective-c', 'standard': 'c17', 'ownership': 'arc',
            'provider_ownership': 'arc', 'exports': bundle['exports'], 'dependencies': []}
        write_json(inputs / 'package.json', package)
        workspace = root / 'workspace.json'
        write_json(workspace, {'schema': 'objc3c.source-workspace.v4', 'packages': ['provider/package.json'],
            'application': {'root': 'app', 'source': 'main.m', 'language': 'objective-c', 'standard': 'c17',
                'ownership': 'arc', 'dependencies': [{'name': MODULE, 'package': PACKAGE, 'visibility': 'private'}]}})
        build, fingerprint = root / 'build', None
        for stage in ('cold', 'unchanged', 'changed-leaf'):
            if stage == 'changed-leaf':
                provider.write_text(bundle['provider'].replace('seed + 0', 'seed + (0)'))
            row = self.run(f'graph-{scale}-{iteration}-{stage}', [sys.executable, '-B', '-m',
                'scripts.objc3c_toolchain.source_graph', '--sdk-root', self.sdk, '--workspace', workspace,
                '--output-dir', build], scale=scale, transport='source-graph', phase=stage,
                iteration=iteration, warmup=iteration == 0)
            report = json.loads((build / 'build-report.json').read_text())
            row['reused'] = validate_reuse(report, stage)
            if report.get('sdk_seal') != self.manifest['seal_sha256']:
                raise ValueError('source graph benchmark used another sealed SDK')
            current = report['modules'][MODULE]['semantic_fingerprint']
            if fingerprint is not None and fingerprint != current:
                raise ValueError('private leaf implementation edit changed the public import identity')
            fingerprint = current
            row['semantic_fingerprint'] = current
            snapshot = root / (stage+'-build-report.json');write_json(snapshot, report)
            row['build_report'] = {'path': str(snapshot), 'sha256': digest(snapshot)}
            self.save()


def measure(repo, output, *, sdk, repetitions=3, scales=(32,), source_graph=False, budget_seconds=600):
    repo, sdk, output = Path(repo).resolve(strict=True), Path(sdk).resolve(strict=True), Path(output).resolve()
    scales = tuple(scales)
    if (platform.system() != 'Linux' or platform.machine() != 'x86_64' or output.exists() or
            not output.is_relative_to(repo / 'out') or type(repetitions) is not int or not 3 <= repetitions <= 15 or
            not scales or len(scales) != len(set(scales)) or any(type(n) is not int or n not in SCALES for n in scales) or
            type(budget_seconds) is not int or not 60 <= budget_seconds <= 3600 or
            sdk.is_relative_to(output) or output.is_relative_to(sdk)):
        raise ValueError('import measurement requires fresh owned Linux output, reviewed scales and 3..15 repetitions')
    manifest = verify_sdk(sdk)
    if manifest['target'] != 'x86_64-unknown-linux-gnu': raise ValueError('unsupported import benchmark SDK target')
    for name in ('bin/clang', 'bin/clang++', 'bin/objc3c', 'bin/ld.lld', 'lib/libobjc.so',
                 'lib/libobjc3_runtime_services.a'):
        if not (sdk / name).is_file(): raise ValueError('incomplete import benchmark SDK: '+name)
    started = time.monotonic()
    output.mkdir(parents=True);(output / 'logs').mkdir()
    report = {'schema': SCHEMA, 'status': 'RUNNING', 'correctness': 'RUNNING', 'workload_version': 1,
        'timing_decision': 'advisory-inconclusive-uncalibrated', 'repetitions': repetitions, 'scales': list(scales),
        'host': host_context(), 'sdk': {key: manifest[key] for key in ('seal_sha256', 'source_build_id', 'target')},
        'source_graph': source_graph, 'budget_seconds': budget_seconds, 'samples': [], 'summaries': [],
        'inputs': {str(Path(__file__)): digest(Path(__file__))},
        'workloads': [{'scale': scale, 'expected': expected(scale),
            'generated_inputs_sha256': hashlib.sha256(json.dumps(sources(scale), sort_keys=True).encode()).hexdigest()}
            for scale in scales],
        'measurement': {'producer': 'Fresh provider object; interface serialization included only for interface transport.',
            'consumer': 'First/repeated imports use fresh objects. PCH creation and links/execution are separate.',
            'cache': 'Fresh trial directories and module caches; warm shared filesystem caches; no compiler cache launcher.',
            'pch': 'Original header remains for Clang PCH validation; only interface transport removes provider sources.',
            'source_graph': 'Cold build, unchanged incremental build, and private-leaf rebuild; each still links and executes.',
            'rss': 'Largest individual command process and waited descendants; not aggregate tree memory.',
            'policy': 'No new CI cadence or performance threshold; measure cost before lane expansion.'}}
    owner = Measurement(repo, output, sdk, manifest, report)
    try:
        for scale in scales:
            write_json(output / ('workload-source-'+str(scale)+'.json'), sources(scale))
        libraries = [found for name in ('libLLVM.so', 'libclang-cpp.so')
                     if (found := find_installed_shared_library(sdk / 'lib', name)) is not None]
        report['sdk']['loaders'] = {}
        for name in ('clang', 'objc3c'):
            report['sdk']['loaders'][name] = loaded_graph(sdk / 'bin' / name,
                Path('/lib64/ld-linux-x86-64.so.2').resolve(), libraries,
                run=lambda suffix, argv, name=name: Path(owner.run('loader-'+name+'-'+suffix, argv)['log']).read_text())
        report['sdk']['instrumentation'] = {}
        for name, path in [('compiler', libraries[-1] if libraries else sdk / 'bin/clang'),
                           ('runtime', sdk / 'lib/libobjc3_runtime_services.a')]:
            row = owner.run('instrumentation-'+name, ['/usr/bin/nm',
                *(['-D'] if path in libraries else []), '--undefined-only', path])
            text = Path(row['log']).read_text()
            report['sdk']['instrumentation'][name] = [kind for kind in ('asan', 'ubsan', 'tsan', 'llvm_profile')
                                                       if '__'+kind in text]
        for iteration in range(repetitions+1):
            for scale in scales:
                owner.trial(scale, iteration)
                if source_graph: owner.source_graph(scale, iteration)
        keys = {(row['scale'], row['transport'], row['phase']) for row in report['samples'] if 'phase' in row}
        for scale, transport, phase in sorted(keys):
            rows = [row for row in report['samples'] if row.get('scale') == scale and
                    row.get('transport') == transport and row.get('phase') == phase and row.get('warmup') is False]
            if len(rows) != repetitions: raise ValueError('incomplete import benchmark repetition inventory')
            report['summaries'].append({'scale': scale, 'transport': transport, 'phase': phase,
                'wall': summarize(rows), 'cpu': summarize(rows, 'cpu_seconds'),
                'peak_process_rss_kib': summarize(rows, 'peak_process_rss_kib')})
        if host_context() != report['host'] or verify_sdk(sdk)['seal_sha256'] != manifest['seal_sha256']:
            raise ValueError('import benchmark host or sealed SDK changed')
        if any(digest(Path(path)) != value for graph in [*report['sdk']['loaders'].values(),
                                                        report['sdk']['runtime_loader']]
               for path, value in graph['libraries'].items()):
            raise ValueError('import benchmark loader inputs changed')
        if report['inputs'][str(Path(__file__))] != digest(Path(__file__)):
            raise ValueError('import benchmark generator changed during measurement')
        report.update(status='PASS', correctness='PASS')
        verify_report(report, output)
    except BaseException as error:
        report.update(status='FAIL', correctness='FAIL', error=str(error));raise
    finally:
        report['elapsed_seconds'] = time.monotonic()-started
        report['measured_cost'] = {'commands': len(report['samples']),
            'command_wall_seconds': sum(row['wall_seconds'] for row in report['samples']),
            'retained_product_and_log_bytes': sum(path.stat().st_size for path in output.rglob('*') if path.is_file())}
        owner.save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument('--sdk', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--repetitions', type=int, default=3)
    parser.add_argument('--scale', dest='scales', type=int, choices=SCALES, action='append')
    parser.add_argument('--source-graph', action='store_true')
    parser.add_argument('--budget-seconds', type=int, default=600)
    args = vars(parser.parse_args());args['scales'] = args['scales'] or (32,)
    report = measure(**args)
    print(json.dumps({'status': report['status'], 'timing_decision': report['timing_decision']}))


if __name__ == '__main__':
    main()
