#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
lock_file="${repo_root}/packaging/mmrecomprando-android/inputs.lock.json"
source_root="${ZELDA_RANDO_GLUE_DIR:-${repo_root}/MMRecompAPCppGlue}"
build_root="${ZELDA_RANDO_BUILD_DIR:-${repo_root}/build/mmrecomprando-android}"
glue_build_root="${build_root}/glue"
rando_source_root="${build_root}/MMRecompRando"
mod_tool_build_root="${build_root}/mod-tool"

read_lock() {
    python3 -c 'import json, sys; print(json.load(open(sys.argv[1]))[sys.argv[2]])' \
        "${lock_file}" "$1"
}

glue_repository="$(read_lock glue_repository)"
glue_commit="$(read_lock glue_commit)"
apcpp_commit="$(read_lock apcpp_commit)"
ixwebsocket_commit="$(read_lock ixwebsocket_commit)"
n64recomp_commit="$(read_lock n64recomp_commit)"
rando_repository="$(read_lock rando_repository)"
rando_commit="$(read_lock rando_commit)"
rando_syms_commit="$(read_lock rando_syms_commit)"
rando_mm_decomp_commit="$(read_lock rando_mm_decomp_commit)"
rando_version="$(read_lock rando_version)"

if [[ ! -d "${source_root}/.git" ]]; then
    git clone --recursive "${glue_repository}" "${source_root}"
fi
git -C "${source_root}" checkout --detach "${glue_commit}"
git -C "${source_root}" submodule update --init --recursive

[[ "$(git -C "${source_root}" rev-parse HEAD)" == "${glue_commit}" ]]
[[ "$(git -C "${source_root}/lib/APCpp" rev-parse HEAD)" == "${apcpp_commit}" ]]
[[ "$(git -C "${source_root}/lib/APCpp/IXWebSocket" rev-parse HEAD)" == "${ixwebsocket_commit}" ]]

git -C "${source_root}" apply \
    "${repo_root}/packaging/mmrecomprando-android/patches/glue.patch"
git -C "${source_root}/lib/APCpp" apply \
    "${repo_root}/packaging/mmrecomprando-android/patches/apcpp.patch"
git -C "${source_root}/lib/APCpp/IXWebSocket" apply \
    "${repo_root}/packaging/mmrecomprando-android/patches/ixwebsocket.patch"
cp "${repo_root}/packaging/mmrecomprando-android/overlays/apcpp-solo-gen-android.cpp" \
    "${source_root}/apcpp-solo-gen-android.cpp"

: "${ANDROID_NDK_HOME:?ANDROID_NDK_HOME must point to Android NDK 28}"
cmake -S "${source_root}" -B "${glue_build_root}" -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release
cmake --build "${glue_build_root}" --parallel

glue_library="$(find "${glue_build_root}" -name libAPCpp-Glue.so | head -n 1)"
if [[ -z "${glue_library}" ]]; then
    echo "Unable to locate built libAPCpp-Glue.so" >&2
    exit 1
fi
strip_tool="$(find "${ANDROID_NDK_HOME}/toolchains/llvm/prebuilt" -name llvm-strip | head -n 1)"
if [[ -z "${strip_tool}" ]]; then
    echo "Unable to locate NDK llvm-strip" >&2
    exit 1
fi
"${strip_tool}" --strip-debug "${glue_library}"

n64recomp_source="${repo_root}/lib/N64ModernRuntime/N64Recomp"
[[ "$(git -C "${n64recomp_source}" rev-parse HEAD)" == "${n64recomp_commit}" ]]
cmake -S "${n64recomp_source}" -B "${mod_tool_build_root}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
cmake --build "${mod_tool_build_root}" --parallel --target RecompModTool

rm -rf "${rando_source_root}"
git clone --recursive "${rando_repository}" "${rando_source_root}"
git -C "${rando_source_root}" checkout --detach "${rando_commit}"
git -C "${rando_source_root}" submodule update --init --recursive

[[ "$(git -C "${rando_source_root}" rev-parse HEAD)" == "${rando_commit}" ]]
[[ "$(git -C "${rando_source_root}/Zelda64RecompSyms" rev-parse HEAD)" == "${rando_syms_commit}" ]]
[[ "$(git -C "${rando_source_root}/mm-decomp" rev-parse HEAD)" == "${rando_mm_decomp_commit}" ]]

git -C "${rando_source_root}" apply \
    "${repo_root}/packaging/mmrecomprando-android/patches/rando.patch"

make -C "${rando_source_root}" --jobs "$(nproc)" build/mod.elf
mkdir -p "${rando_source_root}/mm_recomp_rando"
(
    cd "${rando_source_root}"
    "${mod_tool_build_root}/RecompModTool" mod.toml mm_recomp_rando
)

rando_nrm="${rando_source_root}/mm_recomp_rando/mm_recomp_rando.nrm"
[[ -f "${rando_nrm}" ]]
python3 - "${rando_nrm}" "${rando_version}" <<'PY'
import json
import pathlib
import sys
import zipfile

nrm = pathlib.Path(sys.argv[1])
expected_version = sys.argv[2]
with zipfile.ZipFile(nrm) as archive:
    manifest = json.loads(archive.read("mod.json"))
    symbols = archive.read("mod_syms.bin")

if manifest.get("version") != expected_version:
    raise SystemExit(
        f"Unexpected Android Rando version: {manifest.get('version')!r}"
    )
if manifest.get("display_name") != "MMRecompRando (Android)":
    raise SystemExit(
        f"Unexpected Android Rando title: {manifest.get('display_name')!r}"
    )
for symbol in (b"recomp_on_setup_init", b"recomp_run_setup_init"):
    if symbol not in symbols:
        raise SystemExit(f"Android Rando compatibility symbol is missing: {symbol!r}")
if b"Setup_Init" in symbols:
    raise SystemExit("Desktop Setup_Init replacement leaked into the Android Rando")
PY

python3 "${repo_root}/tools/package_mmrecomprando_android.py" \
    --rando "${rando_nrm}" \
    --glue "${glue_library}" \
    --output-dir "${repo_root}/dist"
