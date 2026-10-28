# Stilleitfaden – Kern, das gelbe Buch („Kern – Für Entdecker“)

Es gibt drei Bücher mit demselben Inhalt, für verschiedene Leser:

- **Blau** (`kern/docs/buch`): vollständig und technisch, für Profis. **Die Quelle der Wahrheit.**
- **Grün** (`kern/docs/buch-gruen`): genauso vollständig, einfacher erklärt, für Erwachsene ohne Vorwissen.
- **Gelb** (dieses Buch, `kern/docs/buch-gelb`): genauso vollständig, für Jugendliche von **12 bis 16 Jahren**.

## Für wen das gelbe Buch ist

Jugendliche zwischen 12 und 16, die Spiele lieben, neugierig auf Technik sind und vielleicht schon
mit Scratch, Minecraft oder ein paar Zeilen Python gebastelt haben. Sie können Prozente, Brüche,
Koordinatensysteme, Pythagoras und einfache Gleichungen (ungefähr 7. bis 9. Klasse). Englische
Begriffe aus Spielen kennen sie oft schon.

Das Buch ist **fordernd, aber voll verständlich**: Es nimmt die Leser ernst, erklärt echte Technik und
echte Ideen – nichts wird kindisch weichgespült –, aber jeder Schritt ist so erklärt, dass man ihn
mit Mittelstufenwissen nachvollziehen kann. Wer das Buch durch hat, versteht wirklich, wie eine
Game-Engine, eine Physiksimulation und ein lernender Roboter funktionieren.

## Vollständig

- **Jedes Thema des blauen Kapitels kommt vor.** Lies das blaue Kapitel ganz und hake jeden
  Abschnitt ab. Das grüne Kapitel darfst du als Hilfe lesen, wie man etwas einfach erklärt; die
  Fakten kommen aus dem blauen Buch.
- **Nichts erfinden.** Keine Funktion, kein Menü, keine Taste, keine Zahl, die nicht im blauen Buch
  oder im Quellcode (`kern/src`, `kern/include`, `kern/web`) steht. Vereinfachen ja, verfälschen nie.
- Implementierungsdetails (Dateinamen, Funktionsnamen, Parameterlisten) nur, wenn sie zum Verstehen
  nötig sind; sonst in Worten und mit Verweis „Wer es ganz genau wissen will: das blaue Buch“.
- **Mathe** nur mit Schulwissen: Vektoren als Pfeile, Pythagoras, Prozent, Mittelwert, Steigung.
  Was darüber hinausgeht (Matrizen, Ableitungen, Integrale), wird **mit einem Bild und einem
  durchgerechneten Zahlenbeispiel** erklärt, nicht mit Formeln. Höchstens eine kleine Formel pro
  Abschnitt, und nur, wenn sie aus Schulmathe besteht.
- **Code** nur sehr kurz (bis etwa 8 Zeilen), aus dem blauen Buch übernommen, Zeile für Zeile erklärt.

## Spaß und Ton

- **Du-Form**, lebendig, direkt, begeistert. Kurze Sätze. Kein Lehrerton, keine Babysprache,
  keine Jugendsprache-Anbiederung („krass“, „Digga“ und Ähnliches vermeiden).
- Die Spielwelt als roter Faden: Jedes Kapitel ist ein **Level**. Kapitelanfang:
  `> **Dein Level:** …` – 2–3 Sätze, was man in diesem Kapitel lernt und warum es cool ist.
- Wiederkehrende Kästen (Zitatblock mit fettem Etikett, genau so schreiben):
  - `> **Mission:** …` – eine konkrete Aufgabe in Kern Studio auf https://kernengine.de/ (nur
    Dinge, die es laut blauem Buch wirklich gibt). Pro Kapitel 3–5 Missionen.
  - `> **So kannst du es dir vorstellen:** …` – ein Vergleich aus dem Alltag von Jugendlichen
    (Schule, Sport, Handy, Fahrrad, Kochen, Musik, Spiele).
  - `> **Krass, aber wahr:** …` – eine verblüffende Tatsache (aus dem blauen Buch).
  - `> **Vorsicht, Falle:** …` – ein typischer Fehler.
  - `> **Für Profis:** …` – ein Ausblick für besonders Neugierige, etwas anspruchsvoller.
- Am Kapitelende immer:
  - `## Level geschafft!` – 5–7 kurze Punkte: Das kannst du jetzt.
  - `## Rätsel-Ecke` – 3–5 Fragen (vom leichten zum kniffligen „Boss-Rätsel“), darunter
    `### Lösungen` mit kurzen, erklärten Antworten.
- Altersgerecht: Das Spielmodell „Soldat“ (Kapitel 8 und 11) sachlich und kurz nennen, nichts
  ausschmücken. Keine Gewalt, keine Angstszenarien. Sicherheitshinweise zum echten Roboter
  (Kapitel 16) klar: Nur zusammen mit einem Erwachsenen, Hände weg von laufenden Servos.
- Die Micro Duck ist der Roboter von Pollen Robotics und **keine erfundene Figur**: Sie spricht
  nicht, sie erzählt nicht. Man darf sie mögen und beschreiben, aber nicht vermenschlichen.

## Form (wichtig für den Satz)

- Dateiname wie im blauen Buch (z. B. `06-physik.md`). Erste Zeile: `# N Titel` (Anhänge:
  `# Anhang X – Titel`). Der Titel darf spannend sein.
- Abschnitte nummeriert: `## N.1 …`, `## N.2 …`; Unterabschnitte `###` ohne Nummer.
- Bilder: nur vorhandene aus dem blauen Buch, Pfad `../buch/bilder/NAME.png`. **Jedes Bild höchstens
  einmal im ganzen Buch** – die Zuteilung steht im Auftrag deines Kapitels.
- Schaubilder gern als Mermaid (` ```mermaid `), einfach und klein (höchstens etwa 8 Kästen).
- Tabellen höchstens 3 Spalten, kurze Zellen.
- Codeblöcke höchstens etwa 60 Zeichen breit.
- **Keine URLs** außer `https://kernengine.de/` und `https://github.com/ChristianHohlfeld/kernengine.de`.
- Verweise auf andere Kapitel ohne Anker: `[Kapitel 6](06-physik.md)`.
- Letzte Zeilen jeder Datei:

  ```
  ---

  © 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
  ```

- Neue deutsche Rechtschreibung. „die Micro Duck“ (weiblich), „Servomodell“.

## Umfang

Etwa 40–50 % der Wortzahl des blauen Kapitels. Lieber ein Beispiel mehr als ein Fachwort mehr.
Kein Thema auslassen.
