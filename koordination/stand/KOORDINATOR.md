# Koordinator – Mitteilungen an alle (neueste oben)
- 18:27 **BEFUND (aus den zwei 40-Kern-Läufen von k, e5f2bf1 und 3dd3338, beide `convert rc=1`):** das Einlesen ins Level scheitert –
  `net_session_cpp.cpp: line 611, column 20: the number 78 names two nodes` → „the Blueprints were not read: the level is as it was“. Beide Läufe gleich
  (14 Runden, 41–45 min bei 40 Kernen). Steckt das auch in `work` (90b5ab8), endet der Basislauf `work-voll-162644-5528` ebenso ohne gültiges Level.
  Zur Klärung abgelegt (Probe mit Level und Texten): `work-probe-182522-630` auf origin/work, `work-f2-probe-182528-670` auf origin/work-f2 (je net_session + jet_seats + mp_rules).
  Dazu 8 Zeilen „was written and waits now“: Game/game_look_cpp ×7 (look_begin, look_blast, look_camera, look_apply_tier – „invalid use of incomplete type Renderer“ –,
  look_lap, look_people_budget, view_zoom), Game/game_lamps_cpp gaps_lamps_draw („‘Game’ has not been declared“) – BEFUND FÜR t2.
- 18:24 CI 44351625418 auf work-d 5367e14 (Fix F1 + Zwischenstand): build, determinism, native, input, e2e grün (native-spawncity lief noch); mag
  `work-d-engine-174905-11207` lief noch. Probe mit Level funktioniert: `work-probe-180405-238` rc=0.
- 18:10: Proben auf mag haben seit **18:04** das Level (kein „which the level does not have“-Artefakt mehr); volle Läufe liefern Level und Tests gleich nach der
  Konvertierung (`mag-zwischen/`). Einzelheiten: PAKET_COMMON3.md, „Nachtrag 18:10“.
- 17:50: alle Agenten gestoppt (Unterbrechung der Sitzung); Neustart nur auf Chris' Wort.
- Nichts geht auf `work`, bevor das Ergebnis von `work-voll-162644-5528` (läuft seit 17:38:59 auf 90b5ab8) übernommen ist.
