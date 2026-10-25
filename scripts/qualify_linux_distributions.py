"""Qualify an exact Linux SDK archive on pinned fresh Debian/Ubuntu images."""
from __future__ import annotations

import argparse
import csv
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import io
import json
import os
from pathlib import Path
import subprocess
import time
import threading
import uuid

ROOT = Path(__file__).resolve().parents[1]
IMAGES = {
    'debian13': 'debian:13-slim@sha256:a29215f6a35e51e22adffa17f89e9d2ef06214e64a2bad10d765c46aea49f11f',
    'ubuntu2604': 'ubuntu:26.04@sha256:c098726dd9b9733f3e885f8de0ef375206a311b0a317eadc40d347b1115fb944',
}


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def mount(source: Path, target: str) -> str:
    output = io.StringIO()
    csv.writer(output, lineterminator='').writerow(
        ['type=bind', 'src=' + str(source), 'dst=' + target, 'readonly'])
    return output.getvalue()


def qualify(release_file: Path, output: Path, *, distributions=None,
            docker_host='unix:///var/run/docker.sock') -> dict:
    output = output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    receipt = output / 'qualification.json'
    report = {'schema': 'objc3c.fresh-linux-distributions.v1', 'status': 'running',
              'scope': 'native-features', 'distributions': []}
    started = time.monotonic()
    receipt_lock = threading.RLock()

    def persist():
        with receipt_lock:
            temporary = receipt.with_suffix('.json.tmp')
            temporary.write_text(json.dumps(report, indent=2) + '\n')
            temporary.replace(receipt)

    def update_record(record, **fields):
        with receipt_lock:
            record.update(fields)

    persist()  # Never leave an earlier PASS visible during admission or failure.
    qualifier = ROOT / 'scripts/qualify_linux_archive.sh'
    docker = ['docker', '--host=' + docker_host]
    environment = {k: v for k, v in os.environ.items()
                   if k not in ('DOCKER_HOST', 'DOCKER_CONTEXT', 'DOCKER_TLS',
                                'DOCKER_TLS_VERIFY', 'DOCKER_CERT_PATH')}

    def run(arguments, log, timeout):
        with log.open('w') as stream:
            return subprocess.run(arguments, env=environment, stdout=stream,
                                  stderr=subprocess.STDOUT, timeout=timeout).returncode

    try:
        release_file = release_file.resolve(strict=True)
        release_hash = digest(release_file)
        release = json.loads(release_file.read_text())
        if release.get('status') != 'PASS':
            raise ValueError('the selected SDK release has not passed')
        archive = Path(release['archive']).resolve(strict=True)
        archive_hash = digest(archive)
        if archive_hash != release['archive_sha256']:
            raise ValueError('the selected SDK archive digest differs from its release')
        qualifier_hash = digest(qualifier)
        report.update(archive=str(archive), archive_sha256=archive_hash,
                      sdk_seal=release['seal_sha256'],
                      release_sha256=release_hash, qualifier_sha256=qualifier_hash)
        def one_distribution(distribution):
            image = IMAGES[distribution]
            name = 'objc3-features-' + distribution + '-' + uuid.uuid4().hex[:12]
            record = {'distribution': distribution, 'image': image, 'container': name,
                      'status': 'running', 'ordinary_user': True}
            with receipt_lock:
                report['distributions'].append(record)
            persist()
            print(distribution + ': qualifying native features', flush=True)
            create_log = output / (distribution + '-create.log')
            run_log = output / (distribution + '.log')
            cleanup_log = output / (distribution + '-cleanup.log')
            # SDK relocation/reinstall temporarily holds two extracted copies.
            # Keep installations on disk; only disposable consumers use tmpfs.
            command = [*docker, 'run', '--detach', '--name', name, '--cpus', '2',
                       '--memory', '5g', '--cap-add', 'SYS_PTRACE',
                       '--tmpfs', '/tmp:rw,exec,nosuid,nodev,size=1g',
                       '--mount', mount(archive, '/release.tar.zst'),
                       '--mount', mount(qualifier, '/qualify.sh')]
            certificate = Path('/etc/ssl/certs/ca-certificates.crt')
            if certificate.is_file():
                command += ['--mount', mount(certificate, '/run/proxy-ca.pem'),
                            '--env', 'SSL_CERT_FILE=/run/proxy-ca.pem',
                            '--env', 'PIP_CERT=/run/proxy-ca.pem']
            command += [image, 'sleep', 'infinity']
            try:
                code = run(command, create_log, 180)
                if code:
                    raise ValueError(distribution + ': container creation failed')
                setup = ('if test -f /run/proxy-ca.pem; then '
                         'printf \'Acquire::https::CaInfo "/run/proxy-ca.pem";\\n\' '
                         '> /etc/apt/apt.conf.d/99objc-proxy-ca; fi; '
                         'printf "#!/bin/sh\\nexit 0\\n" > /tmp/exec-probe; '
                         'chmod +x /tmp/exec-probe; /tmp/exec-probe; '
                         'exec sh /qualify.sh /release.tar.zst "$1" /opt/objc3 native-features')
                code = run([*docker, 'exec', name, 'sh', '-ec', setup, 'qualify',
                            archive_hash], run_log, 1800)
                update_record(record, exit_code=code, log_sha256=digest(run_log))
                text = run_log.read_text(errors='replace')
                markers = ('Unprivileged Linux native feature consumer: PASS',
                           'Fresh Linux native feature qualification: PASS')
                if code or any(marker not in text for marker in markers):
                    raise ValueError(distribution + ': fresh archive qualification failed')
                update_record(record, status='PASS', tmpfs_execution_verified=True)
            except BaseException:
                update_record(record, status='FAIL')
                raise
            finally:
                cleanup = run([*docker, 'container', 'rm', '--force', name], cleanup_log, 60)
                update_record(record, container_removed=cleanup == 0)
                if cleanup:
                    update_record(record, status='FAIL')
                    raise ValueError(distribution + ': container cleanup failed')
                persist()
            print(distribution + ': PASS', flush=True)

        selected = list(dict.fromkeys(distributions or IMAGES))
        failures = []
        with ThreadPoolExecutor(max_workers=min(2, len(selected))) as executor:
            futures = [executor.submit(one_distribution, name) for name in selected]
            for future in as_completed(futures):
                try:
                    future.result()
                except Exception as error:
                    failures.append(str(error))
        if failures:
            raise ValueError('; '.join(failures))
        if (digest(archive) != archive_hash or digest(qualifier) != qualifier_hash or
                digest(release_file) != release_hash):
            raise ValueError('fresh-distribution inputs changed during execution')
        report['status'] = 'PASS'
    except BaseException as error:
        report.update(status='FAIL', error=str(error))
        raise
    finally:
        report['elapsed_seconds'] = round(time.monotonic() - started, 6)
        persist()
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--release', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--distribution', choices=IMAGES, action='append')
    args = parser.parse_args()
    report = qualify(args.release, args.output, distributions=args.distribution)
    print(report['status'] + ': exact archive passed fresh native feature consumers')


if __name__ == '__main__':
    main()
