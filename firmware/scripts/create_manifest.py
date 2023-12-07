#! /usr/bin/python3

import argparse
import json
import os


default_manifest = {
    "name": "Macro-Keyboard",
    "version": "0.0.1",
    "builds": [
        {
            "chipFamily": "ESP32-S3",
            "parts": [
                { "path": "bootloader.bin",      "offset": 0 },
                { "path": "partition-table.bin", "offset": 32768 },
                { "path": "Macro-Keyboard.bin",  "offset": 65536 }
            ]
        }
    ]
}


def create_manifest(input_file, name, version):

    manifest = default_manifest
    manifest["name"] = name
    manifest["version"] = version

    files = input_file["flash_files"]
    manifest["builds"][0]["parts"] = []

    for offset, path in files.items():
        part = {
            "path": os.path.basename(path), # keep only filename
            "offset": offset
        }
        manifest["builds"][0]["parts"].append(part)


    return manifest


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Create a json manifest for ESP Web tools")
    i = parser.add_argument_group("Input")
    i.add_argument("--file", default="flasher_args.json")
    o = parser.add_argument_group("Output")
    o.add_argument("--name", default=default_manifest["name"])
    o.add_argument("--version", default=default_manifest["version"])
    o.add_argument('--output', default="manifest.json")

    args = parser.parse_args()

    input_file = open(args.file, 'r')
    flasher_args = json.load(input_file) 

    manifest = create_manifest(flasher_args, args.name, args.version)

    output_file = open(args.output, 'w')
    json.dump(manifest, output_file, ensure_ascii=False, indent=4)



