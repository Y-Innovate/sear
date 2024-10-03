"""Build RACFu_py Python extension."""

import os

from glob import glob
from setuptools import Extension
from setuptools.command import build_ext


def build(setup_kwargs: dict):
    """Python extension build entrypoint."""
    os.environ["CC"] = "ibm-clang"
    os.environ["CXX"] = "ibm-clang++"
    setup_kwargs.update(
        {
            "ext_modules": [
                Extension(
                    "RACFu_py",
                    sources=(
                        glob("racfu/c/**/*.cpp")+
                        glob("racfu/**/*.cpp")+
                        ["racfu/python/RACFu_py.c"]
                        ),
                    include_dirs=(
                        glob("racfu/c/**/")+
                        ["racfu/c","externals","artifacts"]
                        ),
                    extra_link_args = [
                        "-m64",
                        "-Wl,-b,edit=no"
                    ],
                    extra_objects = [
                        "racfu/c/irrseq00.s"
                    ]
                )
            ],
            "cmdclass": {"built_ext": build_ext},
        }
    )