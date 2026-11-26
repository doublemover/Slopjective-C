import json
from pathlib import Path
import struct

import pytest

from scripts.objc3c_toolchain import darwin_evidence as evidence
from scripts.objc3c_toolchain.darwin_bootstrap import MACHO_TYPES


REVISION = 'b' * 40


@pytest.fixture
def bootstrap_artifact(tmp_path):
    report = {'schema': 'objc3c.darwin-bootstrap-probe.v1', 'status': 'bootstrap-verified',
              'support_truth': False, 'checkout_revision': REVISION,
              'probe_sha256': evidence.file_digest(Path(evidence.__file__).with_name('darwin_bootstrap.py')),
              'platform_identity': {'os': 'macOS', 'arch': 'arm64', 'sdk_version': evidence.SDK_VERSION},
              'apple_toolchain': {'developer_tools_version': evidence.XCODE},
              'deployment_target': evidence.DEPLOYMENT_TARGET,
              'artifacts': {}, 'commands': []}
    pinned = json.loads((evidence.ROOT / 'toolchains/llvm/manifest.json').read_text())['upstream']
    report['llvm_identity'] = {'revision': pinned['commit'], 'version': pinned['tag'].removeprefix('llvmorg-')}
    for name, relative, kind in (
        ('object', 'answer.o', 'object'),
        ('debug_info', 'static.dSYM/Contents/Resources/DWARF/static', 'debug_info'),
        ('bin/static', 'Relocated install/bin/static', 'executable'),
        ('bin/dynamic', 'Relocated install/bin/dynamic', 'executable'),
        ('lib/libanswer.dylib', 'Relocated install/lib/libanswer.dylib', 'dynamic_library'),
    ):
        path = tmp_path / 'result' / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(struct.pack('<10I', 0xfeedfacf, 0x0100000c, 0, MACHO_TYPES[kind], 1, 8, 0, 0, 0, 8))
        row = evidence.verify_macho(path, kind)
        report['artifacts'][name] = row
        if kind == 'executable':
            report['commands'].append({'argv': [row['path']], 'exit_status': 0})
    archive = tmp_path / 'result/Relocated install/lib/libanswer.a'
    archive.write_bytes(b'!<arch>\n')
    report['artifacts']['archive'] = {'members': ['answer.o'], 'sha256': evidence.file_digest(archive)}
    report_file = tmp_path / 'result/report.json'
    report_file.write_text(json.dumps(report))
    (tmp_path / 'host.log').write_text(f'revision={REVISION}\narm64\n')
    return tmp_path, report, report_file


@pytest.mark.parametrize('defect', [None, 'different-source', 'failed', 'cancelled', 'missing-command', 'artifact'])
def test_terminal_rechecks_retained_bootstrap_revision_commands_and_artifacts(bootstrap_artifact, defect):
    root, report, report_file = bootstrap_artifact
    outcome = defect if defect in ('failed', 'cancelled') else 'success'
    revision = 'c' * 40 if defect == 'different-source' else REVISION
    if defect == 'missing-command':
        report['commands'].pop()
        report_file.write_text(json.dumps(report))
    if defect == 'artifact':
        binary = root / 'result/Relocated install/bin/dynamic'
        binary.write_bytes(binary.read_bytes() + b'changed')
    if defect is None:
        result = evidence.verify(root, revision, outcome=outcome, mode='bootstrap')
        assert result['status'] == 'PASS' and result['support_truth'] is False
    else:
        with pytest.raises(ValueError):
            evidence.verify(root, revision, outcome=outcome, mode='bootstrap')


def test_command_evidence_can_be_empty_but_never_linked(tmp_path):
    log = tmp_path / 'complete.log'
    log.touch()
    assert evidence.retained(tmp_path, log.name, allow_empty=True) == log
    with pytest.raises(ValueError, match='missing, linked or excessive'):
        evidence.retained(tmp_path, log.name)
    alias = tmp_path / 'linked.log'
    alias.symlink_to(log)
    with pytest.raises(ValueError, match='missing, linked or excessive'):
        evidence.retained(tmp_path, alias.name, allow_empty=True)


