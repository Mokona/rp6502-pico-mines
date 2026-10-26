cmake_minimum_required(VERSION 3.21)

include(${CMAKE_CURRENT_LIST_DIR}/tools/rp6502.cmake)

if (RP6502_BASIC)
    project(MY-RP6502-PROJECT BASIC)
    add_executable(pico-mines)
    rp6502_asset(pico-mines help src/help.txt)
    rp6502_asset(pico-mines pico-mines.bas src/main.bas)
    rp6502_basic(pico-mines pico-mines.bas)
    rp6502_web(pico-mines)
else ()
    project(MY-RP6502-PROJECT C CXX ASM)
    add_executable(pico-mines)
    rp6502_align(pico-mines "XRAM_.*")
    rp6502_asset(pico-mines help src/help.txt)
    rp6502_asset(pico-mines XRAM(XRAM_TILES) src/data/cell_hidden.bin)
    rp6502_executable(pico-mines DATA default RESET default)
    rp6502_web(pico-mines CONFIG [[
            title: 'Pico Mines',
    ]])
    target_sources(pico-mines PRIVATE
            src/main.c
            src/input_actions.c
            src/input_actions.h
    )
endif ()
