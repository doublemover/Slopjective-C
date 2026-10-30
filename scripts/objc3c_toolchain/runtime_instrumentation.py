"""Instrument the canonical runtime CMake target and verify every owned object.

Compiler implementation instrumentation is a separate subject. This owner
builds services with the ordinary native compiler and selects a matching pinned
GNUstep dependency; it never stages a compiler or publishes an SDK archive.
"""
from __future__ import annotations

from dataclasses import dataclass
import fcntl
import hashlib
import json
import os
import re
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
import time

from .binary_packages import digest, disjoint, write_json
from .sanitizer_policy import strict_environment


@dataclass(frozen=True)
class RuntimeProfile:
    archive: Path
    objc_runtime: Path
    receipt: Path
    receipt_sha256: str


def _tools(compiler_root: Path, resource: Path):
    paths = {name: Path(shutil.which(name) or '').resolve(strict=True) for name in ('cmake', 'ninja')}
    paths.update(compiler=compiler_root / 'bin/clang++',
                 symbolizer=resource.parents[2] / 'bin/llvm-symbolizer')
    for name in ('nm', 'ar'):
        filename = 'llvm-'+name
        selected = compiler_root / 'bin' / filename
        paths[name] = selected if selected.is_file() else resource.parents[2] / 'bin' / filename
    if any(not path.is_file() for path in paths.values()):
        raise ValueError('runtime instrumentation requires explicit CMake, Ninja and LLVM tools')
    return {name: path.resolve(strict=True) for name, path in paths.items()}


def _matching_objc_runtime(path: Path, sanitizer: str, nm: Path, *, environment: dict) -> dict:
    path = path.resolve(strict=True)
    receipt = path.parents[2] / 'build-report.json'
    report = json.loads(receipt.read_text())
    if (report.get('status') != 'passed' or report.get('configuration', {}).get('sanitizer') != sanitizer or
            (Path(report['stage']) / 'lib/libobjc.so').resolve(strict=True) != path):
        raise ValueError('GNUstep dependency does not have the matching qualified instrumentation profile')
    symbols = subprocess.check_output([nm, '-D', '--undefined-only', path],
                                      env=environment, text=True, timeout=30)
    prefix = '__tsan_' if sanitizer == 'thread' else '__asan_'
    if prefix not in symbols or (sanitizer == 'address,undefined' and '__ubsan_' not in symbols):
        raise ValueError('GNUstep dependency has no evidence of the requested instrumentation')
    return {'path': str(path), 'sha256': digest(path), 'build_receipt': str(receipt),
            'build_receipt_sha256': digest(receipt), 'undefined_symbol_evidence_sha256':
            hashlib.sha256(symbols.encode()).hexdigest()}


