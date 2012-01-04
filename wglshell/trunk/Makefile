.PHONY: all
all: library testharness include

UNAME = ${shell uname}
ifeq (${UNAME},Linux)
HOST_PLATFORM = linux
else ifeq (${UNAME},Darwin)
-include ~/.stem.defines
HOST_PLATFORM = macosx
else
HOST_PLATFORM = windows
endif

include version

define newline_and_tab

	
endef

TARGET_PLATFORMS_macosx = 
TARGET_PLATFORMS_linux = 
TARGET_PLATFORMS_windows = windows

PROJECT_NAME = wglshell
SVNROOT = http://sacredsoftware.net/svn/misc#master_source_only

LIBRARY_TARGETS = library
EXECUTABLE_TARGETS = 
APPLICATION_TARGETS = testharness
TARGETS = ${LIBRARY_TARGETS} ${EXECUTABLE_TARGETS} ${APPLICATION_TARGETS}
CONFIGURATIONS = debug profile release
PLATFORMS = ${filter ${TARGET_PLATFORMS_${HOST_PLATFORM}},windows}
ANALYZERS = splint clang

TARGET_NAME_library = libstemshell_wgl
TARGET_NAME_testharness = wglshell_testharness
HUMAN_READABLE_TARGET_NAME_testharness = WGLShell

#Per-target configurations
CONFIGURATIONS_library = debug profile release
CONFIGURATIONS_testharness = debug profile release

#Per-target platforms
PLATFORMS_library = ${filter ${PLATFORMS},windows}
PLATFORMS_testharness = ${filter ${PLATFORMS},windows}

#Per-target compile/link settings

#Per-target analyzer settings

#Per-configuration compile/link settings
CCFLAGS_debug = -g -DDEBUG
CCFLAGS_profile = -g -O3
CCFLAGS_release = -O3

#Per-platform compile/link settings
CC_windows_i686 = C:/MinGW/bin/gcc.exe
AR_windows = C:/MinGW/bin/ar.exe
RANLIB_windows = C:/MinGW/bin/ranlib.exe
SPLINT_windows = C:/splint-3.1.1/bin/splint.exe
CLANG_windows = C:/llvm/bin/clang.exe
ARCHS_windows = i686
CCFLAGS_windows = -DGLEW_STATIC
LINKFLAGS_windows = -lopengl32 -lglu32 -lwinmm -lgdi32 -lpowrprof
EXECUTABLE_SUFFIX_windows = .exe

#General compile/link settings
DEFINE_CCFLAGS = -DVERSION_MAJOR=${VERSION_MAJOR}u -DVERSION_MINOR=${VERSION_MINOR}u -DVERSION_TWEAK=${VERSION_TWEAK}u
WARNING_CCFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror
OTHER_CCFLAGS = -std=gnu99
CCFLAGS = ${DEFINE_CCFLAGS} ${WARNING_CCFLAGS} ${OTHER_CCFLAGS}

FRAMEWORK_LINKFLAGS = 
LIBRARY_LINKFLAGS = 
OTHER_LINKFLAGS = 
LINKFLAGS = ${FRAMEWORK_LINKFLAGS} ${LIBRARY_LINKFLAGS} ${OTHER_LINKFLAGS}

LINK_ORDER = \
	library \
	glgraphics/libstem_glgraphics.a \
	glew/libglew.a

#Per-target dependencies

PROJECT_LIBRARY_DEPENDENCIES_testharness = library
STEM_LIBRARY_DEPENDENCIES_library = glgraphics/libstem_glgraphics.a
STEM_LIBRARY_DEPENDENCIES_testharness = glgraphics/libstem_glgraphics.a
STEM_SOURCE_DEPENDENCIES_library = 
STEM_SOURCE_DEPENDENCIES_testharness = 
THIRDPARTY_LIBRARY_DEPENDENCIES_library = glew/libglew.a
THIRDPARTY_LIBRARY_DEPENDENCIES_testharness = glew/libglew.a

