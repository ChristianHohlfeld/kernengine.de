# SpawnCity-Steuerung: Original (spawncity.de) und Kern

Vorlage: `/home/claude/nostack-dev/spawncity-de/sim` (Commit 9108537). Quellen als `datei.mjs:Zeile`
(viele Module sind einzeilig minifiziert: eine Zeile kann eine ganze Funktion sein). Das Original gilt.
Umsetzung auf Kern: `src/controls.{h,cpp}` (reine Rechnungen, getestet in `tests/test_controls.cpp`),
`src/game_controls.cpp` (Eingabe je Modus, Geräte-Arbitrierung, Touch-Steuerung aller Modi),
`web/host.mjs` (Maus-Strom unter Pointer-Lock, Gamepad-Abfrage, Hilfezeile), `web/ui.mjs` (Esc/Tab,
Menü-Navigation mit dem Pad), `web/mp_host.mjs` (große Karte mit dem Pad).

## 1. Geräte und Umschalten

| Ereignis | Wirkung | Quelle |
|---|---|---|
| Start | Desktop, wenn `(hover:hover) and (pointer:fine)` oder `maxTouchPoints == 0`, sonst Touch | `desktop_controls.mjs:245-247` |
| Touch/Stift `pointerdown` | Touch-Layout, Pointer-Lock frei, alle gehaltenen Maus/Tasten-Zustände gelöscht | `desktop_controls.mjs:41,44,81` |
| Maus `pointerdown` | Desktop, außer bei grobem Primärzeiger (Telefon/Tablet mit Maus bleibt Touch); `pad-input` aus | `desktop_controls.mjs:38,80,82` |
| Mausbewegung (nicht gesperrt, nicht über UI, feiner Zeiger) | Desktop | `desktop_controls.mjs:128` |
| echte Taste (keine Pad-Synthetik) | Desktop (auch auf Telefonen), `pad-input` aus | `desktop_controls.mjs:160` |
| Pad-Knopf (0..16) | `pad-input` (Cursor weg, Fokusrahmen, Pad-Beschriftungen, Hilfezeile aus); erstes Drücken schaltet XBOX CONTROLLER ein | `pad_actions.mjs:43,57,122` |
| Pad-Rechter-Stick > .22 auf Fuß | beendet einen hängenden Touch-Blick | `gameplay_final_input_extras.mjs:9` |
| Pad bei offenem Menü/Titel | Spiel bekommt nichts (`__arondightPadBlocked`) | `pad_actions.mjs:28-34` |
| Menü mit Pad geschlossen | Drohnen-Pad neutral bis A/B/X/Y/RB/LT/RT/VIEW/MENU < .18 (Release-Latch) | `xbox_gamepad.mjs:17-27,44,55`, `settings_controller_nav.mjs:10` |

Pointer-Lock (`desktop_controls.mjs:48-110,260`): nur Desktop, nur im Spiel, nur LMB/RMB auf die
Spielfläche; der Fang-Klick mit LMB schießt nicht. Ohne `unadjustedMovement` (Touchpads: OS-Beschleunigung
bleibt). Kamera nur aus `mousemove.movementX/Y` unter Lock, pro Ereignis auf ±420 px begrenzt
(`MAX_EVENT_PX`), dann `× Empfindlichkeit %` und Y-Umkehr (`mouse_settings.mjs:9`). Lock verloren
(Esc des Browsers) → Dialog „Zurück zum Hauptmenü?“; Esc ohne Lock → derselbe Dialog; Tab → Pause-Menü
(Maus frei); Tod behält den Lock. Laptop-Touchpad: derselbe Lock-Pfad (OS-Beschleunigung), Zwei-Finger-
Scroll kommt als `wheel` (Waffe/Zoom wie das Mausrad), Tippen ist ein Klick (LMB).

## 2. Tastatur und Maus (Desktop/PC, Laptop-Touchpad)

