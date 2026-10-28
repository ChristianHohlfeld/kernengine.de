# 15 Eigene Modelle trainieren – dein Netz, deine Ente

> **Dein Level:** Jetzt trainierst du selbst. Du lernst, wie du den Lehrer startest, seinen Fortschritt
> liest, das beste Netz auswählst und es im Studio springen lässt – und wie eine neue Bewegung dazukommt.

Dies ist die Anleitung; wie der Lehrer rechnet, steht in [Kapitel 14](14-lehrer.md), die Grundlagen
in [Anhang A](A-grundlagen-neuronale-netze.md). Zwei Wege gibt es: die **Lehrer-Seite im Browser**
(nur Sprung und Stehen) und das **Werkzeug** `kern_lehrer` für alle, die Kern aus dem Quellcode bauen
([Kapitel 2](02-bauen-und-testen.md)).

> **Mission:** Öffne von https://kernengine.de/ aus den Lehrer, wähle „Sprung“ und starte. Lass den
> Tab offen (ein Handy ans Ladekabel). Beobachte „echte Sprünge (sauberer Roboter)“: Ab wann springt
> dein Netz?

## 15.1 Das Werkzeug bauen

```
kern/tools/lehrer.sh           # baut kern/build/kern_lehrer
kern/tools/lehrer.sh /pfad/out # baut nach /pfad/out
```

Das Skript läuft auf Linux und macOS (x86 und ARM), übersetzt Box3D, Kerns Mathebibliothek und die
Quellen außer Studio und App mit den Schaltern für Determinismus und baut nur neu, was sich geändert
hat. `CC`/`CXX` wählen den Compiler, `KERN_JOBS` die Zahl paralleler Compiler, `KERN_NATIVE=0` schaltet
die Optimierung für deinen Prozessor ab – sie ändert **kein Ergebnis**. Pollens Netze lädt das Werkzeug
aus `third_party/microduck/` (oder `--policies ORDNER`).

## 15.2 Alle Befehle

| Befehl | Zweck |
|---|---|
| `--task jump` / `stand` | eine Bewegung mit PPO trainieren |
| `--eval netz.onnx` | Sprung messen, sauber und variiert |
| `--trace …` | eine Episode Takt für Takt |
| `--vorbild-suchen demo.json` | Sprung-Vorbild mit CMA-ES suchen |
| `--vorbild-pruefen …` | ein Vorbild springen lassen |
| `--spielen` | Spielschicht nachahmen (DAgger) |
| `--spielen-pruefen …` | Vorbild oder Netz spielen lassen |
| `--spielen-ueben play.onnx` | Spielschicht mit PPO üben |
| `--spielen-spur` | eine Spielepisode Schritt für Schritt |
| `--biss-fenster` | wo der Schnabel eine Socke greift |
| `--halt-messen` | wie weit die Ente nach „Stopp“ geht |
| `--demo-spur` | das Beispiel-Level wie im Studio |
| `--fingerprint 400` | Determinismus-Fingerabdruck |
| `--kalibrieren` u. a. | Kalibrierung ([Kapitel 16](16-kalibrierung.md)) |

Für alle gilt: `--threads N` (Standard: alle Kerne) und `--seed N` (Standard 1).

## 15.3 Den Sprung trainieren

### Der Standardlauf

```
kern/build/kern_lehrer --task jump --iterations 3000 \
  --out jump.onnx --log jump.csv \
  --checkpoint run.ckpt --keep-every 250
```

So entstand Kerns `jump.onnx` (32 Enten auf 2 Kernen, Seed 1). Was passiert:

1. **Rezept laden:** Warmstart von velstand, Nachahmen (8 Runden à 96 Episoden), Üben mit Rauschen
   0,1 und Lernrate 10⁻⁴. Optionen überschreiben einzelne Werte.
