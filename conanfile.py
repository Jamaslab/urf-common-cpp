import os
from conans import ConanFile, CMake, tools

def get_file(filename):
    f = open(filename, "r")
    return f.read()

class UrfCommonCppConan(ConanFile):
    _windows_import_paths = ["../Windows/{cf.settings.build_type}/bin/{cf.settings.build_type}"]

    name = "urf_common_cpp"
    version = "1.7.0"
    license = "MIT"
    author = "Giacomo Lunghi"
    url = "https://gitlab.com/urobf/urf-common-cpp"
    description = "Unified Robotic Framework Common Objects"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    short_paths = True
    import_paths = []
    default_options = {"shared": True}
    requires = ("spdlog/1.8.2", "nlohmann_json/3.9.1", "eigen/3.3.9")
    build_requires = ("gtest/1.10.0", "cmake/3.25.0")
    generators = "cmake", "cmake_find_package", "virtualenv"
    exports_sources = ["environment/*", "src/*", "tests/*", "CMakeLists.txt", "LICENSE", "README.md"]

    @property
    def default_user(self):
        return "uji-cirtesu-irslab+urobf+urf-common-cpp"

    @property
    def default_channel(self):
        return "stable"

    def requirements(self):
        installer = tools.SystemPackageTool()
        if tools.os_info.is_linux and tools.os_info.linux_distro in ("ubuntu"):
            installer = tools.SystemPackageTool()
            if not installer.installed("lcov"):
                try:
                    installer.install("lcov", update=True)
                except:
                    print("Could not install lcov")

    def configure(self):
        self.options["spdlog"].header_only = True

    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_INSTALL_PREFIX"] = os.path.join(self.recipe_folder, 'package/')
        cmake.definitions["CMAKE_MODULE_PATH"] = self.install_folder.replace("\\", "/")
        cmake.definitions["CMAKE_BUILD_TYPE"] = self.settings.build_type
        cmake.definitions["BUILD_SHARED_LIBS"] = self.options.shared
        if not self.options.shared:
            cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = True

        cmake.configure(
            build_folder='build/%s' % str(self.settings.os) + '/' + str(self.settings.build_type))
        cmake.build()
        cmake.test(output_on_failure=True)
        cmake.install()

    def imports(self):
        # importing the dll where executable is for Visual Studio debugging - insted of use activate.bat script
        # FIXME : Temporary enable without any action to debug from Visual Studio
        # TODO : Use a vs plugins or a proper solution instead of copying dll
        import_paths = getattr(self, 'import_paths') + self._windows_import_paths

        for ipath in import_paths:
            self.copy("*.dll", src="lib", dst=ipath.format(cf=self))
            self.copy("*.pdb", src="lib", dst=ipath.format(cf=self))

    def package(self):
        self.copy("*.hpp", dst=".", src=os.path.join(self.recipe_folder, 'package/'), keep_path=True)
        self.copy("*.h", dst=".", src=os.path.join(self.recipe_folder, 'package/'), keep_path=True)
        if tools.os_info.is_linux:
            self.copy("*.so", dst=".", src=os.path.join(self.recipe_folder, 'package/'), keep_path=True)
            self.copy("*.a", dst=".", src=os.path.join(self.recipe_folder, 'package/'), keep_path=True)
        elif tools.os_info.is_windows:
            self.copy("*.dll", dst=".", src=os.path.join(self.recipe_folder, 'package/'), keep_path=True)
            self.copy("*.lib", dst=".", src=os.path.join(self.recipe_folder, 'package/'), keep_path=True)

    def package_info(self):
        self.cpp_info.libs = ['urf_common']
        self.cpp_info.includedirs = ['include/']
        self.cpp_info.libdirs = ['lib']

        self.env_info.PATH.append(os.path.join(self.package_folder, "lib"))
