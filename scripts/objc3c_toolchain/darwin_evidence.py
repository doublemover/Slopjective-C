"""Verify retained native Apple Silicon bootstrap and experimental SDK evidence."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import signal
import tarfile

from .darwin_bootstrap import DEPLOYMENT_TARGET, ROOT, verify_macho
from .darwin_registration import NATIVE, PORTABLE_HEADERS, PORTABLE_SERVICES, RUNTIME, file_digest, load_profile
from .darwin_sdk import ARCHIVE_NAME, ARCHIVE_ROOT, PAYLOAD_SCHEMA, PROFILE, RECEIPT_SCHEMA, SDK_VERSION, WITNESS, XCODE
from .package import MANIFEST, _digest

RECEIPT_BYTES = 16 * 1024**2
PRODUCT_BYTES = 1024**3
LOADER_ABSENCE_CONTROLS = {'missing-runtime-at-load', 'missing-superclass-at-load'}


def retained(root: Path, relative: str | Path, *, limit=RECEIPT_BYTES, allow_empty=False) -> Path:
    path = root / relative
    if (not path.is_relative_to(root) or path.resolve(strict=True) != path or
            not path.is_file() or not (0 if allow_empty else 1) <= path.stat().st_size <= limit):
        raise ValueError(f'missing, linked or excessive Darwin evidence: {relative}')
    return path


def require_macho(root: Path, relative: str, record: dict, kind: str) -> None:
    actual = verify_macho(retained(root, relative, limit=PRODUCT_BYTES), kind)
    if any(actual[key] != record[key] for key in ('format', 'kind', 'sha256')):
        raise ValueError(f'Darwin Mach-O evidence changed: {relative}')


def info(root: Path, relative: str) -> dict:
    return json.loads(retained(root, relative).read_bytes())


def executable_witness(root: Path, relative: str, record: dict, rows: list[dict], expected: str) -> None:
    require_macho(root, relative, record, 'executable')
    if not any(row['argv'][0] == record['path'] and row['exit_status'] == 0 and
               retained(root, row['log']).read_text().strip() == expected for row in rows):
        raise ValueError(f'Darwin qualification omitted native execution of {relative}')


def bootstrap(root: Path, revision: str) -> dict:
    report = info(root, 'result/report.json')
    host = report['platform_identity']
    pin = json.loads((ROOT / 'toolchains/llvm/manifest.json').read_bytes())['upstream']
    host_log = retained(root, 'host.log').read_text().splitlines()
    if (report['schema'] != 'objc3c.darwin-bootstrap-probe.v1' or
            report['status'] != 'bootstrap-verified' or report['support_truth'] is not False or
            report['checkout_revision'] != revision or 'revision=' + revision not in host_log or
            report['probe_sha256'] != file_digest(Path(__file__).with_name('darwin_bootstrap.py')) or
            host['os'] != 'macOS' or host['arch'] != 'arm64' or 'arm64' not in host_log or
            host['sdk_version'] != SDK_VERSION or
            report['apple_toolchain']['developer_tools_version'] != XCODE or
            report['deployment_target'] != DEPLOYMENT_TARGET or
            report['llvm_identity']['revision'] != pin['commit'] or
            report['llvm_identity']['version'] != pin['tag'].removeprefix('llvmorg-')):
        raise ValueError('Darwin bootstrap does not match the tested source and pinned Apple Silicon host')
    records = report['artifacts']
    for name, relative, kind in (
        ('object', 'answer.o', 'object'),
        ('debug_info', 'static.dSYM/Contents/Resources/DWARF/static', 'debug_info'),
        ('bin/static', 'Relocated install/bin/static', 'executable'),
        ('bin/dynamic', 'Relocated install/bin/dynamic', 'executable'),
        ('lib/libanswer.dylib', 'Relocated install/lib/libanswer.dylib', 'dynamic_library'),
    ):
        require_macho(root, 'result/' + relative, records[name], kind)
    archive = retained(root, 'result/Relocated install/lib/libanswer.a')
    if records['archive']['members'] != ['answer.o'] or file_digest(archive) != records['archive']['sha256']:
        raise ValueError('Darwin bootstrap archive evidence changed')
    commands = report['commands']
    if (any(type(row['exit_status']) is not int or row['exit_status'] != 0 for row in commands) or
            any(not any(row['argv'] == [record['path']] for row in commands)
                for record in (records['bin/static'], records['bin/dynamic']))):
        raise ValueError('Darwin bootstrap omitted a successful relocated static or dynamic execution')
    return report


def registration(root: Path, first: dict, revision: str, *, require_arc: bool) -> dict:
    path = root / 'runtime'
    report = info(path, 'report.json')
    profile, _ = load_profile()
    fixtures = ROOT / 'tests/clang_foundation/darwin_registration'
    if (report['schema'] != 'objc3c.darwin-registration-qualification.v1' or
            report['status'] != 'registration-verified' or report['support_truth'] is not False or
            report['revision'] != revision or report['profile'] != profile or
            report['bootstrap_sha256'] != file_digest(root / 'result/report.json') or
            report['platform_identity'] != first['platform_identity'] or
            report['llvm_identity'] != first['llvm_identity'] or
            report['runtime_source_sha256'] != file_digest(RUNTIME) or
            report['fixtures'] != {file.name: file_digest(file) for file in fixtures.iterdir()}):
        raise ValueError('Darwin registration differs from the current native compiler or fixture inputs')
    labels = {}
    for index, row in enumerate(report['commands']):
        match = re.fullmatch(rf'{index:03d}-(.+)\.log', row['log'])
        if not match or Path(row['log']).name != row['log']:
            raise ValueError('Darwin registration command log is outside the owned inventory')
        log = retained(path, row['log'], allow_empty=True)
        if file_digest(log) != row['log_sha256']:
            raise ValueError(f'Darwin registration command log changed: {row["log"]}')
        label = match[1]
        rejects = row.get('expected_diagnostic', [])
        allowed = {1, -signal.SIGABRT} if label in LOADER_ABSENCE_CONTROLS else {1, 2}
        status = row['exit_status']
        if (type(status) is not int or (rejects and (
                status not in allowed or any(marker not in log.read_text(errors='replace') for marker in rejects))) or
                (not rejects and status != 0)):
            raise ValueError(f'Darwin registration command outcome changed: {label}')
        labels.setdefault(label, []).append(row)
    cxx = report['cxx_toolchain']
    require_macho(path, 'xcode-cxx-hash', cxx['hash_probe'], 'executable')
    if (not re.fullmatch(r'Xcode libc\+\+ \d+', cxx['reported_version']) or
            sorted(cxx['hash_probe']['load_paths']) != ['/usr/lib/libSystem.B.dylib', '/usr/lib/libc++.1.dylib'] or
            labels.get('xcode-libcxx-hash-native', [{}])[0].get('argv') !=
            [cxx['hash_probe']['path'], 'stable-hash-input', 'stable-hash-input']):
        raise ValueError('Darwin registration omitted the native Xcode libc++ link and execution preflight')
    for name in ('bin/static', 'bin/dynamic'):
        executable_witness(path, 'Relocated install/' + name, report['artifacts'][name],
                           labels.get(Path(name).name, []), WITNESS)
    if profile.get('inheritance') == 'stateless-super-v1':
        if not any(row.get('expected_diagnostic') for row in labels.get('missing-superclass-at-load', [])):
            raise ValueError('Darwin inheritance omitted the missing strong parent control')
        for name in ('inheritance-static', 'inheritance-dynamic'):
            artifact = report['artifacts']['bin/' + name]
            executable_witness(path, 'Relocated install/bin/' + name, artifact, labels.get(name + '-native', []),
                               'owned Objective-C 3 Darwin inheritance: PASS')
        for name in ('libinheritance-base.dylib', 'libinheritance-child.dylib'):
            require_macho(path, 'Relocated install/lib/' + name,
                          report['artifacts']['lib/' + name], 'dynamic_library')
        if not {'@rpath/libinheritance-base.dylib', '@rpath/libobjc3darwin.dylib'} <= set(
                report['artifacts']['lib/libinheritance-child.dylib']['load_paths']):
            raise ValueError('Darwin inheritance omitted the strong parent and owned runtime dependencies')
    if require_arc or profile.get('arc') == 'stateless-owned-reference-v1':
        arc = report['arc']
        if arc['status'] != 'PASS' or set(arc['routes']) != {'O0', 'O2'}:
            raise ValueError('Darwin ARC omitted a required optimization route')
        require_macho(path, 'build/arc-threads.o', arc['threads_object'], 'object')
        for optimization, route in arc['routes'].items():
            name = 'arc-' + optimization.lower()
            require_macho(path, 'build/' + name + '.o', route['object'], 'object')
            if file_digest(retained(path, 'build/' + name + '.ll')) != route['ir_sha256']:
                raise ValueError(f'Darwin ARC compiler IR changed: {optimization}')
            for linkage in ('static', 'dynamic'):
                binary = name + '-' + linkage
                executable_witness(path, 'Relocated install/bin/' + binary, route[linkage],
                                   labels.get(binary + '-native', []), 'owned Objective-C 3 Darwin ARC: PASS')
    portable = report['portable_services']
    inputs = {NATIVE / name for name in PORTABLE_HEADERS}
    for service, fixture in PORTABLE_SERVICES.values():
        inputs.update((NATIVE / 'src/runtime/stdlib' / (service + '.cpp'),
                       NATIVE / 'tests' / (fixture + '.cpp')))
    if (portable['status'] != 'PASS' or set(portable['suites']) != PORTABLE_SERVICES.keys() or
            portable['inputs'] != {str(name.relative_to(ROOT)): file_digest(name) for name in inputs}):
        raise ValueError('portable Darwin services lack exact source and execution evidence')
    for name, suite in portable['suites'].items():
        require_macho(path, 'Relocated install/bin/' + name, suite['executable'], 'executable')
        if not any(row['argv'] == [suite['executable']['path']] and 'expected_diagnostic' not in row
                   for row in labels.get(name + '-native', [])):
            raise ValueError(f'portable Darwin service omitted native execution: {name}')
    return report


def package(root: Path, runtime: dict, revision: str) -> dict:
    path = root / 'runtime/sdk'
    packed = info(path, 'package/package.json')
    qualified = info(path, 'qualification/qualification.json')
    installed = qualified['installation']
    archive = retained(path, 'package/' + ARCHIVE_NAME, limit=PRODUCT_BYTES)
    if (runtime['sdk_package'] != packed or qualified != runtime['sdk_qualification'] or
            packed['status'] != 'PASS' or packed['schema'] != RECEIPT_SCHEMA or
            packed['revision'] != revision or packed['profile'] != PROFILE or
            packed['archive'] != ARCHIVE_NAME or packed['archive_bytes'] != archive.stat().st_size or
            packed['archive_sha256'] != file_digest(archive) or
            qualified['schema'] != 'objc3c.experimental-darwin-sdk-qualification.v1' or
            qualified['status'] != 'PASS' or installed['status'] != 'PASS' or
            any(row['support_truth'] is not False for row in (packed, qualified, installed)) or
            any(installed[key] != packed[key] for key in ('seal_sha256', 'archive_sha256', 'host'))):
        raise ValueError('Darwin SDK archive, install and cold qualification evidence differ')
    with tarfile.open(archive, 'r|gz') as contents:
        member = contents.next()
        if (member is None or not member.isfile() or member.name != f'{ARCHIVE_ROOT}/{MANIFEST}' or
                not 0 < member.size <= RECEIPT_BYTES):
            raise ValueError('Darwin SDK archive lacks its bounded first manifest')
        manifest = json.load(contents.extractfile(member))
    compiler = runtime['compiler_build']
    payload = manifest['files']
    if (manifest.pop('seal_sha256') != packed['seal_sha256'] or _digest(manifest) != packed['seal_sha256'] or
            manifest['profile'] != PROFILE or manifest['payload_schema'] != PAYLOAD_SCHEMA or
            manifest['revision'] != revision or manifest['support_truth'] is not False or
            any(manifest[key] != runtime[key] for key in ('bootstrap_sha256', 'runtime_source_sha256')) or
            manifest['compiler_build'] != {key: compiler[key] for key in
                                           ('base_build_id', 'patch_digest', 'configuration_sha256')} or
            payload['bin/clang']['sha256'] != compiler['artifact']['sha256'] or
            any(payload['lib/' + name]['sha256'] != runtime['artifacts'][key]['sha256']
                for name, key in (('libobjc3darwin.a', 'libobjc3darwin.a'),
                                  ('libobjc3darwin.dylib', 'lib/libobjc3darwin.dylib')))):
        raise ValueError('Darwin SDK seal does not represent the executed compiler and runtime')
    consumer = 'qualification/Fresh native consumer/'
    for name, relative, kind in (
        ('provider.o', 'provider.o', 'object'), ('static', 'static', 'executable'),
        ('dynamic', 'dynamic', 'executable'), ('liblate.dylib', 'liblate.dylib', 'dynamic_library'),
        ('debug', 'static.dSYM/Contents/Resources/DWARF/static', 'debug_info'),
    ):
        require_macho(path, consumer + relative, qualified['artifacts'][name], kind)
    for name in ('static', 'dynamic'):
        if not any(row['argv'][0] == qualified['artifacts'][name]['path'] and
                   row['exit_status'] == 0 and row['stdout'].strip() == WITNESS
                   for row in qualified['commands']):
            raise ValueError(f'Darwin SDK omitted cold native execution: {name}')
    return packed


def verify(root: Path, revision: str, *, outcome: str, mode: str) -> dict:
    if (outcome != 'success' or not re.fullmatch(r'[0-9a-f]{40}', revision) or
            revision == '0' * 40 or mode not in ('bootstrap', 'registration', 'package')):
        raise ValueError('Darwin producer did not complete successfully for an exact source revision')
    root = root.resolve(strict=True)
    first = bootstrap(root, revision)
    runtime = registration(root, first, revision, require_arc=mode == 'package') if mode != 'bootstrap' else None
    packed = package(root, runtime, revision) if mode == 'package' else None
    result = {'status': 'PASS', 'revision': revision, 'mode': mode, 'platform': first['platform_identity'],
              'support_truth': False, 'bootstrap_sha256': file_digest(root / 'result/report.json')}
    if runtime is not None:
        result['registration_sha256'] = file_digest(root / 'runtime/report.json')
    if packed is not None:
        result.update(archive_sha256=packed['archive_sha256'], seal_sha256=packed['seal_sha256'])
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--evidence', type=Path, required=True)
    parser.add_argument('--revision', required=True)
    parser.add_argument('--outcome', required=True)
    parser.add_argument('--mode', required=True, choices=('bootstrap', 'registration', 'package'))
    args = parser.parse_args()
    result = verify(args.evidence, args.revision, outcome=args.outcome, mode=args.mode)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