@pytest.mark.parametrize('defect', [None, 'source', 'seal', 'loader', 'runtime', 'results'])
def test_native_xscomic_evidence_requires_the_pinned_source_and_installed_runtime(tmp_path, defect):
    core = tmp_path / 'core'
    core.mkdir()
    pin = json.loads((evidence.ROOT / 'toolchains/llvm/experimental/darwin-xscomic.json').read_text())
    (tmp_path / 'source-revision').write_text(REVISION if defect == 'source' else pin['revision'])
    manifest = {'revision': REVISION, 'compiler_version': 'clang version 23.1.5'}
    seal = evidence._digest(manifest)
    installed = {'status': 'PASS', 'archive_sha256': 'a' * 64, 'seal_sha256': seal,
                 'prefix': '/private/var/tmp/xsComic SDK'}
    (tmp_path / 'install.json').write_text(json.dumps(installed))
    (core / 'sdk.json').write_text(json.dumps({**manifest, 'seal_sha256': 'c' * 64 if defect == 'seal' else seal}))
    (core / 'compiler.txt').write_text(manifest['compiler_version'] + '\n')
    (core / 'xsComicObjC3Tests').write_bytes(
        struct.pack('<10I', 0xfeedfacf, 0x0100000c, 0, MACHO_TYPES['executable'], 1, 8, 0, 0, 0, 8))
    (core / 'load-paths.txt').write_text(
        'xsComicObjC3Tests:\n\t@rpath/libobjc3darwin.dylib (version)\n\t/usr/lib/libSystem.B.dylib (version)\n'
        + ('\t/usr/lib/libobjc.A.dylib (version)\n' if defect == 'runtime' else ''))
    location = '/tmp/wrong SDK' if defect == 'loader' else '/var/tmp/xsComic SDK'
    (core / 'dyld.log').write_text(f'dyld[42]: <id> {location}/lib/libobjc3darwin.dylib\n')
    (core / 'results.txt').write_text(f"=== SUMMARY: 188 passed, {1 if defect == 'results' else 0} failed ===\n")
    sdk = {**installed, 'revision': REVISION, 'mode': 'package'}
    if defect:
        with pytest.raises(ValueError, match='xsComic'):
            evidence.verify_xscomic(tmp_path, sdk)
    else:
        assert evidence.verify_xscomic(tmp_path, sdk)['tests_passed'] == 188


