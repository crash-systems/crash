#!/usr/bin/env python3
import pathlib
import os
import sys


def check_file(filepath: str, basepath: str) -> bool:
    header_groups = []
    group = []

    found = False
    try:
        file_content = pathlib.Path(filepath).read_text()
    except UnicodeDecodeError:
        return False

    for line in file_content.splitlines():
        if line.startswith("#include"):
            if " " not in line:
                print(f"cannot parse: [{line!r}]")
            else:
                group.append(line.split(" ")[1])
        elif group:
            header_groups.append(group.copy())
            group = []

    for group in header_groups:
        correct = sorted(group)

        if correct == group:
            continue

        print("!", filepath.replace(basepath, "."))
        print("\n".join(f"[[ {g} ]]" for g in group))
        print(end="\n")

        found = True
    return found


def main():
    found = False
    search_path = os.path.expanduser(sys.argv[1] if len(sys.argv) > 1 else ".")

    print("Searching in", search_path)
    for dirs, _, files in os.walk(search_path):
        for filename in files:
            if filename[-2:] in {".c", ".h"}:
                found |= check_file(os.path.join(dirs, filename), search_path)

    return found


if __name__ == "__main__":
    main()
