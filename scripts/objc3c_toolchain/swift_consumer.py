"""Qualify unchanged native interface text with an independent pinned Swift importer."""
from __future__ import annotations

from .linux_host import HOSTS, native_loader, native_target


import argparse
import errno
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
CXX_EXPECTED = {'before': 40, 'count': 42, 'total': 2147483690,
                'owner_before': 40, 'owner_after': 42, 'unique_value': 42,
                'unique_allocations': 1, 'unique_destructions': 1}
CXX_LIFETIME = ('owner_allocations', 'owner_destructions', 'owner_copies', 'unique_moves')
ASYNC_EXPECTED = {'value': 42, 'checked_error': True, 'cancellation': True,
                  'pre_cancelled': True, 'cancelled_after_submit': True, 'actor': 42, 'other_actor': 7,
                  'generated_value': 42, 'generated_error': True, 'generated_cancellation': True,
                  'generated_scalars': True, 'generated_void': True, 'generated_enum': 42,
                  'generated_checked_value': 42, 'generated_checked_error': True,
                  'generated_async_record': 42, 'generated_checked_record': 42}
ASYNC_CASES = {(ownership, native, swift) for ownership in ('arc', 'mrc')
               for native in ('O0', 'O2') for swift in ('Onone', 'O')}
QUALIFICATION_SCHEMA = 'objc3c.swift-consumer-qualification.v8'
CXX_IMPORT = ('-cxx-interoperability-mode=default', '-Xcc', '-std=c++20')
REQUIRED_ROWS = {'renamed-primitive': 'objc3_swift_value',
    'nonnull-pointer': 'objc3_swift_read', 'newtype': 'ObjC3SwiftAmount',
    'opaque-c-lifetime': 'ObjC3SwiftToken', 'availability': 'objc3_swift_unavailable',
    'cxx-trivial-value': 'MeterAPI::adjustedReading',
    'cxx-owning-value': 'MeterAPI::adjustedOwner',
    'cxx-move-only-value': 'MeterAPI::adjustedUniqueReading',
    'async-c-continuation': 'swift_bridge_work_async_c',
    'async-c-cancellation': 'swift_bridge_cancelled_async_c',
    'swift-task-cancellation': 'swift_bridge_cancellable_async_c_task',
    'c-actor-continuation': 'SwiftBridgeCounter_add_async_c'}
NEGATIVES = {
    'old-spelling': ('import ForeignAPI\nlet value = objc3_swift_value(41)\n', "'objc3_swift_value' has been renamed to 'projectedValue(_:)'"),
    'wrong-type': ('import ForeignAPI\nlet value: String = projectedValue(41)\n', 'cannot convert value of type'),
    'nullability': ('import ForeignAPI\nlet value = projectedRead(nil)\n', "'nil' is not compatible"),
    'availability': ('import ForeignAPI\nlet value = unavailableValue()\n', 'is unavailable'),
}


def _link_or_copy(source, destination):
    source = Path(source).resolve(strict=True)
    try:
        os.link(source, destination)
    except OSError as error:
        if error.errno != errno.EXDEV:
            raise
        shutil.copy2(source, destination)


def prepare_compiler_prefix(stage, prefix, *, source_build_id, target):
    """Seal current native compiler and runtime inputs without building an archive."""
    stage = stage.resolve(strict=True)
    if prefix.exists() or prefix.resolve().is_relative_to(stage):
        raise ValueError('Swift pilot requires a fresh independent development prefix')
    prefix.mkdir(parents=True)
    for name in ('clang', 'objc3c'):
        (prefix / 'bin').mkdir(exist_ok=True)
        _link_or_copy(stage / 'bin' / name, prefix / 'bin' / name)
    (prefix / 'lib').mkdir()
    for name in ('libLLVM.so', 'libclang-cpp.so', 'libobjc.so', 'libobjc3_runtime_services.a'):
        source = ((stage / 'lib' / name).resolve(strict=True)
                  if name.startswith('libobjc') else find_installed_shared_library(stage / 'lib', name))
        if source is None:
            continue
        _link_or_copy(source, prefix / 'lib' / source.name)
        if source.name != name:
            (prefix / 'lib' / name).symlink_to(source.name)
    for name in ('lib/clang/23', 'include/objc', 'include/objc3c/runtime'):
        shutil.copytree(stage / name, prefix / name, copy_function=_link_or_copy)
    for name in ('clang.cfg', 'clang++.cfg'):
        source = stage / 'bin' / name
        if source.is_file() and not source.is_symlink():
            _link_or_copy(source, prefix / 'bin' / name)
    receipt = {'schema': SCHEMA, 'source_build_id': source_build_id, 'target': target,
        'purpose': 'Development-only compiler and runtime inputs for Swift interop',
        'files': inventory(prefix)}
    receipt['seal_sha256'] = _digest(receipt)
    write_json(prefix / 'objc3-sdk.json', receipt)
    (prefix / 'objc3-sdk.json').chmod(0o644)
    return verify_sdk(prefix)


