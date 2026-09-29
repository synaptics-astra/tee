// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2025, Synaptics Incorporated.
 */
#include "mem_region.h"
#include "mem_region_userdata.h"
#include "ta_info.h"
#include "prot_id.h"
#include "szc_mst.h"
#include "macros.h"
#include "tz_boot_cmd.h"
#include "tz_boot_param.h"
#include "tee_ta_perm_rule_def.h"



/* memory region setting */
#define MAX_DDR_SIZE	0x80000000

#define SYS_INIT_START  0x00000000u
#define SYS_INIT_SIZE   0x00100000u

#define TZ_START    (SYS_INIT_START + SYS_INIT_SIZE)
#define TZ_SIZE     0x00500000u

#define BOOTLOADER_START    (TZ_START + TZ_SIZE)
#define BOOTLOADER_SIZE     0x00700000u

#define BCM_START           (BOOTLOADER_START + BOOTLOADER_SIZE)
#define BCM_SIZE            0x0u

#define GPU_START           (BCM_START + BCM_SIZE)
#define GPU_SIZE            0x0u

#define REGISTER_START      0xF6000000u
#define REGISTER_SIZE       0x04000000u

#define SECURE_START        (GPU_START + GPU_SIZE)
#define SECURE_SIZE         0x0u

#define NONSECURE_NC_START  (SECURE_START + SECURE_SIZE)
#define NONSECURE_NC_SIZE   0x00800000u

#define SYSTEM_START        (NONSECURE_NC_START + NONSECURE_NC_SIZE)
#define SYSTEM_SIZE         (MAX_DDR_SIZE - SYSTEM_START)

#define NONSECURE_SIZE      0x20000000u
#define NONSECURE_START     (MAX_DDR_SIZE - NONSECURE_SIZE - 0x4000000)
/* CMA start address should aligned with 0x400000 */
COMPILE_ASSERT((NONSECURE_START % 0x400000 == 0), cma_start_align_error);
/* CMA size should aligned with 0x400000 */
COMPILE_ASSERT((NONSECURE_SIZE % 0x400000 == 0), cma_size_align_error);

const struct mem_region mem_region_param[] = {
	{"sys_init",	SYS_INIT_START, SYS_INIT_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 0, MT_CACHED, 0), 0,
		{TEE_MR_USER_SI, 0, 0, TEE_ZONE_ACPU}},

	{"tz",		TZ_START, TZ_SIZE,
		MR_ATTR(TZ_SNA_NNA, 0, 0, MT_CACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_TZCORE, 0, 0, TEE_ZONE_ACPU}},

	{"bootloader",	BOOTLOADER_START, BOOTLOADER_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_BL, 0, 0, TEE_ZONE_RSV}},

	{"NonSecure-NC",	NONSECURE_NC_START, NONSECURE_NC_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_UNCACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_ION | 0x00000005, ION_ATTR_NW_NONCACHED, 0, TEE_ZONE_ACPU}},

	{"NonSecure",	NONSECURE_START, NONSECURE_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_ION | 0x00000004, ION_ATTR_NW_ALL, 0, TEE_ZONE_ACPU}},

	{"system",	SYSTEM_START, SYSTEM_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, 0), 0,
		{TEE_MR_USER_SYSTEM, 0, 0, TEE_ZONE_RSV}},

	{"register",	REGISTER_START, REGISTER_SIZE,
		MR_ATTR(TZ_SRW_NNA, 1, 0, MT_DEVICE, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_REGISTER, 0, 0, TEE_ZONE_RSV}},
};

const uint32_t mem_region_num = ARRAY_SIZE(mem_region_param);

const struct ta_info ta_info_param[] = {
	{0, sizeof(struct ta_info), {
		{"SYNAPTIC", 0x0},
		{"VENDOR1", 0x564e4431},
		{"VENDOR2", 0x564e4432},
		{"VENDOR3", 0x564e4433},
		{"VENDOR4", 0x564e4434},
		{"VENDOR5", 0x564e4435}
		}
	},
};

const uint32_t ta_info_param_num = ARRAY_SIZE(ta_info_param);

const uint32_t tz_boot_mode_param = TZ_BOOT_MODE_NORMAL;

const struct tz_boot_param_list bp_list[] = {
	{TZ_BOOT_PARAM_MEMREGION, mem_region_param, sizeof(mem_region_param)},
	{TZ_BOOT_PARAM_TA_INFO, ta_info_param, sizeof(ta_info_param)},
	{TZ_BOOT_PARAM_BOOT_MODE, &tz_boot_mode_param, sizeof(tz_boot_mode_param)},
};

const uint32_t bp_num = ARRAY_SIZE(bp_list);
