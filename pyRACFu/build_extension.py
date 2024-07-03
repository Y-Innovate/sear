"""Build pyRACFu_core Python extesion."""

import os

from setuptools import Extension
from setuptools.command import build_ext


def build(setup_kwargs: dict):
    """Python extension build entrypoint."""
    print('hello world')
    os.system('touch test.txt')
    if os.uname().sysname == "OS/390":
        # Build the real python extension on z/OS
        setup_kwargs.update(
            {
                "ext_modules": [
                    Extension(
                        "pyRACFu_core",
                        sources=["pyRACFu/core/pyRACFu_core.c"],
                        include_dirs=["../src"],
                        libraries = ["../src/racfu_result.h"],
                        extra_compile_args=[
                        ],
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
                        "pyRACFu_core",
                        sources=["pyRACFu/core/pyRACFu_core.c"],
                        include_dirs=["../src"],
                        libraries = ["../src/racfu_result.h"],
                    )
                ],
                "cmdclass": {"built_ext": build_ext},
            }
        )
