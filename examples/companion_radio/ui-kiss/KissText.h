#pragma once

// Message conventions the phone app puts in plain text, shared by the
// K.I.S.S. UIs (ui-kiss and ui-deck):
//
//  - Reactions. MeshCore has no reaction field, so clients send text:
//      r:HHHH:II                      MeshCore Open's, what the app sends:
//                                     II = hex index into a fixed emoji list
//      <emoji>@[<author>]\n<8 chars>  MeshCore One's, which the app also reads
//                                     (the @[author] part is on channels only)
//    The app's protocol/Reactions.kt and MeshCoreOneReactions.kt, parsed the
//    same way (as strictly: a false match would hide something typed). The
//    message reacted to is found as the app finds it (ReactionRouting): by
//    recomputing each candidate's hash from its sender timestamp, sender
//    name (channels) and text. 16 bits can collide; the newest match wins.
//  - Replies: the quoted lines first ("> Kaylee: are you heading out"), then a
//    newline and the reply (the app's protocol/Quoting.kt).
//
// Neither screen can draw emoji, so they are named in words.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <SHA256.h>

struct KissEmoji { const char* utf8; const char* name; };

// The app's Reactions.ALL, in wire-index order (quick, smileys, gestures,
// hearts, objects). Generated from Reactions.kt; only ever append.
static const KissEmoji KISS_EMOJI[] = {
  { "\xF0\x9F\x91\x8D", "thumbs up" },
  { "\xE2\x9D\xA4\xEF\xB8\x8F", "heart" },
  { "\xF0\x9F\x98\x82", "laughing" },
  { "\xF0\x9F\x8E\x89", "party" },
  { "\xF0\x9F\x91\x8F", "clapping" },
  { "\xF0\x9F\x94\xA5", "fire" },
  { "\xF0\x9F\x98\x80", "grinning" },
  { "\xF0\x9F\x98\x83", "smiling face with open mouth" },
  { "\xF0\x9F\x98\x84", "smiling face with open mouth and smiling eyes" },
  { "\xF0\x9F\x98\x81", "grinning face with smiling eyes" },
  { "\xF0\x9F\x98\x85", "smiling face with open mouth and cold sweat" },
  { "\xF0\x9F\x98\x82", "laughing" },
  { "\xF0\x9F\xA4\xA3", "rolling laughing" },
  { "\xF0\x9F\x98\x8A", "smiling face with smiling eyes" },
  { "\xF0\x9F\x98\x87", "smiling face with halo" },
  { "\xF0\x9F\x99\x82", "slightly smiling" },
  { "\xF0\x9F\x99\x83", "upside-down" },
  { "\xF0\x9F\x98\x89", "winking" },
  { "\xF0\x9F\x98\x8C", "relieved" },
  { "\xF0\x9F\x98\x8D", "heart eyes" },
  { "\xF0\x9F\xA5\xB0", "smiling with hearts" },
  { "\xF0\x9F\x98\x98", "face throwing a kiss" },
  { "\xF0\x9F\x98\x97", "kissing" },
  { "\xF0\x9F\x98\x99", "kissing face with smiling eyes" },
  { "\xF0\x9F\x98\x9A", "kissing face with closed eyes" },
  { "\xF0\x9F\x98\x8B", "face savouring delicious food" },
  { "\xF0\x9F\x98\x9B", "face with stuck-out tongue" },
  { "\xF0\x9F\x98\x9D", "face with stuck-out tongue and tightly-closed eyes" },
  { "\xF0\x9F\x98\x9C", "face with stuck-out tongue and winking eye" },
  { "\xF0\x9F\xA4\xAA", "grinning face with one large and one small eye" },
  { "\xF0\x9F\xA4\xA8", "face with one eyebrow raised" },
  { "\xF0\x9F\xA7\x90", "face with monocle" },
  { "\xF0\x9F\xA4\x93", "nerd" },
  { "\xF0\x9F\x98\x8E", "sunglasses" },
  { "\xF0\x9F\xA5\xB8", "disguised" },
  { "\xF0\x9F\xA4\xA9", "grinning face with star eyes" },
  { "\xF0\x9F\xA5\xB3", "face with party horn and party hat" },
  { "\xF0\x9F\x98\x8F", "smirking" },
  { "\xF0\x9F\x98\x92", "unamused" },
  { "\xF0\x9F\x98\x9E", "disappointed" },
  { "\xF0\x9F\x98\x94", "pensive" },
  { "\xF0\x9F\x98\x9F", "worried" },
  { "\xF0\x9F\x98\x95", "confused" },
  { "\xF0\x9F\x99\x81", "slightly frowning" },
  { "\xF0\x9F\x98\xA3", "persevering" },
  { "\xF0\x9F\x98\x96", "confounded" },
  { "\xF0\x9F\x98\xAB", "tired" },
  { "\xF0\x9F\x98\xA9", "weary" },
  { "\xF0\x9F\xA5\xBA", "face with pleading eyes" },
  { "\xF0\x9F\x98\xA2", "crying" },
  { "\xF0\x9F\x98\xAD", "sobbing" },
  { "\xF0\x9F\x98\xA4", "face with look of triumph" },
  { "\xF0\x9F\x98\xA0", "angry" },
  { "\xF0\x9F\x98\xA1", "pouting" },
  { "\xF0\x9F\xA4\xAC", "serious face with symbols covering mouth" },
  { "\xF0\x9F\xA4\xAF", "shocked face with exploding head" },
  { "\xF0\x9F\x98\xB3", "flushed" },
  { "\xF0\x9F\xA5\xB5", "overheated" },
  { "\xF0\x9F\xA5\xB6", "freezing" },
  { "\xF0\x9F\x98\xB1", "screaming" },
  { "\xF0\x9F\x98\xA8", "fearful" },
  { "\xF0\x9F\x98\xB0", "face with open mouth and cold sweat" },
  { "\xF0\x9F\x98\xA5", "disappointed but relieved" },
  { "\xF0\x9F\x98\x93", "face with cold sweat" },
  { "\xF0\x9F\xA4\x97", "hugging" },
  { "\xF0\x9F\xA4\x94", "thinking" },
  { "\xF0\x9F\xA4\xAD", "smiling face with smiling eyes and hand covering mouth" },
  { "\xF0\x9F\xA4\xAB", "face with finger covering closed lips" },
  { "\xF0\x9F\xA4\xA5", "lying" },
  { "\xF0\x9F\x98\xB6", "face without mouth" },
  { "\xF0\x9F\x91\x8D", "thumbs up" },
  { "\xF0\x9F\x91\x8E", "thumbs down" },
  { "\xF0\x9F\x91\x8A", "fisted hand" },
  { "\xE2\x9C\x8A", "raised fist" },
  { "\xF0\x9F\xA4\x9B", "left-facing fist" },
  { "\xF0\x9F\xA4\x9C", "right-facing fist" },
  { "\xF0\x9F\xA4\x9E", "hand with index and middle fingers crossed" },
  { "\xE2\x9C\x8C\xEF\xB8\x8F", "peace" },
  { "\xF0\x9F\xA4\x9F", "i love you hand" },
  { "\xF0\x9F\xA4\x98", "sign of the horns" },
  { "\xF0\x9F\x91\x8C", "ok" },
  { "\xF0\x9F\xA4\x8C", "pinched fingers" },
  { "\xF0\x9F\xA4\x8F", "pinching hand" },
  { "\xF0\x9F\x91\x88", "white left pointing backhand index" },
  { "\xF0\x9F\x91\x89", "white right pointing backhand index" },
  { "\xF0\x9F\x91\x86", "white up pointing backhand index" },
  { "\xF0\x9F\x91\x87", "white down pointing backhand index" },
  { "\xE2\x98\x9D\xEF\xB8\x8F", "white up pointing index" },
  { "\xF0\x9F\x91\x8B", "wave" },
  { "\xF0\x9F\xA4\x9A", "raised back of hand" },
  { "\xF0\x9F\x96\x90\xEF\xB8\x8F", "raised hand with fingers splayed" },
  { "\xE2\x9C\x8B", "raised hand" },
  { "\xF0\x9F\x96\x96", "raised hand with part between middle and ring fingers" },
  { "\xF0\x9F\x91\x8F", "clapping" },
  { "\xF0\x9F\x99\x8C", "person raising both hands in celebration" },
  { "\xF0\x9F\x91\x90", "open hands" },
  { "\xF0\x9F\xA4\xB2", "palms up together" },
  { "\xF0\x9F\xA4\x9D", "handshake" },
  { "\xF0\x9F\x99\x8F", "thanks" },
  { "\xE2\x9C\x8D\xEF\xB8\x8F", "writing hand" },
  { "\xF0\x9F\x92\x85", "nail polish" },
  { "\xF0\x9F\xA4\xB3", "selfie" },
  { "\xF0\x9F\x92\xAA", "strong" },
  { "\xE2\x9D\xA4\xEF\xB8\x8F", "heart" },
  { "\xF0\x9F\xA7\xA1", "orange heart" },
  { "\xF0\x9F\x92\x9B", "yellow heart" },
  { "\xF0\x9F\x92\x9A", "green heart" },
  { "\xF0\x9F\x92\x99", "blue heart" },
  { "\xF0\x9F\x92\x9C", "purple heart" },
  { "\xF0\x9F\x96\xA4", "black heart" },
  { "\xF0\x9F\xA4\x8D", "white heart" },
  { "\xF0\x9F\xA4\x8E", "brown heart" },
  { "\xF0\x9F\x92\x94", "broken heart" },
  { "\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x94\xA5", "heart on fire" },
  { "\xE2\x9D\xA4\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\xA9\xB9", "mending heart" },
  { "\xF0\x9F\x92\x95", "two hearts" },
  { "\xF0\x9F\x92\x9E", "revolving hearts" },
  { "\xF0\x9F\x92\x93", "beating heart" },
  { "\xF0\x9F\x92\x97", "growing heart" },
  { "\xF0\x9F\x92\x96", "sparkling heart" },
  { "\xF0\x9F\x92\x98", "heart with arrow" },
  { "\xF0\x9F\x92\x9D", "heart with ribbon" },
  { "\xF0\x9F\x92\x9F", "heart decoration" },
  { "\xF0\x9F\x92\x8C", "love letter" },
  { "\xF0\x9F\x92\xA2", "anger" },
  { "\xF0\x9F\x92\xA5", "collision" },
  { "\xF0\x9F\x92\xAB", "dizzy" },
  { "\xF0\x9F\x92\xA6", "splashing sweat" },
  { "\xF0\x9F\x92\xA8", "dash" },
  { "\xF0\x9F\x95\xB3\xEF\xB8\x8F", "hole" },
  { "\xF0\x9F\x92\xAC", "speech balloon" },
  { "\xF0\x9F\x91\x81\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x97\xA8\xEF\xB8\x8F", "eye in speech bubble" },
  { "\xF0\x9F\x97\xA8\xEF\xB8\x8F", "left speech bubble" },
  { "\xF0\x9F\x97\xAF\xEF\xB8\x8F", "right anger bubble" },
  { "\xF0\x9F\x92\xAD", "thought balloon" },
  { "\xF0\x9F\x8E\x89", "party" },
  { "\xF0\x9F\x8E\x8A", "confetti" },
  { "\xF0\x9F\x8E\x88", "balloon" },
  { "\xF0\x9F\x8E\x81", "wrapped present" },
  { "\xF0\x9F\x8E\x80", "ribbon" },
  { "\xF0\x9F\xAA\x85", "pinata" },
  { "\xF0\x9F\xAA\x86", "nesting dolls" },
  { "\xF0\x9F\x8F\x86", "trophy" },
  { "\xF0\x9F\xA5\x87", "first place medal" },
  { "\xF0\x9F\xA5\x88", "second place medal" },
  { "\xF0\x9F\xA5\x89", "third place medal" },
  { "\xE2\x9A\xBD", "soccer ball" },
  { "\xE2\x9A\xBE", "baseball" },
  { "\xF0\x9F\xA5\x8E", "softball" },
  { "\xF0\x9F\x8F\x80", "basketball and hoop" },
  { "\xF0\x9F\x8F\x90", "volleyball" },
  { "\xF0\x9F\x8F\x88", "american football" },
  { "\xF0\x9F\x8F\x89", "rugby football" },
  { "\xF0\x9F\x8E\xBE", "tennis racquet and ball" },
  { "\xF0\x9F\xA5\x8F", "flying disc" },
  { "\xF0\x9F\x8E\xB3", "bowling" },
  { "\xF0\x9F\x8F\x8F", "cricket bat and ball" },
  { "\xF0\x9F\x8F\x91", "field hockey stick and ball" },
  { "\xF0\x9F\x8F\x92", "ice hockey stick and puck" },
  { "\xF0\x9F\xA5\x8D", "lacrosse stick and ball" },
  { "\xF0\x9F\x8F\x93", "table tennis paddle and ball" },
  { "\xF0\x9F\x8F\xB8", "badminton racquet and shuttlecock" },
  { "\xF0\x9F\xA5\x8A", "boxing glove" },
  { "\xF0\x9F\xA5\x8B", "martial arts uniform" },
  { "\xF0\x9F\xA5\x85", "goal net" },
  { "\xE2\x9B\xB3", "flag in hole" },
  { "\xF0\x9F\x94\xA5", "fire" },
  { "\xE2\xAD\x90", "star" },
  { "\xF0\x9F\x8C\x9F", "glowing star" },
  { "\xE2\x9C\xA8", "sparkles" },
  { "\xE2\x9A\xA1", "high voltage" },
  { "\xF0\x9F\x92\xA1", "electric light bulb" },
  { "\xF0\x9F\x94\xA6", "electric torch" },
  { "\xF0\x9F\x8F\xAE", "izakaya lantern" },
  { "\xF0\x9F\xAA\x94", "diya lamp" },
  { "\xF0\x9F\x93\xB1", "mobile phone" },
  { "\xF0\x9F\x92\xBB", "personal computer" },
  { "\xE2\x8C\x9A", "watch" },
  { "\xF0\x9F\x93\xB7", "camera" },
  { "\xF0\x9F\x93\xBA", "television" },
  { "\xF0\x9F\x93\xBB", "radio" },
  { "\xF0\x9F\x8E\xB5", "musical note" },
  { "\xF0\x9F\x8E\xB6", "multiple musical notes" },
  { "\xF0\x9F\x9A\x80", "rocket" },
};
#define KISS_EMOJI_COUNT  (int)(sizeof(KISS_EMOJI) / sizeof(KISS_EMOJI[0]))

