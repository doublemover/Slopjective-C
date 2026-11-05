"""Qualify unchanged native interface text with an independent pinned Swift importer."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import shutil

from .binary_packages import digest, find_installed_shared_library, write_json
from .compiler_instrumentation import loaded_graph
from .llvm_ir_execution import command
from .package import SCHEMA, _digest, inventory, verify_sdk
from .sanitizer_policy import require_clean_diagnostic, strict_environment
from .sdk_example import dependencies
from .swift_toolchain import environment as swift_environment, verify as verify_swift, require_host

EXPECTED = {'value': 42, 'amount': 42, 'read': 42, 'live': 0}
REQUIRED_ROWS = {'renamed-primitive': 'objc3_swift_value',
    'nonnull-pointer': 'objc3_swift_read', 'newtype': 'ObjC3SwiftAmount',
    'opaque-c-lifetime': 'ObjC3SwiftToken', 'availability': 'objc3_swift_unavailable'}
NEGATIVES = {
    'old-spelling': ('import ForeignAPI\nlet value = objc3_swift_value(41)\n', "'objc3_swift_value' has been renamed to 'projectedValue(_:)'"),
    'wrong-type': ('import ForeignAPI\nlet value: String = projectedValue(41)\n', 'cannot convert value of type'),
    'nullability': ('import ForeignAPI\nlet value = projectedRead(nil)\n', "'nil' is not compatible"),
    'availability': ('import ForeignAPI\nlet value = unavailableValue()\n', 'is unavailable'),
}


def prepare_compiler_prefix(stage, prefix, *, source_build_id, target):
    """Seal the two built tools and resources for this C pilot, without an archive."""
    stage = stage.resolve(strict=True)
    if prefix.exists() or prefix.resolve().is_relative_to(stage):
        raise ValueError('Swift pilot requires a fresh independent development prefix')
    prefix.mkdir(parents=True)
    for name in ('clang', 'objc3c'):
        (prefix / 'bin').mkdir(exist_ok=True)
        os.link((stage / 'bin' / name).resolve(strict=True), prefix / 'bin' / name)
    (prefix / 'lib').mkdir()
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        source = find_installed_shared_library(stage / 'lib', name)
        if source is None:
            continue
        os.link(source, prefix / 'lib' / source.name)
        if source.name != name:
            (prefix / 'lib' / name).symlink_to(source.name)
    shutil.copytree(stage / 'lib/clang/23/include', prefix / 'lib/clang/23/include', copy_function=os.link)
    receipt = {'schema': SCHEMA, 'source_build_id': source_build_id, 'target': target,
        'purpose': 'Development-only native C/Swift compiler tools; no packaged SDK qualification',
        'files': inventory(prefix)}
    receipt['seal_sha256'] = _digest(receipt)
    write_json(prefix / 'objc3-sdk.json', receipt)
    (prefix / 'objc3-sdk.json').chmod(0o644)
    return verify_sdk(prefix)


def row_manifest(path):
    value = json.loads(path.read_text())
    if (value.get('schema') != 'objc3c.swift-consumer-rows.v1' or
            value.get('target') != 'x86_64-unknown-linux-gnu' or value.get('module') != 'ForeignAPI' or
            not value.get('rows') or len(value['rows']) != len(REQUIRED_ROWS) or
            {row.get('id'): row.get('declaration') for row in value['rows']} != REQUIRED_ROWS or
            any(row.get('required') is not True for row in value['rows'])):
        raise ValueError('Swift consumer requires a nonempty reviewed row inventory')
    if not value.get('unqualified') or any(row.get('status') not in ('unsupported', 'metadata-preserved', 'blocked')
                                         or not row.get('reason') for row in value['unqualified']):
        raise ValueError('Swift consumer requires explicit unqualified boundaries')
    return value


def import_evidence(depfile, work, header, module_map):
    paths = dependencies(depfile, work)
    if header.resolve() not in paths or module_map.resolve() not in paths:
        raise ValueError('Swift did not record the exact selected header/module map')
    return {str(path): digest(path) for path in paths}


def execution_result(text):
    try:
        value = json.loads(text)
    except ValueError as error:
        raise ValueError('Swift consumer omitted its mixed-language result oracle') from error
    if value != EXPECTED or any(type(item) is not int for item in value.values()):
        raise ValueError('Swift mixed-language value or opaque lifetime contract failed')
    return value


def qualify(repo, output, *, sdk, swift_root, symbolizer):
    repo, output, sdk, swift_root = repo.resolve(strict=True), output.resolve(), sdk.resolve(strict=True), swift_root.resolve(strict=True)
    if output.exists() or not output.is_relative_to(repo / 'out'):
        raise ValueError('Swift consumer requires fresh owned evidence')
    installed, foreign = verify_sdk(sdk), verify_swift(repo, swift_root)
    fixture = repo / 'tests/foreign/swift_consumer'
    rows = row_manifest(fixture / 'rows.json')
    output.mkdir(parents=True);(output / 'logs').mkdir()
    symbolizer = symbolizer.resolve(strict=True)
    native_env = strict_environment(sdk / 'bin/clang', symbolizer)
    # No product compiler resources, loader variables, SDKROOT or Swift override
    # variables enter the independent consumer process.
    foreign_env = swift_environment(swift_root)
    report = {'schema': 'objc3c.swift-consumer-qualification.v1', 'status': 'running',
        'host': require_host(),
        'sdk_seal': installed['seal_sha256'], 'source_build_id': installed['source_build_id'],
        'swift_seal_sha256': digest(swift_root / 'objc3-swift-consumer.json'),
        'swift_archive_sha256': foreign['archive_sha256'], 'rows': rows['rows'],
        'unqualified': rows['unqualified'], 'commands': [], 'cases': [], 'controls': [],
        'boundaries': 'Textual C declarations and opaque C resources; no product PCH, GNUstep object or async/actor interoperability.',
        'inputs': {str(path): digest(path) for path in [Path(__file__), *sorted(fixture.iterdir()),
            symbolizer, Path(__file__).with_name('swift_toolchain.py'), Path(__file__).with_name('sdk_example.py')] if path.is_file()}}
    frozen = report['inputs']
    def save(): write_json(output / 'qualification.json', report)
    def run(name, argv, *, work=output, foreign=False, expected=0, contains=()):
        if any(row['name'] == name for row in report['commands']):
            raise ValueError('Swift consumer command requires a unique name')
        env = foreign_env if foreign else native_env
        row = {'name': name, 'environment': env, **command(argv, cwd=work, environment=env,
            log=output / 'logs' / (name+'.log'), timeout=120)}
        report['commands'].append(row);save()
        text = Path(row['log']).read_text(errors='replace')
        if row['timed_out'] or row['exit_code'] != expected:
            raise ValueError('Swift consumer command failed: '+name+'; see '+row['log'])
        require_clean_diagnostic(text)
        if any(part not in text for part in contains):
            raise ValueError('Swift consumer omitted the expected diagnostic: '+name)
        row['status'] = 'PASS';save()
        return text
    swiftc, frontend = [swift_root / 'usr/bin' / name for name in ('swiftc', 'swift-frontend')]
    loader = Path('/lib64/ld-linux-x86-64.so.2').resolve(strict=True)
    try:
        version = run('swift-version', [swiftc, '--version'], foreign=True)
        info = json.loads(run('swift-target-info', [swiftc, '-print-target-info'], foreign=True))
        if ('Swift version 6.4.1 (swift-6.4.1-RELEASE)' not in version or
                info.get('target', {}).get('triple') != rows['target'] or
                Path(info['paths']['runtimeResourcePath']).resolve() != swift_root / 'usr/lib/swift'):
            raise ValueError('Swift consumer compiler/target/resources differ from the admitted pair')
        report.update(swift_version=version, swift_target_info=info,
            native_version=run('native-version', [sdk / 'bin/clang', '--version']),
            swift_clang_version=run('swift-clang-version', [swift_root / 'usr/bin/clang', '--version'], foreign=True),
            symbolizer_version=run('native-symbolizer-version', [symbolizer, '--version']))
        if '23.1.3' not in report['symbolizer_version']:
            raise ValueError('native compiler diagnostics require the pinned LLVM symbolizer')
        for name, binary, is_foreign in (('native', sdk / 'bin/clang', False),
                                         ('swift-driver', swiftc, True), ('swift-frontend', frontend, True)):
            libraries = ([library for library in
                (find_installed_shared_library(sdk / 'lib', candidate)
                 for candidate in ('libLLVM.so', 'libclang-cpp.so')) if library is not None]
                if not is_foreign else [])
            graph = loaded_graph(binary, loader, libraries,
                run=lambda suffix, argv, name=name, is_foreign=is_foreign: run(name+'-'+suffix, argv, foreign=is_foreign))
            if is_foreign and any(Path(path).is_relative_to(sdk) for path in graph['libraries']):
                raise ValueError('Swift loaded product compiler/runtime libraries')
            report.setdefault('loader_graphs', {})[name] = graph
            frozen.update(graph['libraries'])
        provider = output / 'provider-inputs';provider.mkdir()
        for name in ('api.h', 'provider.c'):
            shutil.copyfile(fixture / name, provider / name)
        consumer = (fixture / 'consumer.swift').read_bytes()
        def module(directory, content):
            directory.mkdir(parents=True)
            header, module_map = directory / 'API.h', directory / 'module.modulemap'
            header.write_bytes(content)
            module_map.write_text('module ForeignAPI { header "API.h" export * }\n')
            return header, module_map
        def typecheck(name, module_root, text=consumer, *, expected=0, contains=()):
            work = output / name;work.mkdir()
            source, depfile = work / 'Consumer.swift', work / 'consumer.d'
            source.write_bytes(text)
            run(name, [frontend, '-typecheck', source, '-module-name', 'Consumer', '-swift-version', '6',
                '-target', rows['target'], '-resource-dir', swift_root / 'usr/lib/swift', '-I', module_root,
                '-module-cache-path', work / 'fresh-cache', '-emit-dependencies-path', depfile],
                work=work, foreign=True, expected=expected, contains=contains)
            if expected == 0:
                proof = import_evidence(depfile, work, module_root / 'API.h', module_root / 'module.modulemap')
                frozen.update({path: value for path, value in proof.items() if not Path(path).is_relative_to(output)})
                return proof
        original = output / 'original-module'
        module(original, (fixture / 'api.h').read_bytes())
        original_imports = typecheck('original-positive-control', original)
        report['controls'].append({'name': 'original-header-independent-import', 'status': 'PASS', 'imports': original_imports})
        emitted = output / 'emitted'
        emitted.mkdir()
        interface = emitted / 'ForeignAPI.objc3i'
        def emit(name, source, destination):
            run(name, [sdk / 'bin/objc3c', source, '--objc3', '--emit-interface', destination,
                '--module', 'ForeignAPI', '--package', 'swift-consumer-v1', '--', '--no-default-config',
                '-std=c17', '--target='+rows['target']])
            return json.loads(destination.with_suffix(destination.suffix+'.json').read_text())
        sidecar = emit('native-emission', provider / 'api.h', interface)
        replay = emitted / 'Replay.objc3i'
        replay_sidecar = emit('native-interface-replay', interface, replay)
        if interface.read_bytes() != replay.read_bytes() or sidecar['module'] != replay_sidecar['module']:
            raise ValueError('native foreign metadata replay changed bytes or identity')
        replay_root = output / 'relocated inputs' / 'module'
        module(replay_root, interface.read_bytes())
        replay_imports = typecheck('emitted-positive-control', replay_root)
        report['module_identity'] = sidecar['module']
        report['emitted_header_sha256'] = digest(interface)
        report['controls'].append({'name': 'unmodified-emitted-header-import', 'status': 'PASS', 'imports': replay_imports})
        for name, (source, diagnostic) in NEGATIVES.items():
            typecheck('negative-'+name, replay_root, source.encode(), expected=1, contains=(diagnostic,))
            report['controls'].append({'name': name, 'status': 'PASS'})
        # A public rename must change both native identity and the actual Swift
        # API. Keep these deliberately altered control bytes separate.
        changed = emitted / 'changed.h'
        changed.write_text((provider / 'api.h').read_text().replace('projectedValue(_:)', 'changedValue(_:)'))
        changed_interface = emitted / 'Changed.objc3i'
        altered = emit('changed-public-name', changed, changed_interface)
        if altered['module']['semantic_fingerprint'] == sidecar['module']['semantic_fingerprint']:
            raise ValueError('public Swift name change retained its native fingerprint')
        changed_root = output / 'changed-module';module(changed_root, changed_interface.read_bytes())
        typecheck('changed-name-control', changed_root, b'import ForeignAPI\nlet value = projectedValue(41)\n',
                  expected=1, contains=("cannot find 'projectedValue'",))
        report['controls'].append({'name': 'public-name-identity-and-consumer', 'status': 'PASS'})
        objects = {}
        for optimization in ('O0', 'O2'):
            obj = output / ('provider-'+optimization+'.o')
            run('provider-'+optimization, [sdk / 'bin/clang', '--no-default-config', '--target='+rows['target'],
                '-std=c17', '-'+optimization, '-g', '-c', provider / 'provider.c', '-o', obj])
            objects[optimization] = obj
        retained = output / 'retained-original';retained.mkdir()
        shutil.move(provider, retained / 'provider-inputs')
        shutil.move(original, retained / 'original-module')
        report['relocation'] = {'original_header_hidden': not original.exists(),
            'provider_source_hidden': not provider.exists(), 'replayed_header_unchanged': digest(replay_root / 'API.h') == digest(interface)}
        for transport, content in (('original', (fixture / 'api.h').read_bytes()), ('emitted', interface.read_bytes())):
            for native_opt, obj in objects.items():
                for swift_opt in ('Onone', 'O'):
                    name = '-'.join((transport, native_opt, swift_opt))
                    case = output / name;case.mkdir()
                    header, module_map = module(case / 'module', content)
                    source, binary = case / 'Consumer.swift', case / 'consumer'
                    source.write_bytes(consumer)
                    proof = typecheck(name+'-typecheck', case / 'module')
                    run(name+'-link', [swiftc, '-'+swift_opt, '-swift-version', '6', '-use-ld=lld', '-target', rows['target'],
                        '-I', case / 'module', '-module-cache-path', case / 'fresh-link-cache', source, obj, '-o', binary],
                        work=case, foreign=True)
                    graph = loaded_graph(binary, loader, [], run=lambda suffix, argv, name=name: run(name+'-'+suffix, argv, foreign=True))
                    if any(Path(path).name.startswith('libobjc') or Path(path).is_relative_to(sdk)
                           for path in graph['libraries']):
                        raise ValueError('C/Swift witness accidentally loaded the Objective-C/product runtime')
                    frozen.update(graph['libraries'])
                    observed = execution_result(run(name+'-execute', [binary], work=case, foreign=True))
                    report['cases'].append({'name': name, 'native_optimization': native_opt,
                        'swift_optimization': swift_opt, 'transport': transport, 'status': 'PASS',
                        'result': observed, 'imports': proof, 'header_sha256': digest(header),
                        'module_map_sha256': digest(module_map), 'binary_sha256': digest(binary), 'loaded_graph': graph})
                    shutil.rmtree(case / 'fresh-link-cache')
        missing = output / 'missing-symbol';missing.mkdir()
        source = missing / 'Consumer.swift';source.write_bytes(consumer)
        run('missing-provider-control', [swiftc, '-Onone', '-use-ld=lld', '-I', replay_root, '-module-cache-path', missing / 'fresh-cache',
            source, '-o', missing / 'must-not-exist'], work=missing, foreign=True, expected=1,
            contains=('objc3_swift_value',))
        if (missing / 'must-not-exist').exists():
            raise ValueError('missing provider produced a Swift executable')
        report['controls'].append({'name': 'missing-provider-symbol', 'status': 'PASS'})
        if len(report['cases']) != 8 or any(digest(Path(path)) != value for path, value in frozen.items()):
            raise ValueError('Swift consumer inputs changed or required calls are incomplete')
        if verify_sdk(sdk)['seal_sha256'] != report['sdk_seal']:
            raise ValueError('native SDK changed during Swift qualification')
        verify_swift(repo, swift_root)
        for row in report['rows']:
            row['status'] = 'swift-import-qualified' if row['id'] == 'availability' else 'swift-call-qualified'
        report['artifacts'] = {str(path.relative_to(output)): digest(path) for path in sorted(output.rglob('*'))
            if path.is_file() and 'fresh-cache' not in path.parts and path.name != 'qualification.json'}
        report['status'] = 'PASS'
    except Exception as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        save()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    for name in ('output', 'sdk', 'swift-root', 'symbolizer'):
        parser.add_argument('--'+name, type=Path, required=True)
    result = qualify(**vars(parser.parse_args()))
    print(json.dumps({'status': result['status'], 'cases': len(result['cases']), 'controls': len(result['controls'])}))


if __name__ == '__main__':
    main()
