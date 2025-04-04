"""Build Python extension."""

import json
import os
import subprocess
from glob import glob
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


def assemble(asm_file: str, asm_directory: Path) -> None:
    """Python extension assembling underlying objects."""
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

def load_json_schema(schema_relative_path: Path) -> str:
    """Load a JSON schema as a minified JSON string."""
    schema_absolute_path = Path.cwd() / "schemas" / schema_relative_path
    with open(schema_absolute_path, "r") as file_handle:
        return json.dumps(json.load(file_handle), separators=(",", ":"))

class build_with_asm_ext(build_ext):
    def run(self):
        os.environ["CC"] = "ibm-clang64"
        os.environ["CFLAGS"] = "-std=c99"
        os.environ["CXX"] = "ibm-clang++64"
        os.environ["CXXFLAGS"] = "-std=c++14"
        racfu_source_path = Path("racfu")
        assemble("irrseq00.s", racfu_source_path / "irrseq00")
        super().run()

def main():
    """Python extension build entrypoint."""
    cwd = Path.cwd()
    assembled_object_path = cwd / "artifacts" / "irrseq00.o"
    racfu_parameters_schema = load_json_schema(Path("parameters.json"))
    setup_args = {
        "ext_modules": [
                Extension(
                    "racfu._C",
                    define_macros=[
                        ("RACFU_PARAMETERS_SCHEMA", 
                        f"R\"({racfu_parameters_schema})\"_json")
                    ],
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
            "cmdclass": {"build_ext": build_with_asm_ext}
    }
    setup(**setup_args)

if __name__ == "__main__":
    main()