@pytest.mark.parametrize('arity,fault', [
    (arity, fault) for arity in (1, 2)
    for fault in (None, 'missing-ir', 'wrong-optimization', 'changed-imports', 'missing-run')
] + [(2, 'missing-rejection'), (2, 'wrong-rejection-mode')])
def test_terminal_rechecks_typed_message_ir_imports_and_execution(tmp_path, arity, fault):
    family = 'int-method' if arity == 1 else 'two-int-method'
    prefix = family.replace('-', '_')
    field = 'int_argument_methods' if arity == 1 else 'two_int_argument_methods'
    suffix = 'int_arg_v1' if arity == 1 else 'two_int_args_v1'
    encoding = 'i20@0:8i16' if arity == 1 else 'i24@0:8i16i20'
    witness = f'owned Objective-C 3 Darwin {family.removesuffix("-method")}-argument methods: PASS\n'
    instance, super_lookup = 'objc3_darwin_lookup_' + suffix, 'objc3_darwin_lookup_super_' + suffix
    report = {'artifacts': {}, field: {'status': 'PASS', 'ir': {}, 'imports': {}}}
    proof, labels = report[field], {}

    def put(relative, content):
        path = tmp_path / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(content)
        return path

    def log(label, argv, content):
        return {'argv': argv, 'exit_status': 0, 'log': put(label + '.log', content).name}

    def macho(relative, kind):
        path = put(relative, struct.pack('<10I', 0xfeedfacf, 0x0100000c, 0, MACHO_TYPES[kind], 1, 8, 0, 0, 0, 8))
        return path, evidence.verify_macho(path, kind)

    arguments = ', i32 7' if arity == 2 else ''
    for unit, lookup in (('child', super_lookup), ('main', instance)):
        text = (f'@signature = private constant [{len(encoding)+1} x i8] c"{encoding}\\00"\n'
                f'define i32 @send(ptr %self, ptr %selector) {{\n'
                f'  %first = call ptr @{lookup}(ptr %self, ptr %selector)\n'
                f'  %one = call i32 %first(ptr %self, ptr %selector, i32 -2{arguments})\n'
                f'  %second = call ptr @{lookup}(ptr %self, ptr %selector)\n'
                f'  %two = call i32 %second(ptr %self, ptr %selector, i32 5{arguments})\n'
                '  ret i32 %two\n}\n')
        for level in ('o0', 'o2'):
            label = f'{family}-{unit}-{level}-ir'
            path = put('build/' + label + '.ll', text.encode())
            proof['ir'][label] = {'path': str(path), 'sha256': evidence.file_digest(path)}
            labels[label] = [{'exit_status': 0, 'argv': [
                '/toolchain/clang', '-' + level.upper(), '-S', '-emit-llvm',
                f'/source/tests/clang_foundation/darwin_registration/{prefix}_{unit}.m', '-o', str(path)]}]
    for name, kind, symbol in (
        (f'lib/lib{family}-child.dylib', 'dynamic_library', super_lookup),
        (f'bin/{family}-dynamic', 'executable', instance),
        (f'bin/{family}-arc', 'executable', instance),
        (f'bin/{family}-static', 'executable', 'puts'),
    ):
        path, record = macho('Relocated install/' + name, kind)
        record['load_paths'] = [f'@rpath/lib{family}-base.dylib', '@rpath/libobjc3darwin.dylib']
        report['artifacts'][name] = record
        proof['imports'][name] = [symbol]
        labels[path.name + '-imports'] = [log(path.name + '-imports', ['/usr/bin/nm', '-u', '-j', str(path)],
                                               ('_' + symbol + '\n').encode())]
        if kind == 'executable':
            labels[path.name + '-native'] = [log(path.name + '-native', [str(path)], witness.encode())]
    if arity == 2:
        for unit in ('base', 'child', 'main', 'arc'):
            name = prefix + '_' + ('main_arc' if unit == 'arc' else unit)
            path, report['artifacts'][name] = macho('build/' + name + '.o', 'object')
            fixture_unit = 'main' if unit == 'arc' else unit
            labels[f'{family}-{unit}-object'] = [{'argv': ['/toolchain/clang',
                *(['-fobjc-arc'] if unit == 'arc' else []), '-c',
                f'/source/darwin_registration/two_int_method_{fixture_unit}.m', '-o', str(path)]}]
        base = f'lib/lib{family}-base.dylib'
        _, report['artifacts'][base] = macho('Relocated install/' + base, 'dynamic_library')
        for control in ('type', 'arity', 'send-type', 'send-arity'):
            send = control.startswith('send-')
            subject = 'an ordinary int message' if send else 'ordinary int methods'
            label = 'unsupported-' + family + '-' + control
            labels[label] = [{
                'argv': ['/toolchain/clang', *(['-DDARWIN_UNSUPPORTED_SEND=1'] if send else []),
                         *(['-DDARWIN_UNSUPPORTED_ARITY=1'] if control.endswith('arity') else []),
                         '-c', '/source/tests/clang_foundation/darwin_registration/unsupported_two_int_method.m',
                         '-o', str(tmp_path / 'build' / (label + '.o'))],
                'expected_diagnostic': [f'experimental Darwin registration requires {subject} with at most two int arguments']}]

    if fault == 'missing-ir':
        del proof['ir'][family + '-main-o2-ir']
    elif fault == 'wrong-optimization':
        labels[family + '-main-o2-ir'][0]['argv'][1] = '-O0'
    elif fault == 'changed-imports':
        proof['imports'][f'bin/{family}-dynamic'] = ['different_symbol']
    elif fault == 'missing-run':
        del labels[family + '-dynamic-native']
    elif fault == 'missing-rejection':
        del labels['unsupported-two-int-method-send-arity']
    elif fault == 'wrong-rejection-mode':
        labels['unsupported-two-int-method-send-arity'][0]['argv'].remove('-DDARWIN_UNSUPPORTED_ARITY=1')
    if fault is None:
        evidence.typed_dispatch(tmp_path, report, labels, arity=arity)
    else:
        with pytest.raises(ValueError, match='Darwin'):
            evidence.typed_dispatch(tmp_path, report, labels, arity=arity)


