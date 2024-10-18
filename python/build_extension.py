"""Build _racfu Python extension."""

import os

from glob import glob
from setuptools import Extension
from setuptools.command import build_ext

def assemble(asm_file: str, asm_directory: str):
    """Python extension assembling underlying objects"""
    cwd = os.getcwd()
    include_path = os.path.join(cwd, asm_directory)
    source_path = os.path.join(os.path.join(cwd, asm_directory, asm_file))
    obj_dir = os.path.join(cwd, "artifacts")
    obj_file = asm_file.split(".")[0]+".o"
    obj_path = os.path.join(obj_dir, obj_file)

    print(f"assembling {source_path}")

    mkdir_command = f"mkdir {obj_dir}"
    print(mkdir_command)
    os.system(mkdir_command)

    assemble_command = f"as -mGOFF -I{include_path} -o {obj_path} {source_path}"
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