#Per-target per-platform dependencies

PROJECT_LIBRARY_DEPENDENCIES_unittest_macosx = 
STEM_LIBRARY_DEPENDENCIES_unittest_macosx = 
STEM_SOURCE_DEPENDENCIES_unittest_macosx = 
THIRDPARTY_LIBRARY_DEPENDENCIES_unittest_macosx = 
#...

#Per-target source file lists

SOURCES_library = \
	source/wglshell/WGLShell.c

SOURCES_testharness = \
	test_source/testharness/TestHarness_main.c

#Include files to be distributed with library

INCLUDES = \
	source/wglshell/WGLShell.h \
	source/wglshell/WGLTarget.h

#Target resources

RESOURCES_testharness = 
RESOURCES_testharness_macosx = 
#...

#General analyzer settings
CLANGFLAGS = 
CLANGFLAGS_windows = -I C:/MinGW/include -I C:/MinGW/lib/gcc/mingw32/4.3.2/include
SPLINTFLAGS = -exportlocal

#Source files excluded from static analysis

ANALYZER_EXCLUDE_SOURCES_clang = 
ANALYZER_EXCLUDE_SOURCES_splint = 



define configuration_object_list_template #(target, configuration)
	${foreach platform,${PLATFORMS_$1}, \
		${call platform_object_list_template,$1,$2,${platform}} \
	}
endef

define platform_object_list_template #(target, configuration, platform)
	${foreach arch,${ARCHS_$3}, \
		${call arch_object_list_template,$1,$2,$3,${arch}} \
	}
endef

define arch_object_list_template #(target, configuration, platform, arch)
	${foreach source,${SOURCES_$1} ${SOURCES_$1_$3}, \
		build/intermediate/$1-$2-$3-$4/${notdir ${basename ${source}}}.o \
	}
endef

#Produces OBJECTS_${target}_${configuration} variables for each permutation of target and configuration in that target
${foreach target,${TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${eval OBJECTS_${target}_${configuration} = ${call configuration_object_list_template,${target},${configuration}}} \
	} \
}



define create_directory_target_template #(dir)
.LOW_RESOLUTION_TIME: $1
$1:
	mkdir -p $1
endef

${foreach target,${TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval ${call create_directory_target_template,build/${target}/${configuration}-${platform}}} \
			${foreach arch,${ARCHS_${platform}}, \
				${eval ${call create_directory_target_template,build/intermediate/${target}-${configuration}-${platform}-${arch}}} \
			} \
		} \
	} \
}
${eval ${call create_directory_target_template,build/intermediate}}



define include_ccflags_template #(target, platform)
-I source -I include \
${foreach stem_dependency,${STEM_LIBRARY_DEPENDENCIES_$1} ${STEM_LIBRARY_DEPENDENCIES_$1_$2},-I lib/${dir ${stem_dependency}}include} \
${foreach thirdparty_dependency,${THIRDPARTY_LIBRARY_DEPENDENCIES_$1} ${THIRDPARTY_LIBRARY_DEPENDENCIES_$1_$2},-I lib/${dir ${thirdparty_dependency}}include} \
${foreach source_dependency,${STEM_SOURCE_DEPENDENCIES_$1} ${STEM_SOURCE_DEPENDENCIES_$1_$2},-I dep/${word 1,${subst /, ,${source_dependency}}}/source}
endef

define define_ccflags_template #(target, configuration, platform, arch)
-DSTEM_TARGET_$1 -DSTEM_CONFIGURATION_$2 -DSTEM_PLATFORM_$3 -DSTEM_ARCH_$4
endef

define dependency_template #(target, configuration, platform, arch, source_file)
build/intermediate/$1-$2-$3-$4/${notdir ${basename $5}}.d: $5 ${PREREQS_$1} | build/intermediate/$1-$2-$3-$4
	${CC_$3_$4} ${CCFLAGS} ${CCFLAGS_$1} ${CCFLAGS_$2} ${CCFLAGS_$3} ${call include_ccflags_template,$1,$3} ${call define_ccflags_template,$1,$2,$3,$4} -MM -o $$@.temp $5
	sed 's,\(${notdir ${basename $5}}\)\.o[ :]*,$${basename $$@}.o $${basename $$@}.d: ,g' < $$@.temp > $$@
	rm $$@.temp
