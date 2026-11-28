#!/usr/bin/env python3
"""Draw the K.I.S.S. (CardKB) wiring diagram as an SVG.

    tools/gen_kiss_wiring.py guides/images/kiss-tlora-wiring.svg

Keep its GPIO numbers in step with the T-LoRa kiss_messenger env.
"""
import sys

W, H = 1000, 720
out = []
INK = "#222"
BLUE = "#1f5fa8"


def add(s):
    out.append(s)


def line(*pts, color=INK, w=2):
    p = " ".join("%g,%g" % pt for pt in pts)
    add('<polyline points="%s" fill="none" stroke="%s" stroke-width="%g" stroke-linejoin="round"/>' % (p, color, w))


def text(x, y, s, size=14, anchor="start", weight="normal", color=INK, italic=False):
    st = ' font-style="italic"' if italic else ""
    add('<text x="%g" y="%g" font-size="%g" text-anchor="%s" font-weight="%s" fill="%s"%s>%s</text>'
        % (x, y, size, anchor, weight, color, st, s))


def box(x1, y1, x2, y2, fill="#f4f6f8"):
    add('<rect x="%g" y="%g" width="%g" height="%g" rx="6" fill="%s" stroke="%s" stroke-width="2"/>'
        % (x1, y1, x2 - x1, y2 - y1, fill, INK))


def dot(x, y):
    add('<circle cx="%g" cy="%g" r="4" fill="%s"/>' % (x, y, INK))


def gnd(x, y):
    """Ground symbol hanging from (x, y)."""
    line((x, y), (x, y + 10))
    line((x - 12, y + 10), (x + 12, y + 10))
    line((x - 7, y + 15), (x + 7, y + 15))
    line((x - 2, y + 20), (x + 2, y + 20))


def vplus(x, y, label="3.3 V"):
    line((x - 10, y), (x + 10, y))
    text(x, y - 6, label, 12, "middle")


def resistor_h(x1, x2, y, label):
    """Zigzag resistor between x1 and x2 on row y, label above."""
    n = 6
    step = (x2 - x1 - 10) / n
    pts = [(x1, y), (x1 + 5, y)]
    for i in range(n):
        pts.append((x1 + 5 + step * (i + 0.5), y + (-7 if i % 2 == 0 else 7)))
    pts += [(x2 - 5, y), (x2, y)]
    line(*pts)
    text((x1 + x2) / 2, y - 12, label, 12, "middle")


def resistor_v(x, y1, y2, label):
    n = 6
    step = (y2 - y1 - 10) / n
    pts = [(x, y1), (x, y1 + 5)]
    for i in range(n):
        pts.append((x + (-7 if i % 2 == 0 else 7), y1 + 5 + step * (i + 0.5)))
    pts += [(x, y2 - 5), (x, y2)]
    line(*pts)
    text(x + 14, (y1 + y2) / 2 + 4, label, 12)


def npn(bx, y):
    """NPN transistor, base bar at bx, centred on row y. Returns (collector, emitter) points."""
    add('<circle cx="%g" cy="%g" r="24" fill="none" stroke="%s" stroke-width="1.5"/>' % (bx + 14, y, INK))
    line((bx, y - 14), (bx, y + 14), w=3)
    line((bx, y - 6), (bx + 26, y - 22))
    line((bx, y + 6), (bx + 26, y + 22))
    # emitter arrow
    add('<polygon points="%g,%g %g,%g %g,%g" fill="%s"/>'
        % (bx + 26, y + 22, bx + 15, y + 20, bx + 21, y + 11, INK))
    return (bx + 26, y - 22), (bx + 26, y + 22)


add('<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %d %d" '
    'font-family="Helvetica, Arial, sans-serif">' % (W, H, W, H))
add('<rect width="100%" height="100%" fill="#ffffff"/>')
text(W / 2, 34, "K.I.S.S. Messenger with a CardKB: wiring", 20, "middle", "bold")

# ---- ESP32 ----
box(380, 70, 600, 630, "#eef3fa")
text(490, 318, "ESP32", 22, "middle", "bold")
text(490, 338, "the T-LoRa V2.1-1.6,", 12, "middle")
text(490, 354, "or another ESP32 board", 12, "middle")

# ---- LoRa radio (left) ----
box(60, 80, 240, 320)
text(75, 160, "LoRa radio", 15, weight="bold")
text(75, 180, "SX1276 / RFM95W", 12)
text(75, 200, "3.3 V, GND", 12)
line((110, 80), (110, 52))
line((96, 40), (110, 52), (124, 40))
text(132, 50, "antenna (connect before use)", 11, italic=True)
radio = [("SCK", 5), ("MOSI", 27), ("MISO", 19), ("NSS", 18), ("RESET", 14), ("DIO0", 26), ("DIO1", 33)]
for i, (name, gpio) in enumerate(radio):
    y = 110 + 30 * i
    text(232, y + 4, name, 12, "end")
    line((240, y), (380, y), color=BLUE)
    text(390, y + 4, "GPIO %d" % gpio, 12)

# ---- button ----
y = 400
text(390, y + 4, "GPIO 0", 12)
line((380, y), (262, y), color=BLUE)
add('<circle cx="258" cy="%g" r="4" fill="none" stroke="%s" stroke-width="2"/>' % (y, INK))
add('<circle cx="206" cy="%g" r="4" fill="none" stroke="%s" stroke-width="2"/>' % (y, INK))
line((210, y), (252, y - 16))
line((202, y), (150, y))
gnd(150, y)
text(232, y - 26, "user button (BOOT)", 12, "middle")

