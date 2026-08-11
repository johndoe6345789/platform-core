"""Conan 2 recipe for the Nextra C++ backend."""

import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout


class NextraBackend(ConanFile):
    """Backend dependencies and build configuration."""

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    default_options = {
        "drogon/*:with_postgres": True,
        "drogon/*:with_postgres_batch": True,
        "drogon/*:with_redis": True,
    }

    def requirements(self):
        """Declare runtime dependencies."""
        self.requires("drogon/1.9.3")
        self.requires("nlohmann_json/3.11.3")
        self.requires("cli11/2.4.2")
        self.requires("jwt-cpp/0.7.0")
        self.requires("inja/3.4.0")
        self.requires("mailio/0.23.0")
        self.requires("fmt/10.2.1")
        self.requires("spdlog/1.14.1")
        self.requires("hiredis/1.2.0")
        self.requires("librdkafka/2.5.0")
        # libvips is optional; see CMakeLists.txt.
        # Enable with -o with_vips=True if the recipe
        # is available on the consumer's Conan remote.
        # Pin boost to resolve conflict between
        # drogon (1.83) and mailio (1.86).
        self.requires("boost/1.86.0", force=True)
        # Pin openssl to 3.3.x LTS; openssl/3.6.1 (pulled in
        # transitively via drogon) fails to build on linux/arm64.
        self.requires("openssl/3.3.2", override=True)

    def build_requirements(self):
        """Declare test-only dependencies."""
        if not os.environ.get("CONAN_SKIP_TEST"):
            self.test_requires("gtest/1.14.0")

    def layout(self):
        """Use standard CMake layout."""
        cmake_layout(self)
