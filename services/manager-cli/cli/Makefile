# tools/manager/Makefile -- builds the manager CLI tool.
#
# Uses header-only deps (CLI11, nlohmann/json, inja)
# downloaded on first build. fmt::print is shimmed by
# ./fmt/core.h using C++20 std::format, so fmtlib is
# not needed.
#
# Usage:
#   make            # build (auto-parallel)
#   make deps       # download headers only
#   make clean      # remove binary + deps + objects

NPROCS := $(shell sysctl -n hw.ncpu 2>/dev/null \
           || nproc 2>/dev/null || echo 4)
MAKEFLAGS += -j$(NPROCS)

CXX      ?= g++
CXXFLAGS := -std=c++20 -O3 -Wall -Wextra -I.
LDLIBS   := -lssl -lcrypto
BUILDDIR := build/obj

SRCS := main.cpp \
        commands/ActCmd.cpp \
        commands/ActCmdReg.cpp \
        commands/ActCmdRun.cpp \
        commands/CmakeGenCmd.cpp \
        commands/CmakeGenCtx.cpp \
        commands/ElasticCmd.cpp \
        commands/ElasticCmdDrop.cpp \
        commands/ElasticCmdInit.cpp \
        commands/ElasticCmdReindex.cpp \
        commands/ElasticCmdStatus.cpp \
        commands/SeedTransCmd.cpp \
        commands/SeedTransGen.cpp \
        commands/SeedTransOut.cpp \
        commands/UserCmdReg.cpp \
        commands/UserCmdGen.cpp \
        commands/UserCmdReset.cpp \
        commands/WorkflowCtx.cpp \
        commands/WorkflowExec.cpp \
        commands/WorkflowLoad.cpp \
        commands/WorkflowReg.cpp \
        commands/WorkflowSteps.cpp

OBJS := $(patsubst %.cpp,$(BUILDDIR)/%.o,$(SRCS))
HDRS := CLI/CLI.hpp nlohmann/json.hpp inja/inja.hpp

CLI11_URL := https://github.com/CLIUtils/CLI11/releases/download/v2.4.2/CLI11.hpp
JSON_URL  := https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp
INJA_URL  := https://github.com/pantor/inja/releases/download/v3.4.0/inja.hpp

manager: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: %.cpp $(HDRS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

deps: $(HDRS)

CLI/CLI.hpp:
	mkdir -p CLI && curl -sL $(CLI11_URL) -o $@

nlohmann/json.hpp:
	mkdir -p nlohmann && curl -sL $(JSON_URL) -o $@

inja/inja.hpp:
	mkdir -p inja && curl -sL $(INJA_URL) -o $@

deps-offline:
	mkdir -p CLI nlohmann inja
	curl -fsSL http://localhost:5050/v1/manager-hdrs/cli11/2.4.2/noarch/blob \
		-o CLI/CLI.hpp
	curl -fsSL http://localhost:5050/v1/manager-hdrs/nlohmann-json/3.11.3/noarch/blob \
		-o nlohmann/json.hpp
	curl -fsSL http://localhost:5050/v1/manager-hdrs/inja/3.4.0/noarch/blob \
		-o inja/inja.hpp

clean:
	rm -f manager
	rm -rf $(BUILDDIR)

distclean: clean
	rm -rf CLI nlohmann inja

.PHONY: deps deps-offline clean distclean
