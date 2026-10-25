// Kern game shell: the runtime page of games packaged with Kern Studio, plus
// the JavaScript wrapper over the Studio C ABI that the editor page shares.
//
// The engine module is the same one the editor runs (generated/kern_studio.wasm).
// A packaged game starts it in player-only mode, opens its level and plays it
// full screen: rendering, Box3D physics, controllers and game rules all run in
// C++ (kern::game::GameWorld). This file sizes the canvas, forwards input and
// draws the HUD text - nothing else.
import {loadKern, KERN_NOTICES} from "../kern.mjs";

/**
 * German interface for Kern Studio and the games, purely visual: the engine, its commands, the
 * level files and the console stay in English. The page's text is translated as it appears
 * (a MutationObserver swaps exact phrases in text nodes and in title / placeholder / aria-label),
 * so no panel code changes and nothing the user typed is touched: inputs, the outliner's actor
 * names, the level name and the output log are left alone.
 *
 * The language follows the browser / the operating system (navigator.languages). `?lang=en` or
 * `?lang=de` in the address overrides it and is remembered on this device.
 */

const I18N_KEY = "kern.lang";

function i18nPickLanguage() {
  try {
    const q = new URLSearchParams(location.search).get("lang");
    if (q === "de" || q === "en") { localStorage.setItem(I18N_KEY, q); return q; }
    const kept = localStorage.getItem(I18N_KEY);
    if (kept === "de" || kept === "en") return kept;
  } catch { /* storage blocked: follow the browser */ }
  const langs = (typeof navigator !== "undefined" && (navigator.languages?.length ? navigator.languages : [navigator.language])) || [];
  return String(langs[0] || "en").toLowerCase().startsWith("de") ? "de" : "en";
}

export const I18N_LANG = i18nPickLanguage();

