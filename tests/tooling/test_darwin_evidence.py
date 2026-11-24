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
