"""Build the installed source provider, ARC interface and managed consumer."""
from __future__ import annotations

import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
import time

MODULE = 'fixture.NativePackage'
PACKAGE = 'native-package-v1'
REPORT = 'objc3c.source-module-build.v1'


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def hashes(paths) -> dict:
    return {str(path): digest(path) for path in sorted(set(paths))}


def write_json(path: Path, value: dict) -> None:
    descriptor, name = tempfile.mkstemp(prefix='.' + path.name, dir=path.parent)
    temporary = Path(name)
    try:
        with os.fdopen(descriptor, 'w') as stream:
            json.dump(value, stream, indent=2, sort_keys=True)
            stream.write('\n')
        os.replace(temporary, path)
    finally:
        temporary.unlink(missing_ok=True)


def dependencies(path: Path, cwd: Path) -> list[Path]:
    # Decode Clang's DependencyFile.cpp spelling, rather than shell quoting.
    # Quotes are literal; ordinary backslashes remain literal, while runs
    # before spaces are doubled. Before # Clang escapes only the # character.
    text = path.read_text().replace('\\\n', '').split(':', 1)[1]
    names, current = [], []
    i = 0
    while i < len(text):
        char = text[i]
        if char == '\\':
            end = i
            while end < len(text) and text[end] == '\\':
                end += 1
            count = end - i
            if end < len(text) and text[end] in ' #':
                current.extend('\\' * (count // 2 if text[end] == ' ' else count - 1))
                current.append(text[end])
                i = end + 1
            else:
                current.extend('\\' * count)
                i = end
        elif char == '$' and i + 1 < len(text) and text[i + 1] == '$':
            current.append('$')
            i += 2
        elif char.isspace():
            if current:
                names.append(''.join(current))
                current = []
            i += 1
        else:
            current.append(char)
            i += 1
    if current:
        names.append(''.join(current))
    return [(cwd / name).resolve() for name in names]


def reusable(record: dict, key: str) -> bool:
    if record.get('key') != key or not record.get('outputs') or not record.get('dependencies'):
        return False
    try:
        return all(digest(Path(name)) == expected
                   for group in ('outputs', 'dependencies')
                   for name, expected in record[group].items())
    except OSError:
        return False


def environment(sdk: Path) -> dict:
    result = dict(os.environ)
    for name in ('CPATH', 'C_INCLUDE_PATH', 'CPLUS_INCLUDE_PATH', 'OBJC_INCLUDE_PATH',
                 'LIBRARY_PATH', 'COMPILER_PATH', 'GCC_EXEC_PREFIX', 'LD_PRELOAD', 'LD_AUDIT',
                 'SDKROOT', 'CCC_OVERRIDE_OPTIONS', 'CLANG_CONFIG_FILE_SYSTEM_DIR',
                 'CLANG_CONFIG_FILE_USER_DIR'):
        result.pop(name, None)
    result['LD_LIBRARY_PATH'] = str(sdk / 'lib')
    result['PATH'] = str(sdk / 'bin') + os.pathsep + result.get('PATH', '')
    result['LC_ALL'] = 'C'
    result['SOURCE_DATE_EPOCH'] = '0'
    return result


def build(sdk: Path, sources: Path, output: Path, optimization: str,
          installed_layout: bool, target: str | None) -> dict:
    started = time.monotonic()
    env = environment(sdk)
    manifest = sdk / 'objc3-sdk.json'
    sealed = manifest.is_file()
    if not sealed and not installed_layout:
        raise ValueError('a sealed installed SDK is required')

    def verify():
        subprocess.run([os.sys.executable, '-B', str(sdk / 'share/objc3c/verify-sdk.py'), str(sdk)],
                       env=env, check=True, capture_output=True, timeout=120)

    if sealed:
        verify()
        sdk_data = json.loads(manifest.read_text())
        target = target or sdk_data['target']
        sdk_identity = sdk_data['seal_sha256']
    else:
        if not target:
            raise ValueError('an unsealed build candidate requires --target')
        sdk_identity = 'unsealed-build-candidate'
    admission = time.monotonic() - started
    # Include directory membership, including new headers which could shadow
    # an earlier include. Actual transitive inputs also come from Clang depfiles.
    source_inputs = hashes(path for path in sources.rglob('*') if path.is_file())
    recipe = digest(Path(__file__))
    report_path = output / 'build-report.json'
    prior = json.loads(report_path.read_text()) if report_path.exists() else {}
    prior_stages = prior.get('stages', {}) if prior.get('schema') == REPORT else {}
    stages = {}
    common = [str(sdk / 'bin/objc3c')]
    mode = ['--objc3', '--target', target, '--runtime', 'gnustep-2.2']
    flags = ['--', '--no-default-config', '-std=c17', '-fblocks', '-' + optimization, '-g',
             '-Wno-pragma-once-outside-header']
    base = {'sdk_seal': sdk_identity, 'sdk_root': str(sdk), 'recipe_sha256': recipe,
            'source_root': str(sources), 'output_root': str(output),
            'source_membership': sorted(source_inputs)}

    def stage(name, argv, inputs, products, depfile=None):
        key = hashlib.sha256(json.dumps({**base, 'argv': argv, 'inputs': inputs},
                                       sort_keys=True).encode()).hexdigest()
        saved = prior_stages.get(name, {})
        tick = time.monotonic()
        if sealed and reusable(saved, key):
            stages[name] = {**saved, 'reused': True, 'elapsed_seconds': round(time.monotonic() - tick, 6)}
            return
        # Invalidate the last successful record before invoking the compiler.
        # A failed build must never leave a reusable partial artifact group.
        report_path.unlink(missing_ok=True)
        subprocess.run(argv, cwd=output, env=env, check=True, timeout=180)
        tracked = dependencies(depfile, output) if depfile else []
        tracked.extend(Path(path) for path in inputs)
        stages[name] = {'key': key, 'argv': argv, 'reused': False,
                        'outputs': hashes(products), 'dependencies': hashes(tracked),
                        'elapsed_seconds': round(time.monotonic() - tick, 6)}

    provider = output / 'provider'
    arc = output / 'modules'
    provider.mkdir(exist_ok=True)
    arc.mkdir(exist_ok=True)
    obj = provider / 'provider.o'
    interface = provider / 'NativePackage.objc3i'
    sidecar = interface.with_suffix('.objc3i.json')
    depfile = provider / 'provider.d'
    provider_inputs = {path: sha for path, sha in source_inputs.items()
                       if Path(path).name != 'main.m'}
    stage('provider', common + [str(sources / 'provider.m')] + mode + [
        '--emit-object', str(obj), '--emit-interface', str(interface),
        '--module', MODULE, '--package', PACKAGE] + flags + [
        '-MD', '-MF', str(depfile), '-MT', 'dependencies'],
        provider_inputs, [obj, interface, sidecar, depfile], depfile)
    arc_interface = arc / interface.name
    arc_sidecar = arc_interface.with_suffix('.objc3i.json')
    # The implementation has explicit MRC retain/release. Reparse its exported
    # declarations under ARC; ownership ABI identities are never rewritten.
    stage('arc-interface', common + [str(interface)] + mode + ['--arc',
        '--emit-interface', str(arc_interface), '--module', MODULE, '--package', PACKAGE] + flags,
        hashes([interface, sidecar]), [arc_interface, arc_sidecar])
    identity = json.loads(arc_sidecar.read_text())['module']
    if identity['runtime_abi'] != 'gnustep-2.2;arc' or identity['target_triple'] != target:
        raise ValueError('rebuilt ARC interface has an unexpected target or ownership ABI')
    consumer = output / 'consumer.o'
    record = output / 'consumer-dependencies.json'
    depfile = output / 'consumer.d'
    consumer_inputs = hashes([sources / 'main.m', arc_interface, arc_sidecar])
    stage('consumer', common + [str(sources / 'main.m')] + mode + ['--arc',
        '--source-imports', '--module-search-path', str(arc), '--import-module', MODULE,
        identity['semantic_fingerprint'], PACKAGE, '--emit-object', str(consumer),
        '--emit-dependency-record', str(record)] + flags + [
        '-MD', '-MF', str(depfile), '-MT', 'dependencies'],
        consumer_inputs, [consumer, record, depfile], depfile)
    dependency_record = json.loads(record.read_text())
    modules = dependency_record.get('modules', [])
    if (dependency_record.get('schema') != 'objc3c.dependency-record.v2' or len(modules) != 1 or
            modules[0]['fingerprint'] != identity['semantic_fingerprint'] or
            modules[0]['package'] != PACKAGE or modules[0]['runtime'] != identity['runtime_abi']):
        raise ValueError('consumer dependency record does not bind the rebuilt module')
    executable = output / 'objc3-managed-sample'
    rpath = '$ORIGIN/../lib' if installed_layout else str(sdk / 'lib')
    tick = time.monotonic()
    # Relink every invocation so host CRT/system-library changes are admitted by
    # the native linker. No timestamp or cached host-library assumption is used.
    subprocess.run([str(sdk / 'bin/clang++'), '--no-default-config', '--target=' + target,
                    '-fuse-ld=lld', str(consumer), str(obj), '-L' + str(sdk / 'lib'), '-lobjc',
                    '-Wl,-rpath,' + rpath, '-o', str(executable)], env=env, check=True, timeout=180)
    link_seconds = time.monotonic() - tick
    run_env = dict(env)
    run_env.pop('LD_LIBRARY_PATH', None)
    # During SDK production the executable is copied into bin after building.
    if installed_layout:
        run_env['LD_LIBRARY_PATH'] = str(sdk / 'lib')
    subprocess.run([str(executable)], env=run_env, check=True, timeout=30)
    if hashes(path for path in sources.rglob('*') if path.is_file()) != source_inputs:
        raise ValueError('sample sources changed during the build')
    for saved in stages.values():
        if not reusable(saved, saved['key']):
            raise ValueError('a module input or output changed during the build')
    build_seconds = time.monotonic() - started - admission
    tick = time.monotonic()
    if sealed:
        verify()
    report = {'schema': REPORT, 'status': 'PASS', 'sdk_seal': sdk_identity,
              'source_inputs': source_inputs, 'module_identity': identity,
              'stages': stages, 'executable': str(executable), 'executable_sha256': digest(executable),
              'sdk_admission_seconds': round(admission, 6), 'build_seconds': round(build_seconds, 6),
              'sdk_final_verification_seconds': round(time.monotonic() - tick, 6),
              'link_seconds': round(link_seconds, 6), 'elapsed_seconds': round(time.monotonic() - started, 6)}
    write_json(report_path, report)
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--source-dir', type=Path, help='editable copy of the installed source example')
    parser.add_argument('--installed-layout', action='store_true', help=argparse.SUPPRESS)
    parser.add_argument('--target', help=argparse.SUPPRESS)
    parser.add_argument('--optimization', choices=('O0', 'O2'), default='O2')
    args = parser.parse_args()
    example = Path(__file__).resolve().parent
    sdk = example.parents[3]
    sources = (args.source_dir or example / 'src').resolve()
    output = args.output_dir.resolve()
    if (output.is_relative_to(sdk) or sdk.is_relative_to(output) or
            output.is_relative_to(sources) or sources.is_relative_to(output)):
        parser.error('build output must be separate from the SDK and source directories')
    output.mkdir(parents=True, exist_ok=True)
    with (output / '.build.lock').open('a+b') as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        report = build(sdk, sources, output, args.optimization, args.installed_layout, args.target)
    print(json.dumps({'status': report['status'], 'executable': report['executable'],
                      'build_report': str(output / 'build-report.json'),
                      'reused_stages': [name for name, row in report['stages'].items() if row['reused']]}))


if __name__ == '__main__':
    main()
