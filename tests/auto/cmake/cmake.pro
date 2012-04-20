
CMAKE_VERSION = $$system(cmake --version)

# Cause make to do nothing.
TEMPLATE = subdirs

check.commands =
isEmpty(CMAKE_VERSION) {
    message("cmake executable not found. Not running CMake unit tests")
} else {
    CTEST_VERSION = $$system(ctest --version)
    isEmpty(CTEST_VERSION) {
        message("ctest executable not found. Not running CMake unit tests")
    } else {
        CMAKE_VERSION = $$last(CMAKE_VERSION)
        CMAKE_VERSION_MAJOR = $$section(CMAKE_VERSION, ., 0, 0)
        CMAKE_VERSION_MINOR = $$section(CMAKE_VERSION, ., 1, 1)
        CMAKE_VERSION_PATCH = $$section(CMAKE_VERSION, ., 2, 2)

        VERSION_OK =
        greaterThan(CMAKE_VERSION_MAJOR, 2) {
            VERSION_OK = 1
        } else:greaterThan(CMAKE_VERSION_MAJOR, 1):greaterThan(CMAKE_VERSION_MINOR, 8) {
            VERSION_OK = 1
        } else:greaterThan(CMAKE_VERSION_MAJOR, 1):greaterThan(CMAKE_VERSION_MINOR, 7):greaterThan(CMAKE_VERSION_PATCH, 2) {
            VERSION_OK = 1
        }

        isEmpty(VERSION_OK) {
            message("cmake $$CMAKE_VERSION is too old for this test.")
        } else {
            SET = set
            equals(QMAKE_DIR_SEP, "/"):SET = export

            CMAKE_BUILD_TYPE = Debug
            CONFIG(release, debug|release):CMAKE_BUILD_TYPE = Release

            BUILD_DIR = $$replace($$list($$OUT_PWD/build), /, $$QMAKE_DIR_SEP)

            check.commands = \
                cd . && $$SET CMAKE_PREFIX_PATH=$$[QT_INSTALL_PREFIX] && \
                $(MKDIR) $$BUILD_DIR && cd $$BUILD_DIR && \
                cmake $$_PRO_FILE_PWD_ -DCMAKE_BUILD_TYPE=$${CMAKE_BUILD_TYPE} && \
                $(TESTRUNNER) ctest --output-on-failure

        }
    }
}

QMAKE_EXTRA_TARGETS *= check