| Modus | Taste/Maus | Wirkung | Quelle |
|---|---|---|---|
| überall | Esc | Dialog „Zurück zum Hauptmenü?“ (Enter/Y/J ja, Esc/N nein); bei offener Karte: Karte zu; beim Zielen: Luftschlag abbrechen | `desktop_controls.mjs:162,260`, `quit_confirm.mjs:20`, `world_map_full.mjs`, `fighter_jets.mjs:197` |
| überall | Tab | Pause-/Einstellungsmenü | `desktop_controls.mjs:163` |
| überall | M | große Karte (`/` fokussiert die Suche) | `world_map_full.mjs` |
| überall | P | Mehrspieler an/aus (MULTI) | `pad_actions.mjs:106` |
| Fuß/Drohne | G | EMP | `pad_actions.mjs:108` |
| überall | 4 / J | Luftschlag: Zielen an; im Zielen J/4/Enter/LMB bestätigt, RMB/Esc bricht ab | `pad_actions.mjs:109`, `fighter_jets.mjs:192,197` |
| überall | Z / MMB | Ping | `ping_marker.mjs` |
| überall | H / K | Mikrofon an/aus / stumm | `voice_control_bindings.mjs:3-4` |
| überall | L | GPS LIVE | `gps_live_mode.mjs` |
| Fuß | F | Jet zuerst, sonst wie E; nicht bei Zombie-Reparatur | `pad_actions.mjs:93-103` |
| tot | R / Enter / Leertaste | Respawn | `desktop_controls.mjs:165` |
| Fuß | WASD | laufen 5,5 m/s (Diagonale normiert) | `player_walk_mode_v4.mjs:300-304` |
| Fuß | Shift | sprinten 8,1 m/s | `player_walk_mode_v4.mjs:16,300` |
| Fuß | Leertaste | springen 3,6 m/s; in der Luft nochmal = Jetpack, halten = Schub | `player_walk_mode_v4.mjs:22,30-36,69` |
| Fuß | Maus | 0,0026 rad/px × Blick-% / ADS-Zoom, roh, ohne Assist | `player_walk_mode_v4.mjs:238` |
| Fuß | LMB | Feuer (MP automatisch, sonst je Klick); Fäuste: linke Hand | `player_walk_mode_v4.mjs:300`, `desktop_controls.mjs:191` |
| Fuß | RMB | Zielen/ADS (×1,45, Sniper ×8); Fäuste: rechte Hand; Messer/Fäuste ohne Visier | `first_person_controller_v5.mjs:138-140` |
| Fuß | 1 2 3 5 6 7 8 | MP, Glock, Sniper, Granatwerfer, Raketenwerfer, Fäuste, Messer | `desktop_controls.mjs:168-169` |
| Fuß | Q / Mausrad | nächste Waffe (Rad runter +1, hoch −1; Reihenfolge MP, Glock, Sniper, Granatwerfer, Raketenwerfer, Fäuste, Messer) | `gameplay_final_runtime_v2.mjs:605`, `desktop_controls.mjs:149-153` |
| Fuß | T halten | Handgranate kochen (Zünder 4 s, 3 Stück, 1 je 20 s zurück), loslassen = werfen | `hand_grenade.mjs:9-11` |
| Fuß | E / F | einsteigen (Auto/Jet) | `player_car_mode.mjs:158`, `jet_mode.mjs:587` |
| Fuß | V | Drohne | `player_walk_mode_v4.mjs` |
| Drohne | WASD | linker Stick (x = D−A, y = S−W) → `applyGameStick left` | `desktop_controls.mjs:183` |
| Drohne | Pfeile | rechter Stick (Drehen / Neigen) | `desktop_controls.mjs:184` |
| Drohne | Maus | Rate: `rate += (clamp(px/dt/1500,±1) − rate)(1 − e^(−dt/0,05))` → rechter Stick | `desktop_controls.mjs:181-182` |
| Drohne | RMB halten | freier Blick 0,12°/px (Nicken ×0,85, −75..60°), springt nach 1,4 s zurück | `desktop_controls.mjs:57-60,186` |
| Drohne | Leertaste / Shift | steigen / sinken (Höhenachse) | `desktop_controls.mjs:185` |
| Drohne | LMB | Feuer | `desktop_controls.mjs:117` |
| Drohne | R / C / V | ARM / Kamera / zu Fuß | `desktop_controls.mjs:172-175` |
| Drohne | 1 / 2 | MG / Raketen (3 wählt nichts) | `desktop_controls.mjs:72-73,168` |
| Drohne | Q / Mausrad | Waffe weiter: MG → Raketen → GUIDED | `drone_weapon_modes.mjs:12` |
| Auto | W / S | Gas / Bremse-rückwärts | `player_car_mode.mjs:43` |
| Auto | A / D | lenken | `player_car_mode.mjs:43` |
| Auto | Leertaste | Handbremse (gehalten) | `player_car_mode.mjs:158` |
| Auto | Maus | Kopf 0,12°/px (Nicken ×0,85, −75..60°), springt nach 1,4 s Ruhe zurück (Rate 8/s) | `desktop_controls.mjs:57-60,142` |
| Auto | C / + − / Rad | Kamera / Zoom ×1/1,15, ×1,15 / ×1,1 | `player_car_mode.mjs:139,158` |
| Auto | E / F, N / B | aussteigen, Radio weiter/zurück | `player_car_mode.mjs:158`, `car_radio.mjs:70` |
| Jet | W / S, A / D | Schub-Steigen / Seitenruder | `jet_mode.mjs:264` |
| Jet | Pfeile, Maus | nicken/rollen; Maus = Rate-Stick 730 px/s, τ 25 ms (Maus hoch = Nase hoch) | `jet_mode.mjs:264,267` |
| Jet | Leertaste / LMB | Kanone (55 ms); mit gewählter RAKETE startet LMB die Rakete | `jet_mode.mjs:267,587` |
| Jet | R / B / V / C | RAKETE wählen (umschalten) / Bombe / Schweben-Flug / Kamera | `jet_mode.mjs:359,587` |
| Jet | E / F | aussteigen am Boden, in der Luft Schleudersitz | `jet_mode.mjs:249-257` |
| Fallschirm | Pfeile, Maus | `v.x → rollIn·4`, `v.y → −pitchIn·4` m/s (Welt-Achsen), ab 1,1 s | `jet_mode.mjs:289` |