def row_manifest(path):
    value = json.loads(path.read_text())
    if (value.get('schema') != 'objc3c.swift-consumer-rows.v7' or
            value.get('targets') != [entry[1] for entry in HOSTS.values()] or value.get('module') != 'ForeignAPI' or
            value.get('cxx_module') != 'ForeignCxx' or
            not value.get('rows') or len(value['rows']) != len(REQUIRED_ROWS) or
            {row.get('id'): row.get('declaration') for row in value['rows']} != REQUIRED_ROWS or
            any(row.get('required') is not True for row in value['rows'])):
        raise ValueError('Swift consumer requires a nonempty reviewed row inventory')
    if not value.get('unqualified') or any(row.get('status') not in ('unsupported', 'metadata-preserved', 'blocked')
                                         or not row.get('reason') for row in value['unqualified']):
        raise ValueError('Swift consumer requires explicit unqualified boundaries')
    return {**value, 'target': native_target()}


def import_evidence(depfile, work, header, module_map):
    paths = dependencies(depfile, work)
    if header.resolve() not in paths or module_map.resolve() not in paths:
        raise ValueError('Swift did not record the exact selected header/module map')
    return {str(path): digest(path) for path in paths}


def execution_result(text, language='c'):
    try:
        value = json.loads(text)
    except ValueError as error:
        raise ValueError('Swift consumer omitted its mixed-language result oracle') from error
    if language == 'async':
        if (value != ASYNC_EXPECTED or
                any(type(value[key]) is not type(expected) for key, expected in ASYNC_EXPECTED.items())):
            raise ValueError('Swift async completion, checked error, cancellation or actor contract failed')
        return value
    if not isinstance(value, dict) or any(type(item) is not int for item in value.values()):
        raise ValueError('Swift mixed-language value or opaque lifetime contract failed')
    if language == 'cxx':
        counts = [value.get(key, -1) for key in CXX_LIFETIME]
        allocations, destructions, copies, moves = counts
        fixed = {key: item for key, item in value.items() if key not in CXX_LIFETIME}
        if (fixed != CXX_EXPECTED or allocations != destructions or
                not 1 <= copies <= allocations - 2 or moves < 1):
            raise ValueError('Swift mixed-language C++ owning value or destructor contract failed')
    elif value != EXPECTED:
        raise ValueError('Swift mixed-language value or opaque lifetime contract failed')
    return value


