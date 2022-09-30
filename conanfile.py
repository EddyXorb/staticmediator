import os

from conans import ConanFile, tools, CMake


class StaticMediatorConan(ConanFile):
    name = "StaticMediator"
    version = "1.0"
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of StaticMediator here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    no_copy_source = True
    # No settings/options are necessary, this is header only
    generators = "cmake"
    exports_sources = "*"
    requires = "gtest/1.12.1"

    def build(self):
        cmake = CMake(self)
        cmake.configure(".")
        cmake.build()

    def package(self):
        self.copy("*.hpp", dst="include", src="include")
        self.copy("*test.exe", dst="bin", src="")

    def package_id(self):
        self.info.header_only()