Hilfezeile (`desktop_controls.mjs:191-196,232`): Text je Modus, auf Fuß und Drohne mit `[ WAFFE ]`
davor, 12 s nach der letzten Änderung ausgeblendet (0,6 s), tot `DOWN · SPACE / ENTER / R = RESPAWN`,
bei Pad-Eingabe und auf Touch verborgen.

## 3. Xbox-Controller (Standard-Mapping A0 B1 X2 Y3 LB4 RB5 LT6 RT7 VIEW8 MENU9 L3 10 R3 11 ↑12 ↓13 ←14 →15)

Gemeinsame Aktionen (`pad_actions.mjs:7-18,54-89`):

| Knopf | Wirkung |
|---|---|
| D-Pad ↑ | Luftschlag zielen; im Zielen A / RT / ↑ bestätigen, B abbrechen |
| D-Pad ↓ | Fuß ⇄ Drohne; im Auto/Jet über den Modus-Knopf aussteigen |
| D-Pad ← | vorige Waffe (Fuß/Drohne) |
| D-Pad → | Ping am Fadenkreuz |
| Y | nächste Waffe (Fuß/Drohne), nicht im R3+Y-Akkord |
| X | wie E: ein-/aussteigen, Schleudersitz |
| B | EMP (Fuß/Drohne); Auto: Handbremse |
| VIEW | antippen (< 400 ms, beim Loslassen) Kamera; halten > 0,42 s große Karte; 1,5 s nach dem Öffnen kein Tipp |
| MENU | Einstellungen auf/zu; Titel: START |
| R3 | Mikro an/aus beim Loslassen; R3 + Y stumm (`voice_control_bindings.mjs:7-13`) |
| A / Y tot | Respawn |

