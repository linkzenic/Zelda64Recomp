# MMRecompRando on Android

The Android release publishes two files:

- the signed Zelda64 Recompiled Android APK;
- `MMRecompRando-Android-arm64-v0.9.5.zip`.

Install the APK normally. Install the Rando ZIP from the app's **Install Mods**
button after installing Owls Never Quit, Better Song of Double Time, and MM
Recolors. Message Hooks is a transitive dependency of Better Song of Double
Time.

The Rando ZIP contains the platform-neutral randomizer NRM and the Android ARM64
APCpp glue library. Dependency mods are not redistributed in the ZIP.

Bare server addresses use certificate-validated `wss://`. For a trusted local
Archipelago server without TLS, enter the complete address with `ws://`, for
example `ws://192.168.1.188:38281`.

The `.apworld`, player YAML files, generated rooms, and Archipelago server remain
host-side files and are not part of either Android download.
