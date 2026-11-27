# Lehrer: MAG als optionaler Trainingsrechner

Im Lehrer unter **Training → Rechenbackend** ist **Lokal** bei jedem Öffnen der Seite vorausgewählt.
Das lokale WASM-Training, die Keyframes, die Aufgaben, das Level und der ONNX-Import bleiben unverändert.
Nur bei MAG wird eine private HTTPS-Verbindung benutzt. Die Adresse wird auf dem Gerät gespeichert,
das Token nicht. Kein automatischer Wechsel und kein CPU-Fallback bei einem HIP-Fehler.

## Installation auf MAG (Ubuntu, Benutzer chris)

Voraussetzungen: der bestehende MAG-Stack mit seiner GPU-Sperre, ein funktionierendes ROCm/HIP
für alle MI50-Karten (gfx906), die Kern-Build-Voraussetzungen und Tailscale. Die native HIP-Variante
muss auf **demselben Git-Commit** gebaut werden wie das veröffentlichte Studio. Auf dem Server
wird kein Kernel, Treiber oder bestehendes MAG-Deployment geändert.

Einmalig ein eigenes Checkout anlegen (nicht in MAGs bestehendem Repo):

~~~bash
sudo install -d -o chris -g chris /opt/kern-lehrer
git clone https://github.com/ChristianHohlfeld/kernengine.de.git /opt/kern-lehrer
cd /opt/kern-lehrer
# Nach Freigabe den vollständigen SHA des Studio-Builds aus editor/build.json auschecken:
STUDIO_SHA="$(curl -fsS https://kernengine.de/editor/build.json | python3 -c 'import json,sys; print(json.load(sys.stdin)["sha"])')"
git checkout --detach "$STUDIO_SHA"
# HIPCC bzw. PATH/LD_LIBRARY_PATH auf MAGs tatsächlich funktionierendes ROCm setzen.
KERN_LEHRER_HIP=1 KERN_HIP_ARCH=gfx906 kern/tools/lehrer.sh /opt/kern-lehrer/build-hip
~~~

Vor dem Dienststart prüfen, dass beide Sperrdateien des bestehenden MAG-Stacks vorhanden und
für chris zugänglich sind: /var/lib/mag-idol/gpu.lock und /var/lib/mag-idol/gpu.lock.idol.
Sie **nicht** ersetzen oder neu erzeugen, solange MAG läuft.

~~~bash
sudo install -d -m 755 /usr/local/lib/kern-lehrer
sudo install -o root -g root -m 755 kern/tools/mag-exclusive.py /usr/local/lib/kern-lehrer/mag-exclusive.py
sudo install -o root -g root -m 644 kern/tools/mag-lehrer.service /etc/systemd/system/mag-lehrer.service
printf '%s\n' 'chris ALL=(root) NOPASSWD: /usr/local/lib/kern-lehrer/mag-exclusive.py enter, /usr/local/lib/kern-lehrer/mag-exclusive.py leave' | sudo tee /etc/sudoers.d/kern-lehrer >/dev/null
sudo chmod 440 /etc/sudoers.d/kern-lehrer
sudo visudo -cf /etc/sudoers.d/kern-lehrer
# Einmalig, nur falls /etc/kern-lehrer.env noch nicht existiert; nicht bei jedem Update rotieren.
if ! sudo test -e /etc/kern-lehrer.env; then
  sudo install -m 600 /dev/null /etc/kern-lehrer.env
  printf 'KERN_LEHRER_BUILD=%s\nKERN_LEHRER_TOKEN=%s\n' "$(git rev-parse HEAD)" "$(python3 -c 'import secrets; print(secrets.token_urlsafe(32))')" | sudo tee /etc/kern-lehrer.env >/dev/null
fi
sudo systemctl daemon-reload
sudo systemctl enable --now mag-lehrer.service
~~~

Falls das ROCm-Setup eine eigene Bibliothek benötigt, ergänze im EnvironmentFile
den tatsächlichen LD_LIBRARY_PATH. Kein HIP_VISIBLE_DEVICES setzen; alle für den Dienst
sichtbaren GPUs werden erkannt und einzeln geprüft.

HTTP bindet nur an 127.0.0.1:8799. Verwende **Tailscale Serve**, nicht Funnel. Prüfe
zuerst mit "tailscale serve status", ob Port 8443 frei ist; nichts Bestehendes überschreiben.
Für einen freien Port beispielsweise:

~~~bash
sudo tailscale serve --bg --https=8443 http://127.0.0.1:8799
tailscale serve status
~~~

Nur der eigene Tailnet darf zugreifen. Im Editor die HTTPS-Adresse aus der Ausgabe (inkl. Port)
und das Token aus /etc/kern-lehrer.env eintragen. Im Browser ggf. den lokalen Netzwerkzugriff
erlauben. Die öffentliche Website erhält keine Server-Zugangsdaten. Das Token nur am eigenen Gerät
eingeben; niemals in Git, URL, Logs oder Screenshots.

## Lauf und Ressourcen

Der Dienst akzeptiert eine Aufgabe, Seed, Enten pro Worker, Iterationen und die beiden
bestehenden PPO-Einstellungen. Alle CPUs im Affinitätsbereich des Dienstes werden verwendet,
keine CPU-Quote. Die GPU-Prioritätssperre wird exklusiv gehalten. Eine bereits laufende
Idol-GPU-Aufgabe darf fertig werden (nicht mitten in GPU-Arbeit einfrieren); danach pausiert
der Root-Hook die bekannten MAG-Rechendienste. Bereits vorher inaktive oder eingefrorene
Dienste werden nicht gestartet/aufgetaut. h3d wird nach Erwerb des GPU-Locks regulär gestoppt,
damit der VRAM frei wird, und am Ende wieder gestartet. MAG-Netzwerk, Speicher und Chat-UI
bleiben erreichbar. Keine Gewichte werden gelöscht oder verändert.

Box3D, kleine Inferenz-Batches, ELU, Adam und die PPO-Steuerung bleiben auf der CPU. Große
dichte Vorwärts- und Gradientenberechnungen werden mit HIP auf die sichtbaren GPUs verteilt.
Der native Start prüft auf jeder GPU die Ergebnisse gegen die CPU. Die GPU-Variante ist ein
optionaler Build; keine ROCm-Abhängigkeit für WASM/CPU. Die volle GPU-Auslastung oder ein
Geschwindigkeitsfaktor ist erst nach Messung auf MAG belegbar. Bitgleichheit ganzer HIP-Läufe
ist nicht durch den kleinen Selbsttest bewiesen.

Start/Pause/Resume/Stopp und Fortschritt kommen aus derselben Oberfläche. Pause hält die
Ressourcen für den aktuellen Job; Stopp wartet auf das Ende der laufenden Initialisierung oder Iteration und sichert ONNX
und Checkpoint. Alle 25 Iterationen wird ein neues Netz atomar gespeichert und in das Level
übernommen. Beim Ende, Fehler oder Dienststopp werden nur die vom Hook reservierten Dienste
wieder freigegeben. Der Root-Hook merkt den vorherigen Zustand unter /run/mag-lehrer-exclusive.json.
Bei unvollständiger Wiederherstellung startet kein neuer Job; prüfen:
"sudo journalctl -u mag-lehrer.service", ggf. "sudo /usr/local/lib/kern-lehrer/mag-exclusive.py leave"
**nur nachdem der Trainer beendet ist**. Bei Neustart des Servers sind vorher eingefrorene
Prozesse ohnehin beendet; der Checkpoint bleibt unter /var/lib/kern-lehrer/jobs.

Die Browser-Verbindung darf unterbrochen werden. Mit "Mit MAG wiederverbinden" kann der
aktuelle Lauf auch nach Tab-Neuladen kontrolliert werden; aus Sicherheitsgründen wird dann
nur der Download angeboten, keine automatische Übernahme in ein möglicherweise anderes Level.
Ein Neustart des Dienstes setzt einen laufenden Job nicht automatisch fort. Ein Checkpoint
enthält den Lernzustand, nicht jeden laufenden Physikzustand; keine bitgleiche Fortsetzung
nach Prozessneustart versprechen.

## Prüfungen

~~~bash
python3 kern/tests/lehrer_remote_test.py
node kern/tests/lehrer_remote_test.mjs
~~~

Die Tests verwenden einen simulierten Trainer und prüfen HTTP, Authentifizierung, Versionskonflikt,
Ressourcensperren, Pause, Resume, Stopp, Ergebnisübertragung und Wiederherstellung.
Auf MAG zusätzlich: je ein kurzer Stand- und Keyframe-Lauf mit CPU/GPU-Lastmessung,
ONNX-Import im Studio und Nachweis der wiederhergestellten MAG-Dienste. Kein Live-Release
der HIP-Integration ohne diesen Hardwaretest.