| Modus | Achse/Knopf | Wirkung | Quelle |
|---|---|---|---|
| Fuß | LS | `|v| ≤ .10 → 0`, sonst `sign·((|v|−.10)/.90)^1.28`, dann Lauf-Feinheit | `player_walk_mode_v4.mjs:147,302` |
| Fuß | RS | Totzone 2..20 % (5), Außen .97, Anti .045, dynamische Kurve (Feinheit), 3,75 / 3,40 rad/s × Blick-% / ADS, Raten-Integration 26 / 40 /s | `fps_control_math.mjs:20-58`, `first_person_control_settings.mjs` |
| Fuß | RT > .5 / LT > .35 | Feuer (MP jedes Bild, sonst Flanke) / ADS; Fäuste: RT rechts, LT links | `player_walk_mode_v4.mjs:302`, `first_person_controller_v5.mjs:139` |
| Fuß | A / L3 / LB | springen (Luft: Jetpack) / sprinten / Granate kochen-werfen | `player_walk_mode_v4.mjs:302`, `hand_grenade.mjs:57` |
| Drohne | LS / RS | `|v| ≤ .14 → 0`, linear; LS → `applyGameStick left`, RS → `right` | `xbox_gamepad.mjs:33-37`, `simulator.mjs:877` |
| Drohne | RT − LT | Höhenachse (`|RT−LT| < .05 → 0`) | `xbox_gamepad.mjs:58` |
| Drohne | RB / A | Feuer / ARM (nicht im ersten Bild); B tötet die Motoren NICHT | `simulator.mjs:879` |
| Drohne | LB (Schema AIM) | RS dreht nicht mehr, sondern schaut (105°/s, 82°/s) | `xbox_control_mode.mjs`, `real_world_bootstrap.mjs:261-264` |
| Auto | LS x / RT − LT / RS / B > .55 | lenken (Totzone .08 linear) / Pedal / Kopf (Feder, 1,75 rad · 0,7 rad, k = 1−e^(−dt·9 bzw. 6)) / Handbremse | `player_car_mode.mjs:38-51` |
| Jet | LS y / LS x / RS x / RS y | Schub / Ruder / Rollen / Nicken; Totzone .12 hart, ohne Umskalierung | `jet_mode.mjs:268-273` |
| Jet | RT > .4 / LT > .5 / RB / LB | Kanone (oder gewählte Rakete) / RAKETE wählen / Bombe / Schweben-Flug | `jet_mode.mjs:273` |
| Fallschirm | RS | steuert (wie Pfeile) | `jet_mode.mjs:268,289` |
| Karte | LS / RT−LT / A / X / Y / ←→ / R3 / B, VIEW | schieben −s·|s|·900 px/s (Totzone .15) / Zoom (rt−lt)·2,2/s / Wegpunkt Mitte / Ziel löschen / Suche / Zoom ±1 / mir folgen / schließen | `world_map_full.mjs:355-362` |
| Menüs | ↑↓ bzw. LS y (> .72, frei < .35) / ←→ bzw. LS x / A / LB RB / B, VIEW, MENU | Fokus ±1 mit Umlauf / Regler ±Schritt, Auswahl, Schalter / klicken / Abschnitt / schließen | `settings_controller_nav.mjs:13-46` |
| Titel | ↑↓ / A / MENU / B | Fokus / drücken (sonst START) / START / Welt-Optionen zu | `pad_actions.mjs:45-53` |
| Abstimmung | A / B / ←→ | ja / nein / Fokus | `pad_actions.mjs:71` |

Kein Rumble am Pad (nur `navigator.vibrate` für Ping und Explosionen).

## 4. Touch (Telefon/Tablet)

Rahmen: im Hochformat wird das Spiel als gedrehtes Querformat gezeigt (`simulator.mjs:613-619,765-768`);
Kern zeigt hochkant ohne Drehung (siehe Abweichungen). Sticks immer 168 px, bündig in den Ecken an den
sicheren Rändern, Trefferfläche nur der Kreis (`thumb_layout.mjs:8,23-29`). Auslenkung relativ zum
Aufsetzpunkt, Radius .42·168 (`control_semantics.mjs:143-165`). Knöpfe nach `edge_hud_layout.mjs:32-52`
(SAFE 10, GAP 8, Reihen über den Sticks, untere Reihe links vom Blick-Stick).

