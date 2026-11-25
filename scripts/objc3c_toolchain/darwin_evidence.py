"""Verify retained native Apple Silicon bootstrap and experimental SDK evidence."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import signal
import tarfile

from . import darwin_sdk_sanitizers as sanitizers
from . import darwin_sdk_arc as sdk_arc
from . import darwin_sdk_portable_services as services
from .darwin_bootstrap import DEPLOYMENT_TARGET, ROOT, verify_macho
from .darwin_registration import (
    BLOCK_CAPTURE_IMPORTS, BLOCK_SOURCES, BLOCK_WITNESS, CARRIER_DISPATCH_IMPORTS, CARRIER_DISPATCH_SOURCES,
    CARRIER_DISPATCH_WITNESS, CARRIER_METHOD_DIAGNOSTIC,
    MANAGED_LAYOUT_IMPORTS, MANAGED_LAYOUT_SOURCES, MANAGED_LAYOUT_WITNESS,
    NATIVE, PORTABLE_HEADERS, PORTABLE_SERVICES, RUNTIME, RUNTIME_HEADER,
    TYPED_ERROR_IMPORTS, TYPED_ERROR_SOURCES, TYPED_ERROR_WITNESS,
    TYPED_METHOD_IMPORTS, TYPED_METHOD_SOURCES, TYPED_METHOD_WITNESS,
    file_digest, load_profile, verify_fixed_layout_dwarf, verify_int_method_ir,
)
from .darwin_sdk import (
    API_HEADER, ARCHIVE_NAME, ARCHIVE_ROOT, DEMO, LIBRARY, PAYLOAD_SCHEMA, PROFILE, RECEIPT_SCHEMA, SDK_VERSION,
    SYSTEM, WITNESS, XCODE,
)
from .package import MANIFEST, _digest

RECEIPT_BYTES = 16 * 1024**2
PRODUCT_BYTES = 1024**3
LOADER_ABSENCE_CONTROLS = {'missing-runtime-at-load', 'missing-superclass-at-load'}
INSTALL_CONTROLS = {
    'archive-digest': 'Darwin package archive SHA-256 does not match the receipt',
    'incomplete-prefix': 'install requires a fresh prefix',
    'damaged-prefix': 'install requires a fresh prefix',
    'apple-linker-identity': 'this package does not match the admitted experimental Darwin host and ABI',
}


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


def typed_dispatch(path: Path, report: dict, labels: dict, *, arity: int) -> None:
    if arity == 1:
        family, field, suffix = 'int-method', 'int_argument_methods', 'int_arg_v1'
        witness = 'owned Objective-C 3 Darwin int-argument methods: PASS'
    else:
        family, field, suffix = 'two-int-method', 'two_int_argument_methods', 'two_int_args_v1'
        witness = 'owned Objective-C 3 Darwin two-int-argument methods: PASS'
    proof = report.get(field, {})
    lookups = {'child': 'objc3_darwin_lookup_super_' + suffix, 'main': 'objc3_darwin_lookup_' + suffix}
    ir_labels = {f'{family}-{unit}-{optimization.lower()}-ir': (unit, optimization)
                 for unit in lookups for optimization in ('O0', 'O2')}
    dynamic = f'lib/lib{family}-child.dylib'
    imports = {dynamic: lookups['child'], f'bin/{family}-dynamic': lookups['main'],
               f'bin/{family}-arc': lookups['main'], f'bin/{family}-static': None}
    if (proof.get('status') != 'PASS' or set(proof.get('ir', {})) != set(ir_labels) or
            set(proof.get('imports', {})) != set(imports)):
        raise ValueError(f'Darwin {family} omitted exact typed dispatch evidence')
    for label, (unit, optimization) in ir_labels.items():
        record = proof['ir'][label]
        artifact = retained(path, 'build/' + label + '.ll')
        rows = labels.get(label, [])
        fixture = f'/tests/clang_foundation/darwin_registration/{family.replace("-", "_")}_{unit}.m'
        if (file_digest(artifact) != record['sha256'] or len(rows) != 1 or rows[0]['exit_status'] != 0 or
                rows[0]['argv'][-1] != record['path'] or Path(record['path']).name != artifact.name or
                not {'-' + optimization, '-S', '-emit-llvm'} <= set(rows[0]['argv']) or
                not any(arg.endswith(fixture) for arg in rows[0]['argv'])):
            raise ValueError(f'Darwin {family} IR differs from its exact compile step: {label}')
        verify_int_method_ir(artifact.read_text(), lookups[unit], arity=arity)
    for relative, lookup in imports.items():
        artifact = report['artifacts'][relative]
        name = Path(relative).name
        rows = labels.get(name + '-imports', [])
        if len(rows) != 1 or rows[0]['argv'] != ['/usr/bin/nm', '-u', '-j', artifact['path']]:
            raise ValueError(f'Darwin {family} omitted exact native imports: {relative}')
        log = retained(path, rows[0]['log'], allow_empty=True).read_text()
        names = sorted({symbol.removeprefix('_') for symbol in log.splitlines()})
        if (names != proof['imports'][relative] or (lookup is not None and lookup not in names) or
                (lookup is None and any(name.startswith('objc3_darwin_') for name in names))):
            raise ValueError(f'Darwin {family} native imports differ: {relative}')
        if relative == dynamic:
            require_macho(path, 'Relocated install/' + relative, artifact, 'dynamic_library')
            if not {f'@rpath/lib{family}-base.dylib', '@rpath/libobjc3darwin.dylib'} <= set(artifact['load_paths']):
                raise ValueError(f'Darwin {family} child omitted its strong base and owned runtime dependencies')
        else:
            executable_witness(path, 'Relocated install/' + relative, artifact,
                               labels.get(name + '-native', []), witness)
    if arity == 2:
        for unit in ('base', 'child', 'main', 'arc'):
            object_unit = 'main_arc' if unit == 'arc' else unit
            name = 'two_int_method_' + object_unit
            artifact = report['artifacts'][name]
            require_macho(path, 'build/' + name + '.o', artifact, 'object')
            rows = labels.get(f'two-int-method-{unit}-object', [])
            fixture_unit = 'main' if unit == 'arc' else unit
            fixture = f'/darwin_registration/two_int_method_{fixture_unit}.m'
            if (len(rows) != 1 or rows[0]['argv'][-1] != artifact['path'] or '-c' not in rows[0]['argv'] or
                    not any(arg.endswith(fixture) for arg in rows[0]['argv']) or
                    (unit == 'arc') != ('-fobjc-arc' in rows[0]['argv'])):
                raise ValueError(f'Darwin two-int method omitted its exact object compilation: {unit}')
        base = 'lib/libtwo-int-method-base.dylib'
        require_macho(path, 'Relocated install/' + base, report['artifacts'][base], 'dynamic_library')
    if arity == 2 and 'typed_methods' not in report:
        for label in ('unsupported-two-int-method-type', 'unsupported-two-int-method-arity',
                      'unsupported-two-int-method-send-type', 'unsupported-two-int-method-send-arity'):
            rows = labels.get(label, [])
            send = '-send-' in label
            three_arguments = label.endswith('-arity')
            subject = 'an ordinary int message' if send else 'ordinary int methods'
            diagnostic = f'experimental Darwin registration requires {subject} with at most two int arguments'
            argv = rows[0]['argv'] if len(rows) == 1 else []
            if (len(argv) < 4 or rows[0].get('expected_diagnostic') != [diagnostic] or
                    ('-DDARWIN_UNSUPPORTED_SEND=1' in argv) != send or
                    ('-DDARWIN_UNSUPPORTED_ARITY=1' in argv) != three_arguments or
                    argv[-2] != '-o' or Path(argv[-1]).name != label + '.o' or '-c' not in argv or not any(
                        arg.endswith('/darwin_registration/unsupported_two_int_method.m') for arg in argv)):
                raise ValueError(f'Darwin two-int method omitted an exact rejection control: {label}')


def general_typed_dispatch(path: Path, report: dict, labels: dict) -> None:
    proof = report.get('typed_methods', {})
    if proof.get('status') != 'PASS' or set(proof.get('imports', {})) != set(TYPED_METHOD_IMPORTS):
        raise ValueError('Darwin general typed dispatch omitted its native evidence')
    for relative, lookup in TYPED_METHOD_IMPORTS.items():
        artifact = report['artifacts'][relative]
        name = Path(relative).name
        rows = labels.get(name + '-imports', [])
        if len(rows) != 1 or rows[0]['argv'] != ['/usr/bin/nm', '-u', '-j', artifact['path']]:
            raise ValueError(f'Darwin typed method omitted native imports: {relative}')
        log = retained(path, rows[0]['log'], allow_empty=True).read_text()
        names = sorted({symbol.removeprefix('_') for symbol in log.splitlines()})
        if (names != proof['imports'][relative] or (lookup is not None and lookup not in names) or
                (lookup is None and any(symbol.startswith('objc3_darwin_') for symbol in names))):
            raise ValueError(f'Darwin typed method native imports differ: {relative}')
        if relative.startswith('bin/'):
            executable_witness(path, 'Relocated install/' + relative, artifact,
                               labels.get(name + '-native', []), TYPED_METHOD_WITNESS)
        else:
            require_macho(path, 'Relocated install/' + relative, artifact, 'dynamic_library')
            if not {'@rpath/libtyped-method-base.dylib', '@rpath/libobjc3darwin.dylib'} <= set(artifact['load_paths']):
                raise ValueError('Darwin typed method omitted its base and owned runtime dependencies')
    for label, define in (
        ('unsupported-carrier-method', False),
        ('unsupported-carrier-send', True),
    ):
        rows = labels.get(label, [])
        argv = rows[0]['argv'] if len(rows) == 1 else []
        if (len(argv) < 4 or rows[0].get('expected_diagnostic') != [CARRIER_METHOD_DIAGNOSTIC] or
                ('-DDARWIN_UNSUPPORTED_SEND=1' in argv) != define or
                argv[-2] != '-o' or Path(argv[-1]).name != label + '.o' or '-c' not in argv or not any(
                    arg.endswith('/darwin_registration/unsupported_carrier_method.m') for arg in argv)):
            raise ValueError(f'Darwin typed method omitted an exact rejection control: {label}')


def managed_layout(path: Path, report: dict, labels: dict) -> None:
    proof = report['managed_layout']
    if proof['status'] != 'PASS' or set(proof['imports']) != set(MANAGED_LAYOUT_IMPORTS):
        raise ValueError('Darwin managed layout omitted native evidence')
    for relative, required in MANAGED_LAYOUT_IMPORTS.items():
        artifact = report['artifacts'][relative]
        name = Path(relative).name
        rows = labels.get(name + '-imports', [])
        if len(rows) != 1 or rows[0]['argv'] != ['/usr/bin/nm', '-u', '-j', artifact['path']]:
            raise ValueError(f'Darwin managed layout omitted native imports: {relative}')
        names = {symbol.removeprefix('_') for symbol in retained(path, rows[0]['log'],
                                                                allow_empty=True).read_text().splitlines()}
        if (sorted(names) != proof['imports'][relative] or not required <= names or
                (not required and any(name.startswith('objc3_darwin_') for name in names))):
            raise ValueError(f'Darwin managed layout imported inconsistent runtime functions: {relative}')
        if relative.startswith('bin/'):
            executable_witness(path, 'Relocated install/' + relative, artifact,
                               labels.get(name + '-native', []), MANAGED_LAYOUT_WITNESS)
        else:
            require_macho(path, 'Relocated install/' + relative, artifact, 'dynamic_library')
            if not {'@rpath/libmanaged-layout-base.dylib', '@rpath/libobjc3darwin.dylib'} <= set(artifact['load_paths']):
                raise ValueError('Darwin managed layout omitted its base and owned runtime dependencies')
    for source in (name for name in MANAGED_LAYOUT_SOURCES if name.endswith('.m')):
        name = source.removesuffix('.m')
        artifact = report['artifacts'][name]
        require_macho(path, 'build/' + name + '.o', artifact, 'object')
        rows = labels.get(name.replace('_', '-') + '-object', [])
        if (len(rows) != 1 or not {'-c', '-fobjc-arc'} <= set(rows[0]['argv']) or
                rows[0]['argv'][-1] != artifact['path'] or
                not any(arg.endswith('/darwin_registration/' + source) for arg in rows[0]['argv'])):
            raise ValueError(f'Darwin managed layout omitted ARC compilation: {source}')


def native_owned_program(path, proof, labels, name, source, witness, required, *, flags=()):
    names = {f'{name}-{opt}{route}' for opt in ('o0', 'o2') for route in ('', '-static', '-dynamic')}
    artifacts = proof['artifacts']
    if proof['status'] != 'PASS' or set(artifacts) != names:
        raise ValueError(f'Darwin {name} omitted an optimization or linkage route')
    for optimization in ('o0', 'o2'):
        label = f'{name}-{optimization}'
        obj = artifacts[label]
        require_macho(path, 'build/' + label + '.o', obj, 'object')
        rows = labels.get(label + '-object', [])
        imports = labels.get(label + '-object-imports', [])
        if (len(rows) != 1 or rows[0]['argv'][-1] != obj['path'] or
                not {'-c', '-fobjc-arc', '-' + optimization.upper(), *flags} <= set(rows[0]['argv']) or
                not any(arg.endswith('/darwin_registration/' + source) for arg in rows[0]['argv']) or
                len(imports) != 1 or imports[0]['argv'] != ['/usr/bin/nm', '-u', '-j', obj['path']]):
            raise ValueError(f'Darwin {label} omitted the exact native compile or its imports')
        log = retained(path, imports[0]['log']).read_text()
        symbols = {symbol.removeprefix('_') for symbol in log.splitlines()}
        if not required <= symbols or sorted(symbols) != obj['imports']:
            raise ValueError(f'Darwin {label} did not use the required runtime ABI')
        for linkage in ('static', 'dynamic'):
            binary = label + '-' + linkage
            artifact = artifacts[binary]
            paths = {SYSTEM}
            if linkage == 'dynamic':
                paths.add('@rpath/' + LIBRARY + '.dylib')
            if set(artifact['load_paths']) != paths:
                raise ValueError(f'Darwin {binary} linked unexpected libraries')
            executable_witness(path, 'Relocated install/bin/' + binary, artifact,
                               labels.get(binary + '-native', []), witness)


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
            report['runtime_header_sha256'] != file_digest(RUNTIME_HEADER) or
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
    if profile.get('layout') == 'fixed-root-int-v1':
        for kind in ('base', 'child'):
            name = 'fixed_layout_' + kind
            require_macho(path, 'build/' + name + '.o', report['artifacts'][name], 'object')
            logs = labels.get(f'fixed-layout-{kind}-dwarf', [])
            if len(logs) != 1 or logs[0]['exit_status'] != 0:
                raise ValueError(f'Darwin layout omitted exact {kind} DWARF evidence')
            verify_fixed_layout_dwarf(retained(path, logs[0]['log']).read_text(), 'DarwinFixed' + kind.title())
        for route in ('static', 'dynamic', 'arc'):
            name = 'fixed-layout-' + route
            witness = ('owned Objective-C 3 Darwin fixed root layout ARC lifetime: PASS' if route == 'arc' else
                       'owned Objective-C 3 Darwin fixed root layout: PASS')
            executable_witness(path, 'Relocated install/bin/' + name, report['artifacts']['bin/' + name],
                               labels.get(name + '-native', []), witness)
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
                                   labels.get(binary + '-native', []), sdk_arc.WITNESS)
    typed_dispatch(path, report, labels, arity=1)
    typed_dispatch(path, report, labels, arity=2)
    general_typed_dispatch(path, report, labels)
    managed_layout(path, report, labels)
    native_owned_program(path, report['blocks'], labels, 'blocks', BLOCK_SOURCES[0],
                         BLOCK_WITNESS, BLOCK_CAPTURE_IMPORTS, flags=['-fblocks'])
    native_owned_program(path, report['typed_errors'], labels, 'typed-errors', TYPED_ERROR_SOURCES[0],
                         TYPED_ERROR_WITNESS, TYPED_ERROR_IMPORTS)
    native_owned_program(path, report['carrier_dispatch'], labels, 'carrier-dispatch', CARRIER_DISPATCH_SOURCES[0],
                         CARRIER_DISPATCH_WITNESS, CARRIER_DISPATCH_IMPORTS)
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


def installer_controls(qualified: dict, archived: dict) -> None:
    controls = qualified['installer_controls']
    archive_sha256 = archived['archive_sha256']
    results = controls['observations']
    if (controls['schema'] != 'objc3c.experimental-darwin-sdk-installer-controls.v1' or
            controls['archive_sha256'] != archive_sha256 or results.keys() != INSTALL_CONTROLS.keys()):
        raise ValueError('Darwin installer rejection inventory differs from the admitted archive')
    for name, diagnostic in INSTALL_CONTROLS.items():
        row = results[name]
        prefix = row['prefix_before_sha256']
        request = row['requested_archive_sha256']
        existing = name in ('damaged-prefix', 'incomplete-prefix')
        if (row['status'] != 'REJECTED' or row['diagnostic'] != diagnostic or
                prefix != row['prefix_after_sha256'] or (prefix is not None) != existing or
                not re.fullmatch(r'[0-9a-f]{64}', request) or (request == archive_sha256) != existing or
                (existing and not re.fullmatch(r'[0-9a-f]{64}', prefix))):
            raise ValueError(f'Darwin installer rejection evidence changed: {name}')
    linker = results['apple-linker-identity']
    if (linker['tool'] != 'ld' or linker['observed_tool_sha256'] != archived['host']['tools']['ld']['sha256'] or
            any(not re.fullmatch(r'[0-9a-f]{64}', linker[key]) for key in
                ('required_tool_sha256', 'manifest_seal_sha256')) or
            linker['required_tool_sha256'] == linker['observed_tool_sha256'] or
            linker['manifest_seal_sha256'] == archived['seal_sha256']):
        raise ValueError('Darwin installer omitted the exact Apple linker identity rejection')


def sanitizer_consumers(path: Path, manifest: dict, packed: dict, qualified: dict, *, selected: bool) -> None:
    key = sanitizers.KEY
    installed = qualified['installation']
    if not selected:
        if any(key in row for row in (manifest, packed, installed, qualified)):
            raise ValueError('unselected Darwin package contains sanitizer evidence')
        return
    sanitizers.manifest_files(manifest, manifest['resource_dir'])
    expected = sanitizers.descriptor(manifest['resource_dir'])
    profile = qualified[key]
    if (any(row.get(key) != expected for row in (manifest, packed, installed)) or
            profile['schema'] != sanitizers.SCHEMA or profile['status'] != 'PASS' or
            profile['support_truth'] is not False or profile['scope'] != expected['scope'] or
            profile['routes'].keys() != sanitizers.FLAGS.keys()):
        raise ValueError('Darwin SDK sanitizer evidence differs from the specified consumer profile')
    directory = 'qualification/' + sanitizers.WITNESS_DIRECTORY + '/'
    prefix = Path(installed['prefix'])
    compiler = str(prefix / 'bin/clang')
    source = str(prefix / expected['fixture'])
    for mode, route in profile['routes'].items():
        obj, executable = route['object'], route['executable']
        require_macho(path, directory + mode + '.o', obj, 'object')
        require_macho(path, directory + mode, executable, 'executable')
        sanitizers.validate_imports(mode, '\n'.join(obj['imports']))
        pinned = expected['runtimes'].get(mode)
        loaded = str(prefix / pinned['path']) if pinned else None
        checksum = pinned['sha256'] if pinned else None
        lines = [sanitizers.WITNESS, *([loaded] if loaded else [])]
        if (not any(command['argv'][0] == compiler and command['exit_status'] == 0 and
                    command['argv'][-4:] == ['-c', source, '-o', obj['path']] and
                    set(sanitizers.FLAGS[mode]) <= set(command['argv']) for command in qualified['commands']) or
                route['loaded_runtime'] != loaded or route['runtime_sha256'] != checksum or
                not any(command['argv'] == [executable['path']] and command['exit_status'] == 0 and
                        command['sanitizer_options'] == sanitizers.RUNTIME_OPTIONS[mode] and
                        not command['stderr'] and command['stdout'].splitlines() == lines
                        for command in qualified['commands'])):
            raise ValueError(f'Darwin SDK {mode} did not execute with its exact admitted runtime')


def installed_arc(path: Path, manifest: dict, qualified: dict) -> None:
    arc = qualified['arc']
    installed = qualified['installation']
    prefix, resource = Path(installed['prefix']), manifest['resource_dir']
    runtime = f'lib/{LIBRARY}.dylib'
    inputs = [*(f'{DEMO}/{name}' for name in sdk_arc.SOURCES), 'bin/clang', 'bin/llvm-dwarfdump',
              f'{resource}/include/stddef.h', f'{resource}/lib/darwin/libclang_rt.osx.a', runtime]
    loaded = {'path': str(prefix / runtime), 'sha256': manifest['files'][runtime]['sha256']}
    if (arc['schema'] != sdk_arc.SCHEMA or arc['status'] != 'PASS' or arc['support_truth'] is not False or
            arc['optimization'] != 'O2' or arc['seal_sha256'] != installed['seal_sha256'] or
            arc['inputs'] != {name: manifest['files'][name]['sha256'] for name in inputs} or
            arc['loaded_runtime'] != loaded):
        raise ValueError('Darwin archived ARC consumer differs from the installed SDK inputs')
    directory = 'qualification/' + sdk_arc.DIRECTORY + '/'
    for name, kind in (('arc.o', 'object'), ('arc-threads.o', 'object'), ('arc', 'executable')):
        require_macho(path, directory + name, arc['artifacts'][name], kind)
    executable = arc['artifacts']['arc']
    loaded_paths = [SYSTEM, '@rpath/' + LIBRARY + '.dylib']
    reported_paths = [loaded['path']]
    if loaded['path'].startswith(('/private/var/', '/private/tmp/')):
        reported_paths.append(loaded['path'].removeprefix('/private'))
    if (sorted(executable['load_paths']) != sorted(loaded_paths) or
            not sdk_arc.REQUIRED_IMPORTS <= set(executable['imports']) or
            file_digest(retained(path, directory + 'arc-threads.d')) != arc['thread_dependencies']['sha256'] or
            not any(row['argv'] == [executable['path']] and row['exit_status'] == 0 and
                    row.get('dyld_print_libraries') is True and row['stdout'].strip() == sdk_arc.WITNESS and
                    any(line.endswith(' ' + candidate) for line in row['stderr'].splitlines()
                        for candidate in reported_paths)
                    for row in qualified['commands'])):
        raise ValueError('Darwin archived ARC consumer did not execute with the installed runtime')


def installed_services(path: Path, manifest: dict, runtime: dict, packed: dict, qualified: dict) -> None:
    payload, report = manifest[services.KEY], qualified[services.KEY]
    baseline = runtime['portable_services']
    build_inputs = {**baseline['inputs'], services.EXAMPLE_SOURCE: file_digest(ROOT / services.EXAMPLE_SOURCE)}
    if (report['schema'] != services.SCHEMA or report['status'] != 'PASS' or
            report['support_truth'] is not False or report['abi'] != services.ABI or
            packed[services.KEY] != payload or qualified['installation'][services.KEY] != payload or
            payload['build_inputs'] != build_inputs or
            services.manifest_files(manifest) != report['inputs'].keys() or report['inputs'] != payload['files']):
        raise ValueError('Darwin installed collections and text evidence differs from the sealed payload')
    originals = baseline['suites']
    for name, label in services.MEMBERS.items():
        require_macho(path.parent, 'build/' + name, originals[label]['objects'][name], 'object')
        if payload['objects'][name] != originals[label]['objects'][name]['sha256']:
            raise ValueError('Darwin portable archive differs from the native service build')
    directory = 'qualification/' + services.DIRECTORY + '/'
    require_macho(path, directory + 'example.o', report['object'], 'object')
    require_macho(path, directory + 'example', report['executable'], 'executable')
    if (file_digest(retained(path, directory + 'example.cpp')) != report['inputs'][services.EXAMPLE] or
            not any(row['argv'] == [report['executable']['path']] and row['exit_status'] == 0 and
                    not row['stderr'] and row['stdout'].strip() == services.WITNESS
                    for row in qualified['commands'])):
        raise ValueError('Darwin installed collections and text example did not execute')


def package(root: Path, runtime: dict, revision: str, *, require_sanitizers: bool) -> dict:
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
    installer_controls(qualified, packed)
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
            payload[API_HEADER]['sha256'] != runtime['runtime_header_sha256'] or
            any(payload['lib/' + name]['sha256'] != runtime['artifacts'][key]['sha256']
                for name, key in (('libobjc3darwin.a', 'libobjc3darwin.a'),
                                  ('libobjc3darwin.dylib', 'lib/libobjc3darwin.dylib')))):
        raise ValueError('Darwin SDK seal does not represent the executed compiler and runtime')
    installed_arc(path, manifest, qualified)
    installed_services(path, manifest, runtime, packed, qualified)
    sanitizer_consumers(path, manifest, packed, qualified, selected=require_sanitizers)
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
    for family, sources, witness, flags in (
            ('typed-method', TYPED_METHOD_SOURCES, TYPED_METHOD_WITNESS, set()),
            ('managed-layout', MANAGED_LAYOUT_SOURCES, MANAGED_LAYOUT_WITNESS, set()),
            ('block-lifetime', BLOCK_SOURCES, BLOCK_WITNESS, {'-fblocks'}),
            ('typed-errors', TYPED_ERROR_SOURCES, TYPED_ERROR_WITNESS, set()),
            ('carrier-dispatch', CARRIER_DISPATCH_SOURCES, CARRIER_DISPATCH_WITNESS, set())):
        installed_owned_fixture(path, runtime, installed, payload, qualified, consumer, family, sources, witness, flags)
    return packed


def installed_owned_fixture(path, runtime, installed, payload, qualified, directory, family, sources, witness, flags):
    for source in sources:
        if payload[f'{DEMO}/{source}']['sha256'] != runtime['fixtures'][source]:
            raise ValueError(f'Darwin package substituted an owned runtime fixture: {source}')
        if not source.endswith('.m'):
            continue
        name = source.removesuffix('.m') + '.o'
        artifact = qualified['artifacts'][name]
        require_macho(path, directory + name, artifact, 'object')
        if not any(row['exit_status'] == 0 and {'-c', '-fobjc-arc', name, *flags} <= set(row['argv']) and
                   any(arg.endswith(f'/{DEMO}/{source}') for arg in row['argv']) and
                   row['argv'][0] == f"{installed['prefix']}/bin/clang" for row in qualified['commands']):
            raise ValueError(f'Darwin SDK omitted installed fixture compilation: {source}')
    artifact = qualified['artifacts'][family]
    require_macho(path, directory + family, artifact, 'executable')
    runtime_path = f"{installed['prefix']}/lib/{LIBRARY}.dylib"
    traced_paths = {runtime_path}
    if runtime_path.startswith(('/private/var/', '/private/tmp/')):
        traced_paths.add(runtime_path.removeprefix('/private'))
    if (set(artifact['load_paths']) != {SYSTEM, f'@rpath/{LIBRARY}.dylib'} or
            artifact['loaded_runtime'] != runtime_path or
            not any(row['argv'] == [artifact['path']] and row['exit_status'] == 0 and
                    row.get('dyld_print_libraries') is True and row['stdout'].strip() == witness and
                    any(line.endswith(' ' + candidate) for line in row['stderr'].splitlines()
                        for candidate in traced_paths)
                    for row in qualified['commands'])):
        raise ValueError(f'Darwin SDK omitted cold native {family} execution against the installed runtime')


def verify(root: Path, revision: str, *, outcome: str, mode: str) -> dict:
    if (outcome != 'success' or not re.fullmatch(r'[0-9a-f]{40}', revision) or revision == '0' * 40 or
            mode not in ('bootstrap', 'registration', 'package', 'package-sanitizers')):
        raise ValueError('Darwin producer did not complete successfully for an exact source revision')
    root = root.resolve(strict=True)
    first = bootstrap(root, revision)
    packaged = mode in ('package', 'package-sanitizers')
    runtime = registration(root, first, revision, require_arc=packaged) if mode != 'bootstrap' else None
    packed = package(root, runtime, revision, require_sanitizers=mode == 'package-sanitizers') if packaged else None
    result = {'status': 'PASS', 'revision': revision, 'mode': mode, 'platform': first['platform_identity'],
              'support_truth': False, 'bootstrap_sha256': file_digest(root / 'result/report.json')}
    if runtime is not None:
        result['registration_sha256'] = file_digest(root / 'runtime/report.json')
    if packed is not None:
        result.update(archive_sha256=packed['archive_sha256'], seal_sha256=packed['seal_sha256'])
    return result


def verify_xscomic(root: Path, sdk: dict) -> dict:
    """Bind the native app run to the SDK independently admitted in this run."""
    root = root.resolve(strict=True)
    pin = json.loads((ROOT / 'toolchains/llvm/experimental/darwin-xscomic.json').read_bytes())
    source = retained(root, 'source-revision').read_text().strip()
    installed = info(root, 'install.json')
    manifest = info(root, 'core/sdk.json')
    seal = manifest.pop('seal_sha256')
    if (sdk['status'] != 'PASS' or sdk['mode'] not in ('package', 'package-sanitizers') or
            source != pin['revision'] or installed['status'] != 'PASS' or
            installed['archive_sha256'] != sdk['archive_sha256'] or
            installed['seal_sha256'] != sdk['seal_sha256'] or seal != sdk['seal_sha256'] or
            seal != _digest(manifest) or manifest['revision'] != sdk['revision']):
        raise ValueError('xsComic did not run against the admitted native SDK and source revision')
    compiler = retained(root, 'core/compiler.txt').read_text().splitlines()[0]
    if compiler != manifest['compiler_version']:
        raise ValueError('xsComic did not use the installed compiler identity')
    executable = verify_macho(retained(root, 'core/xsComicObjC3Tests', limit=PRODUCT_BYTES), 'executable')
    loads = retained(root, 'core/load-paths.txt').read_text().splitlines()[1:]
    libraries = [line.strip().split(' (', 1)[0] for line in loads]
    if ({SYSTEM, f'@rpath/{LIBRARY}.dylib'} - set(libraries) or
            any(re.fullmatch(r'libobjc(?:\.[A-Za-z0-9]+)?\.dylib', Path(name).name)
                for name in libraries)):
        raise ValueError('xsComic has inconsistent owned runtime dependencies')
    runtime = str(Path(installed['prefix']) / 'lib' / (LIBRARY + '.dylib'))
    traced_paths = {runtime}
    if runtime.startswith(('/private/var/', '/private/tmp/')):
        traced_paths.add(runtime.removeprefix('/private'))
    trace = retained(root, 'core/dyld.log').read_text().splitlines()
    matches = [line for line in trace if any(line.endswith(' ' + location) for location in traced_paths)]
    results = retained(root, 'core/results.txt')
    summary = re.findall(r'^=== SUMMARY: ([1-9][0-9]*) passed, 0 failed ===$', results.read_text(), re.MULTILINE)
    if len(matches) != 1 or len(summary) != 1:
        raise ValueError('xsComic omitted successful native execution with the installed runtime')
    return {'status': 'PASS', 'repository': pin['repository'], 'source_revision': source,
            'compiler_revision': sdk['revision'], 'compiler_version': compiler, 'archive_sha256': sdk['archive_sha256'],
            'seal_sha256': seal, 'executable': executable, 'load_paths': libraries,
            'tests_passed': int(summary[0]), 'results_sha256': file_digest(results)}


def admit_terminal(replayed: dict, terminal: Path, expected_sha256: str) -> dict:
    """Bind a replay of native package evidence to the retained terminal receipt."""
    if (replayed.get('status') != 'PASS' or replayed.get('mode') not in ('package', 'package-sanitizers') or
            replayed.get('support_truth') is not False or
            any(not isinstance(replayed.get(key), str) or not re.fullmatch(r'[0-9a-f]{64}', replayed[key])
                for key in ('bootstrap_sha256', 'registration_sha256', 'archive_sha256', 'seal_sha256'))):
        raise ValueError('Darwin admission requires exact native package verification')
    terminal = terminal.resolve(strict=True)
    if not terminal.is_file():
        raise ValueError('Darwin terminal receipt must be a bounded regular file')
    with terminal.open('rb') as stream:
        payload = stream.read(RECEIPT_BYTES + 1)
    if not 0 < len(payload) <= RECEIPT_BYTES:
        raise ValueError('Darwin terminal receipt must be a bounded regular file')
    if hashlib.sha256(payload).hexdigest() != expected_sha256:
        raise ValueError('Darwin terminal receipt SHA-256 differs from its pin')
    if json.loads(payload) != replayed:
        raise ValueError('Darwin terminal receipt differs from independent native package verification')
    return {**replayed, 'terminal_sha256': expected_sha256, 'admission': 'experimental-package-verified'}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--evidence', type=Path, required=True)
    parser.add_argument('--revision', required=True)
    parser.add_argument('--outcome', required=True)
    parser.add_argument('--mode', required=True, choices=('bootstrap', 'registration', 'package', 'package-sanitizers'))
    parser.add_argument('--terminal-receipt', type=Path)
    parser.add_argument('--terminal-sha256')
    args = parser.parse_args()
    if (args.terminal_receipt is None) != (args.terminal_sha256 is None):
        parser.error('--terminal-receipt and --terminal-sha256 are required together')
    result = verify(args.evidence, args.revision, outcome=args.outcome, mode=args.mode)
    if args.terminal_receipt is not None:
        result = admit_terminal(result, args.terminal_receipt, args.terminal_sha256)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
