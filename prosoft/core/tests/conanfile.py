from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps

class OSSCoreTestsConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("catch2/3.5.3")
        self.requires("nlohmann_json/3.11.3")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False    # don't generate CMakeUserPresets.json in source directory
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()
