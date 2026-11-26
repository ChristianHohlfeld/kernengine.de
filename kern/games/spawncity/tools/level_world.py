#!/usr/bin/env python3
"""The world settings SpawnCity's code set on its level at every start (until 26.11.2026: Game::build_level), as data:
in the level the 240 Hz step with one Box3D sub-step, the shadow over 110 m and the ground's colour; in the project
(level/spawncity.kproject) the device profile "mobile" with the phone's shadow over 70 m (its "world": laid on the
level's when the game starts on a phone - kern/docs/project.md 2.). Nothing else of either file changes (each written
as it is written: indent 1 and 2, UTF-8). A key there already stops it, and nothing is written.

    level_world.py <level.klevel> <project.kproject>"""
import json
import sys

LEVEL = {"stepHz": 240, "substeps": 1, "shadowDistance": 110, "groundColor": "#5c664f"}  # (0.36, 0.40, 0.31) as #rrggbb
MOBILE = {"shadowDistance": 70}


def read(path, indent):
    text = open(path, encoding="utf-8").read()
    doc = json.loads(text)
    if json.dumps(doc, indent=indent, ensure_ascii=False) + "\n" != text:
        sys.exit(f"level_world.py: {path} is not written with indent {indent} - nothing changed")
    return doc


def write(path, doc, indent):
    open(path, "w", encoding="utf-8").write(json.dumps(doc, indent=indent, ensure_ascii=False) + "\n")


def main(level_path, project_path):
    level, project = read(level_path, 1), read(project_path, 2)   # (the level as the converter writes it, the project as Kern does)
    there = [k for k in LEVEL if k in level["world"]]
    if there:
        sys.exit(f"level_world.py: {level_path} has world.{', world.'.join(there)} already - nothing changed")
    mobile = [p for p in project["settings"]["profiles"] if p["name"] == "mobile"]
    if len(mobile) != 1 or "world" in mobile[0]:
        sys.exit(f"level_world.py: {project_path} has no profile mobile, or it has a world already - nothing changed")
    level["world"].update(LEVEL)
    mobile[0]["world"] = MOBILE
    write(level_path, level, 1)
    write(project_path, project, 2)
    print(f"{level_path}: world " + ", ".join(f"{k} {v}" for k, v in LEVEL.items()))
    print(f"{project_path}: profile mobile world {MOBILE}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
