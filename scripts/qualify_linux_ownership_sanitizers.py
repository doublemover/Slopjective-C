"""Qualify installed Linux ARC/MRC projections with AddressSanitizer and LSan."""
from __future__ import annotations

import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess

from scripts.objc3c_toolchain.package import verify_sdk
from scripts.objc3c_toolchain.qualify_ownership_projections import prepare
from scripts.objc3c_toolchain.binary_packages import disjoint


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def qualify(sdk: Path, resource: Path, output: Path) -> dict:
    sdk, resource, output = sdk.resolve(strict=True), resource.resolve(strict=True), output.resolve()
    disjoint(output, sdk, resource)
    manifest = verify_sdk(sdk)
    runtimes = sorted((resource / 'lib/x86_64-unknown-linux-gnu').glob('libclang_rt.asan*.a'))
    if not runtimes or not (resource / 'include/stddef.h').is_file():
        raise ValueError('a Clang resource directory with Linux x86-64 ASan runtimes is required')
    # A new directory prevents a failed invocation from leaving a stale PASS.
    output.mkdir(parents=True, exist_ok=False)
    fixture = sdk / 'share/objc3c/examples/ownership-projection'
    report = {
        'schema': 'objc3c.ownership-projection-sanitizers.v1', 'status': 'running',
        'sdk_seal': manifest['seal_sha256'], 'source_build_id': manifest['source_build_id'],
        'outside_checkout': not output.is_relative_to(Path(__file__).resolve().parents[1]),
        'sanitizers': ['AddressSanitizer', 'LeakSanitizer'],
        'instrumentation': 'mixed-mode provider, re-export, and consumer; SDK runtime loaded as shipped',
        'sanitizer_resource_root': str(resource),
        'sanitizer_runtime_sha256': {str(path.relative_to(resource)): digest(path) for path in runtimes},
        'fixture_source_hashes': {str(path.relative_to(fixture)): digest(path)
                                  for path in sorted(fixture.rglob('*')) if path.is_file()},
        'harness_sha256': digest(Path(__file__)), 'commands': [], 'executions': [],
    }
    environment = {**os.environ, 'LD_LIBRARY_PATH': str(sdk / 'lib'),
                   'ASAN_OPTIONS': 'detect_leaks=1:halt_on_error=1'}

    def save():
        temporary = output / '.summary.tmp'
        temporary.write_text(json.dumps(report, indent=2) + '\n')
        temporary.replace(output / 'summary.json')

    def run(key, phase, command, cwd):
        command = list(map(str, command))
        log = output / (key + '-' + phase + '.log')
        with log.open('w') as stream:
            result = subprocess.run(command, cwd=cwd, env=environment, stdout=stream,
                                    stderr=subprocess.STDOUT, timeout=180, check=False)
        report['commands'].append({'configuration': key, 'phase': phase, 'command': command,
                                   'exit_code': result.returncode, 'log': str(log),
                                   'log_sha256': digest(log)})
        save()
        if result.returncode:
            raise RuntimeError(f'{phase} failed; inspect {log}')

    save()
    try:
        compiler = sdk / 'bin/objc3c'
        for language, standard in [('objective-c', 'c17'), ('objective-c++', 'c++20'),
                                    ('objective-c++', 'c++23')]:
            for consumer in ('arc', 'mrc'):
                for optimization in ('O0', 'O2'):
                    key = '-'.join((language, standard, consumer, optimization))
                    root = output / key
                    root.mkdir()
                    sources = root / "source with 'quotes'"
                    prepare(sdk, sources, language, standard, consumer)
                    provider = 'mrc' if consumer == 'arc' else 'arc'
                    suffix = '.mm' if language.endswith('++') else '.m'
                    modules = root / 'modules'
                    modules.mkdir()
                    objects = []
                    flags = ['--no-default-config', '-std=' + standard, '-fblocks',
                             '-' + optimization, '-g', '-fstandalone-debug', '-fexceptions',
                             '-fobjc-arc-exceptions', '-fsanitize=address',
                             '-resource-dir', resource, '-isystem', sdk / 'include']
                    for part, name, package, mode in [
                        ('provider', 'fixture.Ownership', 'ownership-v1', provider),
                        ('reexport', 'fixture.Reexport', 'reexport-v1', consumer),
                        ('app', None, None, consumer),
                    ]:
                        source = sources / part / ('main' + suffix if part == 'app' else 'module' + suffix)
                        obj = root / (part + '.o')
                        objects.append(obj)
                        command = [compiler, source, '--objc3']
                        if language.endswith('++'):
                            command.append('--objcxx')
                        if mode == 'arc':
                            command.append('--arc')
                        command.extend(['--emit-object', obj])
                        if part == 'provider':
                            command.extend(['--project-ownership', consumer])
                        if name:
                            command.extend(['--emit-interface', modules / (name + '.objc3i'),
                                            '--module', name, '--package', package])
                        if part != 'provider':
                            dependency, dep_package = (('fixture.Ownership', 'ownership-v1')
                                                       if part == 'reexport'
                                                       else ('fixture.Reexport', 'reexport-v1'))
                            identity = json.loads((modules / (dependency + '.objc3i.json')).read_text())['module']
                            command.extend(['--source-imports', '--module-search-path', modules,
                                            '--import-module', dependency,
                                            identity['semantic_fingerprint'], dep_package,
                                            '--reexport-import' if part == 'reexport' else '--private-import'])
                        run(key, part, [*command, '--', *flags], sources)
                    executable = root / 'application'
                    run(key, 'link', [compiler, '-fsanitize=address', '-resource-dir', resource,
                                      *objects, '-o', executable], sources)
                    run(key, 'execution', [executable], sources)
                    run(key, 'dwarf', [sdk / 'bin/llvm-dwarfdump', '--verify', executable], sources)
                    report['executions'].append({'language': language, 'standard': standard,
                                                 'ownership': consumer, 'provider_ownership': provider,
                                                 'optimization': optimization,
                                                 'executable_sha256': digest(executable)})
                    save()
        if len(report['executions']) != 12 or len(report['commands']) != 72:
            raise ValueError('incomplete ownership sanitizer qualification')
        verify_sdk(sdk, manifest['seal_sha256'])
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        report['finished_utc'] = datetime.now(timezone.utc).isoformat()
        save()
    return report


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sdk-root', type=Path, required=True)
    parser.add_argument('--resource-dir', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    args = parser.parse_args(argv)
    report = qualify(args.sdk_root, args.resource_dir, args.output_dir)
    print(json.dumps({'status': report['status'], 'executions': len(report['executions']),
                      'commands': len(report['commands'])}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
