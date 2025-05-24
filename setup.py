import os
import pathlib
from time import sleep

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_orig


class CMakeExtension(Extension):
    def __init__(self, name):
        # don't invoke the original build_ext for this special extension
        super().__init__(name, sources=[])


class build_ext(build_ext_orig):
    def build_extension(self, ext) -> None:
        self.build_cmake(ext)

    def build_cmake(self, ext):
        cwd = pathlib.Path().absolute()

        build_temp = pathlib.Path(self.build_temp)
        # ensure temporary build directory exists
        build_temp.mkdir(parents=True, exist_ok=True)

        extdir = pathlib.Path(self.get_ext_fullpath(ext.name))
        # ensure output directory exists
        # probably not necessary, as cmake will create the directory during install
        extdir.parent.mkdir(parents=True, exist_ok=True)

        build_lib = pathlib.Path(self.build_lib)
        relative = extdir.relative_to(build_lib)

        config = 'Debug' if self.debug else 'Release'
        cmake_build_dir = "cmake-build"

        cmake_args = [
            "-DCMAKE_BUILD_TYPE=" + config,
            "-DSEAR_PYTHON_EXTENSION_PATH=" + str(relative),
            "--toolchain=cmake/ibm-clang.cmake"
        ]

        build_args = [
            '--config', config,
            '--', '-j4'
        ]

        install_args = [
            "--prefix=" + str(build_lib.absolute())
        ]

        # configure cmake build directory
        self.spawn(['cmake', str(cwd), "-B", cmake_build_dir] + cmake_args)
        if not self.dry_run:
            # first run
            self.spawn(['cmake', '--build', cmake_build_dir] + build_args)
            # then install built extension module
            self.spawn(["cmake", "--install", cmake_build_dir] + install_args)


setup(
    name='sear',
    ext_modules=[CMakeExtension('sear._C')],
    cmdclass={
        'build_ext': build_ext,
    }
)