// copy without U+FE0F (emoji presentation selector), so "❤" matches "❤️"
static int kissStripVs(const char* src, int len, char* out, int out_size) {
  int n = 0;
  for (int i = 0; i < len && n + 1 < out_size; i++) {
    if (i + 2 < len && (uint8_t)src[i] == 0xEF && (uint8_t)src[i + 1] == 0xB8 && (uint8_t)src[i + 2] == 0x8F) { i += 2; continue; }
    out[n++] = src[i];
  }
  out[n] = 0;
  return n;
}

// an emoji given as UTF-8: its index in KISS_EMOJI, or -1
static int kissEmojiIndex(const char* e, int len) {
  char a[48], b[48];
  if (len <= 0 || len >= (int)sizeof(a)) return -1;
  kissStripVs(e, len, a, sizeof(a));
  for (int i = 0; i < KISS_EMOJI_COUNT; i++) {
    kissStripVs(KISS_EMOJI[i].utf8, strlen(KISS_EMOJI[i].utf8), b, sizeof(b));
    if (strcmp(a, b) == 0) return i;
  }
  return -1;
}

// the words for an emoji given as UTF-8, or NULL if it isn't in the list
static const char* kissEmojiName(const char* e, int len) {
  int i = kissEmojiIndex(e, len);
  return i >= 0 ? KISS_EMOJI[i].name : NULL;
}

