# K.I.S.S. Messenger

**Contents**

- [Overview](#overview)
- [Works with the MeshCore phone app](#works-with-the-meshcore-phone-app)
- [LilyGo T-Deck](#lilygo-t-deck)
- [LilyGo T-LoRa V2.1-1.6](#lilygo-t-lora-v21-16)

## Overview

A standalone MeshCore messenger, no phone needed, on two devices. Both run the
companion firmware with their own on-device interface and no Bluetooth, work
like the phone app's simple mode, and keep messages across a restart (Keep
messages, on by default: saved to flash at most every 15 s; turning it off
clears all messages). Repeated copies of the same DM or channel post are shown
once.

A DM is sent up to three times, the last as a flood with a fresh route, as the
app does. If its delivery confirmation arrives within 30 s after that, it
still shows as delivered. A DM that is never confirmed shows a hint: usually
the other side doesn't have your key (the DM request only half completed, or
they removed you), so send a flood advert or ask for a DM again.

Releases: [K.I.S.S. Messenger releases](https://github.com/thatSFguy/meshcore-extras/releases?q=kiss-messenger)
(`-merged.bin` for a fresh install at 0x0, `.bin` to update at 0x10000), or
the [install page](https://thatsfguy.github.io/meshcore-extras/program.html).

## Works with the MeshCore phone app

Messages and channels are standard MeshCore, so a K.I.S.S. chats with any
MeshCore companion and app. Beyond that, it follows the conventions of the
[MeshCore phone app](https://github.com/thatSFguy/meshcore-mobile-app) (Android), so
the two work together:

| Feature | How it works together |
|---|---|
| Private DMs | The app's request/accept exchange in the group chat (`DM request for @[name] mch:r:…` / `DM accepted …`): either side can ask, either side can accept. |
| Channel sharing (T-Deck) | `meshcore://channel/add?name=…&secret=…`, the app's share link (also the official app's and `docs/qr_codes.md`'s format), sent by DM in either direction. |
| Reactions | Received from the app (its `r:HHHH:II` format, shared with MeshCore Open; MeshCore One's format is read too) and matched to their message exactly as the app does. The T-Deck also sends them, so they land on the right message in the app. |
| Replies | The app's `> Sender: quote` line; shown as `[quote] reply`, and the T-Deck sends replies the app shows as replies. |
| Radio presets, channel rules | The T-Deck's region picker is the app's 47 regional presets; adding channels follows the app's rules (public, hashtag, private). |
| Look (T-Deck) | The app's dark theme, avatar colours and letters, delivery ticks and wording. |

## LilyGo T-Deck

The T-Deck's keyboard, touch screen and speaker, with a colour interface
modelled on the phone app: chat list with avatars, message bubbles with
delivery ticks (✓ sent, ✓✓ delivered, ✗ failed), and the app's dark theme and
wording.

<table>
  <tr>
    <td><img src="images/tdeck-chats.png" width="320" alt="Chat list"><br>Chat list</td>
    <td><img src="images/tdeck-dm-reply.png" width="320" alt="A DM with a reply, emoji and reactions"><br>Replies, emoji and reactions</td>
  </tr>
  <tr>
    <td><img src="images/tdeck-channel-reaction.png" width="320" alt="A private channel with a reaction"><br>A private channel</td>
    <td><img src="images/tdeck-react.png" width="320" alt="Reacting to a message"><br>React</td>
  </tr>
  <tr>
    <td><img src="images/tdeck-region.png" width="320" alt="Choosing a region"><br>Region presets</td>
    <td><img src="images/tdeck-channels.png" width="320" alt="Channels"><br>Channels</td>
  </tr>
</table>

- **Install:** [install page](https://thatsfguy.github.io/meshcore-extras/program.html)
  → *K.I.S.S. Messenger* → *LilyGo TDeck*, or build target
  `LilyGo_TDeck_kiss_messenger`.
- **First start** asks for a name, then for your region: the app's 47
  regional radio presets.
- **Input:** type on the keyboard, tap and scroll on the screen; the trackball
  moves and its click selects.
- **Chats:** tap a message for its options: react, reply, message the
  sender privately, accept a DM request, mention, resend a failed DM.
- **Emoji** are drawn in colour, in messages, names and reactions (the
  app's 184 reaction emoji; Noto Emoji images, Apache 2.0). Others show as
  `?`.
- **Reactions** show as chips under the message they're for (👍 2 🔥), and
  only reactions to your own messages count as new. **React** offers the
  app's six quick reactions.
- **Replies:** **Reply** shows *Replying to …* above the text box (tap X or
  press backspace on an empty box to cancel); a received reply shows its
  quote in brackets: `[why] why what?`.
- **Channels** (Settings → Channels): join Public or a hashtag channel, create
  a private one, or join one with its key. **Share** on a channel's page sends
  it to a contact by DM as a `meshcore://channel/add` link (the app's share
  format); a link received in a DM offers *Join <name>?*, and the key is never
  shown in the chat.
- **Settings** also cover adverts, sound, time zone, screen timeout, Keep
  messages and Clear all messages.

## LilyGo T-LoRa V2.1-1.6

A LilyGo T-LoRa V2.1-1.6 with an M5Stack CardKB
keyboard, a piezo buzzer and a vibration motor, in a 3D-printed case. Read and
type messages on its OLED, no phone needed. It runs the MeshCore companion
firmware with its own on-device interface (`ui-kiss`) and no Bluetooth; the
phone app can still connect over a USB-OTG cable.

**Build your own:** the 3D-printable case and build for this device (T-LoRa,
CardKB and piezo buzzer) are on Cults3D:
[K.I.S.S. QWERTY messenger for T-LoRa, CardKB and piezo buzzer](https://cults3d.com/en/3d-model/gadget/k-i-s-s-meshtastic-qwerty-messenger-for-t-lora-cardkb-and-piezo-buzzer).

**Wiring**

| Part | Connection |
|---|---|
| CardKB | I2C on GPIO 21 (SDA) / 22 (SCL), shared with the OLED; 3.3 V and GND |
| Piezo buzzer | GPIO 15, switched by a 2N2222 (GPIO high = on) |
| Vibration motor | GPIO 13, switched by a 2N2222 (GPIO high = on) |
| User button | the board's BOOT button (GPIO 0) |

**Install:** [install page](https://thatsfguy.github.io/meshcore-extras/program.html)
→ *K.I.S.S. Messenger*, or build target `LilyGo_TLora_V2_1_1_6_kiss_messenger`.
Don't flash the GPS companion build to this hardware: it uses GPIO 15/13 for
the GPS. **Set up:** on first boot it asks for a name. Then use the
[K.I.S.S. configuration page](https://thatsfguy.github.io/meshcore-extras/kiss.html)
for the radio (USA / Canada and EU / UK presets) and channels.

**Using it**

| Screen | Keys |
|---|---|
| Chat list | Up/Down to choose, Enter to open, or just start typing. Esc opens the menu. |
| Chat | Type, Enter to send. Up/Down scrolls. Tab asks someone in the channel for a DM. Esc goes back. |
| Menu | Flood / zero-hop advert, name, sound on/off, vibrate on/off, keep messages on/off, about |

The screen sleeps after a minute; the first key press only wakes it. The BOOT
button turns the screen on or off. Sent DMs show `..` while sending, `+` when
delivered and `!` if delivery failed.

**Reactions and replies** from the app read as one line each:
`Bob [why] reacted with thumbs up` (emoji named in words, as the OLED can't
draw them) and `Bob [why] why what?`.

**Simple mode.** Like the phone app's simple mode, it never adds contacts on
its own. To DM someone, ask in the group chat: Tab, pick the person, Enter.
That posts `DM request for @[name] mch:r:<your key>`; when they accept (on a
K.I.S.S. or in the app), their reply carries their key and both sides add each
other. No adverts are involved.

**Clock.** With no app or GPS, the clock is taken from the mesh, but only when
two different nodes agree on the time, so one node with a wrong clock can't
drag it along. The configuration page also sets it from your computer.
