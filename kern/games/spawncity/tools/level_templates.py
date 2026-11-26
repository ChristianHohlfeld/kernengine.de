#!/usr/bin/env python3
"""Puts the actors that tools/level_templates.cpp writes (the drone's template) into a
level file - nothing else of it changes (the file is written as the converter writes it: indent 1, UTF-8).

    level_templates.py <actors.json> <level.klevel>

The actors take the level's next ids in their order (a parent's id follows); the level's nextId moves past them. An
actor of that name that is there already stops it (the level holds them once): nothing is written."""
import json
import sys


def main(actors_path, level_path):
    actors = json.load(open(actors_path))
    text = open(level_path, encoding="utf-8").read()
    level = json.loads(text)
    if json.dumps(level, indent=1, ensure_ascii=False) + "\n" != text:
        sys.exit(f"level_templates.py: {level_path} is not written as the converter writes it (indent 1) - not changed")
    names = {a["name"] for a in level["actors"]}
    there = [a["name"] for a in actors if a["name"] in names]
    if there:
        sys.exit(f"level_templates.py: {level_path} has {', '.join(there)} already - not changed")
    first = max([level.get("nextId", 1)] + [a["id"] + 1 for a in level["actors"]])
    ids = {a["id"]: first + i for i, a in enumerate(actors)}
    for a in actors:
        a["id"] = ids[a["id"]]
        if "parent" in a:
            a["parent"] = ids[a["parent"]]
        level["actors"].append(a)
    level["nextId"] = first + len(actors)
    open(level_path, "w", encoding="utf-8").write(json.dumps(level, indent=1, ensure_ascii=False) + "\n")
    for a in actors:
        print(f"{level_path}: {a['id']} {a['name']}" + (" (template)" if a.get("template") else ""))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