static int kissHexVal(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return -1;   // lower case only, as the app's regex
}

struct KissReaction {
  bool one;                // MeshCore One's format; else r:HHHH:II
  char emoji[40];          // the emoji's words, or "an emoji"
  int16_t index;           // the emoji's index in KISS_EMOJI, or -1
  uint16_t hash16;         // r:HHHH:II's target
  char hash8[9];           // MeshCore One's target (normalised Crockford)
  char author[32];         // MeshCore One, channels: who wrote the target
};

// Parses text (a message body, without any "sender: " prefix) as a reaction.
static bool kissParseReaction(const char* text, KissReaction* r) {
  memset(r, 0, sizeof(*r));
  while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n') text++;
  int len = strlen(text);
  while (len > 0 && (text[len - 1] == ' ' || text[len - 1] == '\t' || text[len - 1] == '\r' || text[len - 1] == '\n')) len--;

  // r:HHHH:II
  if (len == 9 && text[0] == 'r' && text[1] == ':' && text[6] == ':') {
    uint16_t h = 0;
    for (int i = 2; i < 6; i++) {
      int v = kissHexVal(text[i]);
      if (v < 0) return false;
      h = (h << 4) | v;
    }
    int hi = kissHexVal(text[7]), lo = kissHexVal(text[8]);
    if (hi < 0 || lo < 0 || hi * 16 + lo >= KISS_EMOJI_COUNT) return false;
    r->hash16 = h;
    r->index = hi * 16 + lo;
    snprintf(r->emoji, sizeof(r->emoji), "%s", KISS_EMOJI[hi * 16 + lo].name);
    return true;
  }

  // <emoji>[@[author]]\n<8 Crockford characters>
  const char* nl = (const char*)memchr(text, '\n', len);
  if (nl == NULL || nl == text) return false;
  const char* h = nl + 1;
  const char* h_end = text + len;
  while (h < h_end && (*h == ' ' || *h == '\t' || *h == '\r')) h++;
  if (h_end - h != 8) return false;
  for (int i = 0; i < 8; i++) {
    char c = tolower((uint8_t)h[i]);
    if (c == 'o') c = '0';
    if (c == 'i' || c == 'l') c = '1';
    if (c == 0 || !strchr("0123456789abcdefghjkmnpqrstvwxyz", c)) return false;
    r->hash8[i] = c;
  }
  const char* e = text;
  const char* e_end = nl;
  while (e_end > e && (e_end[-1] == ' ' || e_end[-1] == '\r' || e_end[-1] == '\t')) e_end--;
  const char* marker = NULL;
  for (const char* p = e; p + 1 < e_end; p++) if (p[0] == '@' && p[1] == '[') { marker = p; break; }
  if (marker) {
    if (e_end[-1] != ']') return false;
    int alen = (e_end - 1) - (marker + 2);
    if (alen <= 0 || alen >= (int)sizeof(r->author)) return false;
    memcpy(r->author, marker + 2, alen);
    r->author[alen] = 0;
    bool blank = true;
    for (char* p = r->author; *p; p++) if (*p != ' ') blank = false;
    if (blank) return false;
    e_end = marker;
    while (e_end > e && e_end[-1] == ' ') e_end--;
  }
  int elen = e_end - e;
  if (elen <= 0 || elen > 40) return false;   // the app allows 16 UTF-16 units
  for (const char* p = e; p < e_end; p++) {
    uint8_t c = *p;
    if (c == ' ' || c == '\t' || isalnum(c)) return false;   // typed words, not an emoji
  }
  r->index = kissEmojiIndex(e, elen);
  snprintf(r->emoji, sizeof(r->emoji), "%s", r->index >= 0 ? KISS_EMOJI[r->index].name : "an emoji");
  r->one = true;
  return true;
}