// Exact phrases (as the page shows them, trimmed). Product and level names stay English.
const I18N_DE = {
  // menu bar, toolbar, tabs
  "File": "Datei", "Edit": "Bearbeiten", "Window": "Fenster", "Play": "Spielen", "Help": "Hilfe",
  "Untitled": "Unbenannt", "Unsaved changes": "Ungespeicherte Änderungen",
  "⤓ Install": "⤓ Installieren", "Install Kern Studio as an app (works offline, full screen)": "Kern Studio als App installieren (offline, Vollbild)",
  "In Safari tap": "Tippe in Safari auf", "Share": "Teilen", "↑ and then": "↑ und dann auf", "Add to Home Screen": "Zum Home-Bildschirm",
  ": Kern Studio then starts full screen like an app and works offline.": ": Kern Studio startet dann im Vollbild wie eine App und läuft offline.",
  "Save (Ctrl+S)": "Speichern (Strg+S)", "Undo (Ctrl+Z)": "Rückgängig (Strg+Z)", "Redo (Ctrl+Y)": "Wiederholen (Strg+Y)",
  "Place an actor": "Actor platzieren", "Add": "Hinzufügen",
  "Select (Q)": "Auswählen (Q)", "Move (W)": "Verschieben (W)", "Rotate (E)": "Drehen (E)", "Scale (R)": "Skalieren (R)",
  "World / local coordinates (Ctrl+`)": "Welt- / lokale Koordinaten (Strg+`)", "Grid snap ([ and ])": "Am Raster ausrichten ([ und ])",
  "Rotation snap": "Drehung einrasten", "Scale snap": "Skalierung einrasten", "Camera speed (wheel while flying)": "Kamerageschwindigkeit (Mausrad beim Fliegen)",
  "Play in the viewport (Alt+P)": "Im Viewport spielen (Alt+P)", "Simulate physics (Alt+S)": "Physik simulieren (Alt+S)",
  "Pause (Pause)": "Pause (Pause)", "Advance one frame (.)": "Ein Bild weiter (.)", "Stop (Esc)": "Stopp (Esc)", "Eject / possess (F8)": "Aussteigen / übernehmen (F8)",
  "Viewport layout": "Viewport-Aufteilung", "Logic": "Logik",
  "Logic: the level's Blueprint-style graph (actors have their own: Details > Logic)": "Logik: der Graph des Levels im Blueprint-Stil (Actors haben eigene: Details > Logik)",
  "Package the game as one HTML file": "Spiel als eine HTML-Datei verpacken", "Outliner and Details": "Outliner und Details",
  "Stop playing": "Spiel beenden", "⟵ EXIT": "⟵ ENDE", "❚❚ PAUSE": "❚❚ PAUSE", "▶ RESUME": "▶ WEITER", "Resume": "Fortsetzen", "Pause": "Pause",
  "Outliner": "Outliner", "Details": "Details", "World": "Welt", "World Settings": "Welteinstellungen", "Close": "Schließen",
  "New folder for the selection": "Neuer Ordner für die Auswahl", "Search actors": "Actors suchen", "Label": "Name", "Type": "Typ",
  "Place Actors": "Actors platzieren", "Textures": "Texturen", "Sounds": "Sounds", "Output Log": "Ausgabeprotokoll", "Search": "Suchen",
  "Close (Ctrl+Space)": "Schließen (Strg+Leertaste)", "Content drawer (Ctrl+Space)": "Inhaltsablage (Strg+Leertaste)", "Content Drawer": "Inhaltsablage",
  "Enter console command (help)": "Konsolenbefehl eingeben (help)", "Sound on / off": "Ton an / aus", "Output level": "Ausgangspegel", "Version": "Version",
  "Play controls this actor": "Spielen steuert diesen Actor", "Player": "Spieler", "Locked": "Gesperrt",
  "Select an actor in the viewport or the outliner to edit its details.": "Wähle einen Actor im Viewport oder im Outliner, um seine Details zu bearbeiten.",
  "Search details": "Details durchsuchen", "Multiple values": "Mehrere Werte", "Reset to default": "Auf Standard zurücksetzen", "Reset to Default": "Auf Standard zurücksetzen",
  "Copy Value": "Wert kopieren", "Paste Value": "Wert einfügen",

  // menus
  "New Level...": "Neues Level...", "New Level": "Neues Level", "Open Level...": "Level öffnen...", "Save": "Speichern", "Save As...": "Speichern unter...",
  "Example Levels": "Beispiel-Levels", "Package Game (HTML)...": "Spiel verpacken (HTML)...", "Package Game": "Spiel verpacken", "Package": "Verpacken",
  "Import Texture...": "Textur importieren...", "Import texture": "Textur importieren", "Import Sound": "Sound importieren", "Import image": "Bild importieren",
  "Import an image": "Ein Bild importieren", "Import audio (WAV, MP3, OGG, M4A, FLAC)": "Audio importieren (WAV, MP3, OGG, M4A, FLAC)",
  "Undo": "Rückgängig", "Redo": "Wiederholen", "History...": "Verlauf...", "Undo History": "Rückgängig-Verlauf",
  "Cut": "Ausschneiden", "Copy": "Kopieren", "Paste": "Einfügen", "Duplicate": "Duplizieren", "Delete": "Löschen",
  "Select All": "Alles auswählen", "Select Children": "Kinder auswählen", "Deselect": "Auswahl aufheben",
  "Group": "Gruppieren", "Attach": "Anhängen", "Detach": "Lösen", "Hide": "Ausblenden", "Show All": "Alle einblenden", "Lock": "Sperren", "Unlock": "Entsperren",
  "Focus Selection": "Auswahl fokussieren", "Focus": "Fokussieren", "Snap to Floor": "Auf den Boden setzen", "Snap to floor": "Auf den Boden setzen",
  "Make Player": "Zum Spieler machen", "Make Player (control it in Play)": "Zum Spieler machen (beim Spielen steuern)",
  "Connect with Constraint": "Mit Gelenk verbinden", "Keep Simulation Changes": "Simulationsänderungen übernehmen",
  "Keep the simulated poses of the selection": "Die simulierten Posen der Auswahl übernehmen",
  "Move to folder": "In Ordner verschieben", "Remove from folder": "Aus Ordner entfernen", "Rename Folder": "Ordner umbenennen", "Rename folder": "Ordner umbenennen",
  "Remove Folder (keep actors)": "Ordner entfernen (Actors behalten)", "Rename": "Umbenennen", "Remove": "Entfernen",
  "Edit Logic": "Logik bearbeiten", "Add Logic (Blueprint)": "Logik hinzufügen (Blueprint)", "Open this actor's logic graph (Blueprint)": "Logik-Graph dieses Actors öffnen (Blueprint)",
  "Preferences...": "Einstellungen...", "Preferences": "Einstellungen", "Reset Settings to Defaults...": "Einstellungen zurücksetzen...", "Reset Settings to Defaults": "Einstellungen zurücksetzen",
  "Viewport Layout": "Viewport-Aufteilung", "1 viewport": "1 Viewport", "2 viewports": "2 Viewports", "4 viewports": "4 Viewports",
  "Fullscreen": "Vollbild", "Game View": "Spielansicht", "Game view": "Spielansicht", "Game view (hide editor helpers)": "Spielansicht (Editor-Hilfen ausblenden)",
  "Stats": "Statistik", "Icons": "Symbole", "Grid": "Raster", "Bounds": "Begrenzungen", "Collision Shapes": "Kollisionsformen", "Contact Points": "Kontaktpunkte",
  "Constraints": "Gelenke", "Selection Outline": "Auswahlumriss", "Fog": "Nebel", "Sky": "Himmel",
  "Simulate": "Simulieren", "Stop": "Stopp", "Advance One Frame": "Ein Bild weiter", "Restart Game": "Spiel neu starten", "Restart": "Neu starten",
  "Eject from Player": "Aus dem Spieler aussteigen", "Possess Player": "Spieler übernehmen", "Number of Players": "Anzahl der Spieler", "Play: one player": "Spielen: ein Spieler",
  "Host Online Game...": "Online-Spiel hosten...", "Host Online Game": "Online-Spiel hosten", "Join Online Game...": "Online-Spiel beitreten...", "Join Online Game": "Online-Spiel beitreten",
  "Leave Online Game": "Online-Spiel verlassen", "Host": "Hosten", "Join": "Beitreten", "Joining the game...": "Trete dem Spiel bei...", "Network Emulation": "Netzwerk-Emulation",
  "Perfect": "Perfekt", "Relay (optional)": "Relay (optional)", "Online play:": "Online spielen:",
  "Controls...": "Steuerung...", "About Kern Studio": "Über Kern Studio",
  "Perspective": "Perspektive", "Top": "Oben", "Bottom": "Unten", "Front": "Vorne", "Back": "Hinten", "Left": "Links", "Right": "Rechts",
  "Lit": "Beleuchtet", "Unlit": "Unbeleuchtet", "Wireframe": "Drahtgitter", "Show": "Anzeigen", "View Mode": "Ansichtsmodus", "Viewport Type": "Viewport-Typ",
  "Viewport options": "Viewport-Optionen", "Layout": "Aufteilung", "Maximise / restore (quad layout)": "Maximieren / wiederherstellen (4er-Aufteilung)",
  "Bookmarks": "Lesezeichen", "Camera speed": "Kamerageschwindigkeit", "Camera speed (1-8)": "Kamerageschwindigkeit (1-8)",
  "Grid size": "Rastergröße", "Grid snapping": "Am Raster einrasten", "Rotation snapping": "Drehung einrasten", "Scale snapping": "Skalierung einrasten",
  "Rotation step": "Drehschritt", "Scale step": "Skalierungsschritt", "Mouse sensitivity": "Mausempfindlichkeit", "Invert mouse Y": "Maus-Y umkehren",
  "Interface": "Oberfläche", "Apply": "Übernehmen", "Cancel": "Abbrechen", "Create": "Erstellen", "Update": "Aktualisieren", "Later": "Später", "Dismiss": "Schließen",
  "New version available": "Neue Version verfügbar", "Start fresh": "Neu anfangen", "Play it": "Spielen",
  "Your settings are kept in this browser. Edit > Reset Settings to Defaults puts all of them back.": "Deine Einstellungen bleiben in diesem Browser. Bearbeiten > Einstellungen zurücksetzen stellt alle wieder her.",
  "Unsaved changes of the current level are kept in the autosave until you start editing the new one.": "Ungespeicherte Änderungen des aktuellen Levels bleiben in der automatischen Sicherung, bis du das neue bearbeitest.",
  "Builds one self-contained HTML file with the Kern runtime and this level. It plays full screen in any browser - no editor, no server.": "Erstellt eine eigenständige HTML-Datei mit der Kern-Laufzeit und diesem Level. Sie läuft im Vollbild in jedem Browser – ohne Editor, ohne Server.",
  "Game title": "Spieltitel", "Level name": "Level-Name", "Name": "Name", "Actor name": "Actor-Name",
  "World coordinates (Ctrl+` for local)": "Weltkoordinaten (Strg+` für lokal)", "Local coordinates (Ctrl+` for world)": "Lokale Koordinaten (Strg+` für Welt)",
  "Possess the player (F8)": "Spieler übernehmen (F8)", "Eject from the player (F8)": "Aus dem Spieler aussteigen (F8)", "Show four viewports": "Vier Viewports zeigen",
  "Show one viewport": "Einen Viewport zeigen", "Show / hide": "Ein- / ausblenden", "Show / hide (H)": "Ein- / ausblenden (H)", "Advance one simulation step": "Einen Simulationsschritt weiter",
  "Player (controlled in Play)": "Spieler (beim Spielen gesteuert)", "comma, separated": "durch Komma getrennt", "Preview": "Vorhören", "Burst color": "Funkenfarbe",
  "Period": "Periode", "Step rate": "Schrittrate", "Drone": "Drohne", "Sock": "Socke",
  "Pollen Robotics' biped robot on its trained networks: W/S walk, A/D turn, Space quacks, 1 sit, 2 pick up, 3/4 kick, 5 roll, 6 beak": "Der zweibeinige Roboter von Pollen Robotics auf seinen trainierten Netzen: W/S laufen, A/D drehen, Leertaste quakt, 1 sitzen, 2 aufheben, 3/4 kicken, 5 rollen, 6 Schnabel",
  "The Micro Duck on its roller skates: roller.onnx skates, 2 crouches": "Die Micro Duck auf Rollschuhen: roller.onnx fährt, 2 duckt",

  // controls dialog
  "Select (Shift adds, Ctrl toggles); click empty space to deselect": "Auswählen (Umschalt fügt hinzu, Strg schaltet um); Klick ins Leere hebt die Auswahl auf",
  "Marquee select (Shift adds, Ctrl toggles); actors move with the gizmo (W / E / R)": "Rahmenauswahl (Umschalt fügt hinzu, Strg schaltet um); Actors bewegen sich mit dem Gizmo (W / E / R)",
  "Move forward / back (zoom in orthographic views)": "Vor / zurück (Zoom in orthografischen Ansichten)", "Move up / down and sideways while flying": "Hoch / runter und seitwärts beim Fliegen",
  "Orbit / dolly / pan around the selection": "Um die Auswahl kreisen / heranfahren / schwenken", "Orbit around selection (Alt+LMB)": "Um die Auswahl kreisen (Alt+LMT)",
  "Duplicate while moving": "Beim Verschieben duplizieren", "Drag from Place Actors": "Aus „Actors platzieren“ ziehen", "Drop a new actor exactly where you release it": "Neuen Actor genau dort ablegen, wo du loslässt",
  "Select / move / rotate / scale  /  cycle": "Auswählen / verschieben / drehen / skalieren  /  durchschalten", "Focus selection  /  snap to floor": "Auswahl fokussieren  /  auf den Boden setzen",
  "Copy, paste, cut, duplicate, delete": "Kopieren, einfügen, ausschneiden, duplizieren, löschen", "Undo / redo": "Rückgängig / wiederholen",
  "Hide selected / hide others / show all": "Auswahl ausblenden / andere ausblenden / alle einblenden", "Set / jump to camera bookmark": "Kamera-Lesezeichen setzen / anspringen",
  "Smaller / larger grid snap": "Kleineres / größeres Raster", "Perspective / top / front / side view": "Perspektive / oben / vorne / seitlich",
  "Wireframe / unlit / lit / collision view": "Drahtgitter / unbeleuchtet / beleuchtet / Kollisionsansicht", "Play  /  Simulate": "Spielen  /  Simulieren",
  "Stop  /  eject or possess  /  free the mouse": "Stopp  /  aussteigen oder übernehmen  /  Maus freigeben", "Grab and throw physics bodies": "Physikkörper greifen und werfen",
  "Left click": "Linksklick", "Left drag": "Links ziehen", "Right mouse + WASD / QE": "Rechte Maustaste + WASD / QE", "Mouse wheel": "Mausrad",
  "Middle mouse drag": "Mittlere Maustaste ziehen", "Right + left mouse": "Rechte + linke Maustaste", "Alt + left / right / middle": "Alt + links / rechts / Mitte",
  "Alt + drag gizmo": "Alt + Gizmo ziehen", "Drag (Simulate)": "Ziehen (Simulieren)", "K (Simulate)": "K (Simulieren)", "Key": "Taste", "Action": "Aktion",

  // status line
  "Kern Studio ready - Orb Run loaded. Press Play (Alt+P) to drive, or fly around with the right mouse button + WASD.": "Kern Studio bereit – Orb Run geladen. Drück Spielen (Alt+P) zum Fahren oder flieg mit rechter Maustaste + WASD herum.",
  "Playing - WASD / arrows to move, mouse to look, Shift+F1 frees the mouse, F8 ejects, Esc stops": "Spielt – WASD / Pfeiltasten bewegen, Maus schaut, Umschalt+F1 gibt die Maus frei, F8 steigt aus, Esc beendet",
  "Playing - left stick moves, right stick or a drag looks around, the buttons act": "Spielt – linker Stick bewegt, rechter Stick oder Wischen schaut, die Knöpfe handeln",
  "Simulating - physics runs, drag a body to move or lift it, K keeps the selected actors' poses": "Simuliert – die Physik läuft, zieh einen Körper zum Bewegen oder Anheben, K übernimmt die Posen der Auswahl",
  "Stopped - back to editing": "Gestoppt – zurück im Editor", "Paused": "Pausiert", "Resumed": "Fortgesetzt", "Restarted": "Neu gestartet",
  "Ejected - free camera, F8 possesses the player again": "Ausgestiegen – freie Kamera, F8 übernimmt den Spieler wieder", "Possessed the player": "Spieler übernommen",
  "Watching - free camera, F8 takes over the player again": "Zuschauen – freie Kamera, F8 übernimmt den Spieler wieder",
  "Watching - WASD / QE fly, drag to look around, drag a body to move or lift it (wheel: nearer / farther), F8 takes over the player": "Zuschauen – WASD / QE fliegen, ziehen zum Umsehen, einen Körper ziehen zum Bewegen oder Anheben (Mausrad: näher / weiter), F8 übernimmt den Spieler",
  "Watching - drag to look around, pinch to fly, drag a body to move or lift it, F8 takes over the player": "Zuschauen – ziehen zum Umsehen, zwei Finger zum Fliegen, einen Körper ziehen zum Bewegen oder Anheben, F8 übernimmt den Spieler",
  "Mouse released - click the viewport to control the game again": "Maus freigegeben – klick in den Viewport, um wieder zu steuern",
  "Nothing to control yet: drop a player from Content > Players (Soldier, Hover Car...) - simulating instead": "Noch nichts zu steuern: zieh einen Spieler aus Inhalte > Spieler (Soldat, Hovercar...) – stattdessen wird simuliert",
  "Stop playing to add actors": "Beende das Spiel, um Actors hinzuzufügen", "Stop playing to undo": "Beende das Spiel, um rückgängig zu machen", "Stop playing to redo": "Beende das Spiel, um zu wiederholen",
  "Fix the errors above to play": "Behebe die Fehler oben, um zu spielen", "No player to possess": "Kein Spieler zum Übernehmen", "No server to join": "Kein Server zum Beitreten",
  "Only an actor with a Player Controller can be the player": "Nur ein Actor mit Spieler-Steuerung kann der Spieler sein",
  "Select two actors: the constrained one, then (last) the one it hangs on": "Wähle zwei Actors: den verbundenen, dann (zuletzt) den, an dem er hängt",
  "Select actors to put them into a new folder": "Wähle Actors, um sie in einen neuen Ordner zu legen",
  "Keep Simulation Changes works on selected actors while simulating": "„Simulationsänderungen übernehmen“ wirkt beim Simulieren auf ausgewählte Actors",
  "Cannot attach an actor to its own child": "Ein Actor kann nicht an sein eigenes Kind gehängt werden",
  "Only audio files can be imported as sounds": "Nur Audiodateien lassen sich als Sounds importieren",
  "Sound muted": "Ton aus", "Sound on": "Ton an", "Settings reset to defaults": "Einstellungen zurückgesetzt", "Saved": "Gespeichert", "Opened": "Geöffnet",
  "Starting the engine": "Engine startet", "Loading": "Lädt", "Game lost": "Spiel verloren",

  // details: sections and groups
  "Transform": "Transformation", "Location": "Position", "Rotation": "Drehung", "Scale": "Skalierung", "Lock proportions": "Proportionen sperren",
  "Mesh": "Mesh", "Static Mesh": "Statisches Mesh", "Physics": "Physik", "Physics (Box3D)": "Physik (Box3D)", "Rigid Body": "Starrkörper", "Rigid Body (Box3D)": "Starrkörper (Box3D)",
  "Material": "Material", "Collision": "Kollision", "Collider": "Kollider", "Constraint": "Gelenk", "Constraint (joint)": "Gelenk", "Light": "Licht", "Audio": "Audio",
  "Player Controller": "Spieler-Steuerung", "Camera": "Kamera", "Movement": "Bewegung", "Handling": "Fahrverhalten", "Weapon": "Waffe", "Health": "Gesundheit",
  "Pickup": "Pickup", "Trigger": "Trigger", "Mover": "Beweger", "Rotator": "Rotator", "Sky and light": "Himmel und Licht", "Gameplay": "Spielablauf", "Sound": "Sound",
  "Game": "Spiel", "Physical material": "Physikmaterial", "Limits": "Grenzen", "Spring": "Feder", "Motor": "Motor", "Breaking": "Bruch", "Respawn": "Wiedereinstieg",
  "Explosion": "Explosion", "Regeneration": "Regeneration", "Edit world settings": "Welteinstellungen bearbeiten", "Remove component": "Komponente entfernen",
  "Hidden (editor and game)": "Ausgeblendet (Editor und Spiel)", "Locked (not selectable)": "Gesperrt (nicht auswählbar)", "Tags": "Tags", "Outliner folder": "Outliner-Ordner",
  "Attached to": "Angehängt an", "Transform only: a parent or pivot": "Nur Transformation: ein Elternteil oder Drehpunkt",

  // details: fields
  "Shape": "Form", "Size": "Größe", "Radius": "Radius", "Height": "Höhe", "Segments": "Segmente", "Generate collision": "Kollision erzeugen", "Friction": "Reibung",
  "Restitution (bounce)": "Rückprall (Elastizität)", "Rolling resistance": "Rollwiderstand", "Base color": "Grundfarbe", "Roughness": "Rauheit", "Metallic": "Metallisch",
  "Emissive color": "Leuchtfarbe", "Emissive strength": "Leuchtstärke", "Opacity": "Deckkraft", "Texture": "Textur", "Texture tiling": "Texturkachelung", "Two sided": "Beidseitig",
  "Cast shadow": "Wirft Schatten", "Motion": "Bewegungsart", "Mobility": "Mobilität", "Mass (0 = from density)": "Masse (0 = aus Dichte)", "Density": "Dichte",
  "Linear damping": "Lineare Dämpfung", "Angular damping": "Winkeldämpfung", "Gravity scale": "Schwerkraftfaktor", "Allow sleep": "Ruhen erlauben", "Start awake": "Wach starten",
  "Lock position": "Position sperren", "Lock rotation": "Drehung sperren", "Continuous (bullet)": "Kontinuierlich (Geschoss)", "Initial velocity": "Anfangsgeschwindigkeit",
  "Initial spin": "Anfangsdrehung", "Sensor (overlap only)": "Sensor (nur Überlappung)", "Group index": "Gruppenindex", "Channel bits": "Kanal-Bits", "Collides with bits": "Kollidiert mit Bits",
  "Offset": "Versatz", "Box size": "Boxgröße", "Capsule length": "Kapsellänge", "Connected actor": "Verbundener Actor", "World (fixed point)": "Welt (fester Punkt)",
  "Anchor (local)": "Anker (lokal)", "Axis (local)": "Achse (lokal)", "Collide connected": "Verbundene kollidieren", "Limit": "Begrenzung", "Lower": "Unten", "Upper": "Oben",
  "Frequency": "Frequenz", "Damping ratio": "Dämpfungsgrad", "Max force / torque": "Max. Kraft / Drehmoment", "Speed": "Geschwindigkeit",
  "Break force (0 = never)": "Bruchkraft (0 = nie)", "Break torque (0 = never)": "Bruchmoment (0 = nie)", "Break sound": "Bruchgeräusch", "Rest length (0 = at start)": "Ruhelänge (0 = wie beim Start)",
  "Distance": "Abstand", "Light color": "Lichtfarbe", "Intensity": "Intensität", "Attenuation radius": "Reichweite", "Attenuation": "Abschwächung",
  "Volume": "Lautstärke", "Pitch": "Tonhöhe", "Spatial (3D)": "Räumlich (3D)", "Falloff distance": "Abfallentfernung", "Roll-off": "Abfall", "Doppler": "Doppler",
  "Play on start": "Beim Start abspielen", "Bus": "Bus", "Impact sound": "Aufprallgeräusch", "Impact threshold": "Aufprallschwelle",
  "Max speed": "Höchstgeschwindigkeit", "Acceleration": "Beschleunigung", "Turn rate": "Drehrate", "Boost": "Boost", "Boost speed": "Boost-Geschwindigkeit",
  "Jump speed (0 = none)": "Sprunggeschwindigkeit (0 = keiner)", "Hover height": "Schwebehöhe", "Field of view": "Sichtfeld", "Field of View": "Sichtfeld",
  "Eye height": "Augenhöhe", "Crouch speed": "Ducken-Geschwindigkeit", "Look sensitivity": "Blickempfindlichkeit", "Head bob": "Kopfwippen",
  "Fire rate": "Feuerrate", "Damage": "Schaden", "Range": "Reichweite", "Magazine": "Magazin", "Reserve": "Reserve", "Reload time": "Nachladezeit", "Recoil": "Rückstoß",
  "Hip spread": "Streuung aus der Hüfte", "Aimed spread": "Streuung beim Zielen", "Aim field of view": "Sichtfeld beim Zielen", "Pellets per shot": "Schrot pro Schuss",
  "Headshot multiplier": "Kopftreffer-Faktor", "Points for a kill": "Punkte pro Abschuss", "Armor": "Panzerung", "Respawn after (0 = never)": "Wiedereinstieg nach (0 = nie)",
  "Respawn delay": "Wiedereinstiegs-Verzögerung", "Respawn point": "Wiedereinstiegspunkt", "Regenerate after (0 = never)": "Regeneriert nach (0 = nie)",
  "Explosion damage": "Explosionsschaden", "Explosion radius (0 = none)": "Explosionsradius (0 = keiner)", "Impulse on bodies": "Impuls auf Körper",
  "Points": "Punkte", "Heals": "Heilt", "Ammo": "Munition", "Only once": "Nur einmal", "Action": "Aktion", "Message": "Nachricht",
  "Strength (launch m/s, damage)": "Stärke (Start m/s, Schaden)", "Teleport target": "Teleport-Ziel", "Rotation rate": "Drehrate", "Phase": "Phase",
  "Start delay": "Startverzögerung", "Win condition": "Siegbedingung", "Target score": "Zielpunktzahl", "Time limit (0 = none)": "Zeitlimit (0 = keins)",
  "Kill Z (respawn below)": "Kill-Z (darunter Wiedereinstieg)", "Gravity": "Schwerkraft", "Time of day": "Tageszeit", "Day length (0 = frozen)": "Tageslänge (0 = angehalten)",
  "Sun azimuth": "Sonnenrichtung", "Fog density": "Nebeldichte", "Exposure": "Belichtung", "Ambient occlusion radius (0 = off)": "Umgebungsverdeckung (0 = aus)",
  "Shadow distance (smaller: sharper)": "Schattenreichweite (kleiner: schärfer)", "Clouds": "Wolken", "Ground color": "Bodenfarbe", "Endless ground": "Endloser Boden",
  "Ground friction": "Bodenreibung", "Ground bounciness": "Bodenelastizität", "Contact stiffness": "Kontaktsteifigkeit", "Contact damping ratio": "Kontaktdämpfung",
  "Contact push-out speed": "Kontakt-Ausstoßgeschwindigkeit", "Sub-steps": "Teilschritte", "Music": "Musik", "Music volume": "Musiklautstärke", "Ambience": "Atmosphäre",
  "Ambience volume": "Atmosphärenlautstärke", "Reverb": "Hall", "Master volume": "Gesamtlautstärke", "Lit windows at night": "Erleuchtete Fenster bei Nacht",
  "Blocks per side": "Blöcke pro Seite", "Block pitch": "Blockraster", "Street width": "Straßenbreite", "Tallest building": "Höchstes Gebäude", "Trees": "Bäume",
  "Traffic cars": "Verkehr (Autos)", "Seed": "Seed", "Units": "Einheiten", "Pitch by speed": "Tonhöhe nach Tempo", "Engine loop": "Motorschleife",
  "Initial state": "Anfangszustand", "Walking network": "Laufnetz", "Actions to servo goals": "Aktionen zu Servo-Zielen",
  "Kern extras (ball play, sock fetching, beak in the pick)": "Kern-Extras (Ballspiel, Socke holen, Schnabel beim Greifen)",

  // details: choices
  "Box": "Box", "Sphere": "Kugel", "Cylinder": "Zylinder", "Cone": "Kegel", "Capsule": "Kapsel", "Plane": "Ebene", "Torus": "Torus", "Ramp": "Rampe",
  "Static": "Statisch", "Kinematic": "Kinematisch", "Dynamic (simulate physics)": "Dynamisch (Physik simulieren)", "Fit to mesh": "An Mesh anpassen", "Convex hull": "Konvexe Hülle",
  "Hinge (revolute)": "Scharnier (Drehgelenk)", "Slider (prismatic)": "Schiene (Schubgelenk)", "Ball (spherical)": "Kugelgelenk", "Fixed (weld)": "Fest (verschweißt)",
  "Distance / rope": "Abstand / Seil", "Wheel (suspension)": "Rad (Federung)", "Parallel": "Parallel", "No collision (filter)": "Keine Kollision (Filter)",
  "Hover car": "Hovercar", "Rolling ball": "Rollende Kugel", "Character": "Figur", "Flyer": "Flieger", "Vehicle": "Fahrzeug", "Soldier (first person shooter)": "Soldat (Ego-Shooter)",
  "Walker (robot on its trained network)": "Läufer (Roboter auf seinem trainierten Netz)", "Rifle": "Gewehr", "Pistol": "Pistole", "Shotgun": "Schrotflinte", "Sniper rifle": "Scharfschützengewehr",
  "Chase": "Verfolger", "First person": "Ego-Perspektive", "Top down": "Von oben", "Goal (win)": "Ziel (Sieg)", "Hazard (lose)": "Gefahr (Niederlage)",
  "Checkpoint (respawn here)": "Checkpoint (hier wieder einsteigen)", "Jump pad (launch)": "Sprungfeld (Start)", "Teleport": "Teleport", "Damage zone": "Schadenszone",
  "Logic event only": "Nur Logik-Ereignis", "Sine": "Sinus", "Ping-pong": "Hin und her", "Loop": "Schleife", "None (sandbox)": "Keine (Sandkasten)",
  "Collect all pickups": "Alle Pickups sammeln", "Reach a goal trigger": "Ein Ziel erreichen", "Survive the time limit": "Das Zeitlimit überleben", "Reach a target score": "Eine Punktzahl erreichen",
  "None": "Keine", "Linear": "Linear", "Inverse (natural)": "Invers (natürlich)", "Exponential": "Exponentiell", "Automatic": "Automatisch", "Nothing at all": "Gar nichts",
  "As trained (scale 1, no filter)": "Wie trainiert (Faktor 1, kein Filter)", "As the robot's daemon (scale 0.9, low-pass 0.5 / 0.7)": "Wie der Daemon des Roboters (Faktor 0,9, Tiefpass 0,5 / 0,7)",
  "Effects": "Effekte", "Voice": "Stimme",

  // outliner types
  "Procedural City": "Prozedurale Stadt", "Audio Source": "Audioquelle", "Constrained Mesh": "Verbundenes Mesh", "Physics Mesh": "Physik-Mesh", "Point Light": "Punktlicht",
  "Empty Actor": "Leerer Actor", "Actor": "Actor",

  // place actors: groups and descriptions
  "Basic": "Grundformen", "Shapes": "Formen", "Lights": "Lichter", "Players": "Spieler", "Physics Body": "Physikkörper", "Empty": "Leer",
  "Cube": "Würfel", "Physics Cube": "Physik-Würfel", "Physics Sphere": "Physik-Kugel", "Physics Barrel": "Physik-Fass", "Crate Stack": "Kistenstapel",
  "Hinged Door": "Scharniertür", "Moving Platform": "Bewegliche Plattform", "Spinner": "Drehbalken", "See-Saw": "Wippe", "Chain": "Kette", "Pendulum": "Pendel",
  "Jump Pad": "Sprungfeld", "Teleporter": "Teleporter", "Damage Zone": "Schadenszone", "Health Pack": "Medipack", "Ammo Box": "Munitionskiste",
  "Explosive Barrel": "Explosives Fass", "Target Dummy": "Zielpuppe", "Message Trigger": "Nachrichten-Trigger", "Goal": "Ziel", "Hazard": "Gefahr", "Checkpoint": "Checkpoint",
  "Hover Car": "Hovercar", "Rolling Ball": "Rollende Kugel", "Soldier": "Soldat", "Walker": "Läufer", "City Ambience": "Stadtatmosphäre", "Ambient Wind": "Wind",
  "Radio": "Radio", "Orb": "Orb", "Star": "Stern", "Ring": "Ring", "Wheel": "Rad", "Slider": "Schiene", "Hinge": "Scharnier", "Ball": "Kugel", "Cross": "Kreuz",
  "Dynamic Box3D box": "Dynamische Box3D-Box", "Dynamic ball": "Dynamische Kugel", "Dynamic cylinder (convex hull)": "Dynamischer Zylinder (konvexe Hülle)",
  "Pyramid of 10 crates": "Pyramide aus 10 Kisten", "Door on a sprung hinge with limits": "Tür an gefedertem Scharnier mit Anschlägen", "Kinematic platform on a mover": "Kinematische Plattform auf einem Beweger",
  "Rotating kinematic bar": "Rotierender kinematischer Balken", "Plank on a hinge": "Brett an einem Scharnier", "Ball on a distance joint": "Kugel an einem Abstandsgelenk",
  "Launches players up and forward": "Schleudert Spieler hoch und nach vorn", "Two linked pads: step on A, arrive at B": "Zwei verbundene Felder: auf A treten, bei B ankommen",
  "Hurts whoever walks in (25 per entry)": "Schadet jedem, der hineinläuft (25 pro Eintritt)", "Heals 50; stays while you are at full health; returns after 15 s": "Heilt 50; bleibt liegen, solange du volle Gesundheit hast; kommt nach 15 s zurück",
  "Blows up when shot: pushes and hurts what is near": "Explodiert bei Beschuss: stößt und verletzt, was in der Nähe ist", "Shootable target with health and a head; respawns": "Beschießbares Ziel mit Gesundheit und Kopf; kommt zurück",
  "Shows a text when touched": "Zeigt bei Berührung einen Text", "Trigger that wins the game": "Trigger, der das Spiel gewinnt", "Trigger that loses the game": "Trigger, der das Spiel verliert",
  "Glowing pickup worth 1 point": "Leuchtendes Pickup, 1 Punkt", "Spinning pickup worth 10 points": "Drehendes Pickup, 10 Punkte",
  "Coloured light with a radius": "Farbiges Licht mit Reichweite", "Looping city murmur with falloff": "Stadtrauschen in Schleife mit Abfall", "Looping wind, heard everywhere": "Wind in Schleife, überall hörbar",
  "Music box: plays the calm loop in 3D": "Spieluhr: spielt die ruhige Schleife in 3D", "Blocks, streets, trees and traffic": "Blöcke, Straßen, Bäume und Verkehr",
  "Fast Box3D hover car with boost": "Schnelles Box3D-Hovercar mit Boost", "Physics ball you roll and jump": "Physikkugel zum Rollen und Springen",
  "Third-person runner: walks, sprints, jumps, climbs steps": "Läufer aus der dritten Person: geht, sprintet, springt, steigt Stufen",
  "Flyer: Space / E up, Q / Ctrl down": "Flieger: Leertaste / E hoch, Q / Strg runter",
  "First-person shooter: rifle, health, respawn. Mouse, gamepad, touch; online ready": "Ego-Shooter: Gewehr, Gesundheit, Wiedereinstieg. Maus, Gamepad, Touch; online-fähig",
  "Floor, sun and sky - a clean start": "Boden, Sonne und Himmel – ein sauberer Anfang", "Procedural night city, Box3D hover car, 24 orbs to collect": "Prozedurale Nachtstadt, Box3D-Hovercar, 24 Orbs zum Sammeln",
  "Every Box3D joint: hinge, slider, wheel, rope bridge, motor, stacks": "Jedes Box3D-Gelenk: Scharnier, Schiene, Rad, Seilbrücke, Motor, Stapel",
  "Wedge to drive or roll up": "Keil zum Hochfahren oder -rollen", "Upright cone": "Aufrechter Kegel", "Upright cylinder": "Aufrechter Zylinder", "Capsule along Z": "Kapsel entlang Z",
  "Observer camera (Play: fly, watch, drag bodies)": "Beobachterkamera (Spielen: fliegen, zuschauen, Körper ziehen)",

  // game HUD and touch controls
  "collected": "gesammelt", "score": "Punkte", "time": "Zeit", "left": "übrig", "best": "Bestzeit", "Victory": "Gewonnen", "Game over": "Spiel vorbei",
  "Play again": "Nochmal spielen", "R to restart": "R startet neu", "Click to continue": "Klicken zum Fortfahren", "Musik": "Musik", "Effekte": "Effekte",
  "STEER / GAS": "LENKEN / GAS", "MOVE": "LAUFEN", "LOOK": "SCHAUEN", "FLY": "FLIEGEN", "JUMP": "SPRUNG", "BOOST": "BOOST", "DRIFT": "DRIFT", "LIFT": "LIFT",
  "VIEW": "SICHT", "HOP": "HÜPFEN", "USE": "NUTZEN", "UP": "HOCH", "DOWN": "RUNTER", "AIM": "ZIELEN", "RELOAD": "LADEN", "CROUCH": "DUCKEN",
  "QUACK": "QUAK", "PICK": "GREIFEN", "SIT": "SITZ", "KICK L": "KICK L", "KICK R": "KICK R", "ROLL": "ROLLEN", "BEAK": "SCHNABEL", "WHEEE": "HUIII",
};

