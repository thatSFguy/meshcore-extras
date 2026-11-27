#!/usr/bin/env python3
"""Save T-Deck K.I.S.S. Messenger screenshots as PNGs.

Needs a capture build (screenshots are off in normal firmware):

    PLATFORMIO_BUILD_FLAGS="-D DECK_SCREENSHOT" \\
        pio run -e LilyGo_TDeck_kiss_messenger -t upload --upload-port <port>

Then run this with nothing else connected to the T-Deck's USB port, and tap
the screen with two fingers whenever it shows something worth keeping:

    tools/deck_screenshot.py <port> <out dir> [name prefix]

Each frame arrives as "DECKSHOT <w> <h>", then w*h RGB565 pixels
(little-endian), then "DECKSHOT END". Ctrl-C to stop.
"""
import os
import sys
import time

import serial
from PIL import Image


def read_until(port, marker):
    buf = b""
    while True:
        b = port.read(4096)
        if b:
            buf += b
            i = buf.find(marker)
            if i >= 0:
                return buf[i:]
            buf = buf[-len(marker):]


def main(port_name, out_dir, prefix="deck"):
    os.makedirs(out_dir, exist_ok=True)
    port = serial.Serial()
    port.port = port_name
    port.baudrate = 115200
    port.timeout = 0.5
    port.dtr = False
    port.rts = False
    port.open()
    n = len([f for f in os.listdir(out_dir) if f.startswith(prefix + "-") and f.endswith(".png")])
    print("Listening on %s. Tap the T-Deck's screen with two fingers to save a screenshot." % port_name)
    try:
        while True:
            buf = read_until(port, b"DECKSHOT ")
            while b"\n" not in buf:
                buf += port.read(64)
            header, buf = buf.split(b"\n", 1)
            parts = header.split()
            if len(parts) != 3 or parts[1] == b"END":
                continue
            w, h = int(parts[1]), int(parts[2])
            need = w * h * 2
            deadline = time.time() + 10
            while len(buf) < need and time.time() < deadline:
                buf += port.read(need - len(buf))
            if len(buf) < need:
                print("incomplete frame (%d of %d bytes), skipped" % (len(buf), need))
                continue
            px = buf[:need]
            img = Image.new("RGB", (w, h))
            data = []
            for i in range(0, need, 2):
                v = px[i] | (px[i + 1] << 8)
                r, g, b = (v >> 11) & 31, (v >> 5) & 63, v & 31
                data.append(((r * 255 + 15) // 31, (g * 255 + 31) // 63, (b * 255 + 15) // 31))
            img.putdata(data)
            n += 1
            path = os.path.join(out_dir, "%s-%02d.png" % (prefix, n))
            img.save(path)
            print("saved", path)
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