// "reacted with thumbs up" (and, when only the author is known, " to your
// message" / " to Bob's message") for a reaction whose target wasn't found.
static void kissReactionText(const KissReaction& r, const char* my_name, char* out, int out_size) {
  if (r.author[0] == 0) snprintf(out, out_size, "reacted with %s", r.emoji);
  else if (my_name && strcasecmp(r.author, my_name) == 0) snprintf(out, out_size, "reacted with %s to your message", r.emoji);
  else snprintf(out, out_size, "reacted with %s to %s's message", r.emoji, r.author);
}

static bool kissDescribeReaction(const char* text, const char* my_name, char* out, int out_size) {
  KissReaction r;
  if (!kissParseReaction(text, &r)) return false;
  kissReactionText(r, my_name, out, out_size);
  return true;
}

// Dart's String.hashCode (the app's DartStringHash): Jenkins one-at-a-time
// over UTF-16 code units. Fed in pieces: call kissDartAdd for each, then
// kissDartEnd. max_units limits how much of a piece is taken.
static void kissDartAdd(uint32_t& h, const char* utf8, int max_units) {
  const uint8_t* p = (const uint8_t*)utf8;
  int units = 0;
  while (*p && units < max_units) {
    uint32_t cp;
    int n;
    if (*p < 0x80) { cp = *p; n = 1; }
    else if ((*p & 0xE0) == 0xC0) { cp = *p & 0x1F; n = 2; }
    else if ((*p & 0xF0) == 0xE0) { cp = *p & 0x0F; n = 3; }
    else { cp = *p & 0x07; n = 4; }
    for (int i = 1; i < n; i++) {
      if ((p[i] & 0xC0) != 0x80) { n = i; break; }
      cp = (cp << 6) | (p[i] & 0x3F);
    }
    p += n;
    uint16_t u[2];
    int un = 1;
    if (cp >= 0x10000) { cp -= 0x10000; u[0] = 0xD800 | (cp >> 10); u[1] = 0xDC00 | (cp & 0x3FF); un = 2; }
    else u[0] = cp;
    for (int i = 0; i < un && units < max_units; i++, units++) {
      h += u[i];
      h += h << 10;
      h ^= h >> 6;
    }
  }
}

