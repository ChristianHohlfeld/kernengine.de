# SpawnCity-Steuerung: Original (spawncity.de) und Kern – das kleine Spiel (die Drohne)

Vorlage: `/home/claude/nostack-dev/spawncity-de/sim` (Commit 9108537). Quellen als `datei.mjs:Zeile`
(viele Module sind einzeilig minifiziert: eine Zeile kann eine ganze Funktion sein). Das Original gilt.
Umsetzung auf Kern: `src/controls.{h,cpp}` (reine Rechnungen, getestet in `tests/test_controls.cpp`),
`src/spiel.cpp` (`SpawnCity::read_input`: Tastatur, Maus, Pad und Touch auf die Sticks der Drohne),
`src/touch_play.{h,cpp}` (Touch-Layout), `web/host.mjs` (Maus-Strom unter Pointer-Lock, Gamepad-Abfrage),
`web/ui.mjs` (Esc/Tab, Menü-Navigation mit dem Pad). Geprüft im Browser von `tests/input_check.mjs` und
`tests/smoke.mjs`.

Seit 26.11.2026 ist das Spiel nur die Drohne über der Stadt. Die Steuerung zu Fuß, im Auto, im Jet, am
Fallschirm, die Waffen, der Luftschlag, die große Karte, Ping, Sprache, Mehrspieler und GPS LIVE sind mit
ihren Bereichen herausgenommen (`BLUEPRINTS.md`, „Herausgenommen am 26.11.“); ihre Tabellen stehen im
Stand `d90426f3` dieser Datei und kommen mit den Bereichen zurück.

## 1. Geräte und Umschalten

| Ereignis | Wirkung | Quelle |
|---|---|---|
| Start | Desktop, wenn `(hover:hover) and (pointer:fine)` oder `maxTouchPoints == 0`, sonst Touch | `desktop_controls.mjs:245-247` |
| Touch/Stift `pointerdown` | Touch-Layout, Pointer-Lock frei, alle gehaltenen Maus/Tasten-Zustände gelöscht | `desktop_controls.mjs:41,44,81` |
| Maus `pointerdown` | Desktop, außer bei grobem Primärzeiger (Telefon/Tablet mit Maus bleibt Touch); `pad-input` aus | `desktop_controls.mjs:38,80,82` |
| Mausbewegung (nicht gesperrt, nicht über UI, feiner Zeiger) | Desktop | `desktop_controls.mjs:128` |
| echte Taste (keine Pad-Synthetik) | Desktop (auch auf Telefonen), `pad-input` aus | `desktop_controls.mjs:160` |
| Pad-Knopf (0..16) | `pad-input` (Cursor weg, Fokusrahmen, Pad-Beschriftungen, Hilfezeile aus); erstes Drücken schaltet XBOX CONTROLLER ein | `pad_actions.mjs:43,57,122` |
| Pad bei offenem Menü/Titel | Spiel bekommt nichts (`__arondightPadBlocked`) | `pad_actions.mjs:28-34` |
| Menü mit Pad geschlossen | Drohnen-Pad neutral bis A/B/X/Y/RB/LT/RT/VIEW/MENU < .18 (Release-Latch) | `xbox_gamepad.mjs:17-27,44,55`, `settings_controller_nav.mjs:10` |

Pointer-Lock (`desktop_controls.mjs:48-110,260`): nur Desktop, nur im Spiel, ein Klick auf die Spielfläche
fängt die Maus (der Fang-Klick wirkt sonst nicht). Ohne `unadjustedMovement` (Touchpads: OS-Beschleunigung
bleibt). Kamera nur aus `mousemove.movementX/Y` unter Lock, pro Ereignis auf ±420 px begrenzt
(`MAX_EVENT_PX`). Lock verloren (Esc des Browsers) → Dialog „Zurück zum Hauptmenü?“; Esc ohne Lock →
derselbe Dialog; Tab → Pause-Menü (Maus frei).

## 2. Tastatur und Maus (Desktop/PC, Laptop-Touchpad)