| Modus | Element | Wirkung | Quelle |
|---|---|---|---|
| Fuß | linker Stick | laufen analog, Sprint bei `m ≥ .90 && y < −.35` (Ring gelb) | `foot_touch_controller.mjs:102` |
| Fuß | rechter Stick | 1:1-Ziehen (0,0068 / 0,0059 rad/px, Flick bis ×1,35 ab 700 px/s) + Randdrehung ab .92 R über 42 px (4,2 / 3,3 rad/s, Touch-Kurve Innen .025) | `foot_touch_controller.mjs:57-63,138-146`, `fps_control_math.mjs:75-78` |
| Fuß | freie Fläche | Feuer am Finger, sofort; Ziehen bewegt den Zielpunkt (dreht nicht); MP alle 55 ms (≤ 4 nachholen), Glock 170 ms, sonst 1× je Berührung; Fäuste: Bildschirmhälfte = Hand | `foot_touch_controller.mjs:72-75,96,129-136` |
| Fuß | JUMP 64 | springen; in der Luft Jetpack; halten = Schub | `player_walk_mode_v4.mjs:69,197` |
| Fuß | Waffe 60 (rund, ⇄ + Name) | nächste Waffe bei `pointerdown` | `thumb_layout.mjs:16-31` |
| Fuß | SCOPE 60 (nur Sniper) | Umschalter, über dem Waffenknopf | `sniper_scope.mjs:30-45` |
| Fuß | GRANATE 64 | drücken = Stift ziehen, hochziehen = Kraft `.7 + d/110·.6`, loslassen = werfen | `hand_grenade.mjs:41` |
| Fuß | STRIKE 58, MODE (⇄ DRONE) 96×40 | Luftschlag-Zielen / Drohne | `fighter_jets.mjs:196`, `game_exit_button.mjs:55-63` |
| Fuß | 🚗 54 / ✈ 54 / EMP 62×36 | einsteigen (nur in der Nähe) / EMP (nur mit Sternen) | `player_car_mode.mjs:146`, `jet_mode.mjs:467`, `emp_button_layout.mjs` |
| Drohne | linker Stick | `applyGameStick left` (Translation, FC-vorkompensiert) | `simulator.mjs:799-806`, `control_semantics.mjs:171-184` |
| Drohne | rechter Stick | Gieren / Körperneigung (`phoneAxis`, Neigung standardmäßig invertiert) | `control_semantics.mjs:180-183` |
| Drohne | Höhenschieber 48×132 | Federschieber: `axis = (Mitte − pos)/(.40·h)`, Rate `12·u(.25+.75u)` m/s, Totband .08 | `simulator.mjs:780-789`, `control_semantics.mjs:52-58` |
| Drohne | freie Fläche | MG am Finger, alle 92 ms gehalten; Raketen/GUIDED bei Tipp | `flight_fire_fx.mjs:139-154` |
| Drohne | Knöpfe | Waffe, ARM, STOP (Motoren aus), MODE (⇄ FPS), STRIKE, EMP | `edge_hud_layout.mjs:36-44` |
| Auto | linker Stick „LENKEN / GAS“ | x lenken, −y Gas/Bremse | `player_car_mode.mjs:43`, `shared_sticks.mjs` |
| Auto | rechter Stick „UMSCHAUEN“ | Kopf (Feder) | `player_car_mode.mjs:51` |
| Auto | ↗ 54 / CAM / HANDBREMSE / MODE | aussteigen / Kamera / gehalten / aussteigen | `edge_hud_layout.mjs:39-44`, `player_car_mode.mjs:141` |
| Auto | zwei Finger frei | Zoom der Verfolgerkamera ×(alt/neu), 0,55..2,6 | `player_car_mode.mjs:135` |
| Jet | „STEIGEN / SCHUB“ / „KNÜPPEL“ | Schub+Ruder / Nicken+Rollen | `jet_mode.mjs:264` |
| Jet | KANONE (rund) / Tipp frei | Kanone gehalten; Tipp zielt im 35°-Kegel, mit RAKETE startet der Tipp die Rakete | `jet_mode.mjs:330-344,453` |
| Jet | ⇄ FLUG, RAKETE, BOMBE, CAM (unten Mitte, 40 h) | umschalten / wählen / Bombe / Kamera | `jet_mode.mjs:432,462` |
| Jet | EJECT 84×38 oben links | erst nach 650 ms Halten | `jet_mode.mjs:459-460` |
| Fallschirm | rechter Stick | steuern | `jet_mode.mjs:289` |
| alle | Minikarte | Ziehen = Kopf 0,85°/px, −0,62°/px (−75..60), Doppeltipp (360 ms) = große Karte | `real_world_bootstrap.mjs:235-239` |
| alle | Bildschirm | kein Zoom: zweiter Tipp in 340 ms wird verworfen, `touch-action:none` | `mobile_zoom_guard.mjs:1-17` |
| alle | Start | Vollbild + Querformat-Sperre, nicht als installierte PWA | `pwa_fullscreen_policy.mjs`, `simulator.mjs:902-903` |