2. **Enten anlegen:** `--envs` (16 je Thread), abgerundet auf ein Vielfaches der Threads.
3. **Nachahmen,** dann `jump_nachgeahmt.onnx` und `run.ckpt.nachgeahmt` schreiben.
4. **Üben:** 30 Iterationen nur Kritiker; alle 10 Iterationen (`--eval-every`) 5 Probesprünge.
5. **Speichern:** alle 25 Iterationen (`--save-every`), am Ende und alle 250 als Schnappschuss
   (`jump_it00250.onnx`).

Bei mindestens 4 von 5 echten Probesprüngen erscheint `FIRST REAL JUMP at iteration …`, und das Netz
wird gespeichert; `--stop-after-jump` beendet den Lauf dort.

### Fortsetzen, ohne Vorbild, eigenes Vorbild

```
kern_lehrer --task jump --iterations 6000 \
  --resume run.ckpt --checkpoint run.ckpt
```

`--resume` lädt den ganzen Lerner und überspringt das Nachahmen; `--iterations` ist das **Ziel**. Mit
`run.ckpt.nachgeahmt` beginnt das Üben neu.

`--vorbild none` ist reines PPO (mit dem Term `track_height`) – dieser Weg fand den Absprung nicht.
Zum Experimentieren: `--vorbild demo.json`, `--nachahmen 5 --episoden 64`, `--uebungs-std 0.2`,
`--nachahmungs-lohn 0`. Ein eigenes Vorbild suchst du mit `--vorbild-suchen demo.json --iterations 120`
(fortsetzen mit `--vorbild demo.json`) und prüfst es mit `--vorbild-pruefen`. Jede Generation meldet
„a real jump“, „no real jump“ oder „fell“.

Die Lehrer-Seite trainiert dasselbe Rezept: Gleicher Seed und gleich viele Enten insgesamt (Worker ×
Enten je Worker) ergeben dasselbe Netz wie auf dem Rechner.

## 15.4 Fortschritt lesen

Jede Übungsiteration schreibt eine Zeile mit diesen Feldern:

| Feld | Bedeutung | Worauf achten |
|---|---|---|
| `return` | mittlere Rückgabe | steigt langsam |
| `len` | Episodenlänge in Takten | 100 = 2 s ohne Sturz |
| `value` | Fehler des Kritikers | erst hoch, dann fallend |
| `std` | Rauschen σ | sinkt, wenn das Netz sicherer wird |
| `lr` | Lernrate | dauerhaft 1e-5: Schritte zu groß |
| `steps/s` | Entenschritte pro Sekunde | Tempo deines Geräts |
| `jumps n/5` | Sprungprobe | das eigentliche Ziel |

Das Nachahmen meldet pro Runde Beispiele (`samples`) und Fehler (`error`). Mit `--log jump.csv`
landet jede Iteration in einer CSV-Datei für die Tabellenkalkulation (`jumps` = −1: nicht gemessen).
Geht etwas schief, zeigt `--trace jump.onnx` eine Episode Takt für Takt mit jedem Belohnungsterm
(`builtin` statt Datei: das Vorbild; `--randomized`: mit Variation). So siehst du, ob das Netz
springt – oder nur die Belohnung austrickst.

## 15.5 Das beste Netz auswählen

Das letzte Netz ist nicht immer das beste. Darum:

1. Mit `--keep-every` Schnappschüsse behalten.
2. Jeden mit mehr Episoden messen, sauber **und** variiert:

   ```
   for f in jump_it*.onnx; do
     kern_lehrer --eval "$f" --episodes 40
   done
   ```

3. Wählen nach: echte Sprünge mit Variation, dann Stürze (0), dann Steigen, Flugzeit, Neigung.
4. Im Studio prüfen (15.8).

Für Kerns `jump.onnx` ergab das 40 von 40 sauber und 36 von 40 variiert, ohne Sturz.

## 15.6 Hyperparameter wählen

