// SpawnCity on Kern - menus (DOM only, no game logic): the title screen (START, WELT, EINSTELLUNGEN,
// NEU / CHANGELOG, IDEE), the pause menu, the settings dialog built from the game's own settings
// table (C++ `settings schema`), the WORLD options, "Zurueck zum Hauptmenue?", RESET, the corner
// buttons of the HUD, a suggestion card and the installed-app / fullscreen policy.
// Every change goes to the game as a command ("set <key> <value>"); the page keeps a copy in
// localStorage only as a convenience and replays it at start.
// Sources (spawncity-de/sim): settings_help.mjs, settings_controller_nav.mjs, player_settings_sections.mjs,
// control_settings.mjs, mouse_settings.mjs, camera_settings.mjs, audio_settings.mjs, world_options.mjs,
// world_options_menu.mjs, changelog_menu.mjs, quit_confirm.mjs, game_exit_button.mjs, game_reset.mjs,
// suggestion_box.mjs, neon_ui_theme.mjs, edge_hud_layout.mjs, mobile_gameplay_ui.mjs, pwa_fullscreen_policy.mjs.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

const SCUI_STORE = "spawnCityKernSettingsV1";
const SCUI_SUGGEST = "spawnCityKernSuggestV1";
const SCUI_PLACE = "spawnCityKernStartPlaceV1", SCUI_RECENT = "spawnCityKernStartPlaceRecentV1";
const SCUI_LOGBOOK = "spawnCityKernLogbookV1";
const SCUI_PHOTON = "https://photon.komoot.io/api/";
// world_location_selector.mjs WORLD_LOCATION_PRESETS (+ Konstanz, the game's home)
const SCUI_PRESETS = [["Konstanz", 47.6600, 9.1766], ["New York", 40.7128, -74.0060], ["Berlin", 52.5200, 13.4050], ["Zürich", 47.3769, 8.5417],
  ["London", 51.5074, -0.1278], ["Paris", 48.8566, 2.3522], ["Tokyo", 35.6762, 139.6503], ["Los Angeles", 34.0522, -118.2437], ["Singapore", 1.3521, 103.8198],
  ["Dubai", 25.2048, 55.2708], ["Sydney", -33.8688, 151.2093], ["Rio de Janeiro", -22.9068, -43.1729]];
const SCUI_SECTIONS = [["maus", "MAUS"], ["fuss", "ZU FUSS"], ["drohne", "DROHNE"], ["auto", "AUTO"], ["kamera", "KAMERA"], ["ton", "TON"]];

// changelog_menu.mjs: newest first, one line a player understands, only what is new in that update.
const SCUI_CHANGELOG = [
  {date: "2026-11-04", title: "Meilenstein 3: Menues und Ausstattung", items: [
    "Titelbildschirm mit START, WELT, EINSTELLUNGEN, NEU / CHANGELOG; ESC oder MENUE pausiert.",
    "Einstellungen wie auf spawncity.de: Maus, zu Fuss, Drohne, Auto, Kamera, Ton – sofort wirksam.",
    "WELT: Leute, Verkehr, Polizei, Jetpack und Friendly Fire an- und ausschalten.",
    "RESET bringt alles zurueck an den Start; EXIT fragt vorher nach."]},
  {date: "2026-11-04", title: "Meilenstein 2: Stadt, Polizei, Waffen, Mehrspieler", items: [
    "Passanten, Streifenwagen mit Beamten, Polizeidrohnen und Fahndung mit Fluchtuhr.",
    "Das ganze Waffenset mit Granaten, Raketen, Explosionen und Fahrzeugschaden; EMP auf X.",
    "Mehrspieler ueber Kern Client/Server, Minimap und Strassenlaternen."]},
  {date: "2026-10-29", title: "Meilenstein 1: SpawnCity laeuft nativ auf Kern", items: [
    "Drohne mit der echten Arondight45-Firmware, Jet, Autos, Bus und Fallschirm auf Box3D.",
    "Die echte Stadt aus OpenStreetMap-Vektorkacheln, ohne Netz eine prozedurale Ersatzstadt.",
    "Tastatur, Maus, Xbox-Pad und Touch."]},
];

