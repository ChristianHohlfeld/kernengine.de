#!/usr/bin/env python3
"""Makes level/spawncity.klevel and level/spawncity.kproject the small game's (26.11.2026, BLUEPRINTS.md "Herausgenommen
am 26.11."): a drone over the real city. Out of the level go the rule actors of what is out of the game (all but
Controls and Settings), the player with its head (the drone flies in a simulated world), the vehicle templates but the
drone, and every Blueprint of the folder "Source" written from a source that is no longer the game's; out of the project
go its program Blueprints of those parts (person_hit, qr_code, combat_audio). Nothing else changes (the files are
written as the converter writes them: the level indent 1, the project indent 2, UTF-8). Running it again changes
nothing.

    small_level.py [<level.klevel> <project.kproject>]"""
import json
import os
import sys

RULES = {"Controls", "Settings"}
TEMPLATES = {"Drone"}
# the sources of the small game (src/) and the firmware, as the converter names their Blueprints; and the engine's
SOURCE = {"Arondight45_DroneFC_Core", "Arondight45_FirmwareRuntime", "Arondight45_HardwareSensors", "Arondight45_StateControl",
          "blueprint_call", "city_jobs", "city_jobs_cpp", "city_stream", "city_stream_cpp", "common", "controls", "controls_cpp",
          "drone", "drone_cpp", "drone_art", "drone_art_cpp", "main_cpp", "mvt", "mvt_cpp", "rigid_spec",
          "settings", "settings_cpp", "sound_fx", "sound_fx_cpp", "spiel", "spiel_cpp", "terrain", "terrain_cpp", "tiles", "tiles_cpp",
          "touch_play", "touch_play_cpp", "wire"}
PROGRAM_OUT = {"person_hit", "qr_code", "qr_code_cpp", "combat_audio", "combat_audio_cpp"}


def keep(actor):
    name, folder = actor.get("name", ""), actor.get("folder")
    if folder == "Rules":
        return name in RULES
    if folder == "Vehicles":
        return name in TEMPLATES
    if folder == "Source":
        return name in SOURCE or name.startswith("engine_")
    return name not in ("Player", "Head")


def main(level_path, project_path):
    text = open(level_path, encoding="utf-8").read()
    level = json.loads(text)
    if json.dumps(level, indent=1, ensure_ascii=False) + "\n" != text:
        sys.exit(f"small_level.py: {level_path} is not written as the converter writes it (indent 1) - not changed")
    gone = [a for a in level["actors"] if not keep(a)]
    ids = {a["id"] for a in gone}
    level["actors"] = [a for a in level["actors"] if a["id"] not in ids and a.get("parent") not in ids]
    open(level_path, "w", encoding="utf-8").write(json.dumps(level, indent=1, ensure_ascii=False) + "\n")
    project = json.load(open(project_path, encoding="utf-8"))
    out = [b["name"] for b in project.get("blueprints", []) if b["name"] in PROGRAM_OUT]
    project["blueprints"] = [b for b in project.get("blueprints", []) if b["name"] not in PROGRAM_OUT]
    open(project_path, "w", encoding="utf-8").write(json.dumps(project, indent=2, ensure_ascii=False) + "\n")
    print(f"{level_path}: {len(gone)} actors out ({', '.join(a['name'] for a in gone)})")
    print(f"{project_path}: {len(out)} Blueprints out ({', '.join(out)})")


if __name__ == "__main__":
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    args = sys.argv[1:] or [os.path.join(here, "level", "spawncity.klevel"), os.path.join(here, "level", "spawncity.kproject")]
    if len(args) != 2:
        sys.exit(__doc__)
    main(*args)