## 5. Abweichungen (Ist-Stand Kern vor diesem Zweig → behoben)

Gefunden in `src/game.cpp` (read_input … parachute_input, update_touch_layout), `src/game_net.cpp`
(mp_input), `src/game_events.cpp` (update_combat), `src/game_equipment.cpp`, `src/game_ui.cpp`,
`src/touch_pad.*`, Kerns `TouchControls`/`InputArbiter`, `web/host.mjs`, `web/ui.mjs`, `web/mp_host.mjs`.

Desktop/PC:
1. RMB war Ping und die mittlere Taste Zielen (Kern-Bit 1 = RMB) → RMB zielen/Blick, MMB Ping. **behoben**
2. Pointer-Lock bei jedem LMB, der Fang-Klick schoss → nur LMB/RMB im Spiel, Fang-Klick verschluckt. **behoben**
3. Kamera aus `pointermove` ohne Begrenzung → eigener `mousemove`-Strom unter Lock, ±420 px je Ereignis. **behoben**
4. Esc öffnete das Pause-Menü, Lock-Verlust auch → Esc/Lock-Verlust = „Zurück zum Hauptmenü?“, Tab = Menü. **behoben**
5. X war EMP → G. **behoben**
6. J feuerte den Luftschlag sofort; 4 fehlte → 4/J/STRIKE/D-Pad ↑ = Zielen (Laser), J/4/Enter/LMB/A/RT/↑ bestätigen, RMB/Esc/B abbrechen. **behoben**
7. Q fehlte (Drohne: Q/E drehten) → Q nächste Waffe auf Fuß und Drohne, E/Q drehen nicht. **behoben**
8. Mausrad ohne Wirkung → Waffe (Fuß, Drohne), Zoom (Auto). **behoben**
9. P fehlte → MULTI. **behoben**
10. T warf sofort (2,5 s Zünder, 4 Stück) → kochen: halten/loslassen, 4 s, 3 Stück, +1 je 20 s, Explosion in der Hand. **behoben**
11. Strg duckte, R lud nach, O wechselte den Auftrag, L startete das Rennen, G schaltete GAME/ANGLE, K Motoren aus, B Neigung null — gibt es im Original nicht (L = GPS LIVE, K = stumm). **behoben** (Rennen, Auftrag, GAME/ANGLE bleiben als Befehle)
12. Drohne: Maus 900 px/s, Körperneigung integriert → 1500 px/s, τ 50 ms als rechter Stick; RMB freier Blick; Pfeile rechter Stick. **behoben**
13. Drohnen-Sticks: Kurven vertauscht (links/rechts) und ohne FC-Vorkompensation → `applyGameStick` exakt (Feinheit, km/h-Skala, inverse GAME-Kennlinie, Neigung invertiert). **behoben**
14. Drohne 3 = GUIDED → 3 wählt nichts (GUIDED über Q/Rad/Y/Knopf). **behoben**
15. Auto: Maus-Kopf rad-basiert ohne Rücksprung → 0,12°/px, 1,4 s, Rate 8/s; Pad-Kopf als Feder (9 bzw. 6); Totzone .08 statt .14; Zoom mit + − Rad und zwei Fingern. **behoben**
16. Jet: R feuerte die Rakete, Pad RB Rakete, Y Bombe → R/LT wählt RAKETE (dann LMB/Leertaste/RT/Tipp), RB/B Bombe; Totzone .12 hart. **behoben**
17. Fallschirm: WASD kamerarelativ → Pfeile/Maus/RS/rechter Stick in Welt-Achsen ×4 m/s, Nachführung 0,8/s. **behoben**
18. Hilfezeile: eigene Texte, immer an → Originaltexte, `[ WAFFE ]`, 12-s-Ausblenden, tot-Zeile, bei Pad aus. **behoben**
19. Tot: Tasten gingen ans Spiel → R/Enter/Leertaste/A/Y werden verschluckt (Kern belebt selbst nach 3 s, siehe „Später in die Engine“). **behoben**

