"""Build RACFu_py Python extension."""

import os

from glob import glob
from setuptools import Extension
from setuptools.command import build_ext

def build(setup_kwargs: dict):
    """Python extension build entrypoint."""
    os.environ["CC"] = "ibm-clang"
    os.environ["CXX"] = "ibm-clang++"
    os.system(f"as -mGOFF -I{os.path.join('racfu','irrseq00')} -o {os.path.join('racfu','irrseq00','irrseq00.o')} {os.path.join('racfu','irrseq00','irrseq00.s')}")
    setup_kwargs.update(
        {
            "ext_modules": [
                Extension(
                    "RACFu_py",
                    sources=(
                        glob("racfu/**/*.cpp")+
                        glob("racfu/*.cpp")+
                        ["racfu_py/RACFu_py.c"]
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
                        "racfu/irrseq00/irrseq00.o"
                    ]
                )
            ],
            "cmdclass": {"built_ext": build_ext},
        }
    )