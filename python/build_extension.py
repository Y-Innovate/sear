"""Build _racfu Python extension."""

import os

from pathlib import Path
from glob import glob
from setuptools import Extension
from setuptools.command import build_ext

def assemble(asm_file: str, asm_directory: str):
    """Python extension assembling underlying objects"""
    obj_file = asm_file.split(".")[0]+".o"
    cwd = Path.cwd()
    source_file = cwd / asm_directory / asm_file
    obj_file = cwd / "artifacts" / obj_file

    if obj_file.exists():
        print(f"using existing object {obj_file}")
        return

    print(f"assembling {source_file}")

    if not obj_file.parents[0].is_dir():
        if obj_file.parents[0].exists():
            raise Exception(f'local "artifacts" file exists but is not a directory; cannot use this for {source_file} assembly')
        mkdir_command = f"mkdir {obj_file.parents[0]}"
        print(mkdir_command)
        os.system(mkdir_command)

    assemble_command = f"as -mGOFF -I{source_file.parents[0]} -o {obj_file} {source_file}"
    print(assemble_command)
    os.system(assemble_command)


def build(setup_kwargs: dict):
    """Python extension build entrypoint."""
    os.environ["CC"] = "ibm-clang"
    os.environ["CXX"] = "ibm-clang++"
    assemble("irrseq00.s", os.path.join('racfu','irrseq00'))
    setup_kwargs.update(
        {
            "ext_modules": [
                Extension(
                    "racfu._C",
                    sources=(
                        glob("racfu/**/*.cpp")+
                        glob("racfu/*.cpp")+
                        ["racfu/python/_racfu.c"]
                        ),
                    include_dirs=(
                        glob("racfu/**/")+
                        ["racfu","externals"]
                        ),
                    extra_link_args = [
                        "-m64",
                        "-Wl,-b,edit=no"
                    ],
                    extra_objects = [
                        "artifacts/irrseq00.o"
                    ]
                )
            ],
            "cmdclass": {"built_ext": build_ext},
        }
    )