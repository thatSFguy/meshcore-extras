# Building and the fork's layout

**Contents**

- [Build from source](#build-from-source)
- [What this fork changes](#what-this-fork-changes)
- [The website](#the-website)

## Build from source

The easiest way is the
[install page](https://thatsfguy.github.io/meshcore-extras/program.html).
To build from source instead, same as upstream. From this directory:

```bash
pio run -e <your_target>        # build
pio run -e <your_target> -t upload   # flash
```

Every board with a stock repeater target also has a personal relay target:
`<board>_personal_relay` (e.g. `RAK_4631_personal_relay`, `Heltec_v3_personal_relay`).
Each one mirrors its board's normal repeater target with only the example
swapped to `examples/personal_relay` and the advert name changed to
`<board> Personal Relay`. Or run the **Build Personal Relay Firmwares**
workflow from the Actions tab and download the artifact (`.uf2` for the
RAK4631).

See the [upstream README](https://github.com/meshcore-dev/MeshCore#readme)
for hardware compatibility, the official web flasher, clients, and unit tests
(`pio test -e native` covers `src/`, which this fork does not modify).

## What this fork changes

Everything stock builds and behaves as upstream. Core protocol code in `src/`
and the stock examples are unmodified, apart from a portable replacement for
the non-standard `ltoa()` in `src/helpers/TxtDataHelpers.cpp` (needed for
STM32 builds). The additions:

- **Personal Relay:** `examples/personal_relay/`, a `<board>_personal_relay`
  build target for every board, `build.sh` and its workflow. See
  [Personal Relay](personal-relay.md#what-the-relay-changes-vs-upstream).
- **K.I.S.S. Messenger:** `examples/companion_radio/ui-kiss/` (T-LoRa) and
  `examples/companion_radio/ui-deck/` (T-Deck), their build targets in
  `variants/lilygo_tlora_v2_1/` and `variants/lilygo_tdeck/`, and small public
  helpers added to the companion's `MyMesh` (`floodAdvert()`,
  `persistContacts()`, `persistChannels()`, `lastChannelTimestamp()`).
- **GPS companion:** a T-LoRa V2.1-1.6 BLE companion with an external GPS
  module (`LilyGo_TLora_V2_1_1_6_companion_radio_ble_gps`).
- **Website:** `site/`, `tools/build_site_manifest.py` and the Pages workflow.

## The website

The project website does everything over USB, with no tools to install:

| Page | What it does |
|---|---|
| [**Install firmware**](https://thatsfguy.github.io/meshcore-extras/program.html) | Pick Personal Relay or K.I.S.S. Messenger and your board. ESP32 boards flash straight from the browser; nRF52 (RAK4631, T1000-E, …) and RP2040 boards get a `.uf2` to drag onto the board's USB drive; STM32 boards get a `.hex`. |
| [**Configure relay**](https://thatsfguy.github.io/meshcore-extras/relay.html) | Owners, group channels, echo suppression, home detection, options, name, admin password and radio (with presets), plus a command console. |
| [**Configure K.I.S.S.**](https://thatsfguy.github.io/meshcore-extras/kiss.html) | Name, radio (with presets), channels, including creating or joining a private channel. Also sets the device clock from your computer. |

The site lives in `site/` and is published by the *Build and deploy site to
GitHub Pages* workflow (run it manually from the Actions tab). It serves the
newest published *Personal Relay* release and a K.I.S.S. build of `main`.