endef

#Produces dependency build targets for all source files in each configuration/platform/arch
ifeq ($(filter clean full_dist commit_dist analyze analyze_clang analyze_splint,${MAKECMDGOALS}),)
${foreach target,${TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${foreach arch,${ARCHS_${platform}}, \
				${foreach source,${SOURCES_${target}} ${SOURCES_${target}_${platform}}, \
					${eval ${call dependency_template,${target},${configuration},${platform},${arch},${source}}} \
					${eval -include build/intermediate/${target}-${configuration}-${platform}-${arch}/${notdir ${basename ${source}}}.d} \
				} \
			} \
		} \
	} \
}
endif



define compile_template #(target, configuration, platform, arch, source_file)
build/intermediate/$1-$2-$3-$4/${notdir ${basename $5}}.o: $5 ${PREREQS_$1} | build/intermediate/$1-$2-$3-$4
	${CC_$3_$4} ${CCFLAGS} ${CCFLAGS_$1} ${CCFLAGS_$2} ${CCFLAGS_$3} ${call include_ccflags_template,$1,$3} ${call define_ccflags_template,$1,$2,$3,$4} -c -o $$@ $5
endef

#Produces object build targets for all source files in each configuration/platform/arch
${foreach target,${TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${foreach arch,${ARCHS_${platform}}, \
				${foreach source,${SOURCES_${target}} ${SOURCES_${target}_${platform}}, \
					${eval ${call compile_template,${target},${configuration},${platform},${arch},${source}}} \
				} \
			} \
		} \
	} \
}



define library_template #(target, configuration, platform, arch, output_file)
build/intermediate/$1-$2-$3-$4/$5: ${call arch_object_list_template,$1,$2,$3,$4}
	${AR_$3} rc $$@ $$^
	${RANLIB_$3} $$@
endef

#Produces static library build targets for each arch/platform/target for library targets
${foreach target,${LIBRARY_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${foreach arch,${ARCHS_${platform}}, \
				${eval ${call library_template,${target},${configuration},${platform},${arch},${TARGET_NAME_${target}}.a}} \
			} \
		} \
	} \
}



define executable_template #(target, configuration, platform, arch, output_file, dependent_libraries)
build/intermediate/$1-$2-$3-$4/$5: ${call arch_object_list_template,$1,$2,$3,$4} $6
	${CC_$3_$4} -o $$@ $$^ ${LINKFLAGS} ${LINKFLAGS_$3}
endef

define library_dependency_template #(target, configuration, platform)
	${foreach link_library,${LINK_ORDER}, \
		${foreach library,${filter ${link_library},${PROJECT_LIBRARY_DEPENDENCIES_$1} ${PROJECT_LIBRARY_DEPENDENCIES_$1_$3}}, \
			build/${library}/$2-$3/${TARGET_NAME_${library}}.a \
		} \
		${foreach library,${filter ${link_library},${STEM_LIBRARY_DEPENDENCIES_$1} ${STEM_LIBRARY_DEPENDENCIES_$1_$3}}, \
			lib/${dir ${library}}library/$2-$3/${notdir ${library}} \
		} \
		${foreach library,${filter ${link_library},${STEM_SOURCE_DEPENDENCIES_$1} ${STEM_SOURCE_DEPENDENCIES_$1_$3}}, \
			dep/${word 1,${subst /, ,${library}}}/build/${word 2,${subst /, ,${library}}}/$2-$3/${word 3,${subst /, ,${library}}} \
		} \
		${foreach library,${filter ${link_library},${THIRDPARTY_LIBRARY_DEPENDENCIES_$1} ${THIRDPARTY_LIBRARY_DEPENDENCIES_$1_$3}}, \
			lib/${dir ${library}}library/$3/${notdir ${library}} \
		} \
	}
