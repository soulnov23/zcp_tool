include Inc.mk
include ../Inc.mk

all: googletest spdlog yaml-cpp nlohmann abseil-cpp

#cmake构建行以+开头且不写-j：让cmake生成的make加入上层jobserver，总并发受顶层-j控制，
#写死-j32会重置jobserver，多个库同时编译时并发数叠加；代价是make -n时这些行也会真正执行
googletest:
	wget https://github.com/google/googletest/archive/refs/tags/$(GTEST).tar.gz
	tar -zvxf $(GTEST).tar.gz
	rm -rf $(GTEST).tar.gz
	rm -rf gtest gmock
	+cd googletest-$(GTEST:v%=%) && mkdir -p build && cd build && \
	cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_CXX_STANDARD=20 -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX=$(PROJ_PATH)/third_party/googletest-$(GTEST:v%=%)/install .. && \
	cmake --build . && cmake --install .
	cp -rf googletest-$(GTEST:v%=%)/install/include/gtest ./
	cp -rf googletest-$(GTEST:v%=%)/install/include/gmock ./
	cp -rf googletest-$(GTEST:v%=%)/install/lib/libgmock*.so* $(LIB_DIR)
	cp -rf googletest-$(GTEST:v%=%)/install/lib/libgtest*.so* $(LIB_DIR)
	rm -rf googletest-$(GTEST:v%=%)

spdlog:
	wget https://github.com/gabime/spdlog/archive/refs/tags/$(SPDLOG).tar.gz
	tar -zvxf $(SPDLOG).tar.gz
	rm -rf $(SPDLOG).tar.gz
	rm -rf spdlog
	mv -f spdlog-$(SPDLOG:v%=%)/include/spdlog ./
	rm -rf spdlog-$(SPDLOG:v%=%)

yaml-cpp:
	wget https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-$(YAML_CPP).tar.gz
	tar -zvxf yaml-cpp-$(YAML_CPP).tar.gz
	rm -rf yaml-cpp-$(YAML_CPP).tar.gz
	rm -rf yaml-cpp
	+cd yaml-cpp-yaml-cpp-$(YAML_CPP) && mkdir -p build && cd build && \
	cmake -DYAML_BUILD_SHARED_LIBS=ON -DYAML_CPP_BUILD_TESTS=OFF -DYAML_CPP_BUILD_TOOLS=OFF -DYAML_CPP_INSTALL=ON -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=$(PROJ_PATH)/third_party/yaml-cpp-yaml-cpp-$(YAML_CPP)/install -DCMAKE_INSTALL_LIBDIR=lib .. && \
	cmake --build . && cmake --install .
	cp -rf yaml-cpp-yaml-cpp-$(YAML_CPP)/install/include/yaml-cpp ./
	cp -rf yaml-cpp-yaml-cpp-$(YAML_CPP)/install/lib/libyaml-cpp.so* $(LIB_DIR)
	rm -rf yaml-cpp-yaml-cpp-$(YAML_CPP)

nlohmann:
	wget https://github.com/nlohmann/json/archive/refs/tags/$(NLOHMANN).tar.gz
	tar -zvxf $(NLOHMANN).tar.gz
	rm -rf $(NLOHMANN).tar.gz
	rm -rf nlohmann
	mv -f json-$(NLOHMANN:v%=%)/include/nlohmann ./
	rm -rf json-$(NLOHMANN:v%=%)

abseil-cpp:
	wget https://github.com/abseil/abseil-cpp/archive/refs/tags/$(ABSEIL_CPP).tar.gz
	tar -zvxf $(ABSEIL_CPP).tar.gz
	rm -rf $(ABSEIL_CPP).tar.gz
	rm -rf absl
	+cd abseil-cpp-$(ABSEIL_CPP) && mkdir -p build && cd build && \
	cmake -DABSL_BUILD_TESTING=OFF -DABSL_ENABLE_INSTALL=ON -DABSL_PROPAGATE_CXX_STD=ON -DBUILD_SHARED_LIBS=ON -DABSL_BUILD_MONOLITHIC_SHARED_LIBS=ON -DCMAKE_CXX_STANDARD=20 -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_PREFIX=$(PROJ_PATH)/third_party/abseil-cpp-$(ABSEIL_CPP)/install .. && \
	cmake --build . && cmake --install .
	cp -rf abseil-cpp-$(ABSEIL_CPP)/install/include/absl ./
	cp -rf abseil-cpp-$(ABSEIL_CPP)/install/lib/libabseil_dll.so* $(LIB_DIR)
	rm -rf abseil-cpp-$(ABSEIL_CPP)

clean:
	rm -rf gtest gmock spdlog yaml-cpp nlohmann absl
	rm -rf $(LIB_DIR)/libgmock*.*
	rm -rf $(LIB_DIR)/libgtest*.*
	rm -rf $(LIB_DIR)/libyaml-cpp.so*
	rm -rf $(LIB_DIR)/libabseil_dll.so*

#所有目标名都和同名目录重名，不声明为PHONY的话make会认为"目录已存在即最新"而直接跳过
.PHONY: all googletest spdlog yaml-cpp nlohmann abseil-cpp clean

.DEFAULT_GOAL := all