#!/usr/bin/env python3
import argparse
import struct
import sys
import zipfile
from pathlib import Path


LINUX_ONLY_NEEDED = {
    "ld-linux-aarch64.so.1",
    "libc.so.6",
    "libdl.so.2",
    "libgcc_s.so.1",
    "libm.so.6",
    "libpthread.so.0",
    "libstdc++.so.6",
}

MIN_LOAD_ALIGNMENT = 16384


def c_string(data, offset):
    end = data.find(b"\0", offset)
    if end == -1:
        end = len(data)
    return data[offset:end].decode("utf-8", errors="replace")


def vaddr_to_offset(load_segments, vaddr):
    for file_offset, virtual_addr, file_size in load_segments:
        if virtual_addr <= vaddr < virtual_addr + file_size:
            return file_offset + (vaddr - virtual_addr)
    return None


def read_needed(data):
    e_phoff = struct.unpack_from("<Q", data, 32)[0]
    e_phentsize = struct.unpack_from("<H", data, 54)[0]
    e_phnum = struct.unpack_from("<H", data, 56)[0]
    load_segments = []
    dynamic = None

    for index in range(e_phnum):
        offset = e_phoff + index * e_phentsize
        p_type = struct.unpack_from("<I", data, offset)[0]
        p_offset = struct.unpack_from("<Q", data, offset + 8)[0]
        p_vaddr = struct.unpack_from("<Q", data, offset + 16)[0]
        p_filesz = struct.unpack_from("<Q", data, offset + 32)[0]
        if p_type == 1:  # PT_LOAD
            load_segments.append((p_offset, p_vaddr, p_filesz))
        elif p_type == 2:  # PT_DYNAMIC
            dynamic = (p_offset, p_filesz)

    if dynamic is None:
        return []

    needed_offsets = []
    strtab_vaddr = None
    dyn_offset, dyn_size = dynamic
    for offset in range(dyn_offset, dyn_offset + dyn_size, 16):
        tag = struct.unpack_from("<q", data, offset)[0]
        value = struct.unpack_from("<Q", data, offset + 8)[0]
        if tag == 0:  # DT_NULL
            break
        if tag == 1:  # DT_NEEDED
            needed_offsets.append(value)
        elif tag == 5:  # DT_STRTAB
            strtab_vaddr = value

    if strtab_vaddr is None:
        return []

    strtab_offset = vaddr_to_offset(load_segments, strtab_vaddr)
    if strtab_offset is None:
        return []

    return [c_string(data, strtab_offset + needed_offset) for needed_offset in needed_offsets]


def read_load_alignments(data):
    e_phoff = struct.unpack_from("<Q", data, 32)[0]
    e_phentsize = struct.unpack_from("<H", data, 54)[0]
    e_phnum = struct.unpack_from("<H", data, 56)[0]
    if e_phentsize < 56:
        raise ValueError("invalid program header size")

    alignments = []
    for index in range(e_phnum):
        offset = e_phoff + index * e_phentsize
        if offset + 56 > len(data):
            raise ValueError("truncated program header table")
        p_type = struct.unpack_from("<I", data, offset)[0]
        if p_type == 1:  # PT_LOAD
            alignments.append(struct.unpack_from("<Q", data, offset + 48)[0])
    return alignments


def validate_so(label, data):
    errors = []
    if len(data) < 64 or data[:4] != b"\x7fELF":
        return [f"{label}: not an ELF file"]
    if data[4] != 2:
        errors.append(f"{label}: expected ELF64, got class {data[4]}")
    if data[5] != 1:
        errors.append(f"{label}: expected little-endian ELF, got data encoding {data[5]}")

    if len(data) >= 20 and data[5] == 1:
        e_type = struct.unpack_from("<H", data, 16)[0]
        e_machine = struct.unpack_from("<H", data, 18)[0]
        if e_type != 3:
            errors.append(f"{label}: expected shared object ET_DYN, got e_type {e_type}")
        if e_machine != 183:
            errors.append(f"{label}: expected AArch64 e_machine 183, got {e_machine}")

    try:
        needed = read_needed(data)
    except (struct.error, IndexError) as exc:
        errors.append(f"{label}: failed to read dynamic dependencies: {exc}")
        needed = []

    try:
        load_alignments = read_load_alignments(data)
    except (struct.error, IndexError, ValueError) as exc:
        errors.append(f"{label}: failed to read load segment alignment: {exc}")
        load_alignments = []

    bad_alignments = [alignment for alignment in load_alignments if alignment < MIN_LOAD_ALIGNMENT]
    if bad_alignments:
        formatted = ", ".join(f"0x{alignment:x}" for alignment in bad_alignments)
        errors.append(f"{label}: PT_LOAD alignment below 16 KiB: {formatted}")

    bad_needed = sorted(set(needed) & LINUX_ONLY_NEEDED)
    if bad_needed:
        errors.append(f"{label}: has non-Android Linux dependencies: {', '.join(bad_needed)}")

    status = "ok" if not errors else "invalid"
    align_text = ", ".join(f"0x{alignment:x}" for alignment in load_alignments) if load_alignments else "none"
    print(f"{label}: {status} needed=[{', '.join(needed) if needed else 'none'}] load_align=[{align_text}]")
    return errors


def iter_native_libs(path):
    if path.is_dir():
        for child in sorted(path.iterdir()):
            yield from iter_native_libs(child)
        return

    if path.suffix == ".so":
        yield str(path), path.read_bytes()
        return

    if path.suffix in {".nrm", ".zip"}:
        try:
            with zipfile.ZipFile(path) as archive:
                for entry in archive.infolist():
                    if entry.filename.endswith(".so"):
                        with archive.open(entry) as entry_file:
                            yield f"{path}!{entry.filename}", entry_file.read()
        except zipfile.BadZipFile:
            return


def main():
    parser = argparse.ArgumentParser(description="Validate Android native mod shared libraries.")
    parser.add_argument("paths", nargs="+", type=Path)
    args = parser.parse_args()

    errors = []
    checked = 0
    for path in args.paths:
        for label, data in iter_native_libs(path):
            checked += 1
            errors.extend(validate_so(label, data))

    if checked == 0:
        errors.append("no native mod shared libraries found")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