// spawncity.de's own changelog (changelog_menu.mjs CHANGELOG, unchanged): the history of the game this is a clone of.
const SCUI_CHANGELOG_ORIGINAL = [{"date":"2026-10-17","time":"13:20","title":"Jet-HUD und Laufstil verbessert","items":["EJECT sitzt jetzt links oben und verdeckt die Minimap nicht mehr.","Bei Kopftreffern gibt es weder HEADSHOT-Schrift noch Ansager – nur ein kurzes neutrales Knacken.","Rennende Passanten und Jogger lehnen sich wieder deutlich zurück und strecken die Füße nach vorne."]},{"date":"2026-10-17","time":"11:44","title":"Vollbild und klassische Hände zurück","items":["Vollbild startet wieder wie vor der App-Umstellung; die Steuerung bleibt in normaler Ansicht und Vollbild nutzbar.","In der Ego-Ansicht sind die Hände und Waffen wieder kompakt wie vor den neuen großen Handschuh-Armen."]},{"date":"2026-10-16","time":"14:42","title":"Neueste Spielversion und Changelog wieder live","items":["Alle bisherigen Änderungen und Updates erscheinen wieder in der richtigen Reihenfolge im NEU / CHANGELOG-Menü.","Die aktuelle Spielversion wird jetzt einheitlich unter spawncity.de veröffentlicht – einschließlich des verbesserten Touchpad-Mouselooks.","Das Spiel prüft beim Veröffentlichen, ob die richtige Version tatsächlich auf spawncity.de angekommen ist."]},{"date":"2026-10-16","time":"14:11","title":"Touchpad-Mouselook in Ego-Sicht","items":["Notebook-Touchpads drehen die Kamera in der Ego-Perspektive wieder gleichmäßig, auch bei schnellen Fingerbewegungen.","Die Steuerung nutzt zuverlässige Mausbewegungen statt Raw-Input-Zwang; Schießen, Gamepad und Touch-Bedienung bleiben erhalten."]},{"date":"2026-10-16","time":"10:46","title":"Headshots, Sniper-Fixes, echte Jogger","items":["HEADSHOT! Kopftreffer mit Sniper und Glock töten sofort, mit der MP dreifacher Schaden – mit Einblendung und Ansager.","Sniper-Zielfernrohr hängt nicht mehr: Drohne, Auto, Jet, Drehen des Handys oder Tod beenden das Zielen sofort.","Beim Zielen durchs Fernrohr tauchen keine Zombies oder Leute mehr plötzlich neben dir auf.","Ein Sniper-Schuss sprengt kein Auto mehr: Karosserie braucht drei Treffer, der Motorraum zwei – erst raucht es, dann knallt es.","Polizei, Militär und Zombies sind weniger tödlich: weniger Schaden, kurze Reaktionszeit, keine Salven mehr.","Jogger joggen jetzt wirklich – mit Laufstil, Sportklamotten und ohne durch Häuser zu laufen.","Die losen Physik-Objekte (Kisten, Fässer, Hütchen) sind raus, auch aus dem Menü."]},{"date":"2026-10-16","time":"08:57","title":"Teilen","items":["Neuer Knopf TEILEN im Startmenü: schick SpawnCity direkt per WhatsApp, X, Instagram & Co. – oder der Link wird kopiert. Geteilte Links zeigen jetzt ein großes Vorschaubild."]},{"date":"2026-10-16","time":"07:10","title":"Polizei kommt, Busse fahren, neue Autos","items":["Polizei, Polizeidrohnen und Militär kommen wieder: du entkommst erst, wenn sie da waren – nicht mehr, bevor das erste Auto eintrifft. Das Militär rückt bei fünf Sternen deutlich früher aus.","Ein Haus wegzuschießen reißt keinen ganzen Block mehr mit: brennende Autos, Trümmer und Polizeiwagen beschädigen keine Häuser mehr, nur noch echte Waffen.","Busse kannst du jetzt genauso fahren wie Autos – mit Außen- und Innenansicht.","Mehr Autos auf den Straßen: Kombis, SUVs, Transporter und ein flacher Elektro-Roadster.","Raketenwerfer: nie mehr blockiert – eine neue Rakete übernimmt den Laserpunkt, die alte fliegt ungelenkt weiter. Der Punkt ist jetzt wirklich rot. Lenkraketen ziehen runde Rauchwolken statt Vierecken.","Die Glock ist feiner gebaut (Leuchtvisier, Auswurffenster, Abzug), der Schlitten fährt bei jedem Schuss sichtbar zurück, und der Schuss knallt satter.","Das Messer hinterlässt Spuren: Wunden und Blut bei Treffern, Schnittkerben an Wänden und Autos.","Die Arme in der Ich-Ansicht verdecken weniger: kürzer, und die zweite Hand kommt von unten statt quer durchs Bild.","Passanten, die die Arme hochreißen, schreien dabei.","Spieler halten etwa 18 % mehr aus.","Zombies brechen nicht mehr sofort durch: eine Barrikade hält, egal wie viele drauf einschlagen, und durch Bretter wird niemand gekratzt. Nachts sind die Eingänge nur noch verbarrikadiert, ohne Türen.","Routenpfeile liegen jetzt als Pfeile auf der Straße statt als flache Symbole im Bild.","Die installierte App startet nicht mehr im erzwungenen Vollbild – kein „Zum Beenden nach oben wischen“ mehr, du kommst ganz normal raus."]},{"date":"2026-10-16","time":"06:38","title":"Feste Häuser, Tauben, Laserpunkt","items":["Häuser sind überall fest: Autos fahren nicht mehr durch Gebäude (auch nicht in der Innenansicht), Passanten und Tiere laufen nicht mehr hindurch – egal wie weit du dich von deiner Drohne entfernst.","Die Physik repariert sich selbst: ein einzelner kaputter Körper kann die Kollisionen nicht mehr lahmlegen, und sollte die Physik doch einmal abstürzen, wird sie sofort neu aufgebaut.","Tauben! Kleine Trupps laufen in der Stadt herum, nicken, picken und gurren – kommst du ihnen zu nahe, fährt ein Auto vorbei oder knallt es, flattern alle zusammen auf und landen woanders.","Vögel fliegen jetzt mit dem Kopf voran statt seitwärts.","Raketenwerfer: nur noch ein großer roter Laserpunkt, kein Strahl mehr. Auf dem Handy lenkst du die fliegende Rakete mit dem Finger: Tipp auf ein neues Ziel und zieh, die Rakete folgt dem Punkt.","Im Jet gibt es nur noch einen EJECT-Knopf (oben rechts) – der Flugmodus-Knopf unten ist wieder frei erreichbar.","Die Ton- und Musik-Knöpfe oben wackeln nicht mehr.","Sounds sind zurück: Motor, Umgebung und Explosionen setzen nach dem Zurückkehren in die App wieder ein (spätestens beim ersten Tippen).","Straßen und Gehwege verschwinden nicht mehr kurz, wenn irgendwo etwas explodiert.","Innenansicht im Auto ganz ohne Lenkrad – freie Sicht auf die Straße."]},{"date":"2026-10-16","time":"03:30","title":"Neue Hände, Sniper-Timing, Fixes","items":["Neue Hände in der Ego-Sicht und eine schnellere Messer-Animation.","Sniper: Treffer und Einschlag kommen erst an, wenn die Kugel da ist.","Granatwerfer-Explosionen sind wieder deutlich zu hören.","Auf Dachaufbauten (Treppenhäuser) kann man jetzt stehen.","Gebäude sind beim Start nicht mehr schon eingestürzt.","Startmenü passt auf das Handy quer, und EXIT ist immer erreichbar."]},{"date":"2026-10-16","time":"02:30","title":"Neuer Mehrspieler-Raum","items":["SpawnCity hat jetzt einen eigenen Mehrspieler-Raum.","Bitte die Seite einmal neu laden. Wer noch eine ältere Version offen hat, sieht die anderen erst nach dem Neuladen.","Die Fernsteuerung (Remote) wird einfach neu gekoppelt wie bisher."]},{"date":"2026-10-15","time":"23:59","title":"Flak!","items":["Ab und zu steht irgendwo in der Stadt eine Vierlings-Flak hinter Sandsäcken. Fliegst du mit Drohne oder Jet in ihre Nähe (gut 1 km, Sichtlinie), feuert sie wie verrückt: vier Rohre, Leuchtspur bei jedem Schuss, Sprengwolken um dich herum, harter Sound. Nahe Detonationen beschädigen dich.","Ausschalten: eine Explosion daneben oder ein paar Scharfschützen-Treffer. Alle Spieler finden die Flak an denselben Orten. Im Menü unter WORLD → FLAK abschaltbar."]},{"date":"2026-10-15","time":"23:58","title":"spawncity.de ist die Adresse","items":["Das Spiel läuft jetzt unter spawncity.de. Die Startseite geht direkt ins Spiel.","Die Home-Bildschirm-App bleibt an der Adresse, mit der du sie angelegt hast. Einmal unter spawncity.de öffnen und neu auf den Home-Bildschirm legen."]},{"date":"2026-10-15","time":"23:54","title":"Ping-Markierung, Luftschlag in Spielerfarbe, Voice-Reichweite","items":["PING: zeig deinen Mitspielern eine Richtung oder einen Treffpunkt – PC: Z oder mittlere Maustaste, Xbox: Steuerkreuz rechts, Handy: Bildschirm 0,6 s gedrückt halten. Nochmal auf deinen Ping = weg; nach 90 s verschwindet er von selbst.","Der Luftschlag-Strahl vom Himmel hat jetzt die Farbe des Spielers, der ihn gerufen hat – so ist klar, woher ein roter (oder blauer, grüner …) Strahl kommt.","Vor dem Luftschlag zeigt eine pulsierende blaue Pfeilspur am Boden, in welche Richtung die Bomben fallen.","Einstellungen: VOICE ÜBERALL HÖRBAR – aus (Standard) = nur Mitspieler in der Nähe hören dich, an = wie Funk für alle."]},{"date":"2026-10-15","time":"23:49","title":"Trümmer sind fest","items":["Ein weggesprengtes Haus hinterlässt jetzt einen niedrigen Trümmersockel, der genauso fest ist, wie er aussieht: Du, Passanten, Autos und Tiere laufen nicht mehr hindurch.","Auf den Trümmersockel kannst du hochspringen und darauf herumlaufen.","Kühe laufen nicht mehr durch Häuser – auch nicht, wenn sie in Panik davonrennen."]},{"date":"2026-10-15","time":"23:43","title":"Neuer Name: SpawnCity","items":["Das Spiel heißt jetzt SpawnCity – du spawnst genau dort, wo du gerade stehst, in deiner echten Stadt. Neue Adresse: spawncity.de.","Neuer Titel auf dem Startbildschirm (SPAWN über CITY) und ein neues App-Symbol für den Home-Bildschirm.","Deine Einstellungen, der gewählte Startort und die schon geladenen Karten bleiben beim Umzug auf den neuen Namen erhalten."]},{"date":"2026-10-15","time":"22:02","title":"Lasergelenkter Raketenwerfer, Autos ohne Ruckeln","items":["Raketenwerfer im Half-Life-2-Stil: ein roter Laserpunkt zeigt, wohin du zielst, und die Rakete fliegt dem Punkt hinterher – du lenkst sie mit dem Zielen. Immer nur eine Rakete gleichzeitig; wechselst du die Waffe, fliegt sie ungelenkt weiter.","Der Raketenwerfer hat ein neues Aussehen (dunkles Metall, Laser-Zielgerät mit rotem Licht), die Rakete fliegt gerade ausgerichtet und zieht eine Rauchspur hinter sich her.","Zerstörte Häuser: an der Stelle eines weggeschossenen Hauses bleibt keine unsichtbare Kollision mehr stehen – kein Hochschleudern mehr beim Springen oder mit dem Jetpack.","Autos ruckeln nicht mehr beim Fahren (Außen- und Innenansicht): Auto, Räder und Kamera werden in jedem Bild an derselben Stelle gezeichnet, auch die Polizeiautos.","Fahrer sitzen tiefer – ihre Köpfe ragen nicht mehr durchs Autodach.","Wird die App geschlossen oder in den Hintergrund gelegt, verstummen Musik und alle Geräusche; beim Zurückkehren geht es genau dort weiter."]},{"date":"2026-10-15","time":"18:50","title":"Fahrer in den Autos, Autoklau mit Folgen","items":["In jedem fahrenden Auto sitzt jetzt ein Fahrer – durch die getönten Scheiben zu sehen.","Klaust du ein Auto, fliegt der Fahrer raus und steht neben dem Wagen: je nach Typ rennt er weg, schimpft erst und haut dann ab oder geht auf dich los. Leute in der Nähe drehen sich um und schimpfen, ein Zeuge meldet es (ein kleiner Stern).","Ein geräumtes Auto bleibt leer stehen und fährt erst weiter, wenn es keiner mehr sieht. Steigt ein wütender Fahrer aus, steht er sofort sichtbar da.","Selten zieht ein Hitzkopf selbst einen Fahrer aus einem wartenden Auto und rast damit davon – das Opfer flieht, die Umstehenden schreien."]},{"date":"2026-10-15","time":"18:45","title":"Echte Panik","items":["Fliehende Passanten laufen jetzt jeder anders schnell. Bei echter Angst (Schüsse nah, Explosion) sprinten sie ein paar Sekunden mit 6–8,5 m/s, viele reißen dabei die Hände über den Kopf. Danach bleiben sie eine Weile schreckhaft und gehen eilig weiter."]},{"date":"2026-10-15","time":"18:43","title":"Handy-Knöpfe an den Rand, Autos ohne Ruckeln","items":["Handy: alle Knöpfe sitzen jetzt am Rand, in Reichweite des Daumens, der sie braucht – Luftschlag und Granate über dem linken Stick, Waffe/Springen bzw. ARM/STOP links neben dem rechten Stick, Einsteigen und EMP über dem rechten Stick. Die Bildmitte bleibt frei. Gleich in allen Modi und im Hoch- wie Querformat.","Im Auto: Handbremse, Kamera und Radio rechts über dem Daumen, Aussteigen unten rechts. Mikrofon und Stummschalten wandern in die obere Leiste; GPS, Musik und Ton liegen auch im Hochformat oben rechts.","Autos ruckeln nicht mehr: die Physik läuft mit festen 60 Schritten pro Sekunde, gezeichnet wird jetzt sauber dazwischen – auch auf 90/120-Hz-Displays und bei schwankender Bildrate.","Kein „Dach weg“ mehr: steht hinter dem Auto eine Wand, rutscht die Kamera nicht mehr ins Auto, sondern hebt sich darüber.","Innenansicht: nur noch ein kleines Lenkrad unten im Bild – keine Armaturen, Säulen oder Motorhaube, freie Sicht auf die Straße."]},{"date":"2026-10-15","time":"18:15","title":"Echter Standort, Offline-App, ruhigere Stadt, Lenkrakete","items":["Jeder Start beginnt an deinem echten Standort: frischer, genauer GPS-Fix (nie ein alter); kommt der genaue Fix spät, springt die Welt nach. Zurück aus dem Menü: bist du über 400 m weiter, kommt die Welt zu dir. Ein im Menü gesuchter Ort gilt nur noch bis zum Schließen der App.","Installierte App (Startbildschirm): startet auch offline, holt aber online immer sofort die neueste Version. Kartendaten von Orten, an denen du warst, bleiben gespeichert.","Lenkrakete: jeder Druck feuert – mit Ziel in der Nähe des Fadenkreuzes fliegt sie es an, sonst auf den anvisierten Punkt. 5 s Nachladen, sichtbar auf dem Waffenknopf und unter dem Fadenkreuz. Dafür etwas schwächer.","Passanten sind deutlich friedlicher (weniger Hitzköpfe, kaum Waffen, langsamer am Abzug). Unsichtbare Schützen gibt es nicht mehr: nur wer sichtbar ist, kann dich angreifen.","Polizei entspannter: mehr Sterne nötig, weniger Streifen, kommen später, schießen seltener und ungenauer, du entkommst schneller.","Kühe grasen gemütlich auf ihrem Fleck statt über die Weide zu rasen."]},{"date":"2026-10-15","time":"00:29","title":"Jet-Sticks zeigen dahin, wo du ziehst","items":["Handy im Hochformat: die beiden Sticks im Jet (und im Auto) lasen bisher die Bildschirm-Achsen statt der gedrehten Spielansicht – deshalb ging der linke Stick „irgendwohin“. Jetzt gilt: wohin du ziehst, dahin geht’s. Der Ausschlag zählt ab da, wo der Daumen aufsetzt (kein Ruck beim Antippen).","Belegung wie bei Flugsimulatoren (Mode 2): links ↑↓ Schub/Steigen, ←→ Seitenruder; rechts Nicken und Rollen."]},{"date":"2026-10-15","time":"00:26","title":"Deine Ideen fürs nächste Update","items":["Nach etwa 15 Minuten Spielzeit mit einer Version fragt das Hauptmenü einmal kurz nach deiner Idee für das nächste Update – eine Zeile, anonym, ohne Konto. Alle Vorschläge werden gesammelt und zusammengefasst; „Später“ fragt nach weiteren 10 Minuten nochmal, „Nein danke“ nie mehr für diese Version."]},{"date":"2026-10-15","time":"00:23","title":"Spielen, wo du willst","items":["Im Hauptmenü unter START: 📍 Startort. Standard ist dein GPS-Standort – oder du suchst eine Stadt, Straße oder Adresse (mit Vorschlägen beim Tippen) und startest dort. Die letzten Orte bleiben gespeichert; ein neuer Ort während des Spiels gilt beim nächsten START."]},{"date":"2026-10-14","time":"23:48","title":"Multiplayer: Blickrichtung, echte Drohnen, kein Rauswurf","items":["Mitspieler schauen jetzt in die Richtung, in die sie wirklich schauen (vorher war die Drehung seitenverkehrt) – auch im Auto per Live-GPS.","Fremde Drohnen sehen aus wie deine eigene: Arme, Motoren, drehende Propeller, Kufen – in der Spielerfarbe an Haube, Motorglocken und LED-Ring.","Mitspieler und ihre Drohnen verschwinden nicht mehr, wenn ihr Handy kurz pausiert oder die Verbindung hakt – sie bleiben sichtbar stehen.","Updates werfen dich nicht mehr aus einer laufenden Runde: ist ein Spiel gestartet, erscheint nur „UPDATE“ (tippen oder U). „Später“ lässt 10 Minuten Ruhe.","Live-GPS auf Android: die Standortabfrage kommt sofort beim Tippen (vorher blieb es ohne Nachfrage bei „Suche GPS“ hängen); drinnen wird nach kurzer Zeit der ungefähre Standort genommen; blockiert ist klar angezeigt."]},{"date":"2026-10-14","time":"23:41","title":"Maus-Empfindlichkeit, ADAC zerstörbar","items":["PC: in den Einstellungen gibt es jetzt MAUS-EMPFINDLICHKEIT (20–300 %) und MAUS Y INVERTIEREN – gilt zu Fuß, in der Drohne, im Auto und im Jet.","Der gelbe Abschleppwagen kann jetzt zerstört werden: Explosionen und genug Treffer lassen ihn ausbrennen, sein Auftrag fällt weg."]},{"date":"2026-10-14","time":"23:37","title":"Keine Geister-Räder mehr, festere Häuser, Kisten anschießen","items":["Autos aus großer Entfernung (Jet, Fallschirm, Dach) erscheinen nicht mehr als vier lose Räder: die Räder verschwinden jetzt zusammen mit der Karosserie.","Häuser-Kollision: L-förmige Häuser und Höfe bestehen aus wenigen festen Blöcken statt vieler Dreiecke – dadurch sind fast doppelt so viele Häuser rundum fest, und ein Haus ist immer ganz fest, nie nur zur Hälfte.","Kisten, Fässer, Pylonen und Tonnen reagieren jetzt auf jeden Treffer, auch auf die MP – mit dem echten Impuls der Kugel (Scharfschützengewehr wirft sie um, Pistole schubst Pylonen). Im Multiplayer sehen alle den Flug."]},{"date":"2026-10-14","time":"23:25","title":"Granatwerfer und Raketenwerfer wieder getrennt","items":["Zu Fuß gibt es wieder beide: der Granatwerfer (5) schießt springende 40-mm-Granaten mit Zeitzünder, der neue Raketenwerfer (6) eine schnelle, flache Rakete mit Feuerschweif, die beim Aufprall explodiert. Fäuste jetzt 7, Messer 8.","Raketenwerfer als eigenes Schulterrohr mit sichtbarem Gefechtskopf; Xbox und Mausrad schalten durch alle Waffen.","PC: die Tastenhilfe liegt unter den Mikrofon-Knöpfen und blendet sich nach 12 s aus – beim Waffen- oder Moduswechsel kommt sie zurück."]},{"date":"2026-10-14","time":"23:15","title":"Radio mit SWR, NDR & Co., Effekte-Schalter, Steuerung","items":["Autoradio: die öffentlich-rechtlichen Sender deines Bundeslands (z. B. SWR3, SWR1, NDR 2, Bayern 3, WDR 2, hr3) plus Deutschlandfunk, danach die Lokalsender. Läuft beim Einsteigen, unabhängig vom Musik-Schalter.","Radio aus: in der Senderliste gibt es den Platz „RADIO AUS“ (📻 tippen bzw. N/B), 📻 halten schaltet direkt aus. Der Motor ist leiser und tritt unter dem Radio zurück.","„Effekte aus“ schaltet jetzt wirklich alles stumm – auch Düsenjet, Sirenen, Tiere und Motoren.","Handy: Schießen funktioniert bis direkt an die Sticks heran – die Stickfläche ist genau der gezeichnete Kreis.","Kein Airstrike mehr, solange man tot ist. Respawn am PC jetzt auch mit der Leertaste.","Nach dem Schleudersitz verschwinden die Jet-Knöpfe (Flug/Rakete/Bombe).","Weit entfernte Personen sind Silhouetten statt Klötze und erst ab 130 m vereinfacht.","Klick neben das Hauptmenü löst keinen Schuss mehr aus. „1 Runde überlebt“ statt „1 Runden“.","PC: die Tastenhilfe sitzt oben, unten bleibt Platz für ARM/STOP und Fahrzeug-Tasten."]},{"date":"2026-10-13","title":"Passanten sprinten jetzt panisch","items":["Bei Schüssen und Explosionen fliehen Passanten jetzt im echten Sprint mit längeren Schritten, stärkerem Beinabstoß und vorgebeugtem Oberkörper."]},{"date":"2026-10-13","title":"ARCADE JET, Voice Toggle und übersichtlicheres Gameplay","items":["VOICE: Mikrofon ist jetzt ein echter EIN/AUS-Schalter statt Push-to-Talk. Tippen bzw. H oder Xbox R3 schaltet dauerhaft ein und wieder aus; ohne Freigabe bleibt es aus. Mitspielerpositionen werden für die 70-m-Reichweite korrekt übertragen.","Die mobilen Voice-Tasten sitzen tiefer an der Daumen-Zone und meiden weiterhin Joysticks, Fahrzeugaktionen und das Sichtfeld.","Jet-Steuerung reagiert schneller, mit deutlicherem Nick- und Rollverhalten; die Box3D-Kollisionen, Aerodynamik und alle Personen bleiben erhalten.","Jet-Raketen: erst RAKETE wählen (1), dann auf Ziel tippen bzw. auslösen (2). Danach 2,4 Sekunden Nachladezeit mit Anzeige; kein versehentlicher Geschützschuss während des Abschusses.","Mobile EJECT ist weg von den Daumentasten und erfordert 650 ms bewusstes Halten. JET-EINSTEIGEN ist leicht nach unten versetzt.","Die Taste FPS / DRONE ist kürzer und steht weiter links. Die große Karte kann direkt über das ⛶-Symbol aus allen Spielmodi geöffnet werden.","Der Standard-8-Bit-Song läuft ausschließlich im Titelmenü. Im Spiel bleiben Radio und Effekte. Regelmäßige Düsenjäger sind 15 % leiser.","Dynamische Physikschritte holen bei schwankenden Mobile-FPS Rückstand auf; die Jet-HUD-Synchronisation wurde entlastet, ohne Weltumfang zu streichen."]},{"date":"2026-10-12","title":"Voice-Chat für Maus, Tastatur und Xbox-Controller","items":["PC: Mikrofon mit H gedrückt halten, Stummschaltung mit K umschalten. Grössere Voice-Felder zeigen die Belegung direkt im HUD und sind mit der Maus bedienbar.","Xbox: rechten Stick (R3) gedrückt halten zum Sprechen, R3 halten und Y drücken zum Stummschalten. Y wechselt dabei keine Waffe.","Die Voice-Steuerung nutzt in SOLO und MULTI dieselbe Logik. Beim Loslassen, im Menü und beim Controller-Verlust endet die Mikrofonaufnahme zuverlässig."]},{"date":"2026-10-12","title":"Voice-Buttons weichen Fahrzeug-Aktionen aus","items":["Erscheint beim Gehen spontan EINSTEIGEN für ein Auto oder einen Jet, bleiben Mikrofon und Stumm-Taste sichtbar. Sie suchen einen freien Platz am Rand statt zu verschwinden.","Auch SCORE/FLOW, Waffen und Daumensteuerung werden bei der Positionierung berücksichtigt; die Mitte bleibt zum Zielen frei."]},{"date":"2026-10-12","title":"Mobile: Voice bleibt frei von Diagnose-Einblendungen","items":["Das kleine Build-/Online-Label überdeckt beim Spielen keine Mikrofon- und Stumm-Taste mehr. Die Versionsprüfung läuft im Hintergrund weiter."]},{"date":"2026-10-12","title":"Voice-Chat: gleiche Bedienung in SOLO und MULTI","items":["Mikrofon (Push-to-Talk) und Stummschaltung bleiben auch allein im Spiel sichtbar und nutzbar, unabhängig vom Multiplayer-Modus.","SOLO startet keine öffentliche Mitspielersuche. Erst MULTI verbindet öffentlich; Voice überträgt nur an tatsächlich verbundene Mitspieler in Reichweite.","Das Mikrofon öffnet sich nur bei gedrücktem PTT und wird beim Loslassen wieder vollständig freigegeben. Ein RESET ändert die gewählte Stummschaltung nicht.","Die mobilen Voice-Tasten meiden Bewegungs-, Waffen-, Granaten- und Menütasten, damit die Sicht zum Spielen frei bleibt."]},{"date":"2026-10-12","title":"Mobile: kompakte Hochformat-Tasten sauber getrennt","items":["Auch bei 360 Pixel breiten Handys mit gedrehter Spielansicht berühren sich FLOW und GRANATE nicht mehr; die Tasten und die freie Sicht bleiben an ihrem Platz."]},{"date":"2026-10-12","title":"Mobile: mehr Sicht, weniger HUD-Überlagerungen","items":["Schmale Querformate und Safari-Browserleisten: STRIKE und GRANATE verdecken keine Punkte- oder FLOW-Anzeige mehr. Auf besonders flachen Displays tritt die optionale Punkteanzeige zurück.","Im Multiplayer verschwindet die doppelte Solo-Punkteanzeige; Mitspieler-Namensschilder bleiben über den ARM-, STOP- und Daumentasten.","Die mobile Sichtprüfung kontrolliert zusätzlich Hoch- und Querformat, kurze Bildschirme, Drohne, Fußmodus und die freie Bildschirmmitte."]},{"date":"2026-10-12","title":"PC: Waffenwechsel beim Start zuverlässig sichtbar","items":["Auf PCs ohne Touchscreen startet die passende Maus-/Tastatur-Steuerung auch dann korrekt, wenn der Browser den Mauszeiger nicht als primäres Eingabegerät meldet.","Die Waffenwahl wird dadurch nicht mehr versehentlich wie auf einem Handy ausgeblendet; der automatische Desktop-Test prüft jetzt auch den tatsächlichen Eingabemodus."]},{"date":"2026-10-12","title":"Gebäudezerstörung und Kollisionen repariert","items":["Komplett zerstörte Häuser blockieren nicht mehr unsichtbar: Ihre alten Physik-Körper werden vollständig entfernt.","Beim Nachladen von Häusern bleiben Zerstörung und beschädigte Dächer erhalten – auch wenn sich die Kartenkacheln ändern.","Wände und Dächer werden in der Physik nach Beschädigung korrekt neu aufgebaut. RESET stellt unbeschädigte Gebäude wieder her.","Stabilere Box3D-Kollisionen durch lokale Gebäude-Geometrie und präzise Aktualisierung beim Fahren und Fliegen."]},{"date":"2026-10-12","title":"Handy: kleine Fahrzeugtasten am Daumen","items":["Auto und Jet erscheinen als zwei getrennte runde Symbole über dem rechten Stick – auch wenn beide in Reichweite sind. Die Bildschirmmitte bleibt frei, keine Bedienelemente überlappen."]},{"date":"2026-10-12","title":"Desktop: erster Rechtsklick aktiviert Zielen und Schießen","items":["Wer zuerst RMB zum Zielen gedrückt hält, aktiviert damit die Maussteuerung und kann sofort mit LMB schießen. Schießen und Zielen bleiben unabhängig; auch der erste Schuss geht nicht verloren."]},{"date":"2026-10-12","title":"Jet-Einstieg per F und freiere Mobile-Sicht","items":["F steigt zu Fuß in einen erreichbaren Jet ein. Die Zombie-Reparatur blockiert F nur, wenn sie wirklich möglich ist.","Jet- und Auto-Einstieg liegen auf dem Handy an den Rändern statt vor dem Fadenkreuz; Sprachknöpfe weichen ihnen aus."]},{"date":"2026-10-12","title":"Mobile: Raketen-Abschuss und Waffenwahl stabil","items":["GUN, ROCKETS und GUIDED haben jetzt einen eindeutigen Schussweg. Ein Raketenabschuss nutzt zuverlässig die feste Bildschirmrichtung und funktioniert erst nach Waffenwahl und Freigabe."]},{"date":"2026-10-12","title":"Handy: HUD bleibt außerhalb der Daumen und der Bildmitte","items":["Voice-Knöpfe erscheinen nur im verbundenen Multiplayer und finden am linken oder rechten Bildschirmrand einen freien Platz knapp über den Sticks. Alle Button-Ziele bleiben getrennt; der Moduswechsel hat jetzt Abstand zum linken Stick."]},{"date":"2026-10-12","title":"Desktop: Schießen während Zielen repariert","items":["Rechte Maustaste zum Zielen gedrückt halten und gleichzeitig mit links schießen funktioniert wieder – zu Fuß und in der Drohne. Auch Fäuste behalten getrennte linke und rechte Schläge."]},{"date":"2026-10-11","title":"HUD aufgeräumt: Regeln ins Menü, Voice oben","items":["Abstimmungen erscheinen nur noch im WORLD-Menü bzw. der Pause-Menü-Sektion; sie verdecken nicht mehr das Spiel. PTT und Stummschaltung sind runde Symbole oben, außerhalb der Bewegungssticks."]},{"date":"2026-10-11","title":"Multiplayer: Sprachchat in der Nähe","items":["Halte 🎙 PTT oder V zum Sprechen gedrückt; Mikrofon erst nach Freigabe. Nahe Mitspieler hören dich leiser mit zunehmender Entfernung; MUTE schaltet ab."]},{"date":"2026-10-11","title":"Multiplayer: RULES-Abstimmung","items":["Änderungen an Friendly Fire brauchen jetzt die Zustimmung aller verbundenen Spieler. Bei NEIN, Zeitablauf oder Spielerwechsel bleibt die bisherige Regel bestehen."]},{"date":"2026-10-11","title":"Multiplayer: Friendly Fire","items":["Im WORLD-Menü kann der erste Spieler Friendly Fire einschalten oder ausschalten. Ohne Freigabe verletzen Waffen keine Mitspieler."]},{"date":"2026-10-11","title":"GUIDED: fairer und ausweichbar","items":["Lenkraketen sind langsamer, schwächer und seltener. Lock-on erhält eine sichtbare Warnung; sprintende Fußgänger können die Zielverfolgung abschütteln."]},{"date":"2026-10-11","title":"NUKE vollständig entfernt","items":["Atomwaffen sind aus Drohne, Raketenwerfer, Menüs und WORLD-Regeln entfernt. Normale Explosionen und Gebäudeschaden bleiben erhalten."]},{"date":"2026-10-11","title":"Minimap: Double-Tap auch zu Fuß","items":["Die Minimap lässt sich per Doppeltipp jetzt auch zu Fuß und in der First-Person-Ansicht vollständig vergrößern; Auto, Jet und Drohne bleiben unverändert."]},{"date":"2026-10-11","title":"Multiplayer: Host-Verbindung stabilisiert","items":["Beim Beitreten wird die Host-Reihenfolge nach Aufbau der Verbindung bestätigt; alle Spieler erkennen denselben Host."]},{"date":"2026-10-11","title":"Multiplayer: Erster Spieler leitet die Runde","items":["Der zuerst beigetretene Spieler behält die Spielleitung – unabhängig von seiner Netzwerk-ID. Beim Verlassen übernimmt der nächste Spieler."]},{"date":"2026-10-11","title":"Multiplayer: Spawn bei der Spielergruppe","items":["Neue Mitspieler erscheinen bei der groessten aktiven Spielergruppe statt an einer weit entfernten Startposition."]},{"date":"2026-10-11","title":"Multiplayer: alle Spieler suchen dieselbe Lobby","items":["Die automatische Spielersuche verwendet nun denselben öffentlichen Raum statt unterschiedliche GPS- oder Zeit-Räume."]},{"date":"2026-10-11","title":"Live-Prüfung: Gelände, Update und Handy-Steuerung","items":["Die Live-Prüfung testet echte Geländehöhen und prüft den Update-Hinweis nach einer neuen Versionsabfrage zuverlässig.","Handy- und Desktop-Tests prüfen die sichtbaren Waffen und den gedrehten MODE-Knopf in der tatsächlichen Bediengröße."]},{"date":"2026-10-11","title":"Terrain: Live-Prüfung der echten Karte","items":["Gelände und Moduswechsel werden zusätzlich direkt auf der veröffentlichten echten Karte geprüft, einschließlich Update-Hinweis für laufende Spiele."]},{"date":"2026-10-11","title":"Terrain: stabile Höhen beim Nachladen","items":["Unvollständige Höhendaten verbiegen den Boden nicht mehr; Straßen und Bodenkollision folgen gemeinsam der aktuellen Höhe.","Beim Wechsel der Stadt werden verspätete Höhen verworfen. Auch weit entfernte Mitspieler behalten dieselbe Höhenreferenz."]},{"date":"2026-10-11","title":"Terrain: Boden erscheint direkt beim Moduswechsel","items":["Der echte Boden wird sofort geladen; Satellitenfarben folgen später, ohne Bodenlöcher oder weiße Flächen beim Umschalten."]},{"date":"2026-10-11","title":"Multiplayer: flüssigere Auto- und Jet-Marker","items":["Die Symbole anderer Autos und Jets bewegen sich auf der Karte ruhiger und springen bei einem Fahrzeugwechsel nicht quer über die Straße."]},{"date":"2026-10-11","title":"Multiplayer: Mitspieler auch außerhalb der Karte finden","items":["Pfeile am Kartenrand zeigen jetzt in die tatsächliche Richtung des Mitspielers – nicht immer nach Nordost."]},{"date":"2026-10-11","title":"AR: Wegpfeile bleiben beim Umschauen auf der Strecke","items":["Die 3D-Wegpfeile folgen jetzt jeder Kamerabewegung, während die Straßenroute sparsam berechnet wird."]},{"date":"2026-10-11","title":"AR: sichere Routensuche und Abbiegehinweise","items":["Beim Wechsel des Ziels oder Fahrmodus überschreibt keine verspätete Routenantwort die neue Strecke.","Die Navigation kündigt Links- und Rechtskurven an, ohne jeden kleinen Straßenknick als Abzweigung zu behandeln."]},{"date":"2026-10-11","title":"AR: Wegpfeile entlang der echten Straßen","items":["Beim Gehen und Fahren zeigen mehrere Pfeile entlang der berechneten Strecke den Weg.","Drohne und Jet bleiben bei der direkten Zielführung; die Mobile- und Desktop-Tests wurden an den neuen Bedienungsaufbau angepasst."]},{"date":"2026-10-11","title":"Multiplayer: zwei Spieler in einem Jet","items":["Ein Spieler fliegt als Pilot, ein zweiter kann als Passagier mitfliegen; die Sitzplätze werden innerhalb der Mehrspieler-Runde abgestimmt.","Alle Mitspieler erscheinen auf der großen Karte; die Passagierfigur wird auch im Flug am Jet angezeigt."]},{"date":"2026-10-11","title":"Multiplayer: Jet bleibt stehen, Mitspieler auf der Minimap","items":["Geparkte Jets springen nicht mehr ständig zur Seite, wenn man einsteigen will.","Alle verbundenen Spieler werden auf der Minimap mit Position und Blickrichtung angezeigt."]},{"date":"2026-10-10","title":"Flüssiger im Jet und bei hohem Tempo","items":["Häuser-Kollisionen werden beim schnellen Fliegen und Fahren nicht mehr komplett neu gebaut: unveränderte Häuser behalten ihre Physik, nur neue kommen dazu (vorher alle paar hundert Millisekunden alles, in drei Physik-Welten).","Grundrisse werden nur einmal trianguliert; Bäume werden während des Jetflugs nicht ständig neu verteilt.","Live-Anzeigen (Jet-HUD, Navigation) lösen keine Neu-Scans der Oberfläche mehr aus – vorher liefen mehrere DOM-Beobachter pro Bild.","EXIT und MULTI überlappen am PC nicht mehr."]},{"date":"2026-10-10","title":"Vollbild-Karte mit Zielsuche, einheitliche Bedienung, Jet-HUD","items":["Vollbild-Karte wie in GTA: Minimap doppelt antippen, M oder VIEW gedrückt halten (Xbox). Ziehen, zoomen, tippen = Wegpunkt; ✕ / M / Esc / Ⓑ schließt.","Ziel suchen: Adresse eintippen (z. B. „Dorfstraße 8, Kirchberg an der Jagst“) – Route über Straßen (zu Fuß über Fußwege), dezenter Richtungspfeil am Horizont und eine Zeile mit Entfernung. Ohne Ziel wird nichts angezeigt.","Überall gleich: EXIT oben links (mit Nachfrage), MULTI direkt daneben. Der Modus-Knopf sitzt unten links am Daumen und zeigt, wohin er wechselt: TO DRONE, GO ON FOOT, EJECT.","Jet: grünes, dezentes HUD – Stadtname mit Linie zum Stadtzentrum, Geschwindigkeit links, Höhe rechts, Flugbahnmarke. Auf Touch schießt die Kanone dorthin, wo du tippst.","Jets stehen nicht mehr in Häusern: der Abstellplatz braucht freien Platz für Flügel und Heck und rückt nach, wenn Häuser nachladen.","Zielfernrohr: feineres Absehen mit Vergrößerungsanzeige; Einschläge, Splitter und Blut bleiben auch auf große Entfernung sichtbar.","Bäume stehen nicht mehr in Häusern oder auf der Fahrbahn.","Schnellerer Start: die Umgebungsbeleuchtung braucht den schweren Himmels-Shader nicht mehr."]},{"date":"2026-10-10","title":"Polizei rückt aus, echte Sirenen","items":["Randale zu Fuß ruft die Polizei: Schüsse in der Nähe von Leuten, Faustschläge und Messerstiche bringen Sterne – Streifenwagen und Polizeidrohnen kommen.","Streifenwagen lassen sich zerschießen: sie qualmen, brennen und explodieren.","Echte Sirenen: Streifenwagen mit deutschem Martinshorn (Tatütata), Polizeidrohnen mit leisem elektronischem Heulton – räumlich, mit Doppler, dezent im Mix."]},{"date":"2026-10-10","title":"Steuerung wie CoD, schlauere Fahrer und Passanten","items":["Zielen wie in Call of Duty: Mobile-Daumen 1:1 ohne Totzone und schneller, PC-Maus roh ohne Beschleunigung, Xbox mit „Dynamic“-Kurve.","Ein Glock-Schuss sprengt kein Auto mehr; keine Kettenexplosionen die Straße runter.","Zombies und Passanten in deiner Nähe sind keine grauen Blöcke mehr.","Fahrer halten Abstand, bremsen vor Kurven, hupen seltener. Gerammt oder beschossen: Panik (Rückwärtsgang, Vollgas weg), Angriff (versuchen dich zu überfahren) oder Wutbürger steigt aus – manchmal mit Messer oder Pistole.","Passanten hören jeden Schuss in der Nähe, sprinten erst los, laufen nicht mehr durch Wände, prügeln sich manchmal untereinander und flüchten vor tief fliegenden Drohnen und Jets.","Fäuste treffen zuverlässig (auch Hunde, Katzen, Kühe), Tiere fliegen nicht mehr übers Eis und stehen wieder auf; die schwarze Katze lebt nach ihrem Angriff weiter.","Neue, rundere Fäuste."]},{"date":"2026-10-10","title":"Nukes per Schalter, LIVE GPS, Kühe, Boxen","items":["Hauptmenü → WORLD: DRONE NUKE und LAUNCHER NUKE (Standard aus). Im Multiplayer gelten die Schalter des Hosts.","Raketenwerfer feuert ohne Nuke normale Raketen – mit LAUNCHER NUKE die Atombombe.","Changelog im Hauptmenü.","Drohnen-LENKRAKETE: Ziel markieren (Mensch, Tier, Vogel, Auto, Drohne), nochmal drücken – Marschflugkörper mit Triebwerkssound, Reiseflug und Sturzflug.","Autoradio wie in GTA – mit den echten Lokalsendern der Region (am Bodensee die Bodensee-Sender). 📻 / N / B / Steuerkreuz.","Die Laufgruppe: ab und zu joggt eine Gruppe im Gleichschritt mit eigenem Schlachtruf durch die Straßen – mit Streak fürs Überfahren.","Messer als eigene Waffe (Taste 7, im Waffenwechsel nach den Fäusten).","Gangster: wenige Passanten tragen ein Messer, ganz selten eine Pistole – und kommen auf dich zu, statt wegzurennen.","RESET räumt alles auf: Laternen, Kühe, Autos, Passanten, Kisten, Granaten in der Luft.","Explosionsspuren dezenter; Ruß und Löcher verschwinden mit der eingestürzten Wand.","Raketen im Flug und Leuchtspur werfen echtes Licht; Asphalt hat Flicken und Körnung statt einer Einheitsfarbe.","Auto-Innenansicht: die Frontscheibe ist das Bild – schmale Säulen, flaches Armaturenbrett, Motorhaube in Wagenfarbe, Lenkrad unten.","LIVE GPS (Taste L): die Welt folgt deiner echten Position – Joggen, Rad, Auto. Im Multiplayer sehen die anderen dich laufen, Rad fahren (mit Fahrrad) oder im Auto.","Kühe auf den echten Weiden: physisch, treffbar, mit Muhen und Schmerzlaut.","Passanten haben Temperament: meist ängstlich, manche rennen weg, wenige schlagen zurück; sie schreien, fluchen und beruhigen sich wieder.","Panik nur in der Nähe von Schüssen – nicht mehr die ganze Stadt.","Boxen: zwei unabhängige Fäuste, Kombos, Wusch- und Treffergeräusche; Tiere reagieren auf Schläge.","Handgranaten (T / LB): Stift ziehen, kochen, werfen.","Eine Granate anschießen zündet sie sofort; Werfergranaten explodieren an Gebäuden, Menschen, Laternen und Tieren, auf dem Boden springen sie.","Echtes Licht von Laternen, Scheinwerfern, Mündungsfeuer, Explosionen und Jetpack.","Jetpack mit Zündung, Schubdröhnen und 3D-Sound anderer Spieler.","Die schwarze Katze faucht und kreischt beim Angriff.","Mobile: kleinere Sticks in den Ecken, nur ein Waffenknopf, JUMP links davon, Scope darüber; Touch-Schüsse gehen exakt auf den Finger."]},{"date":"2026-10-09","title":"Fäuste, Autos, Multiplayer-Körper","items":["Fäuste für dich – und für Passanten, die genug haben.","Jedes Auto, in das du steigst, fährt; Cockpit-Ansicht (C).","Drohne feuert Raketensalven; Werferraketen explodieren beim Aufprall.","Tag und Nacht sind im Multiplayer für alle gleich; Esc fragt „zurück ins Hauptmenü?“.","Multiplayer: Trefferzonen, Ragdolls mit Todeskamera, Jetpack repliziert, Mitspieler überfahren, Gravity Gun auf alles.","Klassische Glock, saubere Luftschlag-Zielwahl, Reset-Abstimmung per Desktop und Pad."]},{"date":"2026-10-08","title":"Laternen, Zombie-Nächte, Polizei, Performance","items":["Straßenlaternen an jeder echten Straße mit Lichtkegeln bei Nacht.","Zombie-Nächte, Schwingtüren mit Barrikaden.","Reset: allein = ganzes Level zurück; mit anderen = Abstimmung (ein Nein bricht ab).","GTA-Polizei am Boden: Streifenwagen jagen dich, Beamte steigen aus und schießen.","Kampfjets mit Luftschlag; ferne Passanten als Impostor.","Autos mit echter Federung; geparkte Autos bleiben stehen und sind repliziert.","Glasfenster von innen, Einschussrisse in Scheiben, stabile Schatten.","3D-Kreuzblick-Modus in den Einstellungen.","Performance: Boden in 64 Chunks, Straßennetz viel leichter, nur sichtbare Menschen werden gezeichnet."]},{"date":"2026-10-06","title":"OPPENHEIMER","items":["Neuer Name OPPENHEIMER; als App auf den Home-Bildschirm installierbar, immer der neueste Stand.","Klassische Pilzwolke; jede Explosion kann Gebäude zerstören.","Drohne regeneriert nach Zerstörung; Panik in der Fallout-Zone.","Nahtloser Übergang Menü → Spiel, Menü-Flyover, Sound-Schalter.","Performance: eine 3D-Ansicht statt zwei, weniger Speichermüll in Verkehr und Physik."]},{"date":"2026-10-02","title":"Nuke und Mobile-Multiplayer","items":["Drohnen-Nuke mit Druckwelle, Kamerabeben und Pilzwolke.","Drohnen-Waffenwahl auf dem Desktop; STOP- und EMP-Knopf, Reset auf Mobile.","Mobile-Multiplayer findet Mitspieler zuverlässig."]},{"date":"2026-08-24","title":"Zu Fuß in der Ego-Perspektive","items":["Aussteigen und zu Fuß: Dual-Stick-Steuerung, Pistole und MP, Touch-Feuer.","Xbox-Controller: klassischer Flug- und Zielmodus.","Belebte Stadt: Passanten, Verkehr und Tiere.","Audio-Einstellungen; realistische FPV-Kamera."]},{"date":"2026-08-17","title":"Die echte Welt","items":["Echte Luftbilder und Gebäude mit Kollision auf der WORLD-Karte.","Immer Querformat, Minimap von oben, Xbox-Steuerung."]},{"date":"2026-08-12","title":"Multiplayer VS","items":["Mitspieler in der Nähe und im WLAN finden sich automatisch; verschlüsselte Verbindung.","Kampf-HUD, Explosionen und Respawn."]},{"date":"2026-08-10","title":"Hardware-treuer Drohnensimulator","items":["Flugregler wie auf dem echten ESP32 (1 kHz), GAME-Modus bis 90 km/h.","Stick-Invertierungen, Schüsse exakt auf den Touchpunkt, Einschusslöcher."]}];