class RuntimeProfiles:
    """One immutable set per qualification; each configuration has its own tree."""
    def __init__(self, repo: Path, compiler_root: Path, resource: Path, output: Path, *, jobs=2):
        self.repo, self.compiler_root, self.resource = (path.resolve(strict=True) for path in (repo, compiler_root, resource))
        self.output = output.resolve()
        self.jobs = jobs
        if jobs < 1:
            raise ValueError('positive runtime build parallelism is required')
        disjoint(self.output, self.repo / 'native', self.compiler_root, self.resource)
        self.tools = _tools(self.compiler_root, self.resource)
        self.environment = strict_environment(self.tools['compiler'], self.tools['symbolizer'])
        self.environment['PATH'] = os.pathsep.join(dict.fromkeys([
            str(self.tools['cmake'].parent), str(self.tools['ninja'].parent),
            *self.environment['PATH'].split(os.pathsep)]))
        self.profiles = {}
        self.dependencies = {}

    def _dependency(self, sanitizer, requested):
        if sanitizer in self.dependencies:
            dependency = self.dependencies[sanitizer]
            if requested is not None and requested.resolve(strict=True) != dependency:
                raise ValueError('one runtime profile set cannot change its GNUstep dependency')
            return dependency
        if requested is not None:
            dependency = requested.resolve(strict=True)
        else:
            self.output.mkdir(parents=True, exist_ok=True)
            label = sanitizer.replace(',', '-')
            report = self.output / ('gnustep-'+label+'.json')
            log = self.output / ('gnustep-'+label+'.log')
            # All callers share the content-addressed dependency builder. Keep
            # one Ninja/configuration writer in that tree across whole jobs.
            lock = self.repo / 'out/.objc3-gnustep-instrumentation.lock'
            lock.parent.mkdir(parents=True, exist_ok=True)
            with lock.open('a') as held, log.open('w') as stream:
                fcntl.flock(held, fcntl.LOCK_EX)
                result = subprocess.run([sys.executable, '-B', '-m',
                    'scripts.objc3c_toolchain_runtime.build_linux', '--repo', self.repo,
                    '--compiler-root', self.resource.parents[2], '--sanitizer', sanitizer,
                    '--jobs', str(self.jobs), '--report-file', report], cwd=self.repo,
                    env=self.environment, stdout=stream, stderr=subprocess.STDOUT, timeout=600)
            if result.returncode:
                raise ValueError('matching GNUstep instrumentation build failed; inspect '+str(log))
            dependency = Path(json.loads(report.read_text())['stage']) / 'lib/libobjc.so'
        _matching_objc_runtime(dependency, sanitizer, self.tools['nm'], environment=self.environment)
        self.dependencies[sanitizer] = dependency.resolve(strict=True)
        return self.dependencies[sanitizer]

    def get(self, sanitizer: str, optimization: str, *, objc_runtime: Path | None = None) -> RuntimeProfile:
        if sanitizer not in ('address,undefined', 'thread') or optimization not in ('O0', 'O1', 'O2'):
            raise ValueError('unsupported owned runtime instrumentation profile')
        key = sanitizer, optimization
        if key in self.profiles:
            profile = self.profiles[key]
            if objc_runtime is not None and objc_runtime.resolve(strict=True) != profile.objc_runtime:
                raise ValueError('cached runtime cannot change its native runtime dependency')
            if digest(profile.receipt) != profile.receipt_sha256:
                raise ValueError('runtime instrumentation receipt changed')
            report = json.loads(profile.receipt.read_text())
            if digest(profile.archive) != report['archive_sha256'] or any(
                    digest(Path(path)) != value for path, value in report['inputs'].items()):
                raise ValueError('runtime instrumentation inputs or artifact changed')
            return profile
        dependency = self._dependency(sanitizer, objc_runtime)
        output = self.output / (sanitizer.replace(',', '-')+'-'+optimization)
        output.mkdir(parents=True, exist_ok=False)
        build = output / 'build'
        query = build / '.cmake/api/v1/query'
        query.mkdir(parents=True)
        (query / 'codemodel-v2').touch()
        source = self.repo / 'native/objc3c/src/runtime'
        inputs = [path for path in source.rglob('*') if path.is_file() and
                  (path.suffix in ('.cpp', '.h', '.hpp', '.mm') or path.name == 'CMakeLists.txt')]
        inputs += [Path(__file__), *self.tools.values()]
        lock = self.repo / 'out/.objc3-gnustep-instrumentation.lock'
        lock.parent.mkdir(parents=True, exist_ok=True)
        with lock.open('a') as held:
            fcntl.flock(held, fcntl.LOCK_EX)
            dependency_evidence = _matching_objc_runtime(dependency, sanitizer, self.tools['nm'], environment=self.environment)
            original_receipt = Path(dependency_evidence['build_receipt'])
            captured = original_receipt.read_bytes()
            if hashlib.sha256(captured).hexdigest() != dependency_evidence['build_receipt_sha256']:
                raise ValueError('GNUstep build receipt changed while capturing provenance')
            snapshot = output / 'gnustep-build-receipt.json'
            snapshot.write_bytes(captured)
            dependency_evidence['original_build_receipt'] = str(original_receipt)
            dependency_evidence['build_receipt'] = str(snapshot)
        inputs += [dependency, snapshot]
        report = {'schema': 'objc3c.owned-runtime-instrumentation.v1', 'status': 'running',
                  'subject': 'owned-services-and-native-runtime-dependency', 'sanitizer': sanitizer,
                  'optimization': optimization, 'compiler_implementation_instrumented': False,
                  'inputs': {str(path): digest(path) for path in inputs},
                  'native_runtime': dependency_evidence, 'commands': [], 'objects': []}
        receipt = output / 'qualification.json'

        def run(name, command):
            log = output / (name+'.log')
            started = time.monotonic()
            with log.open('w') as stream:
                result = subprocess.run(list(map(str, command)), env=self.environment,
                    stdout=stream, stderr=subprocess.STDOUT, timeout=600)
            report['commands'].append({'name': name, 'argv': list(map(str, command)),
                'exit_code': result.returncode, 'elapsed_seconds': round(time.monotonic()-started, 6),
                'log': str(log), 'log_sha256': digest(log)})
            write_json(receipt, report)
            if result.returncode:
                raise ValueError('instrumented runtime '+name+' failed; inspect '+str(log))
            return log.read_text()

        try:
            flags = ' '.join(shlex.quote(value) for value in [
                '-'+optimization, '-g', '-fsanitize='+sanitizer, '-fno-sanitize-recover=undefined',
                '-fno-omit-frame-pointer', '-resource-dir', str(self.resource), '-fuse-ld=lld'])
            run('configure', [self.tools['cmake'], '-S', source, '-B', build, '-G', 'Ninja',
                '-DBUILD_TESTING=OFF', '-DCMAKE_BUILD_TYPE=RelWithDebInfo',
                '-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
                '-DCMAKE_MAKE_PROGRAM='+str(self.tools['ninja']),
                '-DCMAKE_CXX_COMPILER='+str(self.tools['compiler']),
                '-DCMAKE_CXX_COMPILER_ARG1=--driver-mode=g++', '-DCMAKE_CXX_FLAGS='+flags])
            run('build', [self.tools['cmake'], '--build', build, '--target',
                          'objc3_runtime_services', '--parallel', str(self.jobs)])
            reply = build / '.cmake/api/v1/reply'
            indexes = list(reply.glob('index-*.json'))
            if len(indexes) != 1:
                raise ValueError('runtime File API reply is not from one fresh configuration')
            index = json.loads(indexes[0].read_text())
            codemodel = json.loads((reply / index['reply']['codemodel-v2']['jsonFile']).read_text())
            configuration, = codemodel['configurations']
            target_entry, = [target for target in configuration['targets'] if target['name'] == 'objc3_runtime_services']
            target = json.loads((reply / target_entry['jsonFile']).read_text())
            owned = {(source / item['path']).resolve(strict=True) for item in target['sources']
                     if 'compileGroupIndex' in item}
            commands = json.loads((build / 'compile_commands.json').read_text())
            if not owned or {Path(command['file']).resolve(strict=True) for command in commands} != owned:
                raise ValueError('runtime compile commands omit or add canonical target sources')
            for command in commands:
                argv = command.get('arguments') or shlex.split(command['command'])
                effective_optimization = [arg[1:] for arg in argv if re.fullmatch(r'-O(?:[0-3szg]|fast)', arg)]
                if ('-fsanitize='+sanitizer not in argv or effective_optimization != [optimization] or
                        any(arg.startswith('-fno-sanitize=') for arg in argv)):
                    raise ValueError('owned runtime translation unit lacks its instrumentation/optimization')
                object_path = Path(command['directory']) / (command.get('output') or argv[argv.index('-o')+1])
                symbols = run('object-'+str(len(report['objects']))+'-symbols',
                              [self.tools['nm'], '--undefined-only', object_path])
                prefix = '__tsan_' if sanitizer == 'thread' else '__asan_'
                if prefix not in symbols or (sanitizer == 'address,undefined' and '__ubsan_' not in symbols):
                    raise ValueError('owned object has no requested instrumentation symbols: '+command['file'])
                report['objects'].append({'source': command['file'], 'object': str(object_path),
                    'argv': argv, 'sha256': digest(object_path),
                    'instrumentation_symbols': sorted({line.split()[-1] for line in symbols.splitlines() if prefix in line or '__ubsan_' in line})})
            archive = build / 'libobjc3_runtime_services.a'
            members = run('archive-members', [self.tools['ar'], 't', archive]).splitlines()
            expected_members = {Path(obj['object']).name: obj['sha256'] for obj in report['objects']}
            if (len(members) != len(owned) or len(set(members)) != len(members) or
                    set(members) != set(expected_members)):
                raise ValueError('runtime archive does not contain exactly the canonical owned objects')
            for member, expected in expected_members.items():
                command = [self.tools['ar'], 'p', archive, member]
                started = time.monotonic()
                extracted = subprocess.run(command, env=self.environment, stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE, timeout=30)
                actual = hashlib.sha256(extracted.stdout).hexdigest()
                report['commands'].append({'name': 'verify-member-'+member, 'argv': list(map(str, command)),
                    'exit_code': extracted.returncode, 'elapsed_seconds': round(time.monotonic()-started, 6),
                    'stdout_sha256': actual, 'stderr_sha256': hashlib.sha256(extracted.stderr).hexdigest()})
                if extracted.returncode or actual != expected:
                    raise ValueError('runtime archive member differs from its instrumented object: '+member)
            report['archive_members'] = expected_members
            if any(digest(Path(path)) != value for path, value in report['inputs'].items()):
                raise ValueError('runtime instrumentation inputs changed during build')
            report.update(status='PASS', archive=str(archive), archive_sha256=digest(archive),
                compile_commands_sha256=digest(build / 'compile_commands.json'),
                canonical_target_sha256=digest(reply / target_entry['jsonFile']),
                coverage_boundary='Every canonical services object is instrumented. Executed paths are established by the consuming witnesses, not by this build receipt.')
        except BaseException as error:
            report.update(status='FAIL', error=str(error))
            raise
        finally:
            write_json(receipt, report)
        profile = RuntimeProfile(archive, dependency, receipt, digest(receipt))
        self.profiles[key] = profile
        return profile

    def receipts(self):
        for sanitizer, optimization in self.profiles:
            self.get(sanitizer, optimization)
        return [{'path': str(profile.receipt), 'sha256': profile.receipt_sha256,
                 'sanitizer': sanitizer, 'optimization': optimization}
                for (sanitizer, optimization), profile in self.profiles.items()]
