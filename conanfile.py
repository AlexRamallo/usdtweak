import os
from pathlib import Path
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake, cmake_layout
from conan.tools.files import copy, get


class USDTweak(ConanFile):
    name = "usdtweak"
    version = '0.1.0'
    settings = 'os', 'build_type', 'arch', 'compiler'

    default_options = {
        'openusd/*:openvdb': False
    }

    def requirements(self):
        self.requires('glfw/3.4')
        self.requires('openusd/25.02a')
        self.requires('materialx/1.38.10')
        self.requires('expat/2.6.2', override=True)
        self.requires('opengl/system')

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        dep = CMakeDeps(self)

        tc.variables['glfw3_DIR'] = self.dependencies["glfw"].package_folder
        tc.variables['PXR_LIBRARIES'] = 'openusd::openusd'
        tc.variables['MATERIALX_LIBRARIES'] = 'materialx::materialx'

        tc.generate()
        dep.generate()

    _cmake = None
    def _configure_cmake(self):
        if self._cmake:
            return self._cmake
        self._patch_sources_cmake()
        self._cmake = CMake(self)
        self._cmake.configure()
        return self._cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
