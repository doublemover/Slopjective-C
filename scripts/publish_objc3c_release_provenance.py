#!/usr/bin/env python3
"""Publish SBOM and attestation artifacts for the canonical objc3c release payload."""

from __future__ import annotations

from objc3c_release_manifest.publication import main


if __name__ == '__main__':
    raise SystemExit(main())
