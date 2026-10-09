#!/usr/bin/env python3
"""Ordinary-Clang source-only producer/consumer baseline (no extension import claim)."""
import pathlib
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
FIXTURE = ROOT / "tests" / "clang_foundation"
compiler = shutil.which("clang") or shutil.which("clang-18") or shutil.which("clang-17")
if not compiler:
    raise SystemExit("SKIP: ordinary Clang is unavailable")
with tempfile.TemporaryDirectory(prefix="objc3-interface-baseline-") as directory:
    work = pathlib.Path(directory)
    for name in ("provider.c", "provider.h", "consumer.c"):
        shutil.copy2(FIXTURE / name, work / name)
    subprocess.run([compiler, "-std=c18", "-c", "provider.c", "-o", "provider.o"], cwd=work, check=True)
    (work / "provider.c").unlink()
    subprocess.run([compiler, "-std=c18", "-c", "consumer.c", "-o", "consumer.o"], cwd=work, check=True)
    subprocess.run([compiler, "provider.o", "consumer.o", "-o", "consumer"], cwd=work, check=True)
    subprocess.run([str(work / "consumer")], cwd=work, check=True)
print("PASS: C18 Clang source-only producer/consumer baseline (no extension import claim)")