endef

#Produces executable build targets for each arch/platform/target for executable and application targets
${foreach target,${EXECUTABLE_TARGETS} ${APPLICATION_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${foreach arch,${ARCHS_${platform}}, \
				${eval ${call executable_template,${target},${configuration},${platform},${arch},${TARGET_NAME_${target}}${EXECUTABLE_SUFFIX_${platform}},${call library_dependency_template,${target},${configuration},${platform}}}} \
			} \
		} \
	} \
}



define dependency_submake_template #(dependency)
.PHONY: $1
$1:
	${MAKE} -C dep/${word 1,${subst /, ,$1}}
endef

#Invokes make for each source dependency
${foreach dependency,${sort ${foreach target,${TARGETS},${foreach platform,${PLATFORMS_${target}},${STEM_SOURCE_DEPENDENCIES_${target}} ${STEM_SOURCE_DEPENDENCIES_${target}_${platform}}}}}, \
	${eval ${call dependency_submake_template,${dependency}}} \
}



define thin_binary_list_template #(target, configuration, platform, target_name)
	${foreach arch,${ARCHS_$3}, \
		build/intermediate/$1-$2-$3-${arch}/$4 \
	}
endef

#Produces THIN_BINARIES_${target}_${configuration}_${platform} variables for each target/configuration/platform for library targets
${foreach target,${LIBRARY_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval THIN_BINARIES_${target}_${configuration}_${platform} = ${call thin_binary_list_template,${target},${configuration},${platform},${TARGET_NAME_${target}}.a}} \
		} \
	} \
}

#Produces THIN_BINARIES_${target}_${configuration}_${platform} variables for each target/configuration/platform for executable targets
${foreach target,${EXECUTABLE_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval THIN_BINARIES_${target}_${configuration}_${platform} = ${call thin_binary_list_template,${target},${configuration},${platform},${TARGET_NAME_${target}}${EXECUTABLE_SUFFIX_${platform}}}} \
		} \
	} \
}

#Produces THIN_BINARIES_${target}_${configuration}_${platform} variables for each target/configuration/platform for application targets
${foreach target,${APPLICATION_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval THIN_BINARIES_${target}_${configuration}_${platform} = ${call thin_binary_list_template,${target},${configuration},${platform},${TARGET_NAME_${target}}${EXECUTABLE_SUFFIX_${platform}}}} \
		} \
	} \
}



define assemble_library_windows #(target, configuration, platform)
build/$1/$2-$3/${TARGET_NAME_$1}.a: ${THIN_BINARIES_$1_$2_$3} | build/$1/$2-$3
	cp ${THIN_BINARIES_$1_$2_$3} $$@
endef

#Produces final library build targets
${foreach target,${LIBRARY_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval ${call assemble_library_${HOST_PLATFORM},${target},${configuration},${platform}}} \
		} \
	} \
}

define copy_target_resources #(target, platform, resources_dir)
	${foreach resource,${RESOURCES_$1} ${RESOURCES_$1_$2}, \
		cp -r ${resource} $3${newline_and_tab} \
	}
	find $3 -name .svn -print0 -or -name .DS_Store -print0 | xargs -0 rm -rf
endef

define assemble_executable_windows #(target, configuration, platform)
build/$1/$2-$3/${TARGET_NAME_$1}.exe: ${THIN_BINARIES_$1_$2_$3} ${RESOURCES_$1} ${RESOURCES_$1_$3} | build/$1/$2-$3
	cp ${THIN_BINARIES_$1_$2_$3} $$@
	${call copy_target_resources,$1,$3,$${dir $$@}}
endef

#Produces final executable build targets
${foreach target,${EXECUTABLE_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval ${call assemble_executable_${HOST_PLATFORM},${target},${configuration},${platform}}} \
		} \
	} \
}