static uint16_t kissDartEnd(uint32_t h) {
  h += h << 3;
  h ^= h >> 11;
  h += h << 15;
  if (h == 0) h = 1;
  return h & 0xFFFF;
}

// Whether reaction r points at a message with this sender timestamp, sender
// name (channels; NULL for DMs) and text (the body, without "sender: ").
static bool kissReactionMatches(const KissReaction& r, uint32_t sent_ts, const char* sender, const char* text) {
  if (sent_ts == 0) return false;
  if (!r.one) {
    // the app's Reactions.targetHash: "$timestamp$sender$first5" (no sender on DMs)
    char ts[12];
    snprintf(ts, sizeof(ts), "%lu", (unsigned long)sent_ts);
    uint32_t h = 0;
    kissDartAdd(h, ts, 1000);
    if (sender) kissDartAdd(h, sender, 1000);
    kissDartAdd(h, text, 5);
    return kissDartEnd(h) == r.hash16;
  }
  // MeshCore One: on channels the author must match; then SHA-256(text +
  // little-endian timestamp), first 5 bytes, Crockford Base32
  if (sender && r.author[0] && strcasecmp(sender, r.author) != 0) return false;
  uint8_t le[4] = { (uint8_t)sent_ts, (uint8_t)(sent_ts >> 8), (uint8_t)(sent_ts >> 16), (uint8_t)(sent_ts >> 24) };
  uint8_t d[32];
  SHA256 sha;
  sha.update(text, strlen(text));
  sha.update(le, 4);
  sha.finalize(d, sizeof(d));
  static const char ALPHA[] = "0123456789abcdefghjkmnpqrstvwxyz";
  char out[9];
  uint64_t v = 0;
  for (int i = 0; i < 5; i++) v = (v << 8) | d[i];
  for (int i = 0; i < 8; i++) out[i] = ALPHA[(v >> (35 - 5 * i)) & 0x1F];
  out[8] = 0;
  return strcmp(out, r.hash8) == 0;
}