**Hyperparameter** sind die Einstellungen vor dem Lernen. Eine Iteration mit 32 Enten: 32 × 48 = 1536
Zeilen (Rauschen 0,1), aufgeteilt in 4 Minibatches à 384 Zeilen (je 8 Teile à 48), 5 Epochen × 4 =
20 Adam-Schritte, vor jedem die Lernratenprüfung. Beim Nachahmen wären 8 × 96 × 100 = 76 800 Beispiele
möglich; es waren 49 936, weil eine Episode früher endet, sobald die Ente fällt oder ein anderes Teil
als die Sohlen den Boden berührt.

| Option | Sprung-Rezept | Wirkung |
|---|---|---|
| `--envs N` | 16 je Thread | mehr Daten; **ändert das Netz** |
| `--threads N` | alle Kerne | Tempo (bei festem `--envs`) |
| `--steps N` | 48 | Takte je Ente und Iteration |
| `--epochs`, `--minibatches` | 5, 4 | Lernschritte je Iteration |
| `--lr X` | 10⁻⁴ | Startlernrate |
| `--kl X` | 0,01 | Größe jedes Schritts |
| `--init-std X` | 0,3 | Rauschen vor dem Nachahmen |
| `--uebungs-std X` | 0,1 | Rauschen beim Üben |
| `--entropy X` | 0,005 | bremst das Sinken von σ |
| `--actor`, `--critic` | 512,256,128 / 256,128 | Netzgrößen |
| `--warm-start` | velstand | oder `none` |
| `--substeps N` | 16 | nicht ändern! |
| `--profil servo.json` | XL330 | kalibrierte Servos |

Dazu `--adaptive-lr 0|1`. Beim Warmstart muss der Actor zu velstand passen. Ohne `--envs` gilt
„16 je Thread“: Wer dann die Threads ändert, ändert auch die Zahl der Enten – und damit das Netz.
`--warm-start` kennt nur `velstand`; jeder andere Wert startet mit Zufallsgewichten. Einen Warmstart
aus einer Datei gibt es nur beim Üben der Spielschicht. Tipps aus echten Läufen:
nach dem Nachahmen vorsichtig sein; mehr Rauschen hilft nicht beim Finden; wenige Enten bremsen; mit
8 statt 16 Unterschritten lernt das Netz eine andere Physik.

> **So kannst du es dir vorstellen:** Die Lernrate ist wie die Schrittlänge beim Laufen im Dunkeln.
> Zu kurz, und du kommst nie an. Zu lang, und du stolperst über die Bordsteinkante.

## 15.7 Die Spielschicht trainieren

Das geht nur mit dem Werkzeug, in sechs Schritten:

1. **Messen:** `--biss-fenster` zeigt auf einem Gitter von 11 × 9 Stellen, wo der Schnabel eine Socke
   hält, und die beste Stelle (das Training misst es selbst). `--halt-messen` zeigt den Nachlauf nach
   „Stopp“.
2. **Nachahmen:**

   ```
   kern_lehrer --spielen --nachahmen 12 \
     --episoden 32 --out play.onnx
   ```

   Standard: 8 Runden à 64 Episoden; jede Runde misst das Netz in 16 sauberen Episoden.
3. **Prüfen:** `--spielen-pruefen vorbild` bzw. `play.onnx` zählt Kicks, Aufheben, Ausspucken,
   Griffe und Stürze, sauber und variiert. Das Vorbild zeigt, was höchstens drin ist.
4. **Ansehen:** `--spielen-spur --seed 3` zeigt jede Entscheidung (25 pro Sekunde); mit `--netz` fährt
   das Netz, und du siehst, wo es vom Vorbild abweicht.
5. **Üben:** `--spielen-ueben play.onnx --iterations 300 --envs 32`: Rauschen 0,1, erst 20 Iterationen
   nur Kritiker, alle 20 Iterationen ein Schnappschuss.