define assemble_application_windows #(target, configuration)
build/$1/$2-windows/${TARGET_NAME_$1}.exe: ${THIN_BINARIES_$1_$2_windows} ${RESOURCES_$1} ${RESOURCES_$1_windows} | build/$1/$2-windows
	mkdir -p build/$1/$2-windows/Resources
	${call copy_target_resources,$1,windows,build/$1/$2-windows/Resources}
	cp ${THIN_BINARIES_$1_$2_windows} $$@
endef

#Produces final application build targets
${foreach target,${APPLICATION_TARGETS}, \
	${foreach configuration,${CONFIGURATIONS_${target}}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval ${call assemble_application_${platform},${target},${configuration}}} \
		} \
	} \
}

define library_template #(target, configuration, platform)
${STEM_SOURCE_DEPENDENCIES_$1} ${STEM_SOURCE_DEPENDENCIES_$1_$3} build/$1/$2-$3/${TARGET_NAME_$1}.a
endef

define executable_template #(target, configuration, platform)
${STEM_SOURCE_DEPENDENCIES_$1} ${STEM_SOURCE_DEPENDENCIES_$1_$3} build/$1/$2-$3/${TARGET_NAME_$1}${EXECUTABLE_SUFFIX_$3}
endef

define application_template #(target, configuration, platform)
${STEM_SOURCE_DEPENDENCIES_$1} ${STEM_SOURCE_DEPENDENCIES_$1_$3} build/$1/$2-$3/${call application_file_template_$3,$1}
endef

define application_file_template_windows #(target)
${TARGET_NAME_$1}.exe
endef

define target_template #(target, target_type)
.PHONY: $1
$1: ${foreach configuration,${CONFIGURATIONS_$1},${foreach platform,${PLATFORMS_$1},${call $2_template,$1,${configuration},${platform}}}}
endef

${foreach target,${LIBRARY_TARGETS}, \
	${eval ${call target_template,${target},library}} \
}

${foreach target,${EXECUTABLE_TARGETS}, \
	${eval ${call target_template,${target},executable}} \
}

${foreach target,${APPLICATION_TARGETS}, \
	${eval ${call target_template,${target},application}} \
}

define analyze_file_template_clang #(target, platform, file)
build/analyzer-results/clang-$1-$2/${basename ${notdir $3}}.txt: $3 ${PREREQS_$1} | build/analyzer-results/clang-$1-$2
	${CLANG_$2} --analyze ${call include_ccflags_template,$1,$2} ${call define_ccflags_template,$1,analyze,$2,none} ${CLANGFLAGS} ${CLANGFLAGS_$1} ${CLANGFLAGS_$2} -o $${basename $$@}.plist $3 > $$@ 2>&1; true
	@cat $$@
endef

define analyze_file_template_splint #(target, platform, file)
build/analyzer-results/splint-$1-$2/${basename ${notdir $3}}.txt: $3 ${PREREQS_$1} | build/analyzer-results/splint-$1-$2
	${SPLINT_$2} ${call include_ccflags_template,$1,$2} ${call define_ccflags_template,$1,analyze,$2,none} ${SPLINTFLAGS} ${SPLINTFLAGS_$1} ${SPLINTFLAGS_$2} $3 > $$@ 2>&1; true
	@cat $$@
endef

define analyzed_sources_template #(analyzer, target, platform)
	${sort ${filter-out ${ANALYZER_EXCLUDE_SOURCES_$1},${SOURCES_$2} ${SOURCES_$2_$3}}}
endef

define analyzer_output_template #(analyzer, target, platform)
	${foreach file,${call analyzed_sources_template,$1,$2,$3}, \
		build/analyzer-results/$1-$2-$3/${basename ${notdir ${file}}}.txt \
	}
endef

define analyze_target_template #(analyzer, target, platform)
.PHONY: analyze_$1_$2_$3
analyze_$1_$2_$3: ${call analyzer_output_template,$1,$2,$3}
endef

