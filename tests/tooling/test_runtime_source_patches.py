import hashlib
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_toolchain_runtime.source_patches import prepare_source, verify_source


def source_fixture(tmp_path):
    source = tmp_path / 'source'
    source.mkdir()
    def git(*args):
        return subprocess.check_output(['git', '-C', str(source), *args])
    git('init', '-q')
    file = source / 'runtime.m'
    file.write_text('original runtime\n')
    git('add', 'runtime.m')
    git('-c', 'user.name=Fixture', '-c', 'user.email=fixture@example.invalid',
        'commit', '-qm', 'pinned fixture')
    revision = git('rev-parse', 'HEAD').decode().strip()
    file.write_text('qualified runtime\n')
    patch = tmp_path / 'runtime.patch'
    patch.write_bytes(git('diff', '--binary', '--abbrev=9', '--no-ext-diff', 'HEAD'))
    file.write_text('original runtime\n')
    profile = {'runtime_commit': revision, 'source_patches': [
        {'file': patch.name, 'sha256': hashlib.sha256(patch.read_bytes()).hexdigest()}]}
    return source, profile


def test_owned_runtime_patch_materializes_once_and_verifies(tmp_path):
    source, profile = source_fixture(tmp_path)
    assert not verify_source(tmp_path, source, profile, allow_pristine=True)
    with pytest.raises(ValueError, match='differs'):
        verify_source(tmp_path, source, profile)
    prepare_source(tmp_path, source, profile)
    assert (source / 'runtime.m').read_text() == 'qualified runtime\n'
    assert verify_source(tmp_path, source, profile)
    prepare_source(tmp_path, source, profile)
    assert verify_source(tmp_path, source, profile)


@pytest.mark.parametrize('damage', ['content', 'mode', 'extra', 'patch', 'pin', 'missing'])
def test_runtime_source_damage_is_rejected_and_preserved(tmp_path, damage):
    source, profile = source_fixture(tmp_path)
    prepare_source(tmp_path, source, profile)
    file = source / 'runtime.m'
    if damage == 'content':
        file.write_text('unowned edit\n')
    elif damage == 'mode':
        file.chmod(0o755)
    elif damage == 'extra':
        (source / 'injected.h').write_text('unowned file')
    elif damage == 'patch':
        (tmp_path / 'runtime.patch').write_text('changed patch')
    elif damage == 'pin':
        profile['runtime_commit'] = '0' * 40
    else:
        file.unlink()
    def snapshot():
        return {str(p.relative_to(source)): (p.read_bytes(), p.stat().st_mode)
                for p in source.iterdir() if p.is_file()}
    before = snapshot()
    with pytest.raises(ValueError):
        prepare_source(tmp_path, source, profile)
    assert snapshot() == before


def test_foreign_edits_are_preserved_before_first_application(tmp_path):
    source, profile = source_fixture(tmp_path)
    file = source / 'runtime.m'
    file.write_text('existing user edit\n')
    with pytest.raises(ValueError, match='preserving'):
        prepare_source(tmp_path, source, profile)
    assert file.read_text() == 'existing user edit\n'


@pytest.mark.parametrize('damage', ['external', 'duplicate', 'malformed'])
def test_invalid_patch_inventory_cannot_change_source(tmp_path, damage):
    repo = tmp_path / 'repo'
    repo.mkdir()
    source, profile = source_fixture(repo)
    if damage == 'external':
        outside = tmp_path / 'outside.patch'
        outside.write_bytes((repo / 'runtime.patch').read_bytes())
        profile['source_patches'][0]['file'] = '../outside.patch'
    elif damage == 'duplicate':
        profile['source_patches'] *= 2
    else:
        profile['source_patches'][0]['file'] = None
    with pytest.raises(ValueError):
        prepare_source(repo, source, profile)
    assert (source / 'runtime.m').read_text() == 'original runtime\n'


def test_dwarf_leb_decoding_is_defined_across_full_width_values(tmp_path):
    import shutil
    repo = Path(__file__).resolve().parents[2]
    source = repo / 'out/toolchains/libobjc2-v2.3/src'
    compiler = shutil.which('clang')
    if not compiler or not source.is_dir():
        pytest.skip('requires Clang and the pinned native runtime source')
    executable = tmp_path / 'dwarf-leb'
    subprocess.run([compiler, '--no-default-config', '-std=c17',
                    '-fsanitize=undefined', '-fno-sanitize-recover=undefined',
                    '-I', str(source),
                    str(repo / 'tests/clang_foundation/runtime_dwarf_leb.c'),
                    '-o', str(executable)], check=True)
    subprocess.run([str(executable)], check=True)