6. **Im Level prüfen:** `--demo-spur --netz play_geuebt.onnx --seed 120` spielt das Micro-Duck-Level
   wie im Studio, hier 120 s – bei diesem Befehl ist `--seed` die Dauer in Sekunden! Mit
   `KERN_JUMP_AT=SEKUNDE` drückt es dort die Taste 7.

> **Vorsicht, Falle:** `--out` wird nach **jeder** Runde überschrieben. Willst du eine frühere Runde
> behalten, kopiere die Datei während des Laufs. Für das alte `play.onnx` war Runde 10 von 12 die beste.

## 15.8 Im Studio benutzen

Kerns Netze heißen fest `jump.onnx` und `play.onnx`. So kommt deins ins Studio:

1. **Ins Repository:** die Datei nach `kern/policies/` kopieren. Eine fertige Seite lädt aus
   `policies/` neben der Seite.
2. **Aus dem Browser-Lehrer:** Er speichert sein Netz auf demselben Gerät. Das Studio lädt es beim
   Start und übernimmt jedes neue sofort, auch in offenen Tabs. Das gilt nur für den Sprung.
3. **Prüfen:** Micro-Duck-Level öffnen, Play, Taste **7**. Für die Spielschicht die Ente in Ruhe lassen,
   mit Autonomous und Kern extras an und Ball oder Socke in Sicht.

Das Netz braucht die Form eines rsl_rl-Actors; ein Spielnetz ohne 23 Eingänge und 9 Ausgänge wird
nicht gerechnet.

> **Mission:** Hast du im Browser-Lehrer einen Sprung trainiert? Öffne auf demselben Gerät Kern Studio,
> lade File › Example Levels › Micro Duck (läuft Play, erst mit Esc stoppen), starte Play, drücke F8
> und dann **7**. Öffne die
> Browser-Konsole: Dort steht „Kern: trained network jump.onnx loaded“. Jetzt springt **dein** Netz.

> **Mission:** Klick im Micro-Duck-Level die Ente an und schau unter Details › Player Controller ›
> Walker nach **Autonomous** und **Kern extras**. Starte Play einmal mit und einmal ohne Kern extras
> (zum Umschalten erst mit Esc stoppen). Was macht die Ente mit Ball und Socke anders?

## 15.9 Eine neue Aufgabe hinzufügen

Eine neue Bewegung (61 → 14) braucht vier Dinge, oft dazu ein Vorbild.

**1. Die Aufgabe:** in `src/lehrer/` von `Task` ableiten. Anfang eines Beispiels „bow“ (Verbeugung):

```cpp
class BowTask : public Task {
 public:
  std::string name() const override { return "bow"; }
  f32 episode_seconds() const override { return 2.0f; }
  i32 privileged() const override { return 4; }
```

Zeile 1: `BowTask` ist eine `Task`. Zeilen 3–5: Name, 2 s Dauer, 4 Extra-Werte für den Kritiker.
Dazu kommen Start, Phasensignal (Plätze 55/56), Lehrpläne und Belohnung mit Hilfen wie `upright` und
`fell`. Das ganze Gerüst steht im blauen Buch.

**2. Rezept und Namen** in `recipe.cpp` eintragen (etwa Warmstart velstand, Lernrate 10⁻⁴). Dann
kennen Werkzeug und Browser die Aufgabe; der Lehrer-Seite fehlt nur ein Menüeintrag. Nutzt die
Aufgabe Plätze, die velstand nie gesehen hat, müssen sie beim Warmstart als neue Eingänge gelten;
das macht Kern derzeit nur für die Phasenplätze 55 und 56.

**3. Messen** wie der `JumpMeter`: prüfen, was die Bewegung **ist**, nicht was die Belohnung zählt.

**4. Ein Vorbild (optional).** Findet PPO die Bewegung nicht: wenige Zahlen, die sie beschreiben, eine
Bewertung für CMA-ES und ein Vorbild, das in **jedem** Zustand eine Antwort hat (für DAgger).
`cmaes_minimise` und `Ppo::imitate` sind allgemein, der Rest ist auf den Sprung zugeschnitten.

