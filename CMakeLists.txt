cmake_minimum_required(VERSION 3.17)
project(KoopaHV C ASM)
SET(CMAKE_C_STANDARD 99)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER_WORKS 1)
SET(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER_WORKS 1)
SET(CMAKE_CXX_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_ASM_COMPILER  aarch64-linux-gnu-gcc)
set(CMAKE_AR aarch64-linux-gnu-ar)
set(CMAKE_OBJCOPY aarch64-linux-gnu-objcopy)
set(CMAKE_OBJDUMP aarch64-linux-gnu-objdump)
set(SIZE aarch64-linux-gnu-size)

SET(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/src/linker.ld)
SET(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_SCRIPT}")

SET(COMMON_FLAGS         "-Wall \
                          -nostdlib \
                          -nostartfiles \
                          -ffreestanding \
                          -mgeneral-regs-only")

SET(CMAKE_C_FLAGS "${COMMON_FLAGS} ")

include_directories("${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "src/*"
                          "src/arch/*"
                          "src/bsp/rasp4/*"
                          "src/util/*")

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})


SET(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)


add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND cp ${BIN_FILE} ${PROJECT_SOURCE_DIR}/kernel8.img)