define analyze_template #(analyzer)
.PHONY: analyze_$1
analyze_$1: ${foreach target,${TARGETS},${foreach platform,${PLATFORMS_${target}},analyze_$1_${target}_${platform}}}
endef

${foreach analyzer,${ANALYZERS}, \
	${eval ${call analyze_template,${analyzer}}} \
	${foreach target,${TARGETS}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval ${call analyze_target_template,${analyzer},${target},${platform}}} \
			${foreach file,${call analyzed_sources_template,${analyzer},${target},${platform}}, \
				${eval ${call analyze_file_template_${analyzer},${target},${platform},${file}}} \
			} \
		} \
	} \
}

${foreach analyzer,${ANALYZERS}, \
	${foreach target,${TARGETS}, \
		${foreach platform,${PLATFORMS_${target}}, \
			${eval ${call create_directory_target_template,build/analyzer-results/${analyzer}-${target}-${platform}}} \
		} \
	} \
}

.PHONY: analyze
analyze: ${foreach analyzer,${ANALYZERS},analyze_${analyzer}}

${foreach dir,${sort ${foreach include_file,${INCLUDES},build/include/${notdir ${patsubst %/,%,${dir ${include_file}}}}}}, \
	${eval ${call create_directory_target_template,${dir}}} \
}

.PHONY: include
include: ${INCLUDES} | ${foreach include_file,${INCLUDES},build/include/${notdir ${patsubst %/,%,${dir ${include_file}}}}}
	${foreach include_file,${INCLUDES}, \
		cp ${include_file} build/include/${notdir ${patsubst %/,%,${dir ${include_file}}}}${newline_and_tab} \
	}

.PHONY: clean
clean:
	rm -rf build
	rm -rf dist #master_source_only
	${foreach dependency,${sort ${foreach target,${TARGETS},${foreach platform,${PLATFORMS_${target}},${STEM_SOURCE_DEPENDENCIES_${target}} ${STEM_SOURCE_DEPENDENCIES_${target}_${platform}}}}}, \
		${MAKE} -C dep/${word 1,${subst /, ,${dependency}}} clean${newline_and_tab} \
	}

#master_source_only_begin
.PHONY: full_dist
full_dist: clean all
	mkdir dist dist/include dist/library dist/testharness
	cp Changes.txt License.txt ReadMe.txt dist
	cp -r build/include/* dist/include
	cp -r build/library/* dist/library
	cp -r build/testharness/* dist/testharness

.PHONY: append_dist
append_dist: all
	cp -r build/library/* dist/library
	cp -r build/testharness/* dist/testharness

.PHONY: commit_dist
commit_dist:
	svn import --no-ignore -m "Automated release of ${PROJECT_NAME} ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_TWEAK} from ${HOST_PLATFORM}" dist ${SVNROOT}/Releases/${PROJECT_NAME}/${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_TWEAK}

define filter_source_dist_file #(infile, outfile)
	ruby -e "print STDIN.read.gsub(/^.*#master_source_only\b.*$\n?/, '').gsub(/\n?#master_source_only_begin[\s\S]*?#master_source_only_end/, '').gsub(/#source_dist_only/, '')" < "$1" > "$2"
endef

.PHONY: source_dist
source_dist:
	mkdir -p build/source_dist/include
	mkdir -p build/source_dist/lib
	mkdir -p build/source_dist/source/wglshell
	mkdir -p build/source_dist/test_source/testharness
	
	${call filter_source_dist_file,Makefile,build/source_dist/Makefile}
	cp Changes.txt License.txt ReadMe.txt version build/source_dist
	
	cp -r include/* build/source_dist/include
	cp -r lib/* build/source_dist/lib
	find build/source_dist/include -name .svn -print0 | xargs -0 rm -rf
	find build/source_dist/lib -name .svn -print0 | xargs -0 rm -rf
	
	cp source/wglshell/* build/source_dist/source/wglshell
	
	cp test_resources/* build/source_dist/test_resources
	
	cp test_source/testharness/* build/source_dist/test_source/testharness
#master_source_only_end