**5. Im Studio verdrahten** wie den Sprung: Eintrag in der Bewegungsliste, Taste, Touch-Knopf, Name
beim Logik-Knoten **Robot Move**, Dateiname in `game.mjs`, Datei in `kern/policies/`. Ein Netz mit
**anderem** Format zeigt die Spielschicht: eigene Umgebung, eigener Lerner, eigene Bewertung – PPO und
Nachahmen bleiben gleich.

## 15.10 Beobachtung, Aktion und Belohnung entwerfen

- **Nur, was der Roboter hat:** Sensoren für den Actor, Wahres für den Kritiker.
- **Zeit sichtbar machen:** Phasensignal (cos, sin), Steh- und Gehzeit.
- **Messen statt befehlen:** „Steht“ heißt, was die Beine tun.
- **Aktionen wie Bedienelemente:** Totzone wie beim Stick (unter 20 %), Bewegung startet über 0,5.
- **Benannte Terme**, Strafen wachsen per Lehrplan.
- **Hartes Ende** bei Sturz und falschem Bodenkontakt.
- **Vorbild ohne Geheimwissen;** Wünsche steigen als Rampe.

## 15.11 Fallstricke aus den echten Läufen

- **Belohnung ausgetrickst:** Die Ente saß auf dem Schwanz oder stützte sich ab. Nur strengere Regeln
  halfen.
- **Zu lockere Messung:** Ein „Sprung“ mit 45° Kippen ging durch. Jetzt zählt nur ein echter.
- **Seltene Bewegungen:** PPO allein findet sie nicht; Vorbild und Nachahmen schon.
- **Ahnungsloser Kritiker** zerstört einen guten Actor – darum Kritiker zuerst.
- **Wandernde Normalisierer:** Die alte Policy wird neu bewertet.
- **Fehler in der Welt:** Die Socke kroch, der Ball rollte ewig, Gähnen öffnete den Schnabel. Lernt
  ein Netz etwas nicht, kann das auf so einen Fehler zeigen.
- **Zahlen von Hand veralten:** Die Bissstelle wird bei jedem Lauf gemessen.
- **Determinismus brechen** mit `std::sin` oder Summen in Thread-Reihenfolge; `--fingerprint 400`
  zeigt es.
- **Andere Physik als im Studio** heißt: Das Netz lernt eine andere Welt.

## Level geschafft!

- Du trainierst einen Sprung im Browser oder mit `kern_lehrer`.
- Du liest `return`, `len`, `std` und die Sprungprobe.
- Du wählst das beste Netz, sauber und variiert gemessen.
- Du weißt, was das Netz ändert und was nur das Tempo.
- Du bringst dein Netz ins Studio.
- Du kennst die Schritte für eine neue Bewegung.

## Rätsel-Ecke

1. Welche Taste startet im Studio den Sprung?
2. Ändert `--threads 1` statt `--threads 8` das Netz? Und `--envs`?
3. Dein Lauf zeigt dauerhaft `lr 1e-05`. Was bedeutet das?
4. **Boss-Rätsel:** Bei `--demo-spur --seed 120` – wie lange läuft das Level, und warum ist das eine
   Falle?

### Lösungen

1. Die **7** (auf dem Handy der Knopf HOP).
2. Kommt drauf an: Mit festem `--envs` ändern Threads nur das Tempo. Ohne `--envs` gilt 16 Enten je
   Thread – 16 statt 128 Enten, also andere Daten und ein anderes Netz. `--envs` ändert es immer.
3. Die Policy ändert sich je Schritt zu stark; die Regel drückt die Lernrate ganz nach unten.
4. 120 Sekunden: Nur hier bedeutet `--seed` die Dauer, nicht den Zufallsstart.

---

© 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
