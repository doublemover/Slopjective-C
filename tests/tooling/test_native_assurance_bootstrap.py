"""Bootstrap failures remain failed and retain per-component diagnostics."""
import json
import os
from pathlib import Path
import subprocess

import pytest

from scripts.objc3c_assurance.bootstrap import setup

REPO = Path(__file__).resolve().parents[2]


@pytest.mark.parametrize('exit_code', [0, 7])
def test_setup_copies_component_logs_and_preserves_original_status(tmp_path, exit_code):
    repo = tmp_path / 'source'; (repo / 'scripts').mkdir(parents=True)
    (repo / 'scripts/setup_cloud_environment.sh').write_text('''set -eu
mkdir -p "$OBJC3C_CLOUD_SETUP_ROOT/logs"
printf 'missing verifier dependency\\n' > "$OBJC3C_CLOUD_SETUP_ROOT/logs/install_ccache.log"
echo 'setup console'
exit '''+str(exit_code))
    output = tmp_path / 'evidence'
    assert setup(repo, tmp_path / 'tools', output) == exit_code
    report = json.loads((output / 'result.json').read_text())
    assert report['status'] == ('PASS' if exit_code == 0 else 'FAIL')
    assert report['exit_code'] == exit_code
    assert (output / 'install_ccache.log').read_text() == 'missing verifier dependency\n'
    assert {row['path'] for row in report['artifacts']} == {'setup.log', 'install_ccache.log'}
    with pytest.raises(FileExistsError):
        setup(repo, tmp_path / 'tools', output)


@pytest.mark.parametrize('bad_signature', [False, True])
def test_ccache_verifier_requires_private_pinned_dependency_before_extracting(tmp_path, bad_signature):
    # Replace network/package operations; execute the real shell control flow.
    # The verifier refuses ambient-only libsodium and simulates a bad signature.
    verifier = tmp_path / 'verifier'
    verifier.write_text('''#!/bin/sh
set -eu
test -f "$LD_LIBRARY_PATH/libsodium.so.23"
test "$LD_LIBRARY_PATH" = "$EXPECTED_PRIVATE_LIB"
exit "$SIGNATURE_STATUS"
'''); verifier.chmod(0o755)
    script = '''set -eu
source "$1/scripts/setup_cloud_environment.sh"
mkdir -p "$setup_root/bin"
scratch="$2/scratch"
mkdir -p "$scratch"
export EXPECTED_PRIVATE_LIB="$scratch/minisign/usr/lib/x86_64-linux-gnu"
download_verified() { touch "$2"; }
dpkg-deb() {
  if [[ "$2" == *libsodium.deb ]]; then
    mkdir -p "$3/usr/lib/x86_64-linux-gnu"
    touch "$3/usr/lib/x86_64-linux-gnu/libsodium.so.23"
  else
    mkdir -p "$3/usr/bin"
    cp "$VERIFIER" "$3/usr/bin/minisign"
  fi
}
curl() { :; }
tar() {
  touch "$TRACE"
  mkdir -p "$setup_root/ccache-4.14.2-linux-x86_64-glibc"
  printf '#!/bin/sh\\nexit 0\\n' > "$setup_root/ccache-4.14.2-linux-x86_64-glibc/ccache"
  chmod +x "$setup_root/ccache-4.14.2-linux-x86_64-glibc/ccache"
}
install_ccache
'''
    trace = tmp_path / 'archive-extracted'
    result = subprocess.run(['bash', '-c', script, 'test', str(REPO), str(tmp_path)],
        env={**os.environ, 'OBJC3C_CLOUD_SETUP_ROOT': str(tmp_path / 'tools'), 'VERIFIER': str(verifier),
             'TRACE': str(trace), 'SIGNATURE_STATUS': '1' if bad_signature else '0'},
        capture_output=True, text=True, timeout=15)
    assert (result.returncode != 0) == bad_signature, result.stderr
    assert trace.exists() != bad_signature
    if not bad_signature:
        assert not (tmp_path / 'scratch/libsodium.deb').exists()


def test_ephemeral_git_trust_is_limited_to_the_checked_out_workspace(tmp_path):
    repos = [tmp_path / name for name in ('checkout', 'unrelated')]
    environment = {**os.environ, 'HOME': str(tmp_path / 'home'),
                   'XDG_CONFIG_HOME': str(tmp_path / 'activated-config'),
                   'GIT_CONFIG_GLOBAL': str(tmp_path / 'git-global-config'),
                   'GIT_CONFIG_NOSYSTEM': '1', 'GIT_TEST_ASSUME_DIFFERENT_OWNER': '1'}
    for repo in repos:
        subprocess.run(['git','init','-q',str(repo)], env=environment, check=True)
    def status(repo):
        return subprocess.run(['git','-C',str(repo),'status','--short'], env=environment,
                              capture_output=True,text=True)
    assert status(repos[0]).returncode == 128
    subprocess.run(['git','config','--global','--add','safe.directory',str(repos[0])],
                   env=environment,check=True)
    assert status(repos[0]).returncode == 0
    assert status(repos[1]).returncode == 128