const SCUI_CSS = `
/* neon_ui_theme.mjs: chunky rounded buttons, translucent navy panels, white bold text, one warm accent */
.scui-btn{min-height:40px;padding:10px 18px;border-radius:10px;border:1px solid #ffffff40;background:#ffffff14;color:#eef0f2;font:800 13px/1 Inter,system-ui,sans-serif;letter-spacing:.2em;cursor:pointer;text-transform:uppercase}
.scui-btn:hover,.scui-btn:focus-visible{background:#ffffff26;outline:none}
.scui-btn:focus-visible,body.scui-pad .scui-btn:focus,body.scui-pad .scui-row:focus,body.scui-pad input:focus{outline:3px solid #ffd76a;outline-offset:2px}
.scui-hot{background:linear-gradient(180deg,#ffb02e,#ff7a1a);color:#16120c;border:0}
#scMenu{position:fixed;inset:0;z-index:50;display:flex;align-items:safe center;justify-content:safe center;overflow-y:auto;background:radial-gradient(ellipse at 50% 40%,#0b0d10a8,#0b0d10f0);color:#eef0f2;font-family:Inter,system-ui,sans-serif;touch-action:manipulation}
#scMenu[hidden],.scui-panel[hidden],#scQuit[hidden]{display:none!important}
#scMenu .gm-center{display:flex;flex-direction:column;align-items:center;gap:clamp(8px,2vh,16px);padding:24px;margin:auto;text-align:center}
#scMenu .gm-title{position:relative;display:flex;flex-direction:column;align-items:center;font-weight:900;font-style:italic;font-size:clamp(34px,min(10vw,15vh),120px);letter-spacing:.02em;line-height:.9;color:#f4f5f6;text-shadow:0 3px 0 rgba(0,0,0,.55),0 10px 30px rgba(0,0,0,.6);padding:0 .08em .16em}
#scMenu .gm-title::after{content:"";position:absolute;left:4%;right:4%;bottom:.02em;height:.09em;background:linear-gradient(90deg,transparent,#ff7a1a 18%,#ffb02e 60%,transparent);transform:skewX(-24deg);box-shadow:0 0 18px rgba(255,122,26,.55)}
#scMenu .gm-sub{font-weight:700;font-size:clamp(11px,1.6vw,15px);letter-spacing:.42em;color:rgba(238,240,242,.78)}
#scStart{min-width:min(64vw,300px);padding:15px 28px;font:700 clamp(17px,2.4vw,22px) Inter,system-ui,sans-serif;letter-spacing:.32em;color:#16120c;background:linear-gradient(180deg,#ffb02e,#ff7a1a);border:0;border-radius:4px;box-shadow:0 6px 18px rgba(0,0,0,.55),inset 0 -3px 0 rgba(0,0,0,.18);cursor:pointer;clip-path:polygon(4% 0,100% 0,96% 100%,0 100%)}
#scStart[disabled]{opacity:.7;cursor:default;filter:saturate(.4)}
#scMenu .gm-wide{min-width:min(64vw,300px)}
#scMenuStatus{min-height:1.2em;font-size:11px;font-weight:700;letter-spacing:.3em;color:rgba(238,240,242,.62)}
.scui-panel{position:fixed;inset:0;z-index:60;display:flex;align-items:center;justify-content:center;background:#0b0d10d0;backdrop-filter:blur(4px);font-family:Inter,system-ui,sans-serif;color:#eef0f2}
.scui-card{width:min(94vw,540px);max-height:calc(100dvh - 32px);overflow:auto;padding:16px 16px 12px;border:1px solid #ffffff2e;border-radius:14px;background:#12161ce8;text-align:left;box-shadow:0 12px 40px #000a}
.scui-card h2{margin:0 0 4px;font:900 italic 22px/1 Inter,system-ui,sans-serif;letter-spacing:.06em;color:#ffb02e}
.scui-card .scui-sub{margin:0 0 12px;font:600 12px/1.35 system-ui,sans-serif;color:#b9c3cc}
.scui-tabs{display:flex;flex-wrap:wrap;gap:6px;margin:0 0 10px}
.scui-tabs button{padding:7px 10px;border-radius:8px;border:1px solid #ffffff30;background:#162437;color:#fff;font:850 11px/1 system-ui;letter-spacing:.1em;cursor:pointer}
.scui-tabs button[aria-selected="true"]{border-color:#ffb02e;background:#4a3212}
.scui-row{display:grid;grid-template-columns:1fr auto;align-items:center;gap:4px 10px;width:100%;margin:0 0 6px;padding:9px 11px;border:1px solid #ffffff1f;border-radius:10px;background:#ffffff0a;color:#eef0f2;box-sizing:border-box;font:inherit;text-align:left}
.scui-row b{font:850 13px/1.1 system-ui,sans-serif;letter-spacing:.06em}
.scui-row small{grid-column:1/-1;font:500 11.5px/1.3 system-ui,sans-serif;color:#aab4be}
.scui-row output{font:900 12px ui-monospace,SFMono-Regular,Menlo,monospace;color:#ffd36a}
.scui-row input[type=range]{grid-column:1/-1;width:100%;accent-color:#ff8a1f}
.scui-row.scui-toggle{cursor:pointer}
.scui-row.scui-toggle i{width:44px;height:24px;border-radius:12px;background:#3a414b;position:relative}
.scui-row.scui-toggle i::after{content:"";position:absolute;top:3px;left:3px;width:18px;height:18px;border-radius:50%;background:#dfe5ea}
.scui-row.scui-toggle[aria-checked="true"] i{background:#ff8a1f}.scui-row.scui-toggle[aria-checked="true"] i::after{left:23px}
.scui-row.scui-soon{opacity:.55}
.scui-actions{display:flex;flex-wrap:wrap;gap:8px;margin-top:10px}.scui-actions .scui-btn{flex:1}
.scui-log article{padding:9px 11px;margin:0 0 8px;border:1px solid #ffffff1f;border-radius:10px;background:#ffffff0a}
.scui-log time{font:750 10px ui-monospace,monospace;color:#91a7bc}.scui-log b{display:block;margin:2px 0 4px;font:850 13px system-ui}
.scui-log li{font:500 12px/1.4 system-ui;color:#d5dde5}
#scQuit{position:fixed;inset:0;z-index:70;display:flex;align-items:center;justify-content:center;background:#0006}
#scQuit .qc-box{background:#11151bf2;border:1px solid #ffffff33;border-radius:14px;padding:18px 22px;color:#fff;font:600 14px/1.4 Inter,system-ui,sans-serif;min-width:260px;text-align:center;box-shadow:0 12px 40px #000a}
#scQuit b{display:block;font:900 17px/1.2 Inter,system-ui,sans-serif;letter-spacing:.08em;margin-bottom:12px}
#scQuit .qc-row{display:flex;gap:12px;justify-content:center}#scQuit button{min-width:104px}
#scQuit small{display:block;margin-top:10px;opacity:.7;font-size:11px}
/* game_exit_button.mjs / mobile_gameplay_ui.mjs: EXIT, MENU, RESET top left, in reach and off the picture */
.scui-corner{position:fixed;z-index:20;left:max(10px,env(safe-area-inset-left));top:max(10px,env(safe-area-inset-top));display:flex;gap:6px}
.scui-corner button{min-height:30px;padding:6px 10px;border-radius:8px;border:1px solid #ffffff2a;background:#0a0d12a0;color:#fff;font:800 11px/1 Inter,system-ui,sans-serif;letter-spacing:.12em;cursor:pointer}
.scui-corner[hidden]{display:none}
body.scui-playing .sc-top{top:calc(max(12px,env(safe-area-inset-top)) + 38px)}
#scPlace{display:flex;align-items:center;gap:8px;max-width:min(86vw,420px);padding:8px 14px;border-radius:999px;border:1px solid #ffffff40;background:#0a0d12b0;color:#fff;font:700 12px/1.2 Inter,system-ui,sans-serif;letter-spacing:.06em;cursor:pointer}
#scPlace b{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}#scPlace i{font-style:normal;color:#ffb02e;font-size:10px;letter-spacing:.14em}
#scPlacePanel input{width:100%;box-sizing:border-box;margin:0 0 10px;padding:11px 12px;border-radius:10px;border:1px solid #ffffff38;background:#0c1118;color:#fff;font:600 14px system-ui}
#scPlacePanel .sp-item{display:block;width:100%;margin:0 0 6px;padding:9px 12px;border-radius:10px;border:1px solid #ffffff1f;background:#ffffff0a;color:#eef0f2;text-align:left;cursor:pointer;font:inherit}
#scPlacePanel .sp-item b{display:block;font:800 13px/1.2 system-ui}#scPlacePanel .sp-item small{color:#aab4be;font:500 11.5px/1.3 system-ui}
#scPlacePanel .sp-on{border-color:#ffb02e}#scPlacePanel h4{margin:10px 0 6px;font:900 10px system-ui;letter-spacing:.14em;color:#8fe8ff}
#scGps{position:fixed;z-index:21;left:50%;top:calc(max(10px,env(safe-area-inset-top)) + 46px);transform:translateX(-50%);display:flex;gap:12px;align-items:center;padding:8px 12px;border-radius:12px;background:#0d1117d9;border:1px solid #4285f4aa;color:#fff;font:700 12px/1.2 Inter,system-ui,sans-serif}
#scGps[hidden]{display:none}#scGps b{font-size:15px;color:#9cc2ff}#scGps span{color:#b8c2cc}#scGps button{padding:6px 9px;border-radius:8px;border:1px solid #ffffff38;background:#162437;color:#fff;font:800 10px system-ui;letter-spacing:.1em;cursor:pointer}
#scBanner{position:fixed;z-index:21;left:50%;top:calc(max(10px,env(safe-area-inset-top)) + 54px);transform:translateX(-50%);padding:8px 16px 7px;border-radius:12px;background:#0d1117d9;border:1px solid #ffffff2a;color:#fff;text-align:center;pointer-events:none;transition:opacity .35s;max-width:min(80vw,420px)}
#scBanner b{display:block;font:900 15px/1.15 Inter,system-ui,sans-serif;letter-spacing:.05em;color:#ffd36a;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}#scBanner small{font:700 11px/1.3 system-ui;color:#b8c2cc}
#scRadio{position:fixed;z-index:21;right:max(14px,env(safe-area-inset-right));top:calc(max(12px,env(safe-area-inset-top)) + 160px);width:44px;height:44px;border-radius:50%;border:1px solid #ffffff38;background:#0a0d12b0;font-size:20px;cursor:pointer}
#scRadio[hidden],#scJet[hidden]{display:none}
#scJet{position:fixed;z-index:19;left:50%;bottom:calc(max(34px,env(safe-area-inset-bottom) + 24px) + 12px);transform:translateX(-50%);width:min(160px,30vw);height:4px;border-radius:2px;background:#ffffff26;overflow:hidden}
#scJet i{display:block;height:100%;background:linear-gradient(90deg,#ff8a2a,#ffd36a)}#scJet.burn i{background:#ff5a2a}
/* jetpack_audio.mjs: a warm orange flash at the screen edges at the ignition (with the original's sound, audio.mjs, jetpack_audio.mjs shows its own) */
#scJetFlash{position:fixed;inset:0;z-index:18;pointer-events:none;box-shadow:inset 0 0 90px 30px #ff8a2acc;opacity:0;transition:opacity .45s ease-out}#scJetFlash.on{opacity:1;transition:none}
/* player_settings_sections.mjs: what you control right now */
.scui-modes{display:grid;grid-template-columns:repeat(4,1fr);gap:6px;margin:0 0 10px}.scui-modes button{min-height:34px;border:1px solid #ffffff38;border-radius:8px;background:#162437;color:#fff;font:850 10px/1 system-ui;letter-spacing:.08em;cursor:pointer}
.scui-modes button[data-active="1"]{border-color:#71e5b2;background:#18503d}
/* game_exit_button.mjs: the mode switch bottom left, labelled with where it takes you */
#scModeBtn{position:fixed;z-index:20;left:max(14px,env(safe-area-inset-left));bottom:calc(max(34px,env(safe-area-inset-bottom) + 24px) + 120px);padding:8px 12px;border-radius:10px;border:1px solid #ffffff38;background:#0a0d12b0;color:#fff;font:850 11px/1 Inter,system-ui,sans-serif;letter-spacing:.12em;cursor:pointer}
#scModeBtn[hidden]{display:none}
.scui-log .lb-stats{font:600 11.5px/1.4 ui-monospace,monospace;color:#c7d2dc}
/* drone_simulator.html: sound / music switches top right, HOME SCREEN top left of the title, TEILEN under CHANGELOG */
.scui-switches{position:fixed;z-index:101;top:max(10px,env(safe-area-inset-top));right:max(10px,env(safe-area-inset-right));display:flex;gap:6px}
body.scui-playing .scui-switches{right:auto;left:calc(max(10px,env(safe-area-inset-left)) + 196px)}
.scui-switches button{width:40px;height:32px;display:flex;align-items:center;justify-content:center;padding:0;background:rgba(10,13,18,.56);border:1px solid rgba(255,255,255,.16);border-radius:6px;color:#fff;cursor:pointer}
.scui-switches svg{width:20px;height:20px}.scui-switches [data-on="0"] .ss-wave,.scui-switches [data-on="1"] .ss-x,.scui-switches [data-on="1"] .ms-x{display:none}.scui-switches [data-sw="music"][data-on="0"]{color:rgba(255,255,255,.55)}
#scInstall{position:fixed;z-index:2147480500;top:max(10px,env(safe-area-inset-top));left:max(10px,env(safe-area-inset-left));height:32px;display:flex;align-items:center;gap:7px;padding:0 11px;background:rgba(10,13,18,.56);border:1px solid rgba(255,255,255,.16);border-radius:6px;color:#fff;font:600 11px Inter,system-ui,sans-serif;letter-spacing:.12em;cursor:pointer}
#scInstall svg{width:14px;height:14px}#scInstall[hidden],#scInstallHint[hidden]{display:none!important}
#scInstallHint{position:fixed;z-index:2147480501;top:calc(max(10px,env(safe-area-inset-top)) + 40px);left:max(10px,env(safe-area-inset-left));max-width:min(80vw,320px);padding:12px 14px;background:#12161cf5;border:1px solid rgba(255,255,255,.16);border-radius:14px;color:#fff;font:600 13px/1.4 system-ui,sans-serif}
#scInstallHint b{color:#f5b301}
#scShareToast{position:absolute;left:50%;bottom:8%;transform:translateX(-50%);padding:8px 14px;border-radius:10px;background:#0d1117e0;border:1px solid #ffffff30;font:800 12px system-ui;letter-spacing:.12em;opacity:0;transition:opacity .3s}#scShareToast[data-on="1"]{opacity:1}
/* suggestion_box.mjs: the card bottom right of the title */
#scIdea{position:absolute;right:max(10px,env(safe-area-inset-right));bottom:max(10px,env(safe-area-inset-bottom));z-index:15;width:min(92vw,340px)}#scIdea[hidden]{display:none}
#scIdea .sg-in{display:flex;flex-direction:column;gap:6px;padding:12px;border-radius:12px;border:1px solid #ffd36b66;background:#141009ee;color:#fff3d2;font:600 12px/1.3 system-ui,sans-serif;box-shadow:0 10px 30px #000a;text-align:left}
#scIdea small{opacity:.75;font-weight:500}#scIdea textarea{resize:none;padding:8px;border-radius:8px;border:1px solid #ffd36b55;background:#0a0805;color:#fff;font:500 14px/1.3 system-ui,sans-serif}
#scIdea .sg-row{display:flex;gap:6px}#scIdea .sg-row button{flex:1;padding:8px 4px;border-radius:8px;border:1px solid #ffffff33;background:#ffffff14;color:#fff;font:800 10px/1 system-ui,sans-serif;letter-spacing:.06em;cursor:pointer}
#scIdea .sg-row button[data-sg=send]{background:linear-gradient(180deg,#ffd36b,#f2a515);color:#1c1300;border:0}
`;

