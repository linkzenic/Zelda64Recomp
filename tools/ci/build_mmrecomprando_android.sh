#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
lock_file="${repo_root}/packaging/mmrecomprando-android/inputs.lock.json"
source_root="${ZELDA_RANDO_GLUE_DIR:-${repo_root}/MMRecompAPCppGlue}"
build_root="${ZELDA_RANDO_BUILD_DIR:-${repo_root}/build/mmrecomprando-android}"
download_root="${build_root}/download"
glue_build_root="${build_root}/glue"

read_lock() {
    python3 -c 'import json, sys; print(json.load(open(sys.argv[1]))[sys.argv[2]])' \
        "${lock_file}" "$1"
}

glue_repository="$(read_lock glue_repository)"
glue_commit="$(read_lock glue_commit)"
apcpp_commit="$(read_lock apcpp_commit)"
ixwebsocket_commit="$(read_lock ixwebsocket_commit)"
rando_url="$(read_lock rando_archive_url)"
rando_archive_sha256="$(read_lock rando_archive_sha256)"
rando_nrm_sha256="$(read_lock rando_nrm_sha256)"

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

mkdir -p "${download_root}"
rando_archive="${download_root}/MMRecompRando.zip"
curl --fail --location --retry 3 --output "${rando_archive}" "${rando_url}"
echo "${rando_archive_sha256}  ${rando_archive}" | shasum -a 256 --check
unzip -p "${rando_archive}" mm_recomp_rando.nrm > "${download_root}/mm_recomp_rando.nrm"
echo "${rando_nrm_sha256}  ${download_root}/mm_recomp_rando.nrm" | shasum -a 256 --check

python3 "${repo_root}/tools/package_mmrecomprando_android.py" \
    --rando "${download_root}/mm_recomp_rando.nrm" \
    --glue "${glue_library}" \
    --output-dir "${repo_root}/dist"