| Taste/Maus | Wirkung | Quelle |
|---|---|---|
| Esc | Dialog „Zurück zum Hauptmenü?“ (Enter/Y/J ja, Esc/N nein) | `desktop_controls.mjs:162,260`, `quit_confirm.mjs:20` |
| Tab | Pause-/Einstellungsmenü | `desktop_controls.mjs:163` |
| WASD | linker Stick (x = D−A, y = S−W) → `applyGameStick left` | `desktop_controls.mjs:183` |
| Pfeile | rechter Stick (Drehen / Neigen) | `desktop_controls.mjs:184` |
| Maus (gesperrt) | Rate: `rate += (clamp(px/dt/1500,±1) − rate)(1 − e^(−dt/0,05))` → rechter Stick | `desktop_controls.mjs:181-182` |
| Leertaste / Shift | steigen / sinken (Höhenachse); Steigen am Boden armt | `desktop_controls.mjs:185` |
| R | ARM; in der Luft kontrollierte Landung (ein zweites R bricht sie ab) | `desktop_controls.mjs:172`, `simulator.mjs` |
| C | Kamera: FPV → Verfolger → Dritte Person | `desktop_controls.mjs:173` |

## 3. Xbox-Controller (Standard-Mapping A0 B1 X2 Y3 LB4 RB5 LT6 RT7 VIEW8 MENU9 L3 10 R3 11 ↑12 ↓13 ←14 →15)

| Achse/Knopf | Wirkung | Quelle |
|---|---|---|
| LS / RS | `|v| ≤ .14 → 0`, linear; LS → `applyGameStick left`, RS → `right` | `xbox_gamepad.mjs:33-37`, `simulator.mjs:877` |
| RT − LT | Höhenachse (`|RT−LT| < .05 → 0`) | `xbox_gamepad.mjs:58` |
| A | ARM (nicht im ersten Bild) | `simulator.mjs:879` |
| B | Kern: STOP (Motoren aus) – im Original EMP, das es im kleinen Spiel nicht gibt | `pad_actions.mjs:7-18` |
| Y | Kern: nächste Kamera – im Original die nächste Waffe (Kamera dort: VIEW antippen) | `pad_actions.mjs:7-18` |
| Menüs: ↑↓ bzw. LS y (> .72, frei < .35) / ←→ bzw. LS x / A / LB RB / B, VIEW, MENU | Fokus ±1 mit Umlauf / Regler ±Schritt, Auswahl, Schalter / klicken / Abschnitt / schließen | `settings_controller_nav.mjs:13-46` |
| Titel: ↑↓ / A / MENU / B | Fokus / drücken (sonst START) / START / Welt-Optionen zu | `pad_actions.mjs:45-53` |

Kein Rumble am Pad.

## 4. Touch (Telefon/Tablet)

Sticks immer 168 px, bündig in den Ecken an den sicheren Rändern, Trefferfläche nur der Kreis
(`thumb_layout.mjs:8,23-29`). Auslenkung relativ zum Aufsetzpunkt, Radius .42·168
(`control_semantics.mjs:143-165`). Knöpfe nach `edge_hud_layout.mjs:32-52` (SAFE 10, GAP 8, Reihen über
den Sticks, untere Reihe links vom Blick-Stick). Kein Finger mehr auf dem Glas (die Liste des Browsers): das
Spiel lässt alle Finger los.

| Element | Wirkung | Quelle |
|---|---|---|
| linker Stick | `applyGameStick left` (Translation, FC-vorkompensiert) | `simulator.mjs:799-806`, `control_semantics.mjs:171-184` |
| rechter Stick | Gieren / Körperneigung (`phoneAxis`, Neigung standardmäßig invertiert) | `control_semantics.mjs:180-183` |
| Höhenschieber 48×132 | Federschieber: `axis = (Mitte − pos)/(.40·h)`, Rate `12·u(.25+.75u)` m/s, Totband .08 | `simulator.mjs:780-789`, `control_semantics.mjs:52-58` |
| Knöpfe | ARM, STOP (Motoren aus) | `edge_hud_layout.mjs:36-44` |
| Bildschirm | kein Zoom: zweiter Tipp in 340 ms wird verworfen, `touch-action:none` | `mobile_zoom_guard.mjs:1-17` |
| Start | Vollbild + Querformat-Sperre, nicht als installierte PWA | `pwa_fullscreen_policy.mjs`, `simulator.mjs:902-903` |

Auf der Seite (beide Ansichten): **TON** und **MUSIK** schalten `audio.on` / `music.on`, **RESET** bringt die
Drohne an den Start zurück, auf den Boden (`SpawnCity.command("reset")`).

## 5. Offen

- Das Touch-Layout der Drohne (`touch_play.cpp`) zeigt noch die Knöpfe ⇄ FPS, STRIKE und die Waffe der
  entfernten Bereiche; sie tun nichts. Eine Kamera-Taste hat es nicht (die Ansicht des Originals hat
  `#soloCamera`).
- B und Y weichen vom Original ab (siehe 3.).