const scuiEsc = s => String(s ?? "").replace(/[&<>"]/g, c => ({"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;"}[c]));
function scuiLoad(key, fallback) { try { return JSON.parse(localStorage.getItem(key) || "null") ?? fallback; } catch { return fallback; } }
function scuiSave(key, value) { try { localStorage.setItem(key, JSON.stringify(value)); } catch { /* private mode */ } }

/** Installs the menus. `command` sends a command to the game and returns its answer. */
export function installSpawnCityUi({command, params, canvas, statusEl}) {
  const style = document.createElement("style");
  style.textContent = SCUI_CSS;
  document.head.appendChild(style);
  const coarse = matchMedia("(pointer: coarse)").matches;
  const ui = {open: [], playing: false, started: false, schema: [], tab: "maus", lockWanted: false, playSeconds: 0};

  // ---- settings: replay the stored copy, then read the table back -------------------------
  const stored = scuiLoad(SCUI_STORE, {});
  const pairs = Object.entries(stored).filter(([k, v]) => /^[a-zA-Z.]+$/.test(k) && Number.isFinite(+v)).map(([k, v]) => `${k} ${+v}`);
  if (pairs.length) command("set " + pairs.join(";"));
  const readSchema = () => { try { ui.schema = JSON.parse(command("settings schema")); } catch { ui.schema = []; } };
  readSchema();
  const setOpt = (key, value) => {
    command(`set ${key} ${value}`);
    const row = ui.schema.find(o => o.key === key);
    if (row) row.value = Number(command(`get ${key}`));
    const copy = scuiLoad(SCUI_STORE, {});
    copy[key] = row ? row.value : value;
    scuiSave(SCUI_STORE, copy);
    return row ? row.value : value;
  };

  // ---- DOM ---------------------------------------------------------------------------------
  const el = (html) => { const t = document.createElement("template"); t.innerHTML = html.trim(); return t.content.firstElementChild; };
  const menu = el(`<div id="scMenu" role="dialog" aria-label="SpawnCity">
    <div class="gm-center">
      <div class="gm-title" aria-label="SpawnCity"><span>SPAWN</span><span>CITY</span></div>
      <div class="gm-sub">DROHNE · AUTOS · JET</div>
      <button id="scStart" type="button">START</button>
      <button id="scPlace" type="button" data-act="place"><span>📍</span><b>MEIN STANDORT (GPS)</b><i>ÄNDERN</i></button>
      <div id="scMenuStatus">STADT WIRD VORBEREITET</div>
      <button class="scui-btn gm-wide" data-act="world" type="button">WELT</button>
      <button class="scui-btn gm-wide" data-act="settings" type="button">EINSTELLUNGEN</button>
      <button class="scui-btn gm-wide" data-act="changelog" type="button">NEU / CHANGELOG</button>
      <button class="scui-btn gm-wide" data-act="share" type="button">TEILEN</button>
    </div><div id="scShareToast"></div></div>`);
  const pause = el(`<div class="scui-panel" id="scPause" hidden><div class="scui-card"><h2>PAUSE</h2><p class="scui-sub">Das Spiel steht, solange dieses Menue offen ist.</p>
    <div class="scui-actions" style="flex-direction:column">
      <button class="scui-btn scui-hot" data-act="resume" type="button">WEITER</button>
      <button class="scui-btn" data-act="settings" type="button">EINSTELLUNGEN</button>
      <button class="scui-btn" data-act="world" type="button">WELT</button>
      <button class="scui-btn" data-act="reset" type="button">RESET</button>
      <button class="scui-btn" data-act="gps" type="button">GPS LIVE</button>
      <button class="scui-btn" data-act="logbook" type="button">LOGBUCH</button>
      <button class="scui-btn" data-act="exit" type="button">HAUPTMENUE</button>
    </div></div></div>`);
  const settings = el(`<div class="scui-panel" id="scSettings" hidden><div class="scui-card"><h2>EINSTELLUNGEN</h2>
    <p class="scui-sub">Jede Aenderung wirkt sofort. Unter jeder Zeile steht, wofuer sie gut ist.</p>
    <div class="scui-modes" aria-label="Was du steuerst"><button type="button" data-mode="foot">ZU FUSS</button><button type="button" data-mode="drone">DROHNE</button><button type="button" data-mode="car">AUTO</button><button type="button" data-mode="jet">JET</button></div>
    <div class="scui-tabs" role="tablist"></div><div class="scui-rows"></div>
    <div class="scui-actions"><button class="scui-btn" data-act="place" type="button">STARTORT</button><button class="scui-btn" data-act="reset-section" type="button">STANDARD</button><button class="scui-btn scui-hot" data-act="close" type="button">FERTIG</button></div></div></div>`);
  const world = el(`<div class="scui-panel" id="scWorld" hidden><div class="scui-card"><h2>WELT</h2>
    <p class="scui-sub">Wer in der Stadt lebt und was du kannst. Ausgeschaltetes gibt es nicht: keine Koerper, keine KI, nichts zu zeichnen.</p>
    <div class="scui-rows"></div><div class="scui-actions"><button class="scui-btn scui-hot" data-act="close" type="button">FERTIG</button></div></div></div>`);
  const log = el(`<div class="scui-panel" id="scChangelog" hidden><div class="scui-card"><h2>NEU</h2><p class="scui-sub">Die wichtigsten Punkte jedes Updates, das neueste zuerst.</p>
    <div class="scui-log">${SCUI_CHANGELOG.map(e => `<article><time>${e.date}</time><b>${scuiEsc(e.title)}</b><ul>${e.items.map(i => `<li>${scuiEsc(i)}</li>`).join("")}</ul></article>`).join("")}
    <h4 class="scui-sub">SPAWNCITY.DE – DAS ORIGINAL</h4>
    ${SCUI_CHANGELOG_ORIGINAL.map(e => `<article><time>${scuiEsc(e.date)}${e.time ? " · " + scuiEsc(e.time) : ""}</time><b>${scuiEsc(e.title)}</b><ul>${(e.items || []).map(i => `<li>${scuiEsc(i)}</li>`).join("")}</ul></article>`).join("")}</div>
    <div class="scui-actions"><button class="scui-btn scui-hot" data-act="close" type="button">SCHLIESSEN</button></div></div></div>`);
  const quit = el(`<div id="scQuit" role="dialog" aria-label="Zurueck zum Hauptmenue" hidden><div class="qc-box"><b>ZURUECK ZUM HAUPTMENUE?</b>
    <div class="qc-row"><button class="scui-btn scui-hot" data-yes type="button">JA</button><button class="scui-btn" data-no type="button">NEIN</button></div>
    <small>Enter / Y = ja · Esc / N = nein</small></div></div>`);
  const idea = el(`<div id="scIdea" role="dialog" aria-label="Vorschlag fuer das naechste Update" hidden><div class="sg-in"><b>💡 DEINE IDEE FÜR DAS NÄCHSTE UPDATE?</b>
    <small>Kurz und knapp – was fehlt, was nervt, was wäre geil? Anonym.</small><textarea maxlength="240" rows="2" placeholder="z. B. Boote auf dem See, mehr Jets, …"></textarea>
    <div class="sg-row"><button type="button" data-sg="send">SENDEN</button><button type="button" data-sg="later">SPÄTER</button><button type="button" data-sg="no">NEIN DANKE</button></div>
    <small class="sg-msg" aria-live="polite"></small></div></div>`);
  const corner = el(`<div class="scui-corner" hidden><button type="button" data-act="exit" aria-label="Zurueck zum Hauptmenue">EXIT</button>
    <button type="button" data-act="pause" aria-label="Menue">MENUE</button><button type="button" data-act="reset" aria-label="Zurueck zum Start">RESET</button></div>`);
  const placePanel = el(`<div class="scui-panel" id="scPlacePanel" role="dialog" aria-label="Startort waehlen" hidden><div class="scui-card"><h2>WO WILLST DU SPIELEN?</h2>
    <input type="search" placeholder="Stadt, Strasse oder Adresse ..." autocomplete="off" enterkeyhint="search" aria-label="Ort suchen">
    <div class="sp-list"></div><small class="scui-sub">Suche: © OpenStreetMap-Mitwirkende (Photon von komoot)</small>
    <div class="scui-actions"><button class="scui-btn scui-hot" data-act="close" type="button">SCHLIESSEN</button></div></div></div>`);
  const gpsCard = el(`<div id="scGps" hidden><b data-k="act">🧍 STEHT</b><span data-k="speed">0 km/h</span><span data-k="dist">0 m</span><span data-k="time">0:00</span>
    <span data-k="acc">± – m</span><button type="button" data-act="gps-view">AUGE</button><button type="button" data-act="gps">STOP</button></div>`);
  const logbook = el(`<div class="scui-panel" id="scLogbook" hidden><div class="scui-card"><h2>LOGBUCH</h2>
    <p class="scui-sub">Jeder Drohnenflug von ARM bis DISARM und die Netzereignisse. Nur auf diesem Geraet gespeichert; fliesst nie in Flugregler oder Physik.</p>
    <div class="scui-log" data-k="flights"></div><h4 class="scui-sub">NETZ / MEHRSPIELER</h4><div class="scui-log" data-k="events"></div>
    <div class="scui-actions"><button class="scui-btn" data-act="lb-export" type="button">EXPORT JSON</button><button class="scui-btn" data-act="lb-clear" type="button">LEEREN</button>
    <button class="scui-btn scui-hot" data-act="close" type="button">SCHLIESSEN</button></div></div></div>`);
  const banner = el(`<div id="scBanner" style="opacity:0"><b></b><small></small></div>`);
  const radioBtn = el(`<button id="scRadio" type="button" data-act="radio" aria-label="Autoradio: naechster Sender" hidden>📻</button>`);
  const jetBar = el(`<div id="scJet" hidden><i></i></div>`);
  const jetFlash = el(`<div id="scJetFlash"></div>`);
  const modeBtn = el(`<button id="scModeBtn" type="button" hidden></button>`);
  document.body.appendChild(jetFlash);
  document.body.appendChild(modeBtn);
  modeBtn.addEventListener("click", ev => { ev.stopPropagation(); const a = modeBtn.dataset.cmd; if (a) command(a); });
  settings.addEventListener("click", ev => {
    const b = ev.target.closest("[data-mode]");
    if (!b) return;
    ev.stopPropagation();
    command(`mode ${b.dataset.mode}`);
    while (ui.open.length) closeTop();
  });
  const switches = el(`<div class="scui-switches">
    <button type="button" data-sw="music" aria-label="Musik an/aus" data-on="1"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round"><path d="M9 17.5V6l10-2v11.5"/><circle cx="6.5" cy="17.5" r="2.5"/><circle cx="16.5" cy="15.5" r="2.5"/><path class="ms-x" d="M4 4l16 16"/></svg></button>
    <button type="button" data-sw="sound" aria-label="Effekte an/aus" data-on="1"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round"><path d="M4 9.5h3.5L12 5.5v13l-4.5-4H4z"/><path class="ss-wave" d="M15.5 9a4 4 0 0 1 0 6M18 6.5a7.5 7.5 0 0 1 0 11"/><path class="ss-x" d="M16 9.5l5 5M21 9.5l-5 5"/></svg></button></div>`);
  const install = el(`<button id="scInstall" type="button" hidden aria-label="Zum Home-Bildschirm"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><path d="M12 4v11M7 10l5 5 5-5M5 20h14"/></svg>HOME SCREEN</button>`);
  const installHint = el(`<div id="scInstallHint" hidden role="note"></div>`);
  menu.appendChild(idea);
  for (const n of [menu, pause, settings, world, log, quit, corner, placePanel, gpsCard, logbook, banner, radioBtn, jetBar, switches, install, installHint]) document.body.appendChild(n);
  const startBtn = menu.querySelector("#scStart"), menuStatus = menu.querySelector("#scMenuStatus");

  // ---- open / close: a stack of panels; the game pauses while anything is open ---------------
  const visible = () => !menu.hidden || ui.open.length > 0 || !quit.hidden;
  function sync() {
    const paused = visible();
    command(`pause ${paused ? 1 : 0}`);
    corner.hidden = !ui.started || !menu.hidden;
    document.body.classList.toggle("scui-playing", ui.started && menu.hidden);
    document.body.classList.toggle("scui-menu", paused);
  }
  function openPanel(p) {
    if (!ui.open.includes(p)) ui.open.push(p);
    p.hidden = false;
    focusFirst(p);
    sync();
  }
  function closeTop() {
    const p = ui.open.pop();
    if (p) p.hidden = true;
    if (ui.open.length) focusFirst(ui.open[ui.open.length - 1]);
    else if (!menu.hidden) focusFirst(menu);
    sync();
    if (!visible()) relock();
  }
  function relock() { if (!coarse && !globalThis.__kernNoPointerLock && ui.lockWanted) try { canvas.requestPointerLock?.(); } catch { /* no lock */ } canvas.focus?.(); }
  function focusables(root) {
    return Array.from(root.querySelectorAll("button:not([disabled]),input,textarea,[tabindex='0']")).filter(n => n.offsetParent !== null);
  }
  function focusFirst(root) { const f = focusables(root)[0]; f?.focus?.({preventScroll: false}); }

  // ---- settings dialog -----------------------------------------------------------------------
  function rowHtml(o) {
    const value = o.toggle ? "" : `<output>${fmt(o)}</output>`;
    if (o.toggle) return `<div class="scui-row scui-toggle${o.live ? "" : " scui-soon"}" role="switch" tabindex="0" data-key="${o.key}" aria-checked="${o.value > 0.5}">
      <b>${scuiEsc(o.label)}${o.live ? "" : " · BALD"}</b><i></i><small>${scuiEsc(o.help)}${o.live ? "" : " (Noch nicht auf Kern: der Schalter wird gespeichert.)"}</small></div>`;
    return `<div class="scui-row" data-key="${o.key}"><b>${scuiEsc(o.label)}</b>${value}
      <input type="range" min="${o.min}" max="${o.max}" step="${o.step}" value="${o.value}" aria-label="${scuiEsc(o.label)}"><small>${scuiEsc(o.help)}</small></div>`;
  }
  function fmt(o) {
    const k = o.key, v = o.value;
    if (/sensitivity|look[HV]|steering|throttle|deadzone/.test(k) || o.section === "ton") return `${Math.round(v)} %`;
    if (/Kmh/.test(k)) return `${Math.round(v)} km/h`;
    if (/fov|Tilt/.test(k)) return `${Math.round(v)}°`;
    if (/Distance|Height|hoverAgl/.test(k)) return `${(+v).toFixed(1)} m`;
    return String(Math.round(v * 10) / 10);
  }
  function renderSettings() {
    const tabs = settings.querySelector(".scui-tabs"), rows = settings.querySelector(".scui-rows");
    tabs.innerHTML = SCUI_SECTIONS.map(([id, label]) => `<button type="button" role="tab" data-tab="${id}" aria-selected="${ui.tab === id}">${label}</button>`).join("");
    rows.innerHTML = ui.schema.filter(o => o.section === ui.tab).map(rowHtml).join("");
  }
  function renderWorld() { world.querySelector(".scui-rows").innerHTML = ui.schema.filter(o => o.section === "welt").map(rowHtml).join(""); }
  function bindRows(root, render) {
    root.addEventListener("input", ev => {
      const input = ev.target.closest("input[type=range]"), row = ev.target.closest(".scui-row");
      if (!input || !row) return;
      const o = ui.schema.find(x => x.key === row.dataset.key);
      if (!o) return;
      o.value = setOpt(o.key, input.value);
      row.querySelector("output").textContent = fmt(o);
    });
    const flip = row => {
      const o = ui.schema.find(x => x.key === row.dataset.key);
      if (!o || !o.toggle) return;
      o.value = setOpt(o.key, o.value > 0.5 ? 0 : 1);
      row.setAttribute("aria-checked", String(o.value > 0.5));
    };
    root.addEventListener("click", ev => {
      const tab = ev.target.closest("[data-tab]");
      if (tab) { ui.tab = tab.dataset.tab; render(); focusFirst(root.querySelector(".scui-rows") || root); return; }
      const row = ev.target.closest(".scui-toggle");
      if (row) flip(row);
    });
    root.addEventListener("keydown", ev => {
      const row = ev.target.closest?.(".scui-toggle");
      if (row && (ev.key === "Enter" || ev.key === " ")) { ev.preventDefault(); flip(row); }
    });
  }
  bindRows(settings, renderSettings);
  bindRows(world, renderWorld);

  // ---- quit confirm (quit_confirm.mjs) --------------------------------------------------------
  function askQuit() { quit.hidden = false; focusFirst(quit); sync(); }
  function answerQuit(yes) {
    quit.hidden = true;
    if (yes) { while (ui.open.length) closeTop(); showTitle(); }
    sync();
    if (!yes && !visible()) relock();
  }
  quit.querySelector("[data-yes]").onclick = () => answerQuit(true);
  quit.querySelector("[data-no]").onclick = () => answerQuit(false);

  // ---- actions -------------------------------------------------------------------------------
  function act(name) {
    switch (name) {
      case "world": renderWorld(); openPanel(world); break;
      case "settings": readSchema(); renderSettings(); openPanel(settings); break;
      case "changelog": openPanel(log); break;
      case "pause": openPanel(pause); break;
      case "resume": closeTop(); break;
      case "close": closeTop(); break;
      case "exit": askQuit(); break;
      case "reset": while (ui.open.length) closeTop(); command("reset"); relock(); break;
      case "reset-section": command(`settings reset ${ui.tab}`); { const copy = scuiLoad(SCUI_STORE, {}); for (const o of ui.schema) if (o.section === ui.tab) delete copy[o.key]; scuiSave(SCUI_STORE, copy); } readSchema(); renderSettings(); break;
      case "place": renderPlaces([]); openPanel(placePanel); placePanel.querySelector("input").focus(); break;
      case "gps": if (gpsLive.on) stopGps(); else { while (ui.open.length) closeTop(); startGps(); } break;
      case "gps-view": command("gps view next"); break;
      case "logbook": syncLogbook(); renderLogbook(); openPanel(logbook); break;
      case "radio": command("radio next"); break;
      case "lb-export": {
        const blob = new Blob([JSON.stringify({version: 1, exportedAt: new Date().toISOString(), ...scuiLoad(SCUI_LOGBOOK, {flights: [], events: []})}, null, 2)], {type: "application/json"});
        const a = document.createElement("a");
        a.href = URL.createObjectURL(blob);
        a.download = `spawncity-logbuch-${new Date().toISOString().replaceAll(":", "-")}.json`;
        document.body.appendChild(a); a.click(); a.remove();
        setTimeout(() => URL.revokeObjectURL(a.href), 1000);
        break;
      }
      case "lb-clear": scuiSave(SCUI_LOGBOOK, {flights: [], events: []}); renderLogbook(); break;
      case "share": share(); break;
    }
  }
  for (const root of [menu, pause, settings, world, log, corner, placePanel, gpsCard, logbook, radioBtn])
    root.addEventListener("click", ev => { const b = ev.target.closest("[data-act]"); if (b) { ev.stopPropagation(); act(b.dataset.act); } });

  // ---- title screen --------------------------------------------------------------------------
  function showTitle() { menu.hidden = false; ui.playing = false; try { document.exitPointerLock?.(); } catch { /* none */ } focusFirst(menu); sync(); }
  function start() {
    // As the original: START is always START. The ground under the start point must be there before
    // the title goes (no frame in the sky): a press before that waits for it (usually a second or
    // two - the city itself streams in around the player afterwards).
    if (!ui.placed) { if (!ui.startPending) { ui.startPending = true; startPlace(); fullscreenForGame(); } return; }
    if (!ui.startPending) startPlace();  // (a waiting press already chose the place)
    ui.startPending = false;
    menu.hidden = true;
    ui.started = ui.playing = true;
    ui.lockWanted = !coarse;
    fullscreenForGame();
    sync();
    relock();
  }
  startBtn.onclick = start;
  // pwa_fullscreen_policy.mjs: an installed (fullscreen / standalone) app is already an app window.
  function fullscreenForGame() {
    const installed = matchMedia("(display-mode: fullscreen)").matches || matchMedia("(display-mode: standalone)").matches || navigator.standalone === true;
    if (!coarse || installed || document.fullscreenElement || !document.documentElement.requestFullscreen) return;
    // then the landscape lock (simulator.mjs:903); iOS ignores it and plays the CSS-rotated landscape
    document.documentElement.requestFullscreen({navigationUI: "hide"}).then(() => screen.orientation?.lock?.("landscape")).catch(() => {});
  }
  // Phones and tablets play landscape, always: the lock from the very first touch (the title too),
  // not only from START - turning the phone no longer flips the page between the real landscape and
  // the CSS-turned one. An installed app may lock without fullscreen; iOS ignores the lock and keeps
  // the CSS-rotated landscape (index.html .sc-css-landscape).
  if (coarse && !navigator.webdriver) {
    const lockNow = () => {
      const installed = matchMedia("(display-mode: fullscreen)").matches || matchMedia("(display-mode: standalone)").matches || navigator.standalone === true;
      if (installed) { screen.orientation?.lock?.("landscape")?.catch?.(() => {}); return; }
      fullscreenForGame();
    };
    const first = () => { removeEventListener("pointerdown", first, true); lockNow(); };
    addEventListener("pointerdown", first, true);
    document.addEventListener("fullscreenchange", () => { if (document.fullscreenElement) screen.orientation?.lock?.("landscape")?.catch?.(() => {}); });
    lockNow();
  }
  // Automated browsers and ?autostart=1 skip the title (auto_flight_start.mjs); ?menu=1 forces it.
  const skipTitle = params.get("menu") !== "1" && (navigator.webdriver || params.get("autostart") === "1");
  if (skipTitle) { menu.hidden = true; ui.started = ui.playing = true; }
  sync();

  // ---- keyboard and pad: menus first --------------------------------------------------------
  addEventListener("keydown", ev => {
    const typing = ev.target instanceof HTMLElement && (ev.target.tagName === "TEXTAREA" || (ev.target.tagName === "INPUT" && ev.target.type !== "range"));
    if (!quit.hidden) {
      // quit_confirm.mjs: Enter / Y / J = yes, Esc / N = no
      if (ev.key === "Enter" || ev.code === "KeyY" || ev.code === "KeyJ") answerQuit(true);
      else if (ev.key === "Escape" || ev.code === "KeyN") answerQuit(false);
      ev.stopImmediatePropagation(); ev.preventDefault();
      return;
    }
    if (ev.key === "Escape") {
      // desktop_controls.mjs: Esc asks "back to the main menu?"; it closes a menu, the big map, the strike aiming first
      ev.stopImmediatePropagation(); ev.preventDefault();
      if (ui.open.length) closeTop();
      else if (document.getElementById("mpMap")) command("bigmap close");
      else if (last?.ctl?.aiming) command("strike cancel");
      else if (menu.hidden && ui.started && !last?.ctl?.dead) askQuit();
      return;
    }
    if (ev.key === "Tab" && !ev.repeat && menu.hidden && ui.started && !visible() && !last?.ctl?.dead) {
      // Tab: the pause / settings menu, the mouse is freed for it (desktop_controls.mjs openMenu)
      ev.stopImmediatePropagation(); ev.preventDefault();
      ui.wantMenu = true;
      try { document.exitPointerLock?.(); } catch { /* none */ }
      openPanel(pause);
      return;
    }
    if (visible()) {
      // Inside a menu the keys belong to the menu, not to the game.
      if (!typing && (ev.key === "ArrowDown" || ev.key === "ArrowUp")) { moveFocus(ev.key === "ArrowDown" ? 1 : -1); ev.preventDefault(); }
      ev.stopImmediatePropagation();
    }
  }, {capture: true});
  // Losing the pointer lock while playing (the browser's Esc) asks "back to the main menu?"
  // (desktop_controls.mjs pointerlockchange): not for Tab (the menu), the big map or while dead.
  let hadLock = false;
  document.addEventListener("pointerlockchange", () => {
    const locked = document.pointerLockElement === canvas;
    if (!locked && hadLock && ui.playing && !visible() && !ui.wantMenu && !document.getElementById("mpMap") && !last?.ctl?.dead)
      setTimeout(() => { if (document.pointerLockElement !== canvas && ui.playing && !visible()) askQuit(); }, 0);
    ui.wantMenu = false;
    hadLock = locked;
  });
  function topRoot() { return !quit.hidden ? quit : ui.open.length ? ui.open[ui.open.length - 1] : !menu.hidden ? menu : null; }
  function moveFocus(dir) {
    const root = topRoot();
    if (!root) return;
    const list = focusables(root), i = list.indexOf(document.activeElement);
    list[(i + dir + list.length) % list.length]?.focus();
  }
  // settings_controller_nav.mjs: D-pad up/down moves, left/right changes a slider, A selects, B closes; MENU opens the pause menu.
  const padPrev = new Set();
  let stickY = 0, stickX = 0;
  function pollPad() {
    const pads = navigator.getGamepads ? Array.from(navigator.getGamepads()).filter(p => p && p.connected) : [];
    const p = pads.find(g => g.mapping === "standard") || pads[0];
    if (!p) { stickX = stickY = 0; return; }
    const hit = i => { const on = !!p.buttons[i]?.pressed, was = padPrev.has(i); if (on) padPrev.add(i); else padPrev.delete(i); return on && !was; };
    const a = hit(0), b = hit(1), y = hit(3), lb = hit(4), rb = hit(5), view = hit(8), menuBtn = hit(9), up = hit(12), down = hit(13), left = hit(14), right = hit(15);
    if (menuBtn && ui.started && menu.hidden && quit.hidden) { if (ui.open.length) closeTop(); else openPanel(pause); return; }
    if (menuBtn && !menu.hidden && !ui.open.length && quit.hidden) { startBtn.click(); return; }  // pad_actions.mjs titleNav: MENU = START
    if (!visible()) { stickX = stickY = 0; return; }
    document.body.classList.add("scui-pad");
    // pad_actions.mjs vote / quit: A = yes (or the focused button), B = no, left / right move the focus
    if (!quit.hidden) {
      if (a) { const f = document.activeElement; if (f && quit.contains(f) && f.matches("button")) f.click(); else answerQuit(true); }
      else if (b) answerQuit(false);
      else if (left || right) moveFocus(right ? 1 : -1);
      return;
    }
    // settings_controller_nav.mjs: D-pad or the left stick (fires past .72, re-arms under .35)
    const ax = +p.axes[0] || 0, ay = +p.axes[1] || 0;
    const navY = down ? 1 : up ? -1 : Math.abs(ay) > 0.72 && stickY === 0 ? Math.sign(ay) : 0;
    const navX = right ? 1 : left ? -1 : Math.abs(ax) > 0.72 && stickX === 0 ? Math.sign(ax) : 0;
    if (Math.abs(ay) < 0.35) stickY = 0; else if (navY) stickY = Math.sign(ay) || navY;
    if (Math.abs(ax) < 0.35) stickX = 0; else if (navX) stickX = Math.sign(ax) || navX;
    if (b || view) { if (ui.open.length) closeTop(); return; }
    if (lb || rb) {
      // previous / next section (the tabs)
      const tabs = Array.from(document.querySelectorAll(".scui-tabs [data-tab]")).filter(t => t.offsetParent !== null);
      if (tabs.length) { const i = tabs.findIndex(t => t.getAttribute("aria-selected") === "true"); tabs[((i < 0 ? 0 : i) + (rb ? 1 : -1) + tabs.length) % tabs.length].click(); }
      return;
    }
    if (navY) { moveFocus(navY); return; }
    const f = document.activeElement;
    if (navX && f?.closest?.(".scui-row")) {
      const row = f.closest(".scui-row"), range = row.querySelector("input[type=range]");
      if (range) { range.value = String(+range.value + navX * (+range.step || 1)); range.dispatchEvent(new Event("input", {bubbles: true})); }
      else if (row.classList.contains("scui-toggle")) row.click();  // a checkbox toggles
      return;
    }
    if (a && f && f !== document.body && !(f instanceof HTMLInputElement && f.type === "range")) f.click();
    else if (a && !menu.hidden && !ui.open.length) startBtn.click();  // nothing focused on the title: START
    void y;
  }
  addEventListener("pointerdown", () => document.body.classList.remove("scui-pad"), {capture: true});

  // ---- start place (start_location.mjs, world_location_selector.mjs, auto_flight_start.mjs) -----
  // The chosen place lives only for this browser session: every new start of the app is at your GPS
  // position. Search: Photon (OpenStreetMap), live suggestions.
  const validPlace = p => p && Number.isFinite(+p.lat) && Number.isFinite(+p.lon) && Math.abs(+p.lat) <= 85 && Math.abs(+p.lon) <= 180;
  let place = null;
  try { place = JSON.parse(sessionStorage.getItem(SCUI_PLACE) || "null"); } catch { place = null; }
  if (!validPlace(place)) place = null;
  const explicitStart = params.has("lat") && params.has("lon");  // the URL names the place: no GPS
  function renderPill() {
    const b = menu.querySelector("#scPlace b");
    b.textContent = place ? place.name : explicitStart ? "STARTORT AUS DER ADRESSE" : "MEIN STANDORT (GPS)";
    menu.querySelector("#scPlace").title = place ? `${place.name} · ${place.sub || ""}` : "Start an deinem GPS-Standort";
  }
  function choosePlace(p) {
    place = p && validPlace(p) ? {name: String(p.name || "").slice(0, 80), sub: String(p.sub || "").slice(0, 120), lat: +p.lat, lon: +p.lon} : null;
    try { sessionStorage.setItem(SCUI_PLACE, JSON.stringify(place)); } catch { /* private */ }
    if (place) scuiSave(SCUI_RECENT, [place, ...scuiLoad(SCUI_RECENT, []).filter(q => Math.abs(q.lat - place.lat) > 1e-4 || Math.abs(q.lon - place.lon) > 1e-4)].slice(0, 5));
    renderPill();
    // Picked while a game runs: the world moves there now.
    if (place && ui.started) moveTo(place.lat, place.lon, place.name);
  }
  const placeRow = p => `<button type="button" class="sp-item${place && p && Math.abs(place.lat - p.lat) < 1e-5 && Math.abs(place.lon - p.lon) < 1e-5 ? " sp-on" : ""}" data-lat="${p ? p.lat : ""}" data-lon="${p ? p.lon : ""}" data-name="${scuiEsc(p ? p.name : "")}" data-sub="${scuiEsc(p ? p.sub : "")}"><b>${scuiEsc(p ? p.name : "📡 MEIN STANDORT (GPS)")}</b><small>${scuiEsc(p ? p.sub : "wo du gerade bist")}</small></button>`;
  function renderPlaces(results) {
    const recent = scuiLoad(SCUI_RECENT, []).filter(validPlace);
    const presets = SCUI_PRESETS.map(([name, lat, lon]) => ({name, sub: `${lat.toFixed(4)}, ${lon.toFixed(4)}`, lat, lon}));
    placePanel.querySelector(".sp-list").innerHTML = (results.length ? `<h4>TREFFER</h4>${results.map(placeRow).join("")}` : "") + placeRow(null) +
      (recent.length ? `<h4>ZULETZT</h4>${recent.map(placeRow).join("")}` : "") + `<h4>STAEDTE</h4>${presets.map(placeRow).join("")}`;
  }
  function labelOf(pr) {
    const street = pr.street ? `${pr.street}${pr.housenumber ? " " + pr.housenumber : ""}` : "", head = pr.name || street || pr.city || pr.state || "";
    const tail = [pr.name && street ? street : "", [pr.postcode, pr.city || pr.town || pr.village].filter(Boolean).join(" "), pr.country].filter(x => x && x !== head);
    return {name: head, sub: tail.join(", ")};
  }
  let searchTimer = 0, searchSeq = 0;
  async function search(q) {
    const seq = ++searchSeq;
    if (q.length < 2) { renderPlaces([]); return; }
    try {
      const res = await fetch(`${SCUI_PHOTON}?q=${encodeURIComponent(q)}&limit=6&lang=${(navigator.language || "de").startsWith("de") ? "de" : "en"}`);
      const json = await res.json();
      if (seq !== searchSeq) return;
      renderPlaces((json.features || []).map(f => ({...labelOf(f.properties || {}), lon: f.geometry?.coordinates?.[0], lat: f.geometry?.coordinates?.[1]})).filter(validPlace));
    } catch { if (seq === searchSeq) placePanel.querySelector(".sp-list").insertAdjacentHTML("afterbegin", "<h4>SUCHE NICHT ERREICHBAR</h4>"); }
  }
  placePanel.querySelector("input").addEventListener("input", ev => { clearTimeout(searchTimer); const q = ev.target.value.trim(); searchTimer = setTimeout(() => search(q), 280); });
  placePanel.addEventListener("click", ev => {
    const it = ev.target.closest(".sp-item");
    if (!it) return;
    choosePlace(it.dataset.lat ? {lat: it.dataset.lat, lon: it.dataset.lon, name: it.dataset.name, sub: it.dataset.sub} : null);
    closeTop();
    if (!it.dataset.lat && ui.started) gpsStart();
  });
  renderPill();
  const metres = (a, b) => { const k = Math.PI / 180, x = (b.lon - a.lon) * k * Math.cos((a.lat + b.lat) / 2 * k), y = (b.lat - a.lat) * k; return Math.hypot(x, y) * 6371000; };
  function origin() { const o = last?.ui?.origin; return o ? {lat: o[0], lon: o[1]} : null; }
  let last = null;
  function moveTo(lat, lon, name = "") {
    const o = origin();
    if (o && metres(o, {lat, lon}) < 150) return false;  // already there
    command(`start ${lat.toFixed(6)} ${lon.toFixed(6)} ${name}`.trim());
    return true;
  }
  function startPlace() {
    // the original's start place (web/view/start_location.mjs) when the original view runs
    const orig = globalThis.__arondightStartPlace;
    if (orig) { const p = orig.get(); if (p) { moveTo(p.lat, p.lon, p.name); return; } if (!explicitStart) gpsStart(); return; }
    if (place) { moveTo(place.lat, place.lon, place.name); return; }
    if (!explicitStart) gpsStart();
  }
  // Every START begins where you really are: a fresh fix (never a cached one); after 12 s the network
  // position; a precise fix that comes within a minute and is far from the rough one moves the world.
  function gpsStart() {
    if (!navigator.geolocation) return;
    let used = null, done = false;
    const take = (pos, precise) => {
      const p = {lat: pos.coords.latitude, lon: pos.coords.longitude, acc: pos.coords.accuracy || 0};
      if (!used) { used = p; moveTo(p.lat, p.lon); return; }
      if (precise && p.acc < 50 && metres(used, p) > 300) { used = p; moveTo(p.lat, p.lon); }
    };
    const fallback = setTimeout(() => { if (!used) navigator.geolocation.getCurrentPosition(pos => take(pos, false), () => {}, {enableHighAccuracy: false, maximumAge: 60000, timeout: 20000}); }, 12000);
    const watch = navigator.geolocation.watchPosition(pos => { take(pos, true); if (pos.coords.accuracy < 50) finish(); }, () => {}, {enableHighAccuracy: true, maximumAge: 0, timeout: 60000});
    const finish = () => { if (done) return; done = true; clearTimeout(fallback); navigator.geolocation.clearWatch(watch); };
    setTimeout(finish, 60000);
  }

  // Behind the title the city already loads where START will begin: the chosen place, or - when
  // the browser already allows the location (no prompt) - the last known position. START still
  // takes a fresh fix and moves only when that is far (moveTo: 150 m), so nothing is built twice.
  (function preload() {
    const chosen = globalThis.__arondightStartPlace?.get?.() || (place ? place : null);
    if (chosen) { moveTo(+chosen.lat, +chosen.lon, chosen.name || ""); return; }
    if (!navigator.geolocation || !navigator.permissions?.query) return;
    navigator.permissions.query({name: "geolocation"}).then(st => {
      if (st.state !== "granted" || ui.started) return;
      navigator.geolocation.getCurrentPosition(pos => { if (!ui.started) moveTo(pos.coords.latitude, pos.coords.longitude); }, () => {}, {enableHighAccuracy: false, maximumAge: 600000, timeout: 10000});
    }).catch(() => {});
  })();

  // ---- LIVE GPS (gps_live_mode.mjs): the phone's fixes go to the game, which moves you ---------
  const gpsLive = {on: false, watch: null, coarse: null, wake: null};
  const gpsFix = pos => {
    const c = pos.coords;
    command(`gps fix ${c.latitude.toFixed(7)} ${c.longitude.toFixed(7)} ${(c.accuracy || 0).toFixed(1)} ${Number.isFinite(c.speed) && c.speed !== null ? c.speed.toFixed(2) : -1} ${Number.isFinite(c.heading) && c.heading !== null ? c.heading.toFixed(1) : -1}`);
  };
  const onCompass = e => {
    const h = Number.isFinite(e.webkitCompassHeading) ? e.webkitCompassHeading : (e.absolute && Number.isFinite(e.alpha) ? (360 - e.alpha) % 360 : null);
    if (h !== null) command(`gps compass ${(h * Math.PI / 180).toFixed(4)}`);
  };
  async function startGps() {
    if (!navigator.geolocation) { gpsCard.hidden = false; gpsCard.querySelector('[data-k="act"]').textContent = "KEIN GPS IN DIESEM BROWSER"; return; }
    gpsLive.on = true;
    command("gps on");
    // Ask for the position first, still inside the tap (Android shows no prompt after an await).
    gpsLive.watch = navigator.geolocation.watchPosition(gpsFix, err => { gpsCard.querySelector('[data-k="acc"]').textContent = err?.code === 1 ? "STANDORT BLOCKIERT" : "KEIN GPS-SIGNAL"; },
      {enableHighAccuracy: true, maximumAge: 0, timeout: 20000});
    setTimeout(() => { if (gpsLive.on && !last?.gps?.fix && gpsLive.coarse === null) gpsLive.coarse = navigator.geolocation.watchPosition(gpsFix, () => {}, {enableHighAccuracy: false, maximumAge: 60000, timeout: 30000}); }, 9000);
    try { if (typeof DeviceOrientationEvent !== "undefined" && typeof DeviceOrientationEvent.requestPermission === "function") await DeviceOrientationEvent.requestPermission().catch(() => {}); } catch { /* none */ }
    addEventListener("deviceorientationabsolute", onCompass, true);
    addEventListener("deviceorientation", onCompass, true);
    try { gpsLive.wake = await navigator.wakeLock?.request?.("screen"); } catch { /* none */ }
  }
  function stopGps() {
    gpsLive.on = false;
    command("gps off");
    if (gpsLive.watch !== null) navigator.geolocation.clearWatch(gpsLive.watch);
    if (gpsLive.coarse !== null) navigator.geolocation.clearWatch(gpsLive.coarse);
    gpsLive.watch = gpsLive.coarse = null;
    removeEventListener("deviceorientationabsolute", onCompass, true);
    removeEventListener("deviceorientation", onCompass, true);
    try { gpsLive.wake?.release?.(); } catch { /* none */ }
    gpsLive.wake = null;
  }
  const GPS_ICON = {STEHT: "🧍", GEHEN: "🚶", LAUFEN: "🏃", RAD: "🚴", FAHRT: "🚗"};
  function renderGps(g) {
    gpsCard.hidden = !g;
    if (!g) return;
    const q = k => gpsCard.querySelector(`[data-k="${k}"]`);
    q("act").textContent = g.fix ? `${GPS_ICON[g.activity] || ""} ${g.activity}` : "GPS SUCHT ...";
    const pace = g.speed > 0.8 && (g.activity === "LAUFEN" || g.activity === "GEHEN") ? 1000 / g.speed / 60 : 0;
    q("speed").textContent = pace ? `${Math.floor(pace)}:${String(Math.round(pace % 1 * 60)).padStart(2, "0")} /km` : `${(g.speed * 3.6).toFixed(0)} km/h`;
    q("dist").textContent = g.distance >= 1000 ? `${(g.distance / 1000).toFixed(2)} km` : `${Math.round(g.distance)} m`;
    q("time").textContent = `${Math.floor(g.time / 60)}:${String(Math.floor(g.time % 60)).padStart(2, "0")}`;
    if (g.fix) q("acc").textContent = `± ${g.accuracy} m`;
    gpsCard.querySelector('[data-act="gps-view"]').textContent = g.view;
  }

  // ---- flight logbook (flight_logbook.mjs): the game records, the page keeps the history ----------
  const session = Date.now().toString(36);
  let logSerial = -1;
  function syncLogbook() {
    let fresh;
    try { fresh = JSON.parse(command("logbook")); } catch { return; }
    const book = scuiLoad(SCUI_LOGBOOK, {flights: [], events: []});
    const known = new Set(book.flights.map(f => f.key));
    for (const f of fresh.flights.slice().reverse()) {
      const key = `${session}-${f.id}`;
      if (known.has(key)) continue;
      book.flights.unshift({key, at: new Date().toISOString(), origin: last?.ui?.origin || null, ...f});
    }
    const ekeys = new Set(book.events.map(e => e.key));
    for (const ev of fresh.events.slice().reverse()) {
      const key = `${session}-${ev.t}-${ev.stage}`;
      if (!ekeys.has(key)) book.events.unshift({key, at: new Date().toISOString(), ...ev});
    }
    book.flights = book.flights.slice(0, 50);
    book.events = book.events.slice(0, 120);
    scuiSave(SCUI_LOGBOOK, book);
  }
  const dur = s => { const m = Math.floor(s / 60), r = s - m * 60; return m ? `${m}m ${r.toFixed(1)}s` : `${r.toFixed(1)}s`; };
  function renderLogbook() {
    const book = scuiLoad(SCUI_LOGBOOK, {flights: [], events: []});
    logbook.querySelector('[data-k="flights"]').innerHTML = book.flights.length ? book.flights.map(f => `<article><time>${scuiEsc(new Date(f.at).toLocaleString())}</time>
      <b>${last?.offline ? "ERSATZSTADT" : "WELT"} · ${dur(f.duration)} · ${scuiEsc(String(f.reason).replaceAll("_", " "))}</b>
      <div class="lb-stats">STRECKE ${f.distance.toFixed(1)} m · MAX ${(f.maxSpeed * 3.6).toFixed(0)} km/h · AGL ${f.maxAgl.toFixed(1)} m · AKKU ${f.voltsStart.toFixed(1)} → ${f.voltsMin.toFixed(1)} V · ${f.path.length} Punkte</div></article>`).join("")
      : `<article><b>NOCH KEINE FLUEGE</b></article>`;
    logbook.querySelector('[data-k="events"]').innerHTML = book.events.length ? book.events.slice(0, 30).map(e => `<article><time>${scuiEsc(new Date(e.at).toLocaleString())}</time><b>${scuiEsc(e.stage)}</b></article>`).join("")
      : `<article><b>KEINE NETZEREIGNISSE</b></article>`;
  }

  // ---- sound and music switches (drone_simulator.html #soundSwitch / #musicSwitch) -------------
  const optValue = key => ui.schema.find(o => o.key === key)?.value;
  function renderSwitches() {
    switches.querySelector('[data-sw="sound"]').dataset.on = optValue("audio.on") > 0.5 ? "1" : "0";
    switches.querySelector('[data-sw="music"]').dataset.on = optValue("music.on") > 0.5 ? "1" : "0";
  }
  switches.addEventListener("click", ev => {
    const b = ev.target.closest("[data-sw]");
    if (!b) return;
    ev.stopPropagation();
    const key = b.dataset.sw === "sound" ? "audio.on" : "music.on";
    setOpt(key, optValue(key) > 0.5 ? 0 : 1);
    renderSwitches();
  });
  renderSwitches();

  // ---- HOME SCREEN (install as an app): Chrome's install prompt, the two taps on iOS -----------
  {
    let deferred = null, standalone = matchMedia("(display-mode: standalone)").matches || matchMedia("(display-mode: fullscreen)").matches || navigator.standalone === true;
    const ios = /iphone|ipad|ipod/i.test(navigator.userAgent) || (/macintosh/i.test(navigator.userAgent) && navigator.maxTouchPoints > 1);
    // An offline copy of the page (sw.js), like SpawnCity's service worker; not from file://.
    if ("serviceWorker" in navigator && /^https?:$/.test(location.protocol)) navigator.serviceWorker.register("./sw.js", {scope: "./", updateViaCache: "none"}).catch(() => {});
    // shown whenever this page is not the installed app (the prompt caught early in index.html;
    // without one the browser's own menu entry is named)
    const syncInstall = () => {
      deferred = deferred || window.__scInstallPrompt || null;
      if (window.__scInstalled) standalone = true;
      install.hidden = standalone || menu.hidden;
      if (install.hidden) installHint.hidden = true;
    };
    setInterval(syncInstall, 500);
    syncInstall();
    addEventListener("beforeinstallprompt", e => { e.preventDefault(); deferred = e; window.__scInstallPrompt = e; });
    addEventListener("appinstalled", () => { standalone = true; deferred = null; window.__scInstallPrompt = null; });
    matchMedia("(display-mode: standalone)").addEventListener?.("change", ev => { if (ev.matches) standalone = true; });
    install.addEventListener("click", e => {
      e.preventDefault(); e.stopPropagation();
      if (deferred) { const d = deferred; deferred = null; window.__scInstallPrompt = null; d.prompt(); d.userChoice.then(c => { if (c?.outcome === "accepted") standalone = true; }).catch(() => {}); return; }
      installHint.innerHTML = ios
        ? "Tippe unten in Safari auf <b>Teilen</b> &#x2191; und dann auf <b>Zum Home-Bildschirm</b>. SpawnCity startet danach im Vollbild wie eine App."
        : "Im Browsermenü <b>&#8942;</b> auf <b>App installieren</b> (oder <b>Zum Startbildschirm hinzufügen</b>) tippen. Ist SpawnCity schon installiert, öffnest du es dort mit <b>In App öffnen</b>.";
      installHint.hidden = !installHint.hidden;
    });
    document.addEventListener("click", e => { if (!installHint.hidden && e.target !== install && !install.contains(e.target)) installHint.hidden = true; }, true);
  }

  // ---- TEILEN (share_button.mjs): the share sheet, else copy the link, else a post on X --------
  const SHARE_TEXT = "SpawnCity – spiel in deiner echten Stadt: Drohne, zu Fuß, Auto, Bus und Jet, mit Freunden. Kostenlos im Browser.";
  function shareToast(msg) { const t = menu.querySelector("#scShareToast"); t.textContent = msg; t.dataset.on = "1"; clearTimeout(shareToast.timer); shareToast.timer = setTimeout(() => { t.dataset.on = "0"; }, 2200); }
  async function share() {
    const c = document.querySelector('link[rel="canonical"]')?.href, u = new URL(location.href);
    u.search = ""; u.hash = "";
    const url = c || u.href;
    try { if (navigator.share) { await navigator.share({title: "SpawnCity", text: SHARE_TEXT, url}); return; } } catch (e) { if (e?.name === "AbortError") return; }
    try { await navigator.clipboard.writeText(`${SHARE_TEXT} ${url}`); shareToast("LINK KOPIERT"); return; } catch { /* no clipboard */ }
    open(`https://x.com/intent/post?text=${encodeURIComponent(SHARE_TEXT)}&url=${encodeURIComponent(url)}`, "_blank", "noopener");
  }

  // ---- suggestion card (suggestion_box.mjs): after 15 min of play of this version, asked once in
  // the title menu (never mid-fight); SPAETER snoozes 10 min. The idea goes out anonymously as a signed
  // Nostr event (kind 30078, tags spawncity-suggestion + build) with a throw-away key to public relays;
  // the game signs it (nostr.cpp, BIP-340), the page opens the relay WebSockets.
  const SG_RELAYS = ["wss://relay.damus.io", "wss://nos.lol", "wss://relay.primal.net", "wss://relay-rpi.edufeed.org"];
  const SG_BUILD = "kern-" + (document.querySelector('meta[name="sc-build"]')?.content || "dev");
  const sgState = () => { const st = scuiLoad(SCUI_SUGGEST, null); return st && st.build === SG_BUILD ? st : {build: SG_BUILD, played: 0, done: false, snoozeUntil: 0}; };
  setInterval(() => { if (!ui.playing || visible() || document.hidden) return; const st = sgState(); if (st.done) return; st.played += 5; scuiSave(SCUI_SUGGEST, st); }, 5000);
  setInterval(() => {
    if (menu.hidden || !placePanel.hidden) return;
    const st = sgState();
    if (st.done || st.played < 15 * 60 || st.played < (st.snoozeUntil || 0)) return;
    idea.hidden = false;
  }, 2000);
  const randHex = n => Array.from(crypto.getRandomValues(new Uint8Array(n)), b => b.toString(16).padStart(2, "0")).join("");
  async function sendIdea(text) {
    const id = `${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 8)}`, lang = (navigator.language || "de").slice(0, 5);
    const tags = JSON.stringify([["d", `spawncity-suggestion-${id}`], ["t", "spawncity-suggestion"], ["v", SG_BUILD], ["l", lang]]);
    const content = JSON.stringify({app: "spawncity", text, build: SG_BUILD, lang, ua: /Android/.test(navigator.userAgent) ? "android" : /iPhone|iPad/.test(navigator.userAgent) ? "ios" : "desktop"});
    let secret = "";
    for (let i = 0; i < 4 && !secret; i++) { const k = randHex(32); if (/^[0-9a-f]{64}$/.test(k) && !/^f{56}/.test(k) && !/^0{64}$/.test(k)) secret = k; }
    const ev = command(`nostr event ${Math.floor(Date.now() / 1000)} ${secret} ${randHex(32)} ${tags} ${content}`);
    if (!ev.startsWith("{")) throw Error(ev);
    const one = url => new Promise(resolve => {
      let ws, done = false;
      const finish = ok => { if (done) return; done = true; try { ws?.close(); } catch { /* closed */ } resolve(ok); };
      setTimeout(() => finish(false), 6000);
      try { ws = new WebSocket(url); } catch { finish(false); return; }
      ws.onopen = () => ws.send(`["EVENT",${ev}]`);
      ws.onmessage = m => { try { const d = JSON.parse(m.data); if (d[0] === "OK") finish(d[2] === true); } catch { /* not ours */ } };
      ws.onerror = () => finish(false);
    });
    return (await Promise.all(SG_RELAYS.map(one))).filter(Boolean).length;
  }
  idea.querySelector("textarea").addEventListener("keydown", e => e.stopPropagation());
  idea.addEventListener("click", async e => {
    const b = e.target.closest("[data-sg]");
    if (!b) return;
    e.preventDefault(); e.stopPropagation();
    const st = sgState(), msg = idea.querySelector(".sg-msg"), ta = idea.querySelector("textarea");
    if (b.dataset.sg === "no") { st.done = true; scuiSave(SCUI_SUGGEST, st); idea.hidden = true; return; }
    if (b.dataset.sg === "later") { st.snoozeUntil = st.played + 10 * 60; scuiSave(SCUI_SUGGEST, st); idea.hidden = true; return; }
    const text = ta.value.replace(/\s+/g, " ").trim().slice(0, 240);
    if (text.length < 3) { msg.textContent = "Ein paar Worte bitte 🙂"; ta.focus(); return; }
    b.disabled = true;
    msg.textContent = "SENDE …";
    try {
      const n = await sendIdea(text);
      if (n > 0) { st.done = true; scuiSave(SCUI_SUGGEST, st); msg.textContent = "DANKE! 🙌 Kommt bei uns an."; setTimeout(() => { idea.hidden = true; }, 1600); }
      else { msg.textContent = "Gerade nicht erreichbar – später nochmal?"; b.disabled = false; }
    } catch { msg.textContent = "Gerade nicht erreichbar – später nochmal?"; b.disabled = false; }
  });

  return {
    /** Called every frame with the game's HUD JSON. */
    update(h) {
      pollPad();
      if (!h) return;
      last = h;
      renderGps(h.gps);
      if (h.ui && h.ui.settings !== ui.settingsSeen) { ui.settingsSeen = h.ui.settings; readSchema(); renderSwitches(); }
      if (h.logbook && h.logbook.serial !== logSerial) { logSerial = h.logbook.serial; if (logSerial > 0) syncLogbook(); }
      // car radio banner and the touch button (car_radio.mjs)
      if (h.banner) { banner.querySelector("b").textContent = h.banner; banner.querySelector("small").textContent = h.bannerSub || ""; }
      banner.style.opacity = h.banner ? "1" : "0";
      radioBtn.hidden = !(h.mode === "car" && h.touch);
      // jetpack fuel (player_walk_mode_v4.mjs jetHud): only while it is not full
      // the mode switch (desktop: with its key; touch has its own button in the touch layout)
      const ms = h.mode === "foot" ? ["ZUR DROHNE · V", "mode drone"] : h.mode === "drone" ? ["ZU FUSS · V", "mode foot"] : h.mode === "car" ? ["AUSSTEIGEN · F", "mode foot"]
        : h.mode === "jet" ? (h.agl > 3 ? ["EJECT · F", "eject"] : ["AUSSTEIGEN · F", "mode foot"]) : null;
      modeBtn.hidden = !ms || !!h.touch || !ui.playing || visible();
      if (ms && modeBtn.textContent !== ms[0]) { modeBtn.textContent = ms[0]; modeBtn.dataset.cmd = ms[1]; }
      for (const b of settings.querySelectorAll("[data-mode]")) b.dataset.active = b.dataset.mode === h.mode ? "1" : "0";
      const jp = h.jetpack;
      if (jp && jp.ignitions > (ui.jetIgnitions ?? jp.ignitions) && !globalThis.__scOriginalAudio) { jetFlash.classList.add("on"); requestAnimationFrame(() => requestAnimationFrame(() => jetFlash.classList.remove("on"))); }
      if (jp) ui.jetIgnitions = jp.ignitions;
      jetBar.hidden = !jp || (jp.fuel > 0.995 && !jp.thrusting) || !!h.ui?.paused;
      if (!jetBar.hidden) { jetBar.querySelector("i").style.width = `${Math.round(jp.fuel * 100)}%`; jetBar.classList.toggle("burn", jp.thrusting); }
      const ready = !!h.placed;
      ui.placed = ready;
      if (ready && ui.startPending && !ui.playing) start();
      const status = ready && h.offline ? "OFFLINE · PROZEDURALE ERSATZSTADT" : "BEREIT";
      if (menuStatus.textContent !== status) menuStatus.textContent = status;
    },
    get playing() { return ui.playing; },
    open: name => act(name),
    title: showTitle,
    start,
    get started() { return ui.started; },
    /** The title screen is up. */
    titleShown: () => !menu.hidden,
    /** Every panel and the quit question shut (VR: no page menu where nobody sees it). */
    closeAll() { while (ui.open.length) closeTop(); if (!quit.hidden) quit.hidden = true; sync(); },
  };
}