// A reply: if text starts with quoted lines ("> ..."), writes them (without
// the ">", joined by spaces) to quote and returns where the reply itself
// starts. Otherwise quote is "" and text is returned as is. Only the
// leading run of quoted lines counts: "5 > 3" further down is not a quote.
static const char* kissSplitQuote(const char* text, char* quote, int quote_size) {
  quote[0] = 0;
  if (text[0] != '>') return text;
  int n = 0;
  const char* p = text;
  while (*p == '>') {
    p++;
    while (*p == ' ') p++;
    const char* eol = strchr(p, '\n');
    int len = eol ? eol - p : (int)strlen(p);
    while (len > 0 && (p[len - 1] == ' ' || p[len - 1] == '\r')) len--;
    if (n > 0 && n + 1 < quote_size) quote[n++] = ' ';
    for (int i = 0; i < len && n + 1 < quote_size; i++) quote[n++] = p[i];
    quote[n] = 0;
    if (!eol) return p + strlen(p);   // only a quote
    p = eol + 1;
  }
  while (*p == '\n' || *p == '\r') p++;
  return p;
}

// A short "[...]" reference to a message: its words (a reply's own words,
// not its quote), on one line, cut to max_chars on a UTF-8 boundary.
static void kissSnippet(const char* text, char* out, int out_size, int max_chars) {
  char q[64];
  const char* body = kissSplitQuote(text, q, sizeof(q));
  if (*body == 0) body = q;
  while (*body == ' ') body++;
  int n = 0, chars = 0;
  bool cut = false;
  for (const char* p = body; *p; ) {
    int len = 1;
    uint8_t c = *p;
    if (c >= 0xF0) len = 4; else if (c >= 0xE0) len = 3; else if (c >= 0xC0) len = 2;
    if (chars >= max_chars) { cut = true; break; }
    if (n + len + 3 >= out_size) { cut = true; break; }
    if (c == '\n' || c == '\r' || c == '\t') { if (n > 0 && out[n - 1] != ' ') out[n++] = ' '; p++; chars++; continue; }
    memcpy(out + n, p, len);
    n += len;
    p += len;
    chars++;
  }
  while (n > 0 && out[n - 1] == ' ') n--;
  if (cut) { out[n++] = '.'; out[n++] = '.'; }
  out[n] = 0;
}

// A quote as the app writes it is "Sender: what they said"; the reference
// shows only what was said.
static const char* kissQuoteWords(const char* quote) {
  const char* sep = strstr(quote, ": ");
  if (sep && sep - quote > 0 && sep - quote < 32) return sep + 2;
  return quote;
}