// Phrases with a variable part (counts, names): [pattern, replacement].
const I18N_PATTERNS = [
  [/^(\d+) actors$/, "$1 Actors"], [/^(\d+) actors \((\d+) selected\)$/, "$1 Actors ($2 ausgewählt)"], [/^(\d+) of (\d+) selected$/, "$1 von $2 ausgewählt"],
  [/^Saved (.+)$/, "Gespeichert: $1"], [/^Opened (.+)$/, "Geöffnet: $1"], [/^Deleted (.+)$/, "Gelöscht: $1"], [/^Copied (.+)$/, "Kopiert: $1"],
  [/^Undo (.+)$/, "Rückgängig: $1"], [/^Redo (.+)$/, "Wiederholen: $1"], [/^New level: (.+)$/, "Neues Level: $1"], [/^Physics set to (.+)$/, "Physik: $1"],
  [/^Reopened "(.+)", where you worked last time$/, "„$1“ wieder geöffnet, wo du zuletzt warst"],
  [/^Kept the simulated placement of (.+)$/, "Simulierte Position übernommen: $1"], [/^Packaging (.+)$/, "Verpacke $1"],
  [/^Saving failed: (.+)$/, "Speichern fehlgeschlagen: $1"], [/^Opening failed: (.+)$/, "Öffnen fehlgeschlagen: $1"], [/^Packaging failed: (.+)$/, "Verpacken fehlgeschlagen: $1"],
  [/^Unknown console command: (.+)$/, "Unbekannter Konsolenbefehl: $1"], [/^Camera speed (\d+)$/, "Kamerageschwindigkeit $1"], [/^Grid snap (.+)$/, "Raster $1"],
  [/^Speed (.+)$/, "Tempo $1"],
  [/^(\d+) bodies · (\d+) awake · (\d+) joints( \((\d+) broken\))? · (\d+) contacts$/, "$1 Körper · $2 wach · $3 Gelenke · $6 Kontakte"],
  // Only when the named part is a known phrase ("Add Point Light"), never half-translated.
  [/^Add (.+)$/, "$1 hinzufügen", true], [/^Remove (.+)$/, "$1 entfernen", true],
  [/^(.+) - drag into a viewport or click$/, "$1 – in einen Viewport ziehen oder anklicken", true],
  [/^(.+) \(([^()]*)\)$/, "$1 ($2)", true],  // a field's tooltip with its unit: "Friction (N)"
];