Xbox:
20. B duckte → EMP (Fuß/Drohne). **behoben**
21. Y warf die Granate → nächste Waffe; LB = Granate kochen/werfen. **behoben**
22. D-Pad → nächste Waffe, ↑ EMP → → Ping, ↑ Luftschlag. **behoben**
23. R3 + → Ping → Ping liegt auf → allein (R3 + Y bleibt stumm). **behoben**
24. MENU öffnete die große Karte → MENU = Einstellungen; VIEW antippen Kamera / halten 0,42 s Karte. **behoben**
25. RB lud nach (Fuß) → nichts; RT/LT-Schwellen .5 / .35. **behoben**
26. Fuß-LS linear .14 → .10 mit Exponent 1,28; RS mit Kerns fester Kurve → Original-Kurve (Feinheit, Totzone, Raten-Integration) im Spielordner, an Kern als Grad übergeben. **behoben**
27. Drohne: Höhe RT−LT ohne Totband .05; Schema AIM fehlte; Release-Latch fehlte. **behoben**
28. Auto: D-Pad ←/→ Radio → D-Pad gehört Ping/Waffe wie im Original (Radio N/B, Touch RADIO). **behoben**
29. Karte mit dem Pad: linear 12 px/Bild → −s·|s|·900 px/s, Zoom 2,2/s, ←→ Zoom, R3 folgen, VIEW schließt. **behoben**
30. Menüs: LS-Navigation, LB/RB Abschnitt, VIEW schließt, Schalter mit ←→ fehlten. **behoben**

Touch:
31. Fuß lief über Kerns Soldaten-Layout (RELOAD, AIM, CROUCH, USE; Feuerfinger drehte die Kamera; Sticks schrumpften) → eigene Fuß-Steuerung nach `foot_touch_controller.mjs` mit JUMP, Waffe, SCOPE (Sniper), GRANATE, STRIKE, MODE, 🚗/✈, EMP; Feuerfinger dreht nicht; Sticks fest 168 px. **behoben**
32. Fahrzeug-Sticks: Mittelpunkt fest + halbe Bildschirmhälfte als Fangzone, Radius .5 → Aufsetzpunkt-relativ, .42, nur der Kreis. **behoben**
33. Drohne touch: rechter Stick war Höhe/Drehen → rechts Gieren/Neigung, eigener Höhenschieber; Feuer am Finger statt FEUER-Knopf; STOP fehlte. **behoben**
34. Auto touch: links lenken, rechts Gas → links LENKEN / GAS, rechts UMSCHAUEN. **behoben**
35. Jet touch: FEUER-Knopf statt KANONE + Tipp, EJECT ohne Halten → KANONE, Tipp-Feuer im 35°-Kegel, EJECT 650 ms. **behoben**
36. Knopf-Layout (2er-Gitter über den Sticks) → `edge_hud_layout.mjs`-Packung. **behoben**
37. Minikarte: kein Ziehen/Doppeltipp → Ziehen = Kopfblick (Drohne, Auto, Jet, Fallschirm), Doppeltipp = große Karte. **behoben**

38. Touch-Feuer der Kern-Waffen (MP, Glock, Sniper) ging durch die Bildschirmmitte → das Spiel feuert
    den Schuss selbst durch den Finger (Kerns Magazin, SpawnCitys Schaden/Reichweite). **behoben**
39. Hochkant ohne Drehung → gedrehtes Querformat per CSS wie `simulator.mjs` (Finger werden
    zurückgerechnet), Vollbild + Querformat-Sperre beim Start. **behoben**

Bleibt (nur mit Engine-Änderung, siehe README „Später in die Engine“): Sprint nur bei Vorwärts-
Anteil > 0,3 (Kerns Soldat; SpawnCity sprintet in jede Richtung); Respawn automatisch nach 3 s statt
per Taste/Knopf (die Tasten werden dafür verschluckt); Kerns eigene Zielhilfe für Pad/Touch statt
`fpsAimAssist`; Kopfblick zu Fuß über die Minikarte (Kerns Soldatenkamera hat keinen Zusatzblick).
