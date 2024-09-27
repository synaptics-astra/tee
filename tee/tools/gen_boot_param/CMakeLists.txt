cmake_minimum_required (VERSION 3.19)
project (gen_boot_param)

set(PLATFORM bg2qa1_rdkdmp CACHE STRING "Platform")
set(AMP_TEE_ENABLE OFF CACHE BOOL "AMP TEE Enabled")
set(BERLIN_CHIP "" CACHE STRING "Chip")

if(AMP_TEE_ENABLE)
    set(PREFIX_TZBOOT_PARAM ntz)
else()
    set(PREFIX_TZBOOT_PARAM tz)
endif()

set(PLATFORM_PATH ../../products/${PLATFORM})

set(TZ_BOOT_PARAM_VALUE ${PLATFORM_PATH}/${PREFIX_TZBOOT_PARAM}_boot_param_value.c)
set(TZ_BOOT_PARAM_VALUE_RECOVERY ${PLATFORM_PATH}/${PREFIX_TZBOOT_PARAM}_boot_param_value_recovery.c)

if(NOT EXISTS ${TZ_BOOT_PARAM_VALUE})
    set(TZ_BOOT_PARAM_VALUE_RECOVERY ${TZ_BOOT_PARAM_VALUE})
endif()

string(TOLOWER ${BERLIN_CHIP} CHIP)

include_directories(../../tz/common ../../include ../../chip/${CHIP}/include)
add_compile_definitions(__LINUX__)

add_executable(gen_boot_param gen_boot_param.c ${TZ_BOOT_PARAM_VALUE})
add_executable(gen_boot_param_recovery gen_boot_param.c ${TZ_BOOT_PARAM_VALUE_RECOVERY})
