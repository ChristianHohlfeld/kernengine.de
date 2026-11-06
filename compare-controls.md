# Steuerung: Original und Kern mit echten Eingaben

Gemessen von tests/compare_controls.mjs (Workflow „SpawnCity Vergleich“). Werte: m = Meter, deg = Grad (Gierwinkel, gegen den Uhrzeigersinn positiv), kmh, dz = Höhenänderung; mps, degps, dzps = je Sekunde der Spielzeit, solange die Eingabe gehalten wurde (held s); die Abweichung vergleicht diese Raten.

## desktop

| Eingabe | Original | Kern | Abweichung |
|---|---|---|---|
| lock | locked true | locked true |  |
| W 1 s | m 6.05, mps 4.84, held 1.25, fps undefined | m 7.26, mps 4.94, held 1.4, fps 14 | 2 % |
| S 1 s | m 6.6, mps 5.28, held 1.25, fps undefined | m 6.72, mps 5.4, held 1.17, fps 13 | 2 % |
| D 1 s | m 4.62, mps 4.4, held 1.05, fps undefined | m 8.11, mps 4.99, held 1.55, fps 12 | 13 % |
| Shift+W 1 s | m 8.91, mps 6.85, held 1.3, fps undefined | m 9.61, mps 7.79, held 1.17, fps 15 | 14 % |
| Space (Sprung) | m 0.66, mps 0, held 0.28, fps undefined | m 0, held 0, fps 13 | 0 |
| Maus 200 px rechts | deg -29.8, pitch 0, locked true, held 0, fps undefined | deg -29.8, pitch 0, locked true, degps -298, held 0.1, fps 12 | -900 % |
| Maus 100 px runter | deg -8.7, yaw 0, locked true, degps -43.5, held 0.2, fps undefined | deg -14.9, yaw 0, locked true, degps -112, held 0.13, fps 12 | -157 % |
| Q (Waffe) | weapon WEAPON · GLOCK -> WEAPON · SNIPER, held 0.2, fps undefined | weapon WEAPON · GLOCK -> WEAPON · SNIPER, held 0.25, fps 13 |  |
| RMB (zielen) | body ads-active scope-active, lockEl viewport, aiming undefined, held 0.32, fps undefined | body , lockEl game, aiming false, held 0.24, fps 13, controls {"device":"desktop","pad":false,"locked":true,"aiming":false,"dead":false,"rocket":false,"scope":false,"zoom":1.000,"look":[0.00,0.00],"nade":{"carry":3,"left":-1.00},"lastExit":"","hint":"WASD move · SHIFT sprint · SPACE jump (2× jetpack) · LMB fire · RMB aim / scope · 1 MP 2 Glock 3 Sniper 5 Granatwerfer 6 Raketenwerfer 7 Fäuste (LMB links · RMB rechts) 8 Messer · Z / MMB ping · T halten Granate |  |
| Drohne Space 1 s | m 0, mps 0, held 1.1, fps undefined | m 0, mps 0, held 1.56, fps 11 | 0 |
| Drohne W 1 s | m 0, dz 0, mps 0, dzps 0, held 1.1, fps undefined | m 0, dz 0, mps 0, dzps 0, held 1.67, fps 11 | 0 |
| Drohne Maus 200 px | deg 0, degps 0, held 0.05, fps undefined | deg 0, degps 0, held 0.3, fps 11 | 0 |
| Drohne A 1 s | deg 0, m 0, mps 0, degps 0, held 1.28, fps undefined | deg 0, m 0, mps 0, degps 0, held 1, fps 10 | 0 |
| car | mode foot | mode car |  |
| Auto W 2 s | kmh 0, m 10.72, mps 5.11, held 2.1, fps undefined | kmh 74, m 34.52, mps 15.81, kmhps 16.5, held 2.18, fps 10 | 209 % |
| Auto W+A 1 s | deg 0, degps 0, held 1.3, fps undefined | deg 24.7, degps 14, held 1.77, fps 10 | 14 |
| Auto S 2 s (Bremse) | kmh 0, held 2.1, fps undefined | kmh 10, kmhps -36.5, held 2.08, fps 11 | -36.5 |
| Auto F (aussteigen) | mode foot -> foot, held 0.1, fps undefined | mode car -> foot, held 0.3, fps 11 |  |
| F (einsteigen) | mode foot -> foot, held 0, fps undefined | mode foot -> car, held 0, fps 10 |  |
| Wanted | held 0, fps undefined | held 0, fps 10 |  |
| Esc (Pause) | locked true -> true, held 0, fps undefined | locked true -> false, held 0, fps 13 |  |

Fehler Original: start: page.waitForFunction: Timeout 60000ms exceeded.

Fehler Kern: kern: not placed after 240 s · Failed to execute 'setPointerCapture' on 'Element': InvalidStateError · Failed to execute 'setPointerCapture' on 'Element': InvalidStateError

## pad

| Eingabe | Original | Kern | Abweichung |
|---|---|---|---|
| Pad links hoch 1 s | m 6.05, mps 5.5, held 1.1, fps undefined | m 0.76, mps 0.59, held 1.32, fps 15 | -89 % |
| Pad links 50 % 1 s | m 2.14, mps 1.79, held 1.2, fps undefined | m 0.01, mps 0.06, held 1.12, fps 22 | -97 % |
| Pad rechts 50 % 1 s | deg -42.1, degps -36.6, held 1.15, fps undefined | deg -93.4, degps -85, held 1.1, fps 14 | -132 % |
| Pad rechts 100 % 1 s | deg 158.2, degps 137.6, held 1.15, fps undefined | deg 105.2, degps 80.9, held 1.3, fps 11 | -41 % |
| Pad rechts 20 % 1 s | deg -170.8, degps -142.3, held 1.2, fps undefined | deg -48.2, degps -33.5, held 1.44, fps 18 | 76 % |
| Pad rechts hoch 50 % 1 s | deg 83.1, degps 58, held 1.43, fps undefined | deg 86, degps 65.5, held 1.31, fps 15 | 13 % |
| Pad A (Sprung) | m 0.66, mps 0, held 0.28, fps undefined | m 0, mps 0, held 0.43, fps 17 | 0 |
| Pad L3+links (Sprint) 1 s | m 8.1, mps 8.1, held 1, fps undefined | m 1.36, mps 1.29, held 1.05, fps 15 | -84 % |
| Pad Y (Waffe) | weapon WEAPON · GLOCK -> WEAPON · SNIPER, held 0.35, fps undefined | weapon WEAPON · GLOCK -> WEAPON · SNIPER, held 0.4, fps 13 |  |
| Drohne Pad links hoch 1 s | m 0, dz 0, mps 0, dzps 0, held 1.2, fps undefined | m 0, dz 0, mps 0, dzps 0, held 1.2, fps 14 | 0 |
| Drohne Pad rechts 50 % 1 s | deg 0, m 0, mps 0, degps 0, held 1.1, fps undefined | deg 0, m 0, mps 0, degps 0, held 1.2, fps 14 | 0 |
| Drohne Pad RT 1 s | dz 0, dzps 0, held 1.2, fps undefined | dz 0, dzps 0, held 1, fps 12 | 0 |
| Auto Pad RT 2 s | kmh 0, held 1.85, fps undefined | kmh 43, kmhps 17, held 2, fps 10 | 17 |
| Auto Pad links 50 % 1 s | deg 0, degps 0, held 1.15, fps undefined | deg 10.6, degps 8.6, held 1.23, fps 12 | 8.6 |
| Auto Pad X (aussteigen) | mode foot -> car, held 0.35, fps undefined | mode car -> foot, held 0.2, fps 11 |  |

Fehler Original: start: page.waitForFunction: Timeout 60000ms exceeded.

Fehler Kern: kern: not placed after 240 s

## phone

| Eingabe | Original | Kern | Abweichung |
|---|---|---|---|
| Touch Blick 120 px | deg 161.7, pitch 0, degps 71.9, held 2.25, fps undefined | deg 148.2, pitch 0, degps 32.5, held 4.55, fps 12 | -55 % |
| Touch Blick 60 px hoch | deg 20.3, degps 11, held 1.85, fps undefined | deg 20.3, degps 5.7, held 3.57, fps 12 | -48 % |
| Touch Stick hoch 1 s | m 5.03, mps 2.67, held 1.88, fps undefined | m 8.48, mps 2.84, held 2.9, fps 11 | 6 % |
| Touch Stick halb 1 s | m 2.44, mps 1.3, held 1.87, fps undefined | m 4.57, mps 1.6, held 2.87, fps 12 | 23 % |
| Tippen #footJump | m 0.52, mode foot -> foot, weapon , body , mps 0, held 0.2, fps undefined | m 0.65, mode foot -> foot, weapon , body , mps 0, held 0.4, fps 13 | 0 |
| Tippen #footWeaponToggle | m 0, mode foot -> foot, weapon WEAPON · GLOCK -> WEAPON · SNIPER, body , mps 0, held 0.2, fps undefined | m 0, mode foot -> foot, weapon WEAPON · GLOCK -> WEAPON · SNIPER, body , mps 0, held 0.32, fps 19 | 0 |
| Tippen #mobileGameplayWeapon | m 0, mode foot -> foot, weapon , body , held 0, fps undefined | m 0, mode foot -> foot, weapon , body , held 0, fps 18 | 0 |
| Tippen #handGrenadeBtn | m 0, mode foot -> foot, weapon , body , mps 0, held 0.22, fps undefined | m 0, mode foot -> foot, weapon , body , mps 0, held 0.23, fps 22 | 0 |
| Tippen #scopeButton | m 0, mode foot -> foot, weapon , body ads-active scope-active, mps 0, held 0.2, fps undefined | m 0, mode foot -> foot, weapon , body , held 0, fps 21 | 0 |
| Tippen #airStrikeButton | m 0, mode foot -> foot, weapon , body ads-active scope-active air-strike-targeting, mps 0, held 0.2, fps undefined | m 0, mode foot -> foot, weapon , body air-strike-targeting, mps 0, held 0.53, fps 19 | 0 |
| Tippen #footFire | m 0, mode foot -> foot, weapon , body ads-active scope-active air-strike-targeting, held 0, fps undefined | m 0, mode foot -> foot, weapon , body air-strike-targeting, held 0, fps 19 | 0 |
| Tippen #enterCarButton | m 0, mode foot -> foot, weapon , body ads-active scope-active air-strike-targeting, held 0, fps undefined | m 0, mode foot -> foot, weapon , body air-strike-targeting, held 0, fps 19 | 0 |
| Tippen #enterJetButton | m 0, mode foot -> foot, weapon , body ads-active scope-active air-strike-targeting, held 0, fps undefined | m 0, mode foot -> foot, weapon , body air-strike-targeting, held 0, fps 19 | 0 |
| Tippen #mobileGameplayMode | m 0, mode foot -> drone, weapon WEAPON · SNIPER -> WEAPON · GUN, body , mps 42.45, held 0.2, fps undefined | m 1.94, mode foot -> drone, weapon WEAPON · SNIPER -> WEAPON · GUN, body , mps 6.68, held 0.43, fps 20 | -84 % |
| Drohne Touch links hoch 1 s | Fehler: no #soloLeft | dz 0, m 0, mps 0, dzps 0, held 2.58, fps 16 |  |
| Drohne Touch rechts 1 s | Fehler: no #soloRight | deg -0.2, m 0, mps 0, degps -0.1, held 2.88, fps 13 |  |
| Auto Touch Gas 2 s | kmh 0, held 2.98, fps undefined | kmh 68, kmhps 7.1, held 3.78, fps 11 | 7.1 |
| Auto Tippen Kamera | held 0, fps undefined | held 0.42, fps 15 |  |
| Auto Tippen Aussteigen | mode foot -> foot, held 0, fps undefined | mode car -> car, held 0.32, fps 20 |  |

Fehler Original: Drohne Touch links hoch 1 s: no #soloLeft · Drohne Touch rechts 1 s: no #soloRight

Fehler Kern: kern: not placed after 240 s

