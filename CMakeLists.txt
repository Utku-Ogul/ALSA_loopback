cmake_minimum_required(VERSION 3.20)
project(sesC)
set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED alsa)
pkg_check_modules(OPUS REQUIRED opus)

include_directories(
    ${ALSA_INCLUDE_DIRS} # ALSA header 
    ${OPUS_INCLUDE_DIRS} # Opus header 
    ${CMAKE_CURRENT_SOURCE_DIR} # Proje ana dizini (local .h dosyaları için)
)

add_executable(ses main.c alsa.c codec.c udp.c)

target_link_libraries(ses ${ALSA_LIBRARIES} ${OPUS_LIBRARIES})