def test_candidate_admission_requires_matching_pinned_terminal_bytes(tmp_path):
    result = {'status': 'PASS', 'mode': 'package-sanitizers', 'revision': 'a' * 40, 'support_truth': False,
              'bootstrap_sha256': 'a' * 64, 'registration_sha256': 'b' * 64,
              'archive_sha256': 'c' * 64, 'seal_sha256': 'd' * 64}
    file = tmp_path / 'darwin-terminal.json'
    file.write_text(json.dumps(result) + '\n')
    checksum = evidence.file_digest(file)
    admitted = evidence.admit_terminal(result, file, checksum)
    assert admitted == {**result, 'terminal_sha256': checksum, 'admission': 'experimental-package-verified'}
    file.write_text(json.dumps({**result, 'archive_sha256': 'e' * 64}) + '\n')
    with pytest.raises(ValueError, match='receipt SHA-256'):
        evidence.admit_terminal(result, file, checksum)
    with pytest.raises(ValueError, match='differs from independent'):
        evidence.admit_terminal(result, file, evidence.file_digest(file))
    with pytest.raises(ValueError, match='requires exact native package'):
        evidence.admit_terminal({**result, 'mode': 'bootstrap'}, file, checksum)


@pytest.mark.parametrize('defect', [None, 'missing', 'prefix-changed', 'wrong-archive', 'unchanged-linker'])
def test_sealed_package_requires_all_verified_installer_rejections(defect):
    archive = {'archive_sha256': 'a' * 64, 'seal_sha256': 'b' * 64,
               'host': {'tools': {'ld': {'sha256': 'c' * 64}}}}
    results = {}
    for name, diagnostic in evidence.INSTALL_CONTROLS.items():
        existing = name in ('damaged-prefix', 'incomplete-prefix')
        prefix = 'd' * 64 if existing else None
        results[name] = {'status': 'REJECTED', 'diagnostic': diagnostic,
                         'requested_archive_sha256': ('a' if existing else 'e') * 64,
                         'prefix_before_sha256': prefix, 'prefix_after_sha256': prefix}
    results['apple-linker-identity'].update(
        tool='ld', observed_tool_sha256='c' * 64, required_tool_sha256='f' * 64, manifest_seal_sha256='0' * 64)
    qualified = {'installer_controls': {'schema': 'objc3c.experimental-darwin-sdk-installer-controls.v1',
                                        'archive_sha256': archive['archive_sha256'], 'observations': results}}
    if defect == 'missing':
        del results['archive-digest']
    elif defect == 'prefix-changed':
        results['damaged-prefix']['prefix_after_sha256'] = 'a' * 64
    elif defect == 'wrong-archive':
        results['incomplete-prefix']['requested_archive_sha256'] = 'e' * 64
    elif defect == 'unchanged-linker':
        results['apple-linker-identity']['required_tool_sha256'] = 'c' * 64
    if defect is None:
        evidence.installer_controls(qualified, archive)
    else:
        with pytest.raises(ValueError, match='Darwin installer'):
            evidence.installer_controls(qualified, archive)
