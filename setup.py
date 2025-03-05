"""Build _racfu Python extension."""

import json
import os
import subprocess
from glob import glob
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


def assemble(asm_file: str, asm_directory: str) -> None:
    """Python extension assembling underlying objects"""
    obj_file = asm_file.split(".")[0]+".o"
    cwd = Path.cwd()
    source_file = cwd / asm_directory / asm_file
    obj_file = cwd / "artifacts" / obj_file

    if obj_file.exists():
        return

    print(f"assembling {source_file}")

    if not obj_file.parents[0].is_dir():
        mkdir_command = f"mkdir {obj_file.parents[0]}"
        print(mkdir_command)
        subprocess.run(mkdir_command, shell=True, check=True)

    assemble_command = f"as -mGOFF -I{source_file.parents[0]} -o {obj_file} {source_file}"
    print(assemble_command)
    subprocess.run(assemble_command, shell=True, check=True)

def insert_json_schema(json_schema_file: str, header_file: str, macro: str) -> None:
    """Load a JSON schema as a minified JSON string"""
    cwd = Path.cwd()
    with open(cwd / "schemas" / json_schema_file, "r") as file_handle:
        json_schema = json.dumps(json.load(file_handle), separators=(",", ":"))
    with open(cwd / "racfu" / header_file, "rw") as file_handle:
        header_file_content = file_handle.read().replace(
            macro, f"'\"({json_schema})\"_json'")
        file_handle.write(header_file_content)

class build_and_asm_ext(build_ext):
    def run(self):
        os.environ["CC"] = "ibm-clang"
        os.environ["CFLAGS"] = "-std=c99"
        os.environ["CXX"] = "ibm-clang++"
        os.environ["CXXFLAGS"] = "-std=c++11",
        racfu_source_path = Path("racfu")
        assemble("irrseq00.s", racfu_source_path / 'irrseq00')
        super().run()

def main():
    """Python extension build entrypoint."""
    cwd = Path.cwd()
    assembled_object_path = cwd / "artifacts" / "irrseq00.o"
    insert_json_schema("parameters.json", "security_admin.hpp", "RACFU_PARAMETERS_SCHEMA")
    setup_args ={
        "ext_modules": [
                Extension(
                    "racfu._C",
                    sources=(
                        glob("racfu/**/*.cpp")
                        + glob("racfu/*.cpp")
                        + glob("externals/json-schema-validator/*.cpp")
                        + ["racfu/python/_racfu.c"]
                    ),
                    include_dirs=(
                        glob("racfu/**/")
                        + [
                            "racfu", 
                            "externals/json", 
                            "externals/json-schema-validator"
                        ]
                    ),
                    extra_link_args = [
                        "-m64",
                        "-Wl,-b,edit=no"
                    ],
                    extra_objects = [
                        f"{assembled_object_path}"
                    ]
                )
            ],
            "cmdclass": {"build_ext": build_and_asm_ext}
    }
    setup(**setup_args)

if __name__ == "__main__":
    main()