#!/usr/bin/env python3
"""Build a deterministic MMRecompRando installer for Android ARM64."""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
import zipfile
from pathlib import Path


ZIP_TIMESTAMP = (1980, 1, 1, 0, 0, 0)
EXPECTED_DEPENDENCIES = {
    "mm_recomp_better_double_sot",
    "mm_recomp_colors",
    "owls_never_quit",
}


def parse_args() -> argparse.Namespace:
    repo = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rando", type=Path, required=True)
    parser.add_argument("--glue", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, default=repo / "dist")
    return parser.parse_args()


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def read_mod_manifest(path: Path) -> dict:
    try:
        with zipfile.ZipFile(path) as archive:
            return json.loads(archive.read("mod.json"))
    except (OSError, KeyError, json.JSONDecodeError, zipfile.BadZipFile) as exc:
        raise ValueError(f"Invalid NRM {path}: {exc}") from exc


def dependency_id(spec: str) -> str:
    return spec.split(":", 1)[0]


def deterministic_write(archive: zipfile.ZipFile, name: str, data: bytes) -> None:
    info = zipfile.ZipInfo(name, ZIP_TIMESTAMP)
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = 0o100644 << 16
    archive.writestr(info, data, compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)


def validate_native_library(repo: Path, path: Path) -> None:
    sys.path.insert(0, str(repo / "tools"))
    import verify_android_native_mods  # pylint: disable=import-outside-toplevel

    errors = verify_android_native_mods.validate_so(path.name, path.read_bytes())
    if errors:
        raise ValueError("; ".join(errors))


def main() -> int:
    args = parse_args()
    repo = Path(__file__).resolve().parents[1]
    for path in (args.rando, args.glue):
        if not path.is_file():
            raise FileNotFoundError(f"Missing package input: {path}")

    manifest = read_mod_manifest(args.rando)
    if manifest.get("id") != "mm_recomp_rando":
        raise ValueError(f"Unexpected mod id: {manifest.get('id')!r}")
    if "APCpp-Glue" not in manifest.get("native_libraries", {}):
        raise ValueError("MMRecompRando does not declare APCpp-Glue")

    dependencies = {dependency_id(spec) for spec in manifest.get("dependencies", [])}
    if dependencies != EXPECTED_DEPENDENCIES:
        raise ValueError(
            f"Dependency list changed: expected {sorted(EXPECTED_DEPENDENCIES)}, "
            f"got {sorted(dependencies)}"
        )

    validate_native_library(repo, args.glue)

    version = str(manifest.get("version", "unknown"))
    payloads = {
        "mm_recomp_rando.nrm": args.rando.read_bytes(),
        "APCpp-Glue.so": args.glue.read_bytes(),
    }
    checksums = "".join(
        f"{sha256(data)}  {name}\n" for name, data in payloads.items()
    )
    readme = f"""# MMRecompRando for Android ARM64

Rando version: {version}

Install the required dependency mods first, then select this ZIP with Zelda64
Recompiled Android's **Install Mods** button.

Required mods:

- Owls Never Quit
- Better Song of Double Time
- MM Recolors

Message Hooks may also be installed automatically or separately as a transitive
dependency of Better Song of Double Time.

The package contains only `mm_recomp_rando.nrm` and the Android ARM64
`APCpp-Glue.so`. Host-side `.apworld`, YAML, generator, and server files are not
included.

Server addresses use certificate-validated `wss://` by default. For a trusted
server on your local network which does not provide TLS, enter the complete
address with an explicit `ws://` prefix.
"""

    args.output_dir.mkdir(parents=True, exist_ok=True)
    output = args.output_dir / f"MMRecompRando-Android-Mod-arm64-v{version}.zip"
    with zipfile.ZipFile(output, "w") as archive:
        for name, data in payloads.items():
            deterministic_write(archive, name, data)
        deterministic_write(archive, "README.md", readme.encode())
        deterministic_write(archive, "SHA256SUMS", checksums.encode())

    print(output)
    print(f"SHA-256 {sha256(output.read_bytes())}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
