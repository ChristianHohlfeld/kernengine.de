# Stilleitfaden – Kern, das grüne Buch („Kern – Einfach erklärt“)

Es gibt drei Bücher mit demselben Inhalt, für verschiedene Leser:

- **Blau** (`kern/docs/buch`): vollständig und technisch, für Profis. **Die Quelle der Wahrheit.**
- **Grün** (dieses Buch, `kern/docs/buch-gruen`): genauso vollständig, aber einfacher erklärt.
- **Gelb** (später): für Kinder.

## Für wen das grüne Buch ist

Neugierige mit wenig Vorwissen: Hobby-Entwicklerinnen, Schüler der Oberstufe, Studierende,
Quereinsteiger, Lehrkräfte. Sie können mit einem Computer umgehen, haben vielleicht schon ein wenig
programmiert, kennen aber weder C++ im Detail noch Grafik-, Physik- oder KI-Mathematik.

Nach dem Buch sollen sie **alles verstanden haben, was Kern ausmacht**: was jeder Teil tut, warum er so
gebaut ist, wie man ihn benutzt – und sie sollen **Spaß** dabei gehabt haben.

## Vollständig, aber einfacher

- **Jedes Thema des blauen Kapitels kommt vor**: jedes Konzept, jedes Werkzeug, jede Bedienung, die
  wichtigen Zahlen und Grenzen. Lies das blaue Kapitel ganz, bevor du schreibst, und hake ab.
- **Einfacher heißt**: weniger Implementierungsdetails, keine langen API-Tabellen, keine
  Aufzählungen aller Parameter. Stattdessen: Was ist das? Wozu? Wie fühlt es sich an? Wie benutze ich es?
  Was passiert, wenn …? Lange Referenzlisten fasst du zusammen („über 30 Einstellungen, die wichtigsten
  fünf sind …“) und verweist für alles Weitere auf das blaue Buch oder Kapitel 17.
- **Nichts erfinden.** Keine Funktion, kein Menü, kein Tastenkürzel, keine Zahl, die nicht im blauen
  Buch (oder in `kern/docs/*.md`) steht. Vereinfachen ja, verfälschen nie. Im Zweifel weglassen
  oder vorsichtig formulieren.
- **Fachwörter** beim ersten Auftreten in einem Satz erklären, danach benutzen. Englische Begriffe, die
  im Programm so heißen (Viewport, Outliner, Play), bleiben englisch.
- **Formeln** nur, wenn sie das Verständnis wirklich tragen. Immer zuerst in Worten, dann ein
  Zahlenbeispiel, dann (wenn nötig) die Formel.
- **Code** sparsam: höchstens kurze Ausschnitte (bis etwa 12 Zeilen), nur aus dem blauen Buch
  übernommen (nie erfunden), und danach Zeile für Zeile in Worten erklärt.

## Spaß

- **Du-Form**, warm, begeistert, mit Humor – aber nie albern oder herablassend.
- Kurze Sätze, aktive Verben, Bilder im Kopf: **Alltagsvergleiche** (Kamera = Fotoapparat,
  Physikschritt = Daumenkino, neuronales Netz = Mischpult mit tausend Reglern …).
- Kleine Geschichten und „Was würde passieren, wenn …?“-Fragen.
- Wiederkehrende Kästen (als Zitatblock mit fettem Etikett, genau so schreiben):
  - `> **Kurz gesagt:** …` – direkt unter der Kapitelüberschrift, 2–4 Sätze, worum es geht.
  - `> **Stell dir vor …** …` – eine Analogie.
  - `> **Probier's aus!** …` – eine konkrete Aufgabe in Kern Studio auf https://kernengine.de/ (nur
    Dinge, die es laut blauem Buch wirklich gibt: Menüs, Tasten, Vorlagen wie Orb Run, Physics Lab,
    Micro Duck …).
  - `> **Wusstest du?** …` – eine verblüffende Tatsache (aus dem blauen Buch).
  - `> **Achtung, Falle!** …` – ein typischer Fehler und wie man ihn vermeidet.
  Pro Kapitel etwa 2–4 „Probier's aus!“, 2–4 „Stell dir vor“, je 1–3 der anderen.
- Am Kapitelende immer:
  - `## Das nimmst du mit` – 5–8 kurze Punkte.
  - `## Knobelecke` – 3–5 Fragen zum Nachdenken, darunter `### Lösungen` mit kurzen Antworten.

## Form (wichtig für den Satz)

- Dateiname wie im blauen Buch (z. B. `06-physik.md`). Erste Zeile: `# N Titel` (Anhänge:
  `# Anhang X – Titel`). Der Titel darf freundlicher sein als im blauen Buch.
- Abschnitte nummeriert: `## N.1 …`, `## N.2 …`; Unterabschnitte `###` ohne Nummer.
- Bilder: nur vorhandene aus dem blauen Buch, Pfad `../buch/bilder/NAME.png`, z. B.
  `![Kern Studio mit Orb Run](../buch/bilder/studio.png)`. Vorhanden: studio, studio-selected,
  studio-simulate, studio-quad, studio-physics-lab, studio-play, studio-multiplayer, studio-narrow,
  phone-dock-390, phone-play-390, game, game-phone, duck-level, duck-play, lehrer (alle `.png`).
- Schaubilder gern als Mermaid (` ```mermaid `), einfach und klein (höchstens etwa 8 Kästen).
- Tabellen höchstens 4 Spalten, kurze Zellen.
- Codeblöcke höchstens etwa 70 Zeichen breit.
- **Keine URLs** außer `https://kernengine.de/` und `https://github.com/ChristianHohlfeld/kernengine.de`.
  Fremde Projekte nur mit Namen nennen, nicht verlinken.
- Verweise auf andere Kapitel: `[Kapitel 6](06-physik.md)` (Dateinamen wie im blauen Buch).
- Letzte Zeilen jeder Datei:

  ```
  ---

  © 2026 Christian Heinrich Hohlfeld · Alle Rechte vorbehalten · [Impressum und Rechte](00-impressum.md)
  ```

- Neue deutsche Rechtschreibung. „die Micro Duck“ (weiblich: der Micro Duck, zur Micro Duck),
  „Servomodell“, Projektadresse kernengine.de. Pollen Robotics nur sachlich nennen (sie haben den
  echten Roboter und seine Netze gebaut; Kern ist nicht mit ihnen verbunden).

## Umfang

Etwa 45–60 % der Wortzahl des blauen Kapitels (die Kästen, Analogien und Knobelecken zählen mit).
Lieber ein Beispiel mehr als eine Tabelle mehr. Kein Thema auslassen.

## Nachtrag 28.10.: Prinzipien vor Details (gilt vor allem anderen)

Ein Lesetest mit den Zielgruppen hat gezeigt: Die Bücher waren zu technisch. „Vollständig“ heißt deshalb: Jedes **Prinzip** des blauen Buchs kommt vor und wird verstanden – was es ist, wozu, wie es grundsätzlich funktioniert, warum Kern es so macht, wie man es in Studio sieht. **Nicht** jedes Detail. Funktions- und Klassennamen, Dateipfade, Schalter, Feldnamen, JSON-Schlüssel, Standardwerte und Zahlen ohne Erkenntniswert gehören ins blaue Buch („Die genauen Namen und Werte stehen im blauen Buch, Kapitel N.“), höchstens ein reizvolles Detail pro Abschnitt in einen Kasten (gelb `> **Für Profis:**`, grün `> **Für Neugierige:**`).

- Jeder Abschnitt beginnt mit dem Prinzip in einfachen Worten und einem Bild aus dem Alltag.
- Jedes Kapitel hat direkt nach dem Eingangskasten einen Begriffskasten (gelb `> **Wörter, die du brauchst:**`, grün `> **Begriffe vorab:**`, höchstens 8 Begriffe), damit niemand vorher die Anhänge lesen muss.
- Studio-Bedienung (Menüs, Tasten, Felder) für die Missionen bleibt; lange Listen nur in Kapitel 17.
- Rätsel fragen Prinzipien ab, keine Details.