def qualify_async_bridge(output, fixture, sdk, swift_root, target, loader, run, report):
    runtime = (sdk / 'lib/libobjc.so').resolve(strict=True)
    services = sdk / 'lib/libobjc3_runtime_services.a'
    headers = ('objc3_async_c.h', 'objc3_runtime_services.h')
    swiftc, frontend = (swift_root / 'usr/bin' / name for name in ('swiftc', 'swift-frontend'))
    driver = [swiftc, '-swift-version', '6', '-parse-as-library', '-use-ld=lld', '-target', target]
    module = output / 'async-module'
    support = module / 'objc3c/runtime'
    support.mkdir(parents=True)
    for name in headers:
        shutil.copyfile(sdk / 'include/objc3c/runtime' / name, support / name)
    shutil.copyfile(fixture / 'async_api.h', module / 'AsyncAPI.h')
    module_map = module / 'module.modulemap'
    module_map.write_text('module ForeignAsync {\n  header "Generated.h"\n  header "AsyncAPI.h"\n  export *\n}\n')
    source = output / 'AsyncConsumer.swift'
    shutil.copyfile(fixture / 'async_consumer.swift', source)
    generated = module / 'Generated.h'
    swift_overlay = output / 'GeneratedAsync.swift'
    report['inputs'][str(services)] = digest(services)
    for ownership in ('arc', 'mrc'):
        for native_opt in ('O0', 'O2'):
            name = f'async-{ownership}-{native_opt}'
            provider = output / name
            provider.mkdir()
            for filename in ('async_provider.m', 'async_api.h'):
                shutil.copyfile(fixture / filename, provider / filename)
            obj, header = provider / 'provider.o', provider / 'generated.h'
            emitted_overlay = provider / 'generated.swift'
            run(name + '-provider', [sdk / 'bin/objc3c', provider / 'async_provider.m', '--objc3',
                '--emit-object', obj, '--export-c-async', 'swift_bridge_work', '--export-c-async',
                'swift_bridge_cancelled', '--export-c-async', 'swift_bridge_cancellable',
                '--export-c-async', 'swift_bridge_scalars', '--export-c-async', 'swift_bridge_void',
                '--export-c-async', 'swift_bridge_enum', '--export-c-async', 'swift_bridge_record',
                '--export-c-async', 'SwiftBridgeCounter::add:', '--export-c-throws', 'swift_bridge_checked',
                '--export-c-throws', 'swift_bridge_checked_void', '--export-c-throws', 'swift_bridge_checked_record',
                '--emit-c-header',
                header, '--emit-swift-overlay', emitted_overlay, '--swift-c-module', 'ForeignAsync',
                '--', '-std=c17', '-' + native_opt, '-fblocks', '-fobjc-runtime=gnustep-2.2',
                '-fobjc-exceptions', *(['-fobjc-arc', '-fobjc-arc-exceptions'] if ownership == 'arc'
                                       else ['-fno-objc-arc']),
                '-I' + str(sdk / 'include')], work=provider)
            if generated.exists():
                if generated.read_bytes() != header.read_bytes():
                    raise ValueError('Swift async C signature changed between native modes')
            else:
                shutil.copyfile(header, generated)
            if swift_overlay.exists():
                if swift_overlay.read_bytes() != emitted_overlay.read_bytes():
                    raise ValueError('generated Swift overlay changed between native modes')
            else:
                shutil.copyfile(emitted_overlay, swift_overlay)
            for filename in ('async_provider.m', 'async_api.h', 'generated.h', 'generated.swift'):
                (provider / filename).unlink()
            for swift_opt in ('Onone', 'O'):
                case = provider / swift_opt
                case.mkdir()
                name_opt = name + '-' + swift_opt
                depfile, binary = case / 'swift.d', case / 'consumer'
                run(name_opt + '-typecheck', [frontend, '-typecheck', '-parse-as-library',
                    '-swift-version', '6', '-module-name', 'AsyncConsumer', '-target', target,
                    '-resource-dir', swift_root / 'usr/lib/swift', '-I', module,
                    '-module-cache-path', case / 'fresh-cache', '-emit-dependencies-path', depfile,
                    swift_overlay, source], work=case, foreign=True)
                imports = import_evidence(depfile, case, generated, module_map)
                for path in (module / 'AsyncAPI.h', *(support / header for header in headers)):
                    if imports.get(str(path)) != digest(path):
                        raise ValueError('Swift async importer omitted a bridge header')
                run(name_opt + '-link', [*driver, '-' + swift_opt, '-I', module,
                    '-module-cache-path', case / 'fresh-link-cache', swift_overlay, source, obj, services, runtime,
                    '-Xlinker', '-lstdc++', '-Xlinker', '-rpath', '-Xlinker', runtime.parent,
                    '-o', binary], work=case, foreign=True)
                graph = loaded_graph(binary, loader, [], required=(runtime,),
                    run=lambda suffix, argv: run(name_opt + '-' + suffix, argv, foreign=True))
                if {path for path in graph['libraries'] if Path(path).is_relative_to(sdk)} != {str(runtime)}:
                    raise ValueError('Swift async witness loaded unexpected product libraries')
                result = execution_result(run(name_opt + '-execute', [binary], work=case, foreign=True), 'async')
                report['async_cases'].append({'name': name_opt, 'ownership': ownership,
                    'native_optimization': native_opt, 'swift_optimization': swift_opt, 'status': 'PASS',
                    'header_sha256': digest(generated), 'overlay_sha256': digest(swift_overlay),
                    'binary_sha256': digest(binary), 'imports': imports,
                    'loaded_graph': graph, 'result': result})
                report['inputs'].update(graph['libraries'])
                for cache in ('fresh-cache', 'fresh-link-cache'):
                    shutil.rmtree(case / cache)
    report['async_interface'] = {'header_sha256': digest(generated),
        'overlay_sha256': digest(swift_overlay), 'runtime': str(runtime),
        'runtime_sha256': digest(runtime), 'services': str(services),
        'services_sha256': digest(services), 'provider_source_removed': True}
    invalid = output / 'AsyncNullActor.swift'
    invalid.write_text('import ForeignAsync\nfunc rejected() {\n'
        '    SwiftBridgeCounter_add_async_c(nil, 1, nil) { _, _, _, _ in }\n}\n')
    cache = output / 'fresh-async-control-cache'
    run('async-null-actor-control', [swiftc, '-typecheck', '-swift-version', '6', '-target', target,
        '-I', module, '-module-cache-path', cache, invalid], foreign=True,
        expected=1, contains=("'nil' is not compatible",))
    shutil.rmtree(cache)
    report['controls'].append({'name': 'async-null-actor-handle', 'status': 'PASS'})


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
    report = {'schema': QUALIFICATION_SCHEMA, 'status': 'running',
        'host': require_host(),
        'sdk_seal': installed['seal_sha256'], 'source_build_id': installed['source_build_id'],
        'swift_seal_sha256': digest(swift_root / 'objc3-swift-consumer.json'),
        'swift_archive_sha256': foreign['archive_sha256'], 'rows': rows['rows'],
        'unqualified': rows['unqualified'], 'commands': [], 'cases': [], 'controls': [],
        'relocation': {}, 'interfaces': {}, 'async_cases': [],
        'boundaries': 'Textual C and C++ declarations, opaque C resources, nonthrowing C++ values and generated C task and actor completions.',
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
    loader = native_loader()
    try:
        version = run('swift-version', [swiftc, '--version'], foreign=True)
        info = json.loads(run('swift-target-info', [swiftc, '-print-target-info'], foreign=True))
        if ('Swift version 6.4.2 (swift-6.4.2-RELEASE)' not in version or
                info.get('target', {}).get('triple') != rows['target'] or
                Path(info['paths']['runtimeResourcePath']).resolve() != swift_root / 'usr/lib/swift'):
            raise ValueError('Swift consumer compiler/target/resources differ from the admitted pair')
        report.update(swift_version=version, swift_target_info=info,
            native_version=run('native-version', [sdk / 'bin/clang', '--version']),
            swift_clang_version=run('swift-clang-version', [swift_root / 'usr/bin/clang', '--version'], foreign=True),
            symbolizer_version=run('native-symbolizer-version', [symbolizer, '--version']))
        if '23.1.5' not in report['symbolizer_version']:
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
        def qualify_language(language):
            cxx = language == 'cxx'
            prefix = 'cxx-' if cxx else ''
            header_name, provider_name, consumer_name = (
                ('cxx_api.h', 'cxx_provider.cpp', 'cxx_consumer.swift') if cxx
                else ('api.h', 'provider.c', 'consumer.swift'))
            module_name = rows['cxx_module' if cxx else 'module']
            standard = '-std=c++20' if cxx else '-std=c17'
            import_args = CXX_IMPORT if cxx else ()
            required = '  requires cplusplus\n' if cxx else ''
            provider = output / (prefix + 'provider-inputs')
            provider.mkdir()
            for name in (header_name, provider_name):
                shutil.copyfile(fixture / name, provider / name)
            original_bytes = (provider / header_name).read_bytes()
            consumer = (fixture / consumer_name).read_bytes()

            def module(directory, content):
                directory.mkdir(parents=True)
                header, module_map = directory / 'API.h', directory / 'module.modulemap'
                header.write_bytes(content)
                module_map.write_text(f'module {module_name} {{\n{required}  header "API.h"\n  export *\n}}\n')
                return header, module_map

            def typecheck(name, module_root, text=consumer, *, expected=0, contains=()):
                work = output / name
                work.mkdir()
                source, depfile = work / 'Consumer.swift', work / 'consumer.d'
                source.write_bytes(text)
                run(name, [frontend, '-typecheck', source, '-module-name', 'Consumer', '-swift-version', '6',
                    '-target', rows['target'], '-resource-dir', swift_root / 'usr/lib/swift', *import_args,
                    '-I', module_root, '-module-cache-path', work / 'fresh-cache', '-emit-dependencies-path', depfile],
                    work=work, foreign=True, expected=expected, contains=contains)
                if expected == 0:
                    proof = import_evidence(depfile, work, module_root / 'API.h', module_root / 'module.modulemap')
                    frozen.update({path: value for path, value in proof.items() if not Path(path).is_relative_to(output)})
                    return proof

            original = output / (prefix + 'original-module')
            module(original, original_bytes)
            original_imports = typecheck(prefix + 'original-positive-control', original)
            report['controls'].append({'name': prefix + 'original-header-independent-import',
                                       'status': 'PASS', 'imports': original_imports})
            emitted = output / (prefix + 'emitted')
            emitted.mkdir()
            interface = emitted / (module_name + '.objc3i')

            def emit(name, source, destination):
                run(prefix + name, [sdk / 'bin/objc3c', source, '--objc3', *(['--objcxx'] if cxx else []),
                    '--emit-interface', destination, '--module', module_name, '--package', 'swift-consumer-v2',
                    '--', '--no-default-config', standard, '--target='+rows['target']])
                return json.loads(destination.with_suffix(destination.suffix+'.json').read_text())

            sidecar = emit('native-emission', provider / header_name, interface)
            replay = emitted / 'Replay.objc3i'
            replay_sidecar = emit('native-interface-replay', interface, replay)
            emitted_bytes = interface.read_bytes()
            if emitted_bytes != replay.read_bytes() or sidecar['module'] != replay_sidecar['module']:
                raise ValueError('native foreign metadata replay changed bytes or identity')
            replay_root = output / (prefix + 'relocated inputs') / 'module'
            module(replay_root, emitted_bytes)
            report['interfaces'][language] = {'module_identity': sidecar['module'], 'header_sha256': digest(interface)}

            objects = {}
            for optimization in ('O0', 'O2'):
                obj = output / (prefix + 'provider-' + optimization + '.o')
                run(prefix + 'provider-' + optimization, [sdk / 'bin/clang', '--no-default-config',
                    '--target='+rows['target'], standard, '-'+optimization, '-g', '-c', provider / provider_name, '-o', obj])
                objects[optimization] = obj

            retained = output / (prefix + 'retained-original')
            retained.mkdir()
            shutil.move(provider, retained / 'provider-inputs')
            shutil.move(original, retained / 'original-module')
            report['relocation'][language] = {'original_header_hidden': not original.exists(),
                'provider_source_hidden': not provider.exists(),
                'replayed_header_unchanged': digest(replay_root / 'API.h') == digest(interface)}
            replay_imports = typecheck(prefix + 'emitted-positive-control', replay_root)
            report['controls'].append({'name': prefix + 'unmodified-emitted-header-import',
                                       'status': 'PASS', 'imports': replay_imports})

            if cxx:
                before, after = b'int count;', b'long long count;'
                mismatch_source = consumer
                mismatch_diagnostic = 'cannot convert value of type'
                control = 'field-type-identity-and-consumer'
                symbol = 'MeterAPI::initialReading()'
            else:
                for name, (source, diagnostic) in NEGATIVES.items():
                    typecheck('negative-'+name, replay_root, source.encode(), expected=1, contains=(diagnostic,))
                    report['controls'].append({'name': name, 'status': 'PASS'})
                before, after = b'projectedValue(_:)', b'changedValue(_:)'
                mismatch_source = b'import ForeignAPI\nlet value = projectedValue(41)\n'
                mismatch_diagnostic = "cannot find 'projectedValue'"
                control = 'public-name-identity-and-consumer'
                symbol = 'objc3_swift_value'
            def check_changed_contract(label, before, after, source, diagnostic, control):
                if original_bytes.count(before) != 1:
                    raise ValueError('Swift consumer mutation must target one declaration')
                changed = emitted / (label + '-' + header_name)
                changed.write_bytes(original_bytes.replace(before, after))
                changed_interface = emitted / (label + '.objc3i')
                altered = emit(label + '-public-contract', changed, changed_interface)
                if altered['module']['semantic_fingerprint'] == sidecar['module']['semantic_fingerprint']:
                    raise ValueError('native fingerprint ignored the changed Swift contract')
                changed_root = output / (prefix + label + '-module')
                module(changed_root, changed_interface.read_bytes())
                typecheck(prefix + label + '-contract-control', changed_root, source,
                          expected=1, contains=(diagnostic,))
                report['controls'].append({'name': prefix + control, 'status': 'PASS'})

            check_changed_contract('changed', before, after, mismatch_source, mismatch_diagnostic, control)
            if cxx:
                check_changed_contract('deleted-destructor', b'~ReadingOwner() noexcept;',
                    b'~ReadingOwner() = delete;', consumer,
                    "'ReadingOwner' is not a member type", 'destructor-identity-and-consumer')
                typecheck('cxx-move-only-copy-control', replay_root, b'''import ForeignCxx
func requiresCopy<T: Copyable>(_ value: T) {}
func copyUnique(_ value: borrowing MeterAPI.UniqueReading) { requiresCopy(value) }
''', expected=1, contains=("conform to 'Copyable'",))
                report['controls'].append({'name': 'cxx-move-only-copy-rejected', 'status': 'PASS'})

            driver = [swiftc, '-swift-version', '6', '-use-ld=lld', '-target', rows['target'], *import_args]
            for transport, content in (('original', original_bytes), ('emitted', emitted_bytes)):
                for native_opt, obj in objects.items():
                    for swift_opt in ('Onone', 'O'):
                        name = prefix + '-'.join((transport, native_opt, swift_opt))
                        case = output / name
                        header, module_map = module(case / 'module', content)
                        source, binary = case / 'Consumer.swift', case / 'consumer'
                        source.write_bytes(consumer)
                        proof = typecheck(name+'-typecheck', case / 'module')
                        cache = case / 'fresh-link-cache'
                        run(name+'-link', [*driver, '-'+swift_opt, '-I', case / 'module', '-module-cache-path',
                            cache, source, obj, '-o', binary], work=case, foreign=True)
                        graph = loaded_graph(binary, loader, [], run=lambda suffix, argv: run(name+'-'+suffix, argv, foreign=True))
                        if any(Path(path).name.startswith('libobjc') or Path(path).is_relative_to(sdk)
                               for path in graph['libraries']):
                            raise ValueError('Swift witness accidentally loaded the Objective-C/product runtime')
                        frozen.update(graph['libraries'])
                        observed = execution_result(run(name+'-execute', [binary], work=case, foreign=True), language)
                        report['cases'].append({'name': name, 'language': language, 'native_optimization': native_opt,
                            'swift_optimization': swift_opt, 'transport': transport, 'status': 'PASS',
                            'result': observed, 'imports': proof, 'header_sha256': digest(header),
                            'module_map_sha256': digest(module_map), 'binary_sha256': digest(binary), 'loaded_graph': graph})
                        shutil.rmtree(cache)
            missing = output / (prefix + 'missing-symbol')
            missing.mkdir()
            source = missing / 'Consumer.swift'
            source.write_bytes(consumer)
            absent = missing / 'must-not-exist'
            run(prefix + 'missing-provider-control', [*driver, '-Onone', '-I', replay_root,
                '-module-cache-path', missing / 'fresh-cache', source, '-o', absent], work=missing,
                foreign=True, expected=1, contains=(symbol,))
            if absent.exists():
                raise ValueError('missing provider produced a Swift executable')
            report['controls'].append({'name': prefix + 'missing-provider-symbol', 'status': 'PASS'})

        for language in ('c', 'cxx'):
            qualify_language(language)
        qualify_async_bridge(output, fixture, sdk, swift_root, rows['target'], loader, run, report)
        if (len(report['cases']) != 16 or len(report['async_cases']) != len(ASYNC_CASES) or
                any(digest(Path(path)) != value for path, value in frozen.items())):
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
    print(json.dumps({'status': result['status'], 'cases': len(result['cases']) + len(result['async_cases']),
                      'async_cases': len(result['async_cases']), 'controls': len(result['controls'])}))


if __name__ == '__main__':
    main()
