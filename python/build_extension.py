"""Build racfu_py Python extension."""

import os

from glob import glob
from setuptools import Extension
from setuptools.command import build_ext

def assemble(asm_file: str, asm_directory: str):
    """Python extension assembling underlying objects"""
    print(f"assembling {asm_directory}/{asm_file}")
    make_artifacts = "mkdir artifacts"
    print(make_artifacts)
    os.system(make_artifacts)

    library_file = asm_file.split(".")[0]+".o"
    cwd = os.getcwd()
    assemble_command = f"as -mGOFF -I{os.path.join(cwd, asm_directory)} -o {os.path.join(cwd, 'artifacts', library_file)} {os.path.join(cwd, asm_directory, asm_file)}"
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
                    "racfu_py",
                    sources=(
                        glob("racfu/**/*.cpp")+
                        glob("racfu/*.cpp")+
                        ["python/racfu/racfu_py.c"]
                        ),
                    include_dirs=(
                        glob("racfu/**/")+
                        ["racfu","externals"]
                        ),
                    extra_link_args = [
                        "-m64",
                        "-Wl,-b,edit=no"
                    ],
                    libraries = [
                        "externals/nlohmann/json.hpp"
                    ],
                    extra_objects = [
                        "artifacts/irrseq00.o"
                    ]
                )
            ],
            "cmdclass": {"built_ext": build_ext},
        }
    )