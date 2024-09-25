"""Build pyRACFu_core Python extesion."""

import os

from setuptools import Extension
from setuptools.command import build_ext


def build(setup_kwargs: dict):
    """Python extension build entrypoint."""
    os.system('as -mGOFF -I$(IRRSEQ00_SRC) -o artifacts/irrseq00.o racfu/c/irrseq00/irrseq00.s')
    if os.uname().sysname == "OS/390":
        # Build the real python extension on z/OS
        os.environ["CC"] = "ibm-clang"
        os.environ["CXX"] = "ibm-clang++"
        setup_kwargs.update(
            {
                "ext_modules": [
                    Extension(
                        "RACFu_py",
                        sources=[
                            "racfu/c/irrseq00/extract.cpp",
                            "racfu/c/irrseq00/post_process.cpp",
                            "racfu/c/irrsmo00/irsmo00.cpp",
                            "racfu/c/irrsmo00/xml_generator.cpp",
                            "racfu/c/irrsmo00/xml_parser.cpp",
                            "racfu/c/key_map/key_map.cpp",
                            "racfu/c/racfu.cpp",
                            "racfu/python/RACFU_py.c"
                            ],
                        include_dirs=[
                            "racfu/c/irrseq00",
                            "racfu/c/irrsmo00",
                            "racfu/c/key_map",
                            "racfu/c",
                            "externals"
                            ],
                        libraries = [
                            "artifacts/irrseq00.o"],
                        extra_compile_args=["-o artifacts"
                        ],
                        extra_link_args=["-o dist/racfu.so"]
                    )
                ],
                "cmdclass": {"built_ext": build_ext},
            }
        )
    else:
        # Build the test python extension on non-z/OS
        print("\n\n\n\n")
        setup_kwargs.update(
            {
                "ext_modules": [
                    Extension(
                        "RACFu_py",
                        sources=[
                            "racfu/c/irrseq00/extract.cpp",
                            "racfu/c/irrseq00/post_process.cpp",
                            "racfu/c/irrsmo00/irsmo00.cpp",
                            "racfu/c/irrsmo00/xml_generator.cpp",
                            "racfu/c/irrsmo00/xml_parser.cpp",
                            "racfu/c/key_map/key_map.cpp",
                            "racfu/c/racfu.cpp"],
                        include_dirs=["racfu/c/irrseq00", "racfu/c/irrsmo00", "racfu/c/key_map", "racfu/c"],
                        libraries = ["artifacts/irrseq00.o"],
                        extra_compile_args=["-o artifacts"
                        ],
                        extra_link_args=["-o dist/racfu.so"]
                    )
                ],
                "cmdclass": {"built_ext": build_ext},
            }
        )