const I18N_ATTRS = ["title", "placeholder", "aria-label"];
// What the user wrote or the engine prints stays as it is.
const I18N_SKIP = "input, textarea, select, #log, #outliner .label, #levelName, #console, #logicEditor, [data-i18n-skip], script, style, svg, code, pre";

/** The German phrase for `s` (or `s` itself): exact phrases, then the patterns. */
export function tr(s) {
  if (I18N_LANG !== "de" || !s) return s;
  const key = s.trim();
  if (!key) return s;
  let out = I18N_DE[key];
  if (out === undefined) {
    for (const [re, rep, known] of I18N_PATTERNS) {
      const m = key.match(re);
      if (!m) continue;
      if (known && I18N_DE[m[1]] === undefined) continue;
      // The variable part is translated too when it is a known phrase ("Add Point Light").
      out = rep.replace(/\$(\d)/g, (_, i) => I18N_DE[m[+i]] ?? m[+i]);
      break;
    }
  }
  // Shortcuts as German keyboards label them: Strg, Umschalt, Leertaste, Entf.
  if (out === undefined && /^(Ctrl|Shift|Alt)\+/.test(key))
    out = key.replace(/\bCtrl\b/g, "Strg").replace(/\bShift\b/g, "Umschalt").replace(/\bSpace\b/g, "Leertaste").replace(/\bDelete\b/g, "Entf");
  if (out === undefined || out === key) return s;
  return s.replace(key, out);
}

function i18nSkipped(el) {
  return !el || el.closest?.(I18N_SKIP) != null;
}

function i18nText(node) {
  const v = node.nodeValue;
  if (!v || !/[A-Za-z]/.test(v)) return;
  if (i18nSkipped(node.parentElement)) return;
  const out = tr(v);
  if (out !== v) node.nodeValue = out;
}

function i18nAttrs(el) {
  if (i18nSkipped(el) && !(el.matches?.("input, textarea") && el.closest?.("#log, #outliner .label, [data-i18n-skip]") == null)) return;
  for (const a of I18N_ATTRS) {
    const v = el.getAttribute?.(a);
    if (v) { const out = tr(v); if (out !== v) el.setAttribute(a, out); }
  }
}

function i18nWalk(root) {
  if (root.nodeType === Node.TEXT_NODE) { i18nText(root); return; }
  if (root.nodeType !== Node.ELEMENT_NODE) return;
  i18nAttrs(root);
  const it = document.createTreeWalker(root, NodeFilter.SHOW_ELEMENT | NodeFilter.SHOW_TEXT);
  for (let n = it.nextNode(); n; n = it.nextNode()) {
    if (n.nodeType === Node.TEXT_NODE) i18nText(n);
    else i18nAttrs(n);
  }
}

/** Sets the interface language ("de" or "en"), remembers it on this device and reloads the page. */
export function setLanguage(lang) {
  try { localStorage.setItem(I18N_KEY, lang); } catch { /* storage blocked */ }
  const u = new URL(location.href);
  u.searchParams.delete("lang");
  location.replace(u.href);
}

let i18nStarted = false;
/** Starts translating the page (German browsers only); safe to call more than once. */
export function localizePage() {
  if (I18N_LANG !== "de" || i18nStarted || typeof document === "undefined") return;
  i18nStarted = true;
  document.documentElement.lang = "de";
  const run = () => {
    i18nWalk(document.body);
    new MutationObserver(records => {
      for (const r of records) {
        if (r.type === "characterData") i18nText(r.target);
        else if (r.type === "attributes") i18nAttrs(r.target);
        else for (const n of r.addedNodes) i18nWalk(n);
      }
    }).observe(document.body, {subtree: true, childList: true, characterData: true, attributes: true, attributeFilter: I18N_ATTRS});
  };
  if (document.body) run(); else addEventListener("DOMContentLoaded", run, {once: true});
}


export const POINTER = {down: 0, move: 1, up: 2, cancel: 3, wheel: 4, leave: 5};

/** Thin wrapper over kern/src/studio/api.cpp. */
const AO_QUALITIES = ["low", "medium", "high", "ultra"];
/** PCF taps for the sun's shadow: phones 4 (3x3 kernel), desktop 9 (5x5). ?shadowFilter=1|4|9 overrides. */
function defaultShadowFilter() {
  const q = typeof location === "object" ? +new URLSearchParams(location.search).get("shadowFilter") : 0;
  if (q === 1 || q === 4 || q === 9) return q;
  return typeof matchMedia === "function" && matchMedia("(pointer: coarse)").matches ? 4 : 9;
}
/** Phones: medium at half resolution; desktop: high at full resolution. ?ao=off|low|medium|high|ultra and ?aoRes=half|full override. */
function defaultAmbientOcclusion() {
  const coarse = typeof matchMedia === "function" && matchMedia("(pointer: coarse)").matches;
  const o = coarse ? {quality: "medium", fullResolution: false} : {quality: "high", fullResolution: true};
  const q = typeof location === "object" ? new URLSearchParams(location.search) : null;
  const ao = q?.get("ao"), res = q?.get("aoRes");
  if (ao === "off") o.enabled = false;
  else if (AO_QUALITIES.includes(ao)) o.quality = ao;
  if (res === "half" || res === "full") o.fullResolution = res === "full";
  return o;
}


/** The networks Kern trained itself (kern/policies/, docs/lehrer.md); all others are Pollen's. */
const KERN_POLICIES = new Set(["jump.onnx", "play.onnx"]);

/** Networks Kern Lehrer saved on this device: IndexedDB "kern-lehrer", store "policies", key =
 *  file name ("jump.onnx"), value = the ONNX bytes. [[file, Uint8Array], ...]; `only` picks one. */
export function trainedPolicies(only) {
  return new Promise(resolve => {
    let open;
    try { open = indexedDB.open("kern-lehrer", 1); } catch { resolve([]); return; }
    open.onupgradeneeded = () => open.result.createObjectStore("policies");
    open.onerror = () => resolve([]);
    open.onsuccess = () => {
      const out = [];
      try {
        const cursor = open.result.transaction("policies").objectStore("policies").openCursor();
        cursor.onsuccess = () => {
          const c = cursor.result;
          if (!c) { resolve(out); return; }
          if (!only || c.key === only) out.push([String(c.key), new Uint8Array(c.value)]);
          c.continue();
        };
        cursor.onerror = () => resolve(out);
      } catch { resolve(out); }
    };
  });
}

export class StudioEngine {
  constructor(kern) {
    this.kern = kern;
    this.e = kern.exports;
  }
  withStrings(strings, fn) {
    const enc = this.kern.encoder, ptrs = [], args = [];
    try {
      for (const s of strings) {
        const bytes = enc.encode(s), p = bytes.length ? this.e.kern_alloc(bytes.length) : 0;
        if (bytes.length) this.kern.u8.set(bytes, p);
        ptrs.push(p);
        args.push(p, bytes.length);
      }
      return fn(...args);
    } finally {
      for (const p of ptrs) if (p) this.e.kern_free(p);
    }
  }
  text(ptr) { return ptr ? this.kern.string(ptr, this.e.kern_studio_result_length()) : null; }
  result(ptr) { const t = this.text(ptr); return t == null ? null : JSON.parse(t); }
  init(width, height, playerOnly) {
    if (this.e.kern_studio_init(width, height, playerOnly ? 1 : 0) !== 1) return false;
    this.setAmbientOcclusion(defaultAmbientOcclusion());
    this.e.kern_studio_shadow_filter?.(defaultShadowFilter());
    return true;
  }
  /** {enabled, quality: "low"|"medium"|"high"|"ultra", fullResolution, radius} */
  setAmbientOcclusion({enabled = true, quality = "medium", fullResolution = false, radius = 0} = {}) {
    const q = Math.max(0, AO_QUALITIES.indexOf(quality));
    this.e.kern_studio_ao_set?.(enabled ? 1 : 0, q, fullResolution ? 0 : 1, radius);
  }
  resize(width, height) { this.e.kern_studio_resize(width, height); }
  frame(time) { this.e.kern_studio_frame(time); }
  revision() { return this.e.kern_studio_revision(); }
  command(obj) { return this.withStrings([JSON.stringify(obj)], (p, n) => this.result(this.e.kern_studio_command(p, n))); }
  state() { return this.result(this.e.kern_studio_state()); }
  status() { return this.result(this.e.kern_studio_status()); }
  takeUi() { return this.result(this.e.kern_studio_take_ui()) || []; }
  key(key, code, down, mods, repeat) {
    return this.withStrings([key, code], (kp, kn, cp, cn) => this.result(this.e.kern_studio_key(kp, kn, cp, cn, down ? 1 : 0, mods, repeat ? 1 : 0)));
  }
  pointer(viewport, type, x, y, {button = 0, buttons = 0, mods = 0, wheel = 0} = {}) {
    return this.result(this.e.kern_studio_pointer(viewport, type, x, y, button, buttons, mods, wheel));
  }
  look(dx, dy) { this.e.kern_studio_look(dx, dy); }
  /** A touch / pen finger in css px of the active viewport; true when the engine's touch controls took it. */
  touch(type, id, x, y, time) { return this.e.kern_studio_touch(type, id, x, y, time) === 1; }
  blur() { this.e.kern_studio_blur(); }
  setViewport(i, x, y, w, h, cssW, cssH, dpr, visible) { this.e.kern_studio_set_viewport(i, x, y, w, h, cssW, cssH, dpr, visible ? 1 : 0); }
  takeAutosave(now) { return this.text(this.e.kern_studio_take_autosave(now)); }
  takeAutosaveDiscard() { return this.e.kern_studio_take_autosave_discard() === 1; }
  dirty() { return this.e.kern_studio_dirty() === 1; }
  textureSource(id) { return this.withStrings([id], (p, n) => this.text(this.e.kern_studio_texture_source(p, n))); }
  texturePixels(id, width, height, rgba) {
    const ptr = this.e.kern_alloc(Math.max(4, rgba.length));
    try {
      this.kern.u8.set(rgba, ptr);
      return this.withStrings([id], (p, n) => this.e.kern_studio_texture_pixels(p, n, width, height, ptr));
    } finally {
      this.e.kern_free(ptr);
    }
  }
  /** Loads the robot's policy set (microduck/<file>.onnx next to the page; the dev page reads the
   *  repository's copy, so does the repository's single-file build in generated/) and Kern's own
   *  trained networks (policies/<file>.onnx: jump.onnx, play.onnx, docs/lehrer.md). A file that fails is fetched again later; until then the move it
   *  drives is unavailable (the duck stands and says why). */
  async loadPolicies(bases = /\/kern\/web\/studio\//.test(location.pathname) ? ["../../third_party/microduck/", "../../policies/"] : /\/generated\/[^/]*$/.test(location.pathname) ? ["../kern/third_party/microduck/", "../kern/policies/"] : ["microduck/", "policies/"]) {
    if (!this.trainedWatched) { this.trainedWatched = true; this.watchTrainedPolicies(); }
    if (this.policiesLoading) return this.policiesLoading;
    // A file that failed is asked for again later (5 s, 10 s, 20 s ... at most every minute).
    const retry = this.policyRetry ??= new Map(), now = Date.now();
    const files = (this.result(this.e.kern_studio_pending_policies()) || []).filter(f => !retry.has(f) || retry.get(f).at <= now);
    if (!files.length) return;
    for (const f of files) retry.set(f, {at: Infinity, n: retry.get(f)?.n ?? 0});
    this.policiesLoading = Promise.all(files.map(async file => {
      // Kern's own networks live in policies/, Pollen's in microduck/ (no request that must fail).
      for (const base of bases.filter(b => /policies\/$/.test(b) === KERN_POLICIES.has(file))) {
        try {
          const r = await fetch(base + file);
          if (!r.ok) continue;
          const bytes = new Uint8Array(await r.arrayBuffer());
          const ptr = this.e.kern_alloc(Math.max(4, bytes.length));
          try {
            this.kern.u8.set(bytes, ptr);
            if (this.withStrings([file], (p, n) => this.e.kern_studio_load_policy(p, n, ptr, bytes.length)) === 1) return true;
          } finally {
            this.e.kern_free(ptr);
          }
        } catch { /* next base */ }
      }
      const n = retry.get(file).n + 1;
      retry.set(file, {at: Date.now() + Math.min(60000, 5000 * 2 ** (n - 1)), n});
      console.warn("Kern: robot policy " + file + " not loaded, trying again in " + Math.round(Math.min(60000, 5000 * 2 ** (n - 1)) / 1000) + " s");
      return false;
    })).finally(() => { this.policiesLoading = null; });
    return this.policiesLoading;
  }
  /** Hands one network (ONNX bytes) to the engine under `file`; true when it took it. */
  registerPolicy(file, bytes) {
    const ptr = this.e.kern_alloc(Math.max(4, bytes.length));
    try {
      this.kern.u8.set(bytes, ptr);
      return this.withStrings([file], (p, n) => this.e.kern_studio_load_policy(p, n, ptr, bytes.length)) === 1;
    } finally {
      this.e.kern_free(ptr);
    }
  }
  /** Networks trained in Kern Lehrer on this device (same site): loaded now, and again whenever
   *  the Lehrer saves a newer one (docs/lehrer.md). */
  watchTrainedPolicies() {
    const load = async only => {
      for (const [file, bytes] of await trainedPolicies(only))
        if (this.registerPolicy(file, bytes)) console.info("Kern: trained network " + file + " loaded");
    };
    load().catch(() => {});
    try { new BroadcastChannel("kern-lehrer").onmessage = e => load(e.data?.file).catch(() => {}); } catch { /* no channel */ }
  }
  /** Decodes the level's texture images (data URLs) into RGBA for the engine. */
  pollTextures(maxSide = 2048) {
    this.decoding ??= new Set();
    const ids = this.result(this.e.kern_studio_pending_textures()) || [];
    for (const id of ids) {
      if (this.decoding.has(id)) continue;
      this.decoding.add(id);
      const img = new Image();
      img.onload = () => {
        this.decoding.delete(id);
        const scale = Math.min(1, maxSide / Math.max(img.naturalWidth, img.naturalHeight));
        const c = document.createElement("canvas");
        c.width = Math.max(1, Math.round(img.naturalWidth * scale));
        c.height = Math.max(1, Math.round(img.naturalHeight * scale));
        const ctx = c.getContext("2d", {willReadFrequently: true});
        ctx.drawImage(img, 0, 0, c.width, c.height);
        this.texturePixels(id, c.width, c.height, ctx.getImageData(0, 0, c.width, c.height).data);
      };
      img.onerror = () => { this.decoding.delete(id); this.texturePixels(id, 0, 0, new Uint8Array(4)); };
      img.src = this.textureSource(id);
    }
  }
  // ---- audio ----
  audioInit(rate) { return this.e.kern_studio_audio_init(rate) === 1; }
  /** Renders queued built-in sounds for about `ms`; true when none is left. */
  audioPrepare(ms) { return this.e.kern_studio_audio_prepare(ms) === 1; }
  /** Interleaved stereo PCM of the next `frames` frames (a copy). */
  audioRender(frames) {
    const ptr = this.e.kern_studio_audio_render(frames);
    if (!ptr) return new Float32Array(frames * 2);
    return new Float32Array(this.kern.memory.buffer, ptr, frames * 2).slice();
  }
  soundSource(id) { return this.withStrings([id], (p, n) => this.text(this.e.kern_studio_sound_source(p, n))); }
  soundPcm(id, pcm, frames, channels, rate) {
    const ptr = this.e.kern_alloc(Math.max(4, pcm.length * 4));
    try {
      new Float32Array(this.kern.memory.buffer, ptr, pcm.length).set(pcm);
      return this.withStrings([id], (p, n) => this.e.kern_studio_sound_pcm(p, n, ptr, frames, channels, rate)) === 1;
    } finally {
      this.e.kern_free(ptr);
    }
  }
  soundFailed(id, reason) { this.withStrings([id, reason || ""], (p, n, rp, rn) => this.e.kern_studio_sound_failed(p, n, rp, rn)); }
  /** Decodes the level's imported sounds (data URLs) with Web Audio for the engine's mixer. */
  pollSounds(audio) {
    if (!audio?.context) return;
    this.soundDecoding ??= new Set();
    const ids = this.result(this.e.kern_studio_pending_sounds()) || [];
    for (const id of ids) {
      if (this.soundDecoding.has(id)) continue;
      this.soundDecoding.add(id);
      const src = this.soundSource(id) || "";
      audio.decode(src).then(d => {
        this.soundDecoding.delete(id);
        if (!this.soundPcm(id, d.pcm, d.frames, d.channels, d.rate)) this.soundFailed(id, "the decoded audio was rejected");
      }, error => {
        this.soundDecoding.delete(id);
        this.soundFailed(id, error?.message || "the browser could not decode it");
      });
    }
  }
  // ---- network ----
  netPush(from, bytes) {
    const ptr = this.e.kern_alloc(Math.max(4, bytes.length));
    try {
      this.kern.u8.set(bytes, ptr);
      this.e.kern_studio_net_push(from, ptr, bytes.length);
    } finally {
      this.e.kern_free(ptr);
    }
  }
  /** The next datagram the engine wants sent ({to, bytes}) or null. */
  netTake() {
    const n = this.e.kern_studio_net_take();
    if (!n) return null;
    const bytes = new Uint8Array(this.kern.memory.buffer, this.e.kern_studio_net_out(), n).slice();
    return {to: this.e.kern_studio_net_out_to(), bytes};
  }
  contextLost() { this.e.kern_studio_context_lost(); }
  contextRestored() { return this.e.kern_studio_context_restored() === 1; }
}

// ---------------------------------------------------------------------------
// Audio output: the engine's C++ mixer renders PCM, this moves it to the speakers.

/** AudioWorklet processor: plays queued chunks, reports what it consumed (frames). When the queue runs
 *  dry (the main thread stalled) it never jumps to silence: the last sample glides to zero, and when
 *  data comes back it fades in - a gap without a click. */
const WORKLET_SOURCE = `
const RAMP = 96;  // samples (2 ms): a glide no one hears as a click
class KernOutput extends AudioWorkletProcessor {
  constructor() {
    super();
    this.queue = [];
    this.offset = 0;
    this.consumed = 0;
    this.underruns = 0;
    this.blocks = 0;
    this.lastL = 0;
    this.lastR = 0;
    this.gain = 1;     // fade-in after a gap
    this.dry = false;
    this.port.onmessage = e => { if (e.data === "flush") { this.queue.length = 0; this.offset = 0; } else this.queue.push(e.data); };
  }
  process(inputs, outputs) {
    const out = outputs[0], l = out[0], r = out[1] || out[0], n = l.length;
    let i = 0;
    while (i < n && this.queue.length) {
      const chunk = this.queue[0], frames = chunk.length >> 1;
      const take = Math.min(n - i, frames - this.offset);
      for (let k = 0; k < take; k++) {
        const g = this.gain;
        if (g < 1) this.gain = Math.min(1, g + 1 / RAMP);
        l[i + k] = chunk[(this.offset + k) * 2] * g;
        r[i + k] = chunk[(this.offset + k) * 2 + 1] * g;
      }
      i += take;
      this.offset += take;
      if (this.offset >= frames) { this.queue.shift(); this.offset = 0; }
    }
    if (i > 0) { this.lastL = l[i - 1]; this.lastR = r[i - 1]; this.dry = false; }
    if (i < n) {
      if (!this.dry) { this.underruns++; this.dry = true; }
      for (; i < n; i++) {
        this.lastL *= 1 - 1 / RAMP * 4;
        this.lastR *= 1 - 1 / RAMP * 4;
        if (Math.abs(this.lastL) < 1e-5) this.lastL = 0;
        if (Math.abs(this.lastR) < 1e-5) this.lastR = 0;
        l[i] = this.lastL;
        r[i] = this.lastR;
      }
      this.gain = 0;  // what comes next fades in
    }
    this.consumed += n;
    if (++this.blocks % 4 === 0) this.port.postMessage([this.consumed, this.underruns]);
    return true;
  }
}
registerProcessor("kern-output", KernOutput);
`;

/**
 * Speaker output for the engine mixer. Works everywhere Web Audio does: an
 * AudioWorklet where available (Blob or data: module), a ScriptProcessor
 * otherwise. Browsers start audio only after a user gesture, so the first
 * pointer / key / touch event resumes the context (iOS included; the audio
 * session is set to "playback" so the ring switch does not mute games).
 */
export class KernAudio {
  constructor(render, {latency = 0.07, maxLatency = 0.2} = {}) {
    this.render = render;        // frames -> Float32Array (interleaved stereo)
    this.latency = latency;      // seconds kept queued ahead of the device (grows after a gap)
    this.baseLatency = latency;
    this.maxLatency = maxLatency;
    this.lastUnderruns = 0;
    this.calmSince = 0;
    this.context = null;
    this.node = null;
    this.mode = "none";
    this.written = 0;
    this.consumed = 0;
    this.underruns = 0;
    this.started = false;
  }
  static supported() { return typeof (globalThis.AudioContext || globalThis.webkitAudioContext) === "function"; }
  get rate() { return this.context?.sampleRate || 0; }
  get running() { return this.context?.state === "running"; }
  /** Creates the context (suspended until a gesture) and returns its sample rate. */
  create() {
    if (this.context) return this.rate;
    const Ctx = globalThis.AudioContext || globalThis.webkitAudioContext;
    if (!Ctx) return 0;
    try { if (navigator.audioSession) navigator.audioSession.type = "playback"; } catch { /* not supported */ }
    try { this.context = new Ctx({latencyHint: "interactive"}); } catch { try { this.context = new Ctx(); } catch { return 0; } }
    const unlock = () => {
      if (!this.context) return;
      if (this.context.state !== "running") this.context.resume().catch(() => {});
      if (!this.node) this.connect();
    };
    for (const type of ["pointerdown", "keydown", "touchend", "mousedown"]) addEventListener(type, unlock, {capture: true, passive: true});
    this.unlock = unlock;
    document.addEventListener("visibilitychange", () => { if (!document.hidden && this.context?.state === "interrupted") this.context.resume().catch(() => {}); });
    return this.rate;
  }
  async connect() {
    if (this.node || this.connecting || !this.context) return;
    this.connecting = true;
    const ctx = this.context;
    if (ctx.audioWorklet && globalThis.AudioWorkletNode) {
      const urls = [];
      try { urls.push(URL.createObjectURL(new Blob([WORKLET_SOURCE], {type: "text/javascript"}))); } catch { /* no blobs */ }
      urls.push("data:text/javascript;base64," + btoa(WORKLET_SOURCE));
      for (const url of urls) {
        try {
          await ctx.audioWorklet.addModule(url);
          const node = new AudioWorkletNode(ctx, "kern-output", {numberOfInputs: 0, numberOfOutputs: 1, outputChannelCount: [2]});
          node.port.onmessage = e => { this.consumed = e.data[0]; this.underruns = e.data[1]; };
          node.connect(ctx.destination);
          this.node = node;
          this.mode = "worklet";
          break;
        } catch { /* blocked by a content security policy: try the next way */ }
      }
    }
    if (!this.node && ctx.createScriptProcessor) {
      const node = ctx.createScriptProcessor(1024, 0, 2);
      node.onaudioprocess = e => {
        const l = e.outputBuffer.getChannelData(0), r = e.outputBuffer.getChannelData(1), n = l.length;
        const pcm = this.render(n);
        for (let i = 0; i < n; i++) { l[i] = pcm[i * 2]; r[i] = pcm[i * 2 + 1]; }
      };
      node.connect(ctx.destination);
      this.node = node;
      this.mode = "script";
    }
    this.connecting = false;
    this.started = !!this.node;
  }
  /** Keeps the worklet's queue `latency` seconds ahead; call every frame (and from a timer). */
  pump() {
    if (this.mode !== "worklet" || !this.running) return;
    // Adaptive lead: a gap (the main thread stalled longer than the queue) adds 30 ms, up to
    // maxLatency; after 15 s without one it eases back towards the base, 5 ms at a time.
    const now = this.context.currentTime;
    if (this.underruns > this.lastUnderruns) {
      this.latency = Math.min(this.maxLatency, this.latency + 0.03);
      this.lastUnderruns = this.underruns;
      this.calmSince = now;
    } else if (this.latency > this.baseLatency && now - this.calmSince > 15) {
      this.latency = Math.max(this.baseLatency, this.latency - 0.005);
      this.calmSince = now;
    }
    const rate = this.rate, target = Math.ceil(this.latency * rate / 128) * 128;
    let queued = this.written - this.consumed;
    if (queued < -rate) { this.written = this.consumed; queued = 0; }  // after a long stall: start over
    let guard = 0;
    while (queued < target && guard++ < 16) {
      const frames = Math.min(2048, Math.max(128, target - queued));
      const pcm = this.render(frames);
      this.node.port.postMessage(pcm, [pcm.buffer]);
      this.written += frames;
      queued += frames;
    }
  }
  /** Decodes a data: URL (any format the browser plays) to interleaved PCM (1 or 2 channels). */
  async decode(src) {
    const comma = src.indexOf(",");
    if (!src.startsWith("data:") || comma < 0) throw Error("not a data URL");
    const meta = src.slice(5, comma), body = src.slice(comma + 1);
    let bytes;
    if (/;base64$/i.test(meta)) {
      const bin = atob(body);
      bytes = new Uint8Array(bin.length);
      for (let i = 0; i < bin.length; i++) bytes[i] = bin.charCodeAt(i);
    } else bytes = new TextEncoder().encode(decodeURIComponent(body));
    const buffer = await new Promise((resolve, reject) => {
      const p = this.context.decodeAudioData(bytes.buffer, resolve, reject);
      if (p?.then) p.then(resolve, reject);
    });
    const channels = Math.min(2, buffer.numberOfChannels), frames = buffer.length;
    const pcm = new Float32Array(frames * channels);
    if (buffer.numberOfChannels > 2) {
      // Down-mix surround to stereo (even channels left, odd right).
      const l = new Float32Array(frames), r = new Float32Array(frames);
      for (let c = 0; c < buffer.numberOfChannels; c++) {
        const d = buffer.getChannelData(c), t = c % 2 ? r : l;
        for (let i = 0; i < frames; i++) t[i] += d[i] / Math.ceil(buffer.numberOfChannels / 2);
      }
      for (let i = 0; i < frames; i++) { pcm[i * 2] = l[i]; pcm[i * 2 + 1] = r[i]; }
    } else {
      for (let c = 0; c < channels; c++) {
        const d = buffer.getChannelData(c);
        for (let i = 0; i < frames; i++) pcm[i * channels + c] = d[i];
      }
    }
    return {pcm, frames, channels, rate: buffer.sampleRate, duration: buffer.duration};
  }
}

/** Connects the engine's mixer to the speakers; returns the KernAudio (or null without Web Audio). */
export function startAudio(engine) {
  if (!KernAudio.supported()) return null;
  const audio = new KernAudio(frames => engine.audioRender(frames));
  const rate = audio.create();
  if (!rate || !engine.audioInit(rate)) return null;
  // Timer pump: keeps sound flowing when frames are slow (and while the tab is in the background),
  // and renders the level's built-in sounds between frames.
  setInterval(() => { audio.pump(); engine.audioPrepare(5); }, 20);
  return audio;
}

// ---------------------------------------------------------------------------
// Network transport: moves Kern Net datagrams (kern/src/net) between browsers.
// Kern's C++ stack does acks, resends, channels, the handshake and timeouts;
// this only delivers bytes, like UDP would.
//
//   "local"  BroadcastChannel: tabs of this browser on this machine, no server
//   "relay"  WebSocket to tools/net_relay.mjs: players on different machines

const randomPeerId = () => {
  const a = new Uint32Array(2);
  crypto.getRandomValues(a);
  return (a[0] & 0x1fffff) * 4294967296 + a[1] || 1;  // 53-bit, never 0
};

/** Host-side datagram transport for one engine. */
export class KernNet {
  constructor(engine) {
    this.engine = engine;
    this.kind = null;
    this.room = "";
    this.self = 0;
    this.host = 0;
    this.channel = null;
    this.socket = null;
    this.timer = 0;
    this.onstatus = null;
    this.sent = 0;
    this.received = 0;
  }
  get active() { return !!this.kind; }

  /** Opens the transport; resolves with {self, host} (host 0 = this peer hosts). */
  async open({kind = "local", room = "kern", hosting = false, url = ""} = {}) {
    this.close();
    this.kind = kind;
    this.room = room;
    if (kind === "local") {
      this.self = randomPeerId();
      this.channel = new BroadcastChannel("kern-net-" + room);
      this.channel.onmessage = e => this.onLocal(e.data);
      if (hosting) {
        this.host = this.self;
        this.announce = setInterval(() => this.channel?.postMessage({hello: this.self}), 500);
        this.channel.postMessage({hello: this.self});
      } else {
        this.host = await new Promise((resolve, reject) => {
          const done = id => { clearTimeout(t); this.channel.removeEventListener("message", listen); resolve(id); };
          const listen = e => { if (e.data?.hello) done(e.data.hello); };
          const t = setTimeout(() => { this.channel?.removeEventListener("message", listen); reject(Error(`no game is hosted in room "${room}" in this browser`)); }, 4000);
          this.channel.addEventListener("message", listen);
          this.channel.postMessage({who: this.self});
        });
      }
    } else {
      const ws = new WebSocket(url);
      ws.binaryType = "arraybuffer";
      this.socket = ws;
      const hello = await new Promise((resolve, reject) => {
        ws.onerror = () => reject(Error("the relay " + url + " is not reachable"));
        ws.onopen = () => ws.send(JSON.stringify(hosting ? {host: room} : {join: room}));
        ws.onmessage = e => { if (typeof e.data === "string") resolve(JSON.parse(e.data)); };
      });
      if (!hosting && !hello.host) throw Error(`nobody hosts room "${room}" on the relay`);
      this.self = hello.id;
      this.host = hello.host;
      ws.onmessage = e => {
        if (typeof e.data === "string" || e.data.byteLength < 9) return;
        const view = new DataView(e.data);
        const from = view.getUint32(0, true) + view.getUint32(4, true) * 4294967296;
        this.deliver(from, new Uint8Array(e.data, 8));
      };
      ws.onclose = () => this.onstatus?.("The relay closed the connection");
    }
    this.timer = setInterval(() => this.flush(), 8);
    return {self: this.self, host: hosting ? 0 : this.host};
  }

  onLocal(m) {
    if (!m) return;
    if (m.who && this.host === this.self) this.channel.postMessage({hello: this.self});
    if (m.to === this.self && m.data) this.deliver(m.from, new Uint8Array(m.data));
  }

  deliver(from, bytes) {
    this.received++;
    this.engine.netPush(from, bytes);
  }

  /** Sends what the engine queued (called from a timer and every frame). */
  flush() {
    if (!this.kind) return;
    for (let guard = 0; guard < 256; guard++) {
      const d = this.engine.netTake();
      if (!d) break;
      this.sent++;
      if (this.channel) this.channel.postMessage({from: this.self, to: d.to, data: d.bytes.buffer}, [d.bytes.buffer]);
      else if (this.socket?.readyState === 1) {
        const out = new Uint8Array(8 + d.bytes.length);
        const view = new DataView(out.buffer);
        view.setUint32(0, d.to % 4294967296, true);
        view.setUint32(4, Math.floor(d.to / 4294967296), true);
        out.set(d.bytes, 8);
        this.socket.send(out);
      }
    }
  }

  close() {
    clearInterval(this.timer);
    clearInterval(this.announce);
    this.channel?.close();
    this.socket?.close();
    this.channel = this.socket = null;
    this.kind = null;
    this.host = this.self = 0;
  }
}

/** Opens the network for an engine and hosts or joins (shared by the editor and packaged games). */
export async function openNetwork(engine, net, {host, room = "kern", relay = ""}) {
  const r = await net.open({kind: relay ? "relay" : "local", room, hosting: host, url: relay});
  engine.command({cmd: "netAddress", self: r.self});
  return host ? engine.command({cmd: "netHost"}) : engine.command({cmd: "netJoin", server: r.host});
}

export const modifiers = e => (e.shiftKey ? 1 : 0) | (e.ctrlKey || e.metaKey ? 2 : 0) | (e.altKey ? 4 : 0);

export const toBase64 = bytes => {
  let s = "";
  for (let i = 0; i < bytes.length; i += 0x8000) s += String.fromCharCode.apply(null, bytes.subarray(i, i + 0x8000));
  return btoa(s);
};