# ---- battery divider ----
y = 470
text(390, y + 4, "GPIO 35", 12)
line((380, y), (200, y), color=BLUE)
dot(200, y)
text(62, y + 4, "Battery +", 12)
line((124, y), (136, y))
resistor_h(136, 190, y, "100 kΩ")
line((190, y), (200, y))
resistor_v(200, y, y + 50, "100 kΩ")
gnd(200, y + 50)
text(300, y - 10, "battery level", 12, "middle")

# ---- TX LED ----
y = 575
text(390, y + 4, "GPIO 25", 12)
line((380, y), (310, y), color=BLUE)
resistor_h(250, 310, y, "330 Ω")
line((250, y), (232, y))
add('<polygon points="232,%g 232,%g 206,%g" fill="none" stroke="%s" stroke-width="2"/>' % (y - 12, y + 12, y, INK))
line((204, y - 12), (204, y + 12))
line((204, y), (150, y))
gnd(150, y)
text(222, y - 20, "TX LED (optional)", 12, "middle")

# ---- I2C bus (right) ----
for name, gpio, y in (("SDA", 21, 110), ("SCL", 22, 140)):
    text(590, y + 4, "GPIO %d" % gpio, 12, "end")
    line((600, y), (970, y), color=BLUE)
    text(612, y - 6, name, 11)
box(650, 180, 790, 250)
text(720, 206, "OLED 128×64", 13, "middle", "bold")
text(720, 226, "SSD1306, address 0x3C", 11, "middle")
text(720, 242, "3.3 V, GND", 11, "middle")
box(830, 180, 970, 250)
text(900, 206, "M5Stack CardKB", 13, "middle", "bold")
text(900, 226, "address 0x5F", 11, "middle")
text(900, 242, "3.3 V, GND", 11, "middle")
for xs, xc in ((680, 710), (860, 890)):
    line((xs, 110), (xs, 180), color=BLUE)
    dot(xs, 110)
    line((xc, 140), (xc, 180), color=BLUE)
    dot(xc, 140)
text(810, 270, "4.7 kΩ pull-ups to 3.3 V on SDA/SCL (usually on the OLED)", 11, "middle", italic=True)

# ---- buzzer driver ----
y = 380
text(590, y + 4, "GPIO 15", 12, "end")
line((600, y), (640, y), color=BLUE)
resistor_h(640, 710, y, "1 kΩ")
line((710, y), (760, y))
c, e = npn(760, y)
line(c, (c[0], 336))
add('<rect x="%g" y="300" width="30" height="36" fill="#fff" stroke="%s" stroke-width="2"/>' % (c[0] - 15, INK))
line((c[0] - 9, 312), (c[0] + 9, 312))
line((c[0] - 9, 324), (c[0] + 9, 324))
line((c[0], 300), (c[0], 292))
vplus(c[0], 292)
line(e, (e[0], 418))
gnd(e[0], 418)
text(c[0] + 26, 316, "piezo buzzer", 12, weight="bold")
text(c[0] + 26, 332, "passive (plays tunes)", 11)
text(c[0] + 26, 384, "2N2222", 12)
text(c[0] + 26, 400, "high = on", 11)

# ---- vibration driver ----
y = 560
text(590, y + 4, "GPIO 13", 12, "end")
line((600, y), (640, y), color=BLUE)
resistor_h(640, 710, y, "1 kΩ")
line((710, y), (760, y))
c, e = npn(760, y)
top, bot = 474, 526
line(c, (c[0], bot))
line((c[0], bot), (c[0], 517))
add('<circle cx="%g" cy="500" r="17" fill="#fff" stroke="%s" stroke-width="2"/>' % (c[0], INK))
text(c[0], 505, "M", 13, "middle", "bold")
line((c[0], 483), (c[0], top), (c[0], 466))
vplus(c[0], 466)
# flyback diode across the motor, cathode to the supply
dx = c[0] + 48
line((c[0], top), (dx, top), (dx, 490))
dot(c[0], top)
add('<polygon points="%g,%g %g,%g %g,%g" fill="none" stroke="%s" stroke-width="2"/>'
    % (dx - 10, 510, dx + 10, 510, dx, 492, INK))
line((dx - 10, 490), (dx + 10, 490))
line((dx, 510), (dx, bot), (c[0], bot))
dot(c[0], bot)
text(dx + 16, 496, "1N4148", 11)
text(dx + 16, 510, "flyback diode", 11)
text(c[0] + 26, 564, "2N2222", 12)
text(c[0] + 26, 580, "high = on", 11)
text(c[0] + 26, 454, "vibration motor", 12, weight="bold")
line(e, (e[0], 598))
gnd(e[0], 598)

text(W / 2, 668, "GPIO numbers are the T-LoRa build's defaults. For another board, change them in a copy of its "
     "build environment.", 12, "middle")
text(W / 2, 688, "Every part shares 3.3 V and GND. Buzzer and motor can also run from the battery; join all grounds.",
     12, "middle")
add("</svg>")

open(sys.argv[1], "w").write("\n".join(out) + "\n")