export const escHtml = s => String(s ?? "").replace(/[&<>"']/g, c => ({"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"}[c]));

/**
 * Gamepad (Xbox layout, browser "standard" mapping): sticks, triggers and buttons
 * go to the engine every frame while a pad is connected. Kern applies dead zones,
 * response curves and the soldier's aim rates (SpawnCity mapping):
 * left stick move, right stick look, RT fire, LT aim, A jump, B crouch,
 * X reload, Y interact, L3 / LB sprint, Back / View the vehicle camera, Start pause.
 */
export class GamepadInput {
  constructor(engine) { this.engine = engine; this.connected = false; this.last = ""; }
  poll() {
    const pads = navigator.getGamepads ? Array.from(navigator.getGamepads()).filter(p => p && p.connected) : [];
    const p = pads.find(g => g.mapping === "standard") || pads[0];
    if (!p) {
      if (this.connected) { this.connected = false; this.last = ""; this.engine.command({cmd: "gamepad", connected: false}); }
      return null;
    }
    const b = i => !!p.buttons[i]?.pressed, v = i => p.buttons[i]?.value ?? 0, ax = i => p.axes[i] ?? 0;
    const s = {cmd: "gamepad", connected: true, lx: ax(0), ly: -ax(1), rx: ax(2), ry: ax(3), lt: v(6), rt: v(7),
      a: b(0), b: b(1), x: b(2), y: b(3), lb: b(4), rb: b(5), l3: b(10), start: b(9), view: b(8)};
    // Any pressed button or a stick past .22 marks the pad as the device in use (SpawnCity pad-input).
    s.active = p.buttons.slice(0, 17).some(x => x?.pressed) || p.axes.slice(0, 4).some(a => Math.abs(a) > 0.22);
    const key = JSON.stringify(s);
    // Sticks at rest and nothing pressed: send once, then stay quiet.
    if (key !== this.last || Math.abs(s.rx) > 0.05 || Math.abs(s.ry) > 0.05) { this.last = key; this.engine.command(s); }
    this.connected = true;
    return p;
  }
}

/** Styles of the engine-drawn touch controls (see TouchOverlay). */
export const TOUCH_STYLE = `.k-touch{position:absolute;inset:0;pointer-events:none;z-index:6;font:700 10px Inter,system-ui,sans-serif;letter-spacing:.05em;text-transform:uppercase;color:#f2f4f7;user-select:none;-webkit-user-select:none}
.k-touch .ts{position:absolute;border-radius:50%;border:1.5px solid rgba(255,255,255,.2);box-sizing:border-box}
.k-touch .ts.live{border-color:rgba(255,255,255,.5)}.k-touch .ts.accent{border-color:#f5b301}
.k-touch .ts i{position:absolute;width:31%;height:31%;border-radius:50%;background:rgba(255,255,255,.16);border:1.5px solid rgba(255,255,255,.42);transform:translate(-50%,-50%);box-sizing:border-box}
.k-touch .ts.live i{background:rgba(255,255,255,.34);border-color:rgba(255,255,255,.75)}
.k-touch .ts span{position:absolute;left:0;right:0;top:-15px;text-align:center;opacity:.5;font-size:9px;white-space:nowrap}
.k-touch .tb{position:absolute;display:grid;place-items:center;border:2px solid rgba(255,255,255,.7);box-sizing:border-box;border-radius:50%;font-size:10px;font-weight:900;text-shadow:0 1px 2px #000;transition:transform .06s}
.k-touch .tb.sq{border-radius:10px}.k-touch .tb.pressed{transform:scale(.94);border-color:#fff;background:#ff7a1acc!important}
.k-touch .tb.on{border-color:#f5b301;color:#f5b301}
.k-touch-on .k-hud-top{top:58px}
.k-touch-on .k-hp,.k-touch-on .k-ammo,.k-touch-on .k-hud-speed{left:auto;right:max(14px,env(safe-area-inset-right));bottom:auto}
.k-touch-on .k-ammo,.k-touch-on .k-hud-speed{top:58px}.k-touch-on .k-hp{top:114px}.k-touch-on .k-ammo b{font-size:30px}
.k-touch .tf{position:absolute;width:46px;height:46px;margin:-23px 0 0 -23px;border-radius:50%;border:2px solid rgba(255,138,106,.8);box-shadow:0 0 12px rgba(255,90,60,.5)}`;

/**
 * Draws the touch controls the engine lays out and drives (status.touch, see
 * kern/game/touch.h): sticks, buttons and the firing finger. Purely visual; the
 * fingers go to the engine (StudioEngine.touch).
 */
export class TouchOverlay {
  constructor(parent) {
    this.el = document.createElement("div");
    this.el.className = "k-touch";
    this.el.hidden = true;
    this.key = "";
    parent.appendChild(this.el);
  }
  attach(parent) {
    if (this.el.parentNode === parent) return;
    this.el.parentNode?.classList.remove("k-touch-on");
    parent.appendChild(this.el);
  }
  update(t) {
    // The parent knows the corners belong to the thumbs (the HUD moves up, see TOUCH_STYLE).
    this.el.parentNode?.classList.toggle("k-touch-on", !!t);
    if (!t) { if (!this.el.hidden) { this.el.hidden = true; this.key = ""; } return; }
    this.el.hidden = false;
    const key = JSON.stringify(t);
    if (key === this.key) return;
    this.key = key;
    const px = v => `${Math.round(v)}px`;
    const stick = (s, id) => `<div class="ts${s.live ? " live" : ""}${s.accent ? " accent" : ""}" data-stick="${id}" style="left:${px(s.x)};top:${px(s.y)};width:${px(s.size)};height:${px(s.size)}">` +
      `<span>${escHtml(s.label)}</span><i style="left:${50 + 31 * s.kx}%;top:${50 + 31 * s.ky}%"></i></div>`;
    let html = stick(t.move, "move") + stick(t.look, "look");
    for (const b of t.buttons)
      html += `<div class="tb${b.round ? "" : " sq"}${b.pressed ? " pressed" : ""}${b.on ? " on" : ""}" data-touch="${escHtml(b.id)}" style="left:${px(b.x)};top:${px(b.y)};width:${px(b.w)};height:${px(b.h)};background:${b.color}b8">${escHtml(b.label)}</div>`;
    if (t.fire) html += `<div class="tf" style="left:${px(t.fireX)};top:${px(t.fireY)}"></div>`;
    this.el.innerHTML = html;
  }
}

/**
 * Tells the engine what device this is and which one the player uses right now
 * (SpawnCity's rules, decided in C++: touch switches to the touch layout, a mouse
 * only on fine-pointer devices, any key to desktop), plus the safe-area insets.
 */
export function watchInputDevice(engine) {
  const mq = q => matchMedia(q).matches;
  const probe = document.createElement("div");
  probe.style.cssText = "position:fixed;left:0;top:0;visibility:hidden;pointer-events:none;padding:env(safe-area-inset-top,0px) env(safe-area-inset-right,0px) env(safe-area-inset-bottom,0px) env(safe-area-inset-left,0px)";
  document.body.appendChild(probe);
  const safe = () => { const c = getComputedStyle(probe); return [c.paddingLeft, c.paddingTop, c.paddingRight, c.paddingBottom].map(v => parseFloat(v) || 0); };
  const platform = () => engine.command({cmd: "device", safe: safe(),
    platform: {fine: mq("(hover: hover) and (pointer: fine)"), coarse: mq("(pointer: coarse)"), touchPoints: navigator.maxTouchPoints || 0}});
  platform();
  addEventListener("resize", platform);
  addEventListener("orientationchange", platform);
  let touch = mq("(pointer: coarse)");
  addEventListener("pointerdown", e => {
    touch = e.pointerType !== "mouse";
    engine.command({cmd: "device", event: touch ? "touch" : "mouse"});
  }, true);
  addEventListener("pointermove", e => {
    if (touch && e.pointerType === "mouse" && !document.pointerLockElement && (e.movementX || e.movementY)) {
      engine.command({cmd: "device", event: "mousemove"});
      touch = engine.status()?.input?.device === "touch";
    }
  }, true);
  addEventListener("keydown", () => { touch = false; }, true);
}

/**
 * Routes a viewport's touch / pen pointers to the engine's touch controls.
 * `pos(e)` gives css px in the viewport; returns true when the event was used.
 */
export function routeTouch(engine, type, e, pos) {
  if (e.pointerType === "mouse") return false;
  if (type === POINTER.move) {
    let used = false;
    for (const c of e.getCoalescedEvents?.() || [e]) { const [x, y] = pos(c); used = engine.touch(type, c.pointerId, x, y, c.timeStamp) || used; }
    return used;
  }
  const [x, y] = pos(e);
  return engine.touch(type, e.pointerId, x, y, e.timeStamp);
}

/**
 * Keeps a networked game running while its tab is in the background: browsers
 * pause requestAnimationFrame there (and throttle timers), which would freeze
 * a host and time out its players. A worker's timer keeps ticking; `tick` runs
 * only while the page is hidden.
 */
export function backgroundTicker(tick, ms = 16) {
  let worker = null;
  try {
    const src = `setInterval(() => postMessage(0), ${ms});`;
    worker = new Worker(URL.createObjectURL(new Blob([src], {type: "text/javascript"})));
    worker.onmessage = () => { if (document.hidden) tick(performance.now()); };
  } catch { /* workers unavailable: background tabs pause */ }
  return worker;
}

/** "83.4" -> "1:23.4" */
export function formatTime(seconds) {
  if (!(seconds >= 0)) return "-";
  const m = Math.floor(seconds / 60), s = seconds - m * 60;
  return m ? `${m}:${s.toFixed(1).padStart(4, "0")}` : `${s.toFixed(1)}`;
}

/**
 * HUD markup shared by the editor's Play mode and packaged games:
 * score, pickups, timer, speed and boost, the latest message, and the
 * win / lose banner.
 */
// Music and effects on / off: the two switches at the top right of every game, remembered on this device.
const AUDIO_KEY = "kern.audio";
export const audioPrefs = (() => {
  try { const v = JSON.parse(localStorage.getItem(AUDIO_KEY) || "{}"); return {music: v.music !== false, effects: v.effects !== false}; }
  catch { return {music: true, effects: true}; }
})();
/** Sends the remembered music / effects switches to the engine (call once it runs). */
export function applyAudioPrefs(engine) {
  engine.command({cmd: "audioBus", bus: "music", muted: !audioPrefs.music});
  engine.command({cmd: "audioBus", bus: "effects", muted: !audioPrefs.effects});
}
/** Handles a click in the HUD (the restart button of the win / lose banner). */
export function hudClick(engine, e) {
  if (e.target.closest("[data-hud=restart]")) engine.command({cmd: "restart"});
}
const ICON_MUSIC = `<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M9 18V5l11-2v13" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round"/><circle cx="6.5" cy="18" r="2.5" fill="currentColor"/><circle cx="17.5" cy="16" r="2.5" fill="currentColor"/></svg>`;
const ICON_FX = `<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4 9h4l5-4v14l-5-4H4z" fill="currentColor"/><path d="M16.5 8.5a5 5 0 0 1 0 7M19 6a8.5 8.5 0 0 1 0 12" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>`;
/**
 * The music and effects switches (top right): one element that stays (the HUD markup is redrawn
 * every frame, a tap needs a button that is still there when the finger lifts). Returns it; the
 * caller shows it while a game plays.
 */
export function mountAudioSwitches(parent, engine) {
  const el = document.createElement("div");
  el.className = "k-audio";
  const make = (id, icon, label) => {
    const b = document.createElement("button");
    b.type = "button";
    b.dataset.audio = id;
    b.title = label;
    b.innerHTML = icon;
    const de = I18N_LANG === "de";
    const show = () => { const on = audioPrefs[id]; b.classList.toggle("off", !on); b.setAttribute("aria-pressed", String(on)); b.setAttribute("aria-label", `${label} ${on ? (de ? "an" : "on") : (de ? "aus" : "off")}`); };
    show();
    // Taps stay on the button: they never reach the game's touch controls or the canvas.
    for (const t of ["pointerdown", "pointerup", "touchstart", "touchend", "mousedown"]) b.addEventListener(t, e => e.stopPropagation());
    b.addEventListener("click", e => {
      e.preventDefault();
      e.stopPropagation();
      audioPrefs[id] = !audioPrefs[id];
      try { localStorage.setItem(AUDIO_KEY, JSON.stringify(audioPrefs)); } catch {}
      applyAudioPrefs(engine);
      show();
    });
    el.appendChild(b);
  };
  make("music", ICON_MUSIC, I18N_LANG === "de" ? "Musik" : "Music");
  make("effects", ICON_FX, I18N_LANG === "de" ? "Effekte" : "Effects");
  parent.appendChild(el);
  return el;
}

export function hudHtml(h) {
  if (!h) return "";
  const parts = [];
  if (h.title) parts.push(`<div class="k-hud-title">${escHtml(h.title)}</div>`);
  const stats = [];
  if (h.pickupsTotal) stats.push(`<div class="k-hud-stat"><b>${h.pickups}<i>/${h.pickupsTotal}</i></b><span>collected</span></div>`);
  stats.push(`<div class="k-hud-stat"><b>${Math.round(h.score)}</b><span>score</span></div>`);
  stats.push(h.remaining >= 0
    ? `<div class="k-hud-stat${h.remaining < 10 ? " k-warn" : ""}"><b>${formatTime(h.remaining)}</b><span>left</span></div>`
    : `<div class="k-hud-stat"><b>${formatTime(h.elapsed)}</b><span>time</span></div>`);
  if (h.best > 0) stats.push(`<div class="k-hud-stat"><b>${formatTime(h.best)}</b><span>best</span></div>`);
  parts.push(`<div class="k-hud-stats">${stats.join("")}</div>`);
  let html = "";
  // Boost in the low interior view: streaks rush past the edges of the view.
  if (h.boosting && h.interior && h.status === "playing")
    html += `<div class="k-streaks" style="--a:${Math.round((h.elapsed * 997) % 360)}deg"></div>`;
  html += `<div class="k-hud-top">${parts.join("")}</div>`;
  if (h.soldier) html += soldierHtml(h.soldier, h.status === "playing");
  else if (h.player) {
    const kmh = Math.round(h.speed * 3.6);
    html += `<div class="k-hud-speed"><b>${kmh}</b><span>km/h</span>` +
      (h.boost >= 0 ? `<div class="k-hud-boost"><div style="width:${Math.round(h.boost * 100)}%"></div></div>` : "") + `</div>`;
  }
  if (h.message && h.messageAge < 3.5 && h.status === "playing")
    html += `<div class="k-hud-message" style="opacity:${Math.min(1, (3.5 - h.messageAge) * 1.5).toFixed(2)}">${escHtml(h.message)}</div>`;
  if (h.status === "won" || h.status === "lost")
    html += `<div class="k-hud-banner k-${h.status}"><div>${h.status === "won" ? "Victory" : "Game over"}</div>` +
      `<small>${escHtml(h.message || "")}</small><button type="button" data-hud="restart">Play again</button><em>R to restart</em></div>`;
  return html;
}

/** First-person shooter HUD: crosshair sized by the spread, hit marker, health, ammo, damage direction. */
function soldierHtml(f, playing) {
  let html = "";
  const tanHalf = Math.tan((f.fov || 60) * Math.PI / 360);
  // Crosshair gap: the spread cone's radius on screen (vh), plus a little room.
  const gap = Math.min(18, 50 * Math.tan(f.spread * Math.PI / 180) / tanHalf + 0.6).toFixed(2);
  if (f.respawnIn < 0 && playing) {
    html += f.aiming ? `<div class="k-xh k-xh-dot"></div>`
      : `<div class="k-xh" style="--g:${gap}vh"><i class="k-xh-t"></i><i class="k-xh-b"></i><i class="k-xh-l"></i><i class="k-xh-r"></i><i class="k-xh-c"></i></div>`;
    if (f.hitAge < 0.28) html += `<div class="k-hit${f.hitKill ? " k-kill" : f.hitHead ? " k-head" : ""}" style="opacity:${(1 - f.hitAge / 0.28).toFixed(2)}"><i></i><i></i><i></i><i></i></div>`;
  }
  if (f.damageAge < 1.2) {
    const a = Math.max(0, 1 - f.damageAge / 1.2);
    html += `<div class="k-dmg" style="opacity:${(a * 0.85).toFixed(2)}"></div>`;
    html += `<div class="k-dmg-dir" style="transform:translate(-50%,-50%) rotate(${(-f.damageYaw).toFixed(0)}deg);opacity:${a.toFixed(2)}"><i></i></div>`;
  }
  const hp = Math.max(0, f.health), max = Math.max(1, f.maxHealth);
  html += `<div class="k-hp${hp / max < 0.3 ? " k-low" : ""}"><b>${Math.ceil(hp)}</b><div class="k-hp-bar"><div style="width:${Math.round(100 * hp / max)}%"></div>` +
    (f.armor > 0 ? `<div class="k-armor" style="width:${Math.round(100 * Math.min(1, f.armor / max))}%"></div>` : "") + `</div></div>`;
  if (f.ammo >= 0) {
    html += `<div class="k-ammo${f.ammo === 0 ? " k-empty" : ""}"><b>${f.ammo}</b><span>/ ${f.reserve}</span>` +
      (f.reload >= 0 ? `<div class="k-reload"><div style="width:${Math.round(f.reload * 100)}%"></div></div><em>Reloading</em>`
        : f.ammo === 0 ? `<em>${f.reserve > 0 ? "R / X to reload" : "No ammo"}</em>` : "") + `</div>`;
  }
  if (f.respawnIn >= 0) html += `<div class="k-respawn"><div>Down</div><small>Respawn in ${Math.ceil(f.respawnIn)}</small></div>`;
  return html;
}

export const HUD_STYLE = `.k-hud{position:absolute;inset:0;pointer-events:none;color:#f4f7fb;font:500 13px/1.25 system-ui,-apple-system,"Segoe UI",sans-serif;text-shadow:0 1px 3px rgba(0,0,0,.55);font-variant-numeric:tabular-nums}
.k-hud-top{position:absolute;left:18px;top:16px;display:flex;flex-direction:column;gap:8px}
.k-hud-title{font-size:12px;letter-spacing:.14em;text-transform:uppercase;opacity:.8}
.k-hud-stats{display:flex;gap:18px}
.k-hud-stat b{display:block;font-size:26px;font-weight:700;letter-spacing:-.01em}
.k-hud-stat b i{font-style:normal;opacity:.55;font-size:17px}
.k-hud-stat span{font-size:10px;letter-spacing:.12em;text-transform:uppercase;opacity:.65}
.k-hud-stat.k-warn b{color:#ff8f6b}
.k-hud-speed{position:absolute;right:20px;bottom:18px;text-align:right}
.k-audio{position:absolute;top:max(10px,env(safe-area-inset-top));right:max(10px,env(safe-area-inset-right));display:flex;gap:8px;pointer-events:auto;z-index:20;touch-action:manipulation}.k-audio[hidden]{display:none}
.k-audio button{width:40px;height:40px;padding:9px;border-radius:50%;border:1.5px solid rgba(255,255,255,.55);background:rgba(10,14,20,.45);color:#fff;cursor:pointer;position:relative}
.k-audio button svg{width:100%;height:100%;display:block}
.k-audio button.off{opacity:.55}.k-audio button.off::after{content:"";position:absolute;left:8px;right:8px;top:50%;height:2px;background:#ff6a5a;transform:rotate(-45deg)}
.k-hud-speed b{font-size:34px;font-weight:700}
.k-hud-speed span{margin-left:4px;font-size:11px;opacity:.7;letter-spacing:.1em;text-transform:uppercase}
.k-streaks{position:absolute;inset:0;pointer-events:none;background:repeating-conic-gradient(from var(--a) at 50% 46%,rgba(255,255,255,.0) 0deg 5deg,rgba(220,240,255,.55) 5deg 5.6deg,rgba(255,255,255,0) 5.6deg 11deg);-webkit-mask:radial-gradient(ellipse 70% 62% at 50% 46%,transparent 52%,#000 100%);mask:radial-gradient(ellipse 70% 62% at 50% 46%,transparent 52%,#000 100%);opacity:.55}
.k-hud-boost{width:150px;height:4px;margin-top:6px;margin-left:auto;background:rgba(255,255,255,.18);border-radius:2px;overflow:hidden}
.k-hud-boost div{height:100%;background:linear-gradient(90deg,#4fe3c8,#9ff7e8)}
.k-hud-message{position:absolute;left:50%;top:22%;transform:translateX(-50%);padding:8px 16px;border-radius:999px;background:rgba(8,12,18,.55);font-size:15px;white-space:nowrap}
.k-hud-banner{position:absolute;left:50%;top:42%;transform:translate(-50%,-50%);min-width:280px;padding:22px 30px 18px;border-radius:12px;background:rgba(10,13,19,.82);text-align:center;pointer-events:auto;box-shadow:0 12px 40px rgba(0,0,0,.45)}
.k-hud-banner div{font-size:32px;font-weight:800;letter-spacing:-.01em}
.k-hud-banner.k-won div{color:#7ef0c4}
.k-hud-banner.k-lost div{color:#ff9a7a}
.k-hud-banner.k-paused{pointer-events:none}
.k-hud-banner small{display:block;margin:6px 0 14px;font-size:14px;opacity:.85}
.k-hud-banner button{font:600 14px system-ui,sans-serif;padding:8px 20px;border:0;border-radius:6px;background:#2f7de1;color:#fff;cursor:pointer}
.k-hud-banner em{display:block;margin-top:8px;font-size:11px;font-style:normal;opacity:.55}
.k-xh{position:absolute;left:50%;top:50%;width:0;height:0;--g:1vh;filter:drop-shadow(0 0 1px rgba(0,0,0,.9))}
.k-xh i{position:absolute;background:rgba(255,255,255,.92);border-radius:1px}
.k-xh-t,.k-xh-b{width:2px;height:9px;left:-1px}.k-xh-t{bottom:var(--g)}.k-xh-b{top:var(--g)}
.k-xh-l,.k-xh-r{height:2px;width:9px;top:-1px}.k-xh-l{right:var(--g)}.k-xh-r{left:var(--g)}
.k-xh-c{width:2px;height:2px;left:-1px;top:-1px}
.k-xh-dot{width:4px;height:4px;margin:-2px 0 0 -2px;border-radius:50%;background:rgba(255,255,255,.85)}
.k-hit{position:absolute;left:50%;top:50%;width:0;height:0}
.k-hit i{position:absolute;width:2px;height:10px;background:#fff;left:-1px;top:-5px}
.k-hit i:nth-child(1){transform:rotate(45deg) translateY(-12px)}.k-hit i:nth-child(2){transform:rotate(135deg) translateY(-12px)}
.k-hit i:nth-child(3){transform:rotate(225deg) translateY(-12px)}.k-hit i:nth-child(4){transform:rotate(315deg) translateY(-12px)}
.k-hit.k-head i{background:#ffd34d}.k-hit.k-kill i{background:#ff4d4d;height:14px}
.k-dmg{position:absolute;inset:0;background:radial-gradient(ellipse at center,transparent 55%,rgba(200,20,20,.55) 100%)}
.k-dmg-dir{position:absolute;left:50%;top:50%;width:min(46vh,46vw);height:min(46vh,46vw)}
.k-dmg-dir i{position:absolute;left:50%;top:0;width:60px;height:14px;margin-left:-30px;border-radius:50% 50% 0 0;background:rgba(255,60,50,.85)}
.k-hp{position:absolute;left:20px;bottom:20px;display:flex;align-items:center;gap:10px}
.k-hp b{font-size:30px;font-weight:700;min-width:52px}
.k-hp-bar{position:relative;width:180px;height:8px;border-radius:4px;background:rgba(255,255,255,.18);overflow:hidden}
.k-hp-bar div{position:absolute;left:0;top:0;bottom:0;background:linear-gradient(90deg,#5fe08a,#b5f5c9)}
.k-hp-bar .k-armor{background:#69b7ff;height:3px;top:auto}
.k-hp.k-low b{color:#ff8a7a}.k-hp.k-low .k-hp-bar div:first-child{background:#ff6b5b}
.k-ammo{position:absolute;right:22px;bottom:18px;text-align:right}
.k-ammo b{font-size:38px;font-weight:700}.k-ammo span{margin-left:6px;font-size:16px;opacity:.65}
.k-ammo em{display:block;font-size:11px;font-style:normal;letter-spacing:.1em;text-transform:uppercase;opacity:.8}
.k-ammo.k-empty b{color:#ff8a7a}
.k-reload{width:120px;height:3px;margin:4px 0 2px auto;background:rgba(255,255,255,.2)}.k-reload div{height:100%;background:#fff}
.k-respawn{position:absolute;left:50%;top:40%;transform:translate(-50%,-50%);text-align:center}
.k-respawn div{font-size:34px;font-weight:800;color:#ff8a7a}.k-respawn small{font-size:15px;opacity:.85}`;

export const PLAYER_STYLE = `html,body{margin:0;height:100%;overflow:hidden;background:#0d1015;color:#eef3f8;font:14px system-ui,-apple-system,"Segoe UI",sans-serif;touch-action:none;user-select:none;-webkit-user-select:none}
#gl{position:fixed;inset:0;width:100%;height:100%;display:block;outline:none}
#hud{position:fixed;inset:0;pointer-events:none}
#hint{position:fixed;left:0;right:0;bottom:10px;text-align:center;opacity:.6;font-size:12px;pointer-events:none;text-shadow:0 1px 2px #000}
#error{position:fixed;inset:auto 12px 40px 12px;padding:10px 14px;border-radius:6px;background:#7a1f1f;display:none}
#loading{position:fixed;inset:0;display:grid;place-items:center;font-size:13px;letter-spacing:.2em;text-transform:uppercase;opacity:.6}
#restart{position:fixed;left:max(8px,env(safe-area-inset-left));top:max(8px,env(safe-area-inset-top));width:44px;height:40px;border-radius:10px;border:1.5px solid rgba(255,255,255,.3);background:rgba(10,13,18,.56);color:#fff;font:600 18px system-ui;touch-action:none;z-index:7}
[hidden]{display:none!important}
${HUD_STYLE}
${TOUCH_STYLE}`;

export const PLAYER_BODY = `<canvas id="gl" tabindex="0" aria-label="Game view"></canvas><div id="hud" class="k-hud"></div><div id="loading">Loading</div>
<div id="hint">WASD / arrows move · Space jump · Shift boost · Space lift · X handbrake · double-tap A/D kickflip · C camera · mouse look (click to capture, Esc frees) · P pause · R restart · gamepad supported</div>
<button id="restart" hidden aria-label="Restart">↻</button><div id="error" role="alert"></div>`;

/**
 * Joins ES module sources into one inline module: import lines and export
 * keywords are removed (the sources import each other only by name).
 */
export function inlineModules(...texts) {
  return texts.map(t => t.replace(/^import \{[^}]*\} from "[^"]+";\n/gm, "").replace(/^export (?=(const|let|function|async function|class) )/gm, "")).join("\n");
}

/**
 * Writes a standalone HTML game: engine module (base64), kern.mjs and this
 * file as one inline module, and the level. Plays offline from any host.
 */
export function buildGameHtml({sources, wasmBase64, levelText, title}) {
  const code = inlineModules(sources.kern, sources.game).replace(/<\/script/gi, "<\\/script");
  const boot = `\n{const t=id=>document.getElementById(id).textContent;` +
    `startGame({base64:t("kern-wasm").trim(),levelText:t("kern-level")}).catch(e=>{const b=document.getElementById("error");b.style.display="block";b.textContent="Game failed to start: "+e.message;console.error(e);});}`;
  const notices = typeof KERN_NOTICES === "string" ? `<!--\nMade with Kern (https://github.com/ChristianHohlfeld/kernengine.de).\n${KERN_NOTICES.replace(/--/g, "- -")}\n-->\n` : "";
  return `<!doctype html>\n${notices}<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">` +
    `<meta name="generator" content="Kern Studio"><title>${escHtml(title || "Kern game")}</title><style>${PLAYER_STYLE}</style></head><body>${PLAYER_BODY}` +
    `<script type="application/json" id="kern-level">${levelText.replace(/</g, "\\u003c")}<\/script>` +
    `<script type="text/plain" id="kern-wasm">${wasmBase64}<\/script>` +
    `<script type="module">${code}${boot}<\/script></body></html>`;
}

/**
 * Plays one level full screen. `levelText` is .klevel JSON (or `template` names
 * a built-in level: orbRun, physicsLab, basic); the wasm comes from `bytes`,
 * `base64` or `url`.
 */
export async function startGame({levelText = null, template = null, bytes = null, base64 = null, url = null, canvas = document.getElementById("gl")} = {}) {
  localizePage();  // German interface on German systems (visual only)
  const kern = await loadKern({bytes, base64, url});
  const engine = new StudioEngine(kern);
  const gl = canvas.getContext("webgl2", {antialias: true, alpha: false, powerPreference: "high-performance"});
  if (!gl) throw Error("WebGL2 is not available in this browser");
  kern.attachContext(canvas, gl);
  const coarse = matchMedia("(pointer:coarse)").matches;
  let dpr = 1;
  const size = () => {
    dpr = Math.min(devicePixelRatio || 1, coarse ? 1.5 : 2);
    const w = Math.max(1, Math.round(canvas.clientWidth * dpr)), h = Math.max(1, Math.round(canvas.clientHeight * dpr));
    if (canvas.width !== w || canvas.height !== h) { canvas.width = w; canvas.height = h; return true; }
    return false;
  };
  size();
  if (!engine.init(canvas.width, canvas.height, true)) throw Error(kern.lastError());
  const audio = startAudio(engine);
  const opened = template ? engine.command({cmd: "new", template}) : engine.command({cmd: "open", text: levelText, name: "game.klevel"});
  engine.loadPolicies().catch(() => {});
  if (!opened?.ok) throw Error(engine.state()?.statusText || "the level could not be loaded");
  const params = new URLSearchParams(location.search);
  const online = params.get("host") ?? params.get("join");
  let net = null;
  if (online != null) {
    net = new KernNet(engine);
    net.flushEveryFrame = true;
    try {
      await openNetwork(engine, net, {host: params.has("host"), room: online || "kern", relay: params.get("relay") || ""});
    } catch (error) {
      net.close();
      net = null;
      document.getElementById("loading").textContent = "Online play failed: " + error.message + " - playing alone";
      engine.command({cmd: "play", mode: "play"});
    }
  } else {
    const started = engine.command({cmd: "play", mode: "play"});
    if (!started?.ok) throw Error(engine.state()?.statusText || "the level has errors");
  }
  const $ = id => document.getElementById(id);
  $("loading").hidden = true;
  const hud = $("hud");
  const overlay = new TouchOverlay(document.body);
  watchInputDevice(engine);
  canvas.addEventListener("webglcontextlost", e => { e.preventDefault(); engine.contextLost(); kern.resetTables(); });
  canvas.addEventListener("webglcontextrestored", () => { kern.attachContext(canvas, gl); engine.contextRestored(); });

  // Mouse: click captures the pointer for mouse look; touch drags orbit the camera.
  // Touch drags look around; only mouse buttons fire and aim (touch has its own buttons).
  // Touch and pen fingers go to the engine's touch controls; the mouse looks (pointer lock) and fires.
  const send = (type, e) => engine.pointer(0, type, e.clientX, e.clientY, {button: e.pointerType === "mouse" ? e.button : -1, buttons: e.buttons, mods: modifiers(e), wheel: e.deltaY ?? 0});
  const at = e => [e.clientX, e.clientY];
  canvas.addEventListener("pointerdown", e => {
    canvas.focus();
    try { canvas.setPointerCapture(e.pointerId); } catch { /* ignore */ }
    e.preventDefault();
    if (routeTouch(engine, POINTER.down, e, at)) return;
    // The mouse looks through pointer lock - not while watching (observer camera: it drags bodies).
    if (e.pointerType === "mouse" && document.pointerLockElement !== canvas && !engine.state()?.play?.observing) canvas.requestPointerLock?.()?.catch?.(() => {});
    send(POINTER.down, e);
  });
  canvas.addEventListener("pointermove", e => { if (!routeTouch(engine, POINTER.move, e, at) && document.pointerLockElement !== canvas) send(POINTER.move, e); });
  canvas.addEventListener("pointerup", e => { if (!routeTouch(engine, POINTER.up, e, at)) send(POINTER.up, e); });
  canvas.addEventListener("pointercancel", e => { if (!routeTouch(engine, POINTER.cancel, e, at)) send(POINTER.cancel, e); });
  // No finger left on the screen: nothing can still be held (SpawnCity's ground truth).
  addEventListener("touchend", e => { if (!e.touches.length) engine.command({cmd: "touchRelease"}); });
  addEventListener("visibilitychange", () => { if (document.hidden) engine.command({cmd: "touchRelease"}); });
  canvas.addEventListener("wheel", e => { e.preventDefault(); send(POINTER.wheel, e); }, {passive: false});
  canvas.addEventListener("contextmenu", e => e.preventDefault());
  document.addEventListener("mousemove", e => { if (document.pointerLockElement === canvas) engine.look(e.movementX, e.movementY); });
  // The browser's Escape ends pointer lock without telling the page: pause right away (one press),
  // and the next click into the game resumes and captures the mouse again.
  let wasLocked = false;
  document.addEventListener("pointerlockchange", () => {
    const locked = document.pointerLockElement === canvas;
    if (wasLocked && !locked && document.hasFocus() && engine.status()?.hud?.status === "playing") engine.command({cmd: "pause", value: true});
    if (locked) engine.command({cmd: "pause", value: false});
    wasLocked = locked;
  });

  const key = (e, down) => {
    const r = engine.key(e.key, e.code, down, modifiers(e), e.repeat);
    if (r?.handled && !(e.code === "Escape")) e.preventDefault();
  };
  addEventListener("keydown", e => key(e, true));
  addEventListener("keyup", e => key(e, false));
  addEventListener("blur", () => engine.blur());

  const gamepad = new GamepadInput(engine);
  let touchShown = false;
  $("restart").addEventListener("click", () => engine.command({cmd: "restart"}));
  hud.addEventListener("click", e => hudClick(engine, e));
  applyAudioPrefs(engine);
  mountAudioSwitches(hud.parentNode, engine);

  let rectKey = "", hudKey = "", lastTextures = 0;
  const frame = time => {
    size();
    const k = `${canvas.width}x${canvas.height}`;
    if (k !== rectKey) {
      rectKey = k;
      engine.resize(canvas.width, canvas.height);
      engine.setViewport(0, 0, 0, canvas.width, canvas.height, canvas.clientWidth, canvas.clientHeight, dpr, true);
    }
    engine.frame(time);
    engine.takeUi();
    audio?.pump();
    net?.flush();
    if (time - lastTextures > 250) { lastTextures = time; engine.pollTextures(); engine.pollSounds(audio); engine.loadPolicies().catch(() => {}); }
    gamepad.poll();
    const st = engine.status();
    const html = hudHtml(st?.hud) + (st?.paused ? `<div class="k-hud-banner k-paused"><div>Paused</div><small>Click to continue</small></div>` : "");
    if (html !== hudKey) { hudKey = html; hud.innerHTML = html; }
    overlay.update(st?.touch);
    const onTouch = st?.input?.device === "touch";
    if (onTouch !== touchShown) { touchShown = onTouch; $("restart").hidden = !onTouch; $("hint").hidden = onTouch; }
    requestAnimationFrame(frame);
  };
  requestAnimationFrame(frame);
  if (net) backgroundTicker(time => { engine.frame(time); engine.takeUi(); audio?.pump(); net.flush(); });
  canvas.focus();
  const api = {engine, audio, net, get status() { return engine.status(); }, command: c => engine.command(c)};
  window.KernGame = api;
  return api;
}
