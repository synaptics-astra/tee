/*
 * Copyright (c) 2013-2014 Marvell International Ltd. and its affiliates.
 * All rights reserved.
 *
 * MARVELL CONFIDENTIAL
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Marvell International Ltd or its
 * suppliers or licensors. Title to the Material remains with Marvell
 * International Ltd or its suppliers and licensors. The Material contains
 * trade secrets and proprietary and confidential information of Marvell or
 * its suppliers and licensors.
 * The Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Marvell's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Marvell in writing.
 */
#include "mem_region.h"
#include "mem_region_userdata.h"
#include "ta_info.h"
#include "macros.h"
#include "tz_boot_cmd.h"
#include "tz_boot_param.h"
#include "tee_ta_perm_rule_def.h"

#define ION_ATTR_SW_NONCACHED	\
	(ION_A_CO | ION_A_CC | ION_A_ND | ION_A_FD | ION_A_NC | ION_A_FS)
#define ION_ATTR_SW_OTHER	\
	(ION_A_CO | ION_A_CG | ION_A_CV | ION_A_CC | ION_A_ND | ION_A_FD | \
	 ION_A_NC | ION_A_FC | ION_A_FS)
#define ION_ATTR_NW_ALL	\
	(ION_A_CO | ION_A_CG | ION_A_CV | ION_A_CC | ION_A_ND | ION_A_FD | \
	 ION_A_FC | ION_A_NS)

/* Temporary add. It should be removed once TEE support mapping memory on demand. */
#define ION_ATTR_NW_NONCACHED \
	(ION_A_CO | ION_A_CG | ION_A_CV | ION_A_CC | ION_A_ND | ION_A_FD | \
	 ION_A_NC | ION_A_NS)

/* memory region setting */

#define RC_TZ_START    0x00100000u
#define RC_TZ_SIZE     0x00500000u

#define RC_BOOTLOADER_START    (RC_TZ_START + RC_TZ_SIZE)
#define RC_BOOTLOADER_SIZE     0x00700000u

#define RC_REGISTER_START      0xF6000000u
#define RC_REGISTER_SIZE       0x04000000u

#define RC_NONSECURE_NC_SIZE   0x00800000u
#define RC_SECURE_SIZE         0x0u

#define RC_SYSTEM_START        (RC_BOOTLOADER_START + RC_BOOTLOADER_SIZE)
#define RC_SYSTEM_SIZE         (0x80000000 - RC_SYSTEM_START - RC_NONSECURE_NC_SIZE - RC_SECURE_SIZE)

#define RC_NONSECURE_SIZE      0x20000000u
#define RC_NONSECURE_START     (RC_SYSTEM_START + RC_SYSTEM_SIZE - RC_NONSECURE_SIZE - 0x2800000)
/* CMA start address should aligned with 0x400000 */
COMPILE_ASSERT((RC_NONSECURE_START % 0x400000 == 0), cma_start_align_error);
/* CMA size should aligned with 0x400000 */
COMPILE_ASSERT((RC_NONSECURE_SIZE % 0x400000 == 0), cma_size_align_error);

#define RC_NONSECURE_NC_START  (RC_SYSTEM_START + RC_SYSTEM_SIZE)

#define RC_SECURE_START        (RC_NONSECURE_NC_START + RC_NONSECURE_NC_SIZE)

#define RC_SYSTEM_1_START      RC_SECURE_START + RC_SECURE_SIZE
#define RC_SYSTEM_1_SIZE       0x80000000u
/* RC_SYSTEM_1_START+RC_SYSTEM_1_SIZE should not exceed DDR size */
COMPILE_ASSERT(((RC_SYSTEM_1_START + RC_SYSTEM_1_SIZE) == 0xE0000000u), exceeding_ddr_size_error);

#define RC_SYSTEM_2_START      0x1e0000000u
#define RC_SYSTEM_2_SIZE       0x20000000u

const struct mem_region mem_region_param[] = {

	{"tz",		RC_TZ_START, RC_TZ_SIZE,
		MR_ATTR(TZ_SNA_NNA, 0, 0, MT_CACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_TZCORE, 0, 0, TEE_ZONE_ACPU}},

	{"bootloader",	RC_BOOTLOADER_START, RC_BOOTLOADER_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_BL, 0, 0, TEE_ZONE_RSV}},

	{"NonSecure",	RC_NONSECURE_START, RC_NONSECURE_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_ION | 0x00000004, ION_ATTR_NW_ALL, 0, TEE_ZONE_ACPU}},

	{"system",	RC_SYSTEM_START, RC_SYSTEM_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, 0), 0,
		{TEE_MR_USER_SYSTEM, 0, 0, TEE_ZONE_RSV}},

	{"NonSecure-NC",	RC_NONSECURE_NC_START, RC_NONSECURE_NC_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_UNCACHED, MR_M_PREMAPPED), 0,
		{TEE_MR_USER_ION | 0x00000005, ION_ATTR_NW_NONCACHED, 0, TEE_ZONE_ACPU}},

	{"system_1",	RC_SYSTEM_1_START, RC_SYSTEM_1_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, 0), 0,
		{TEE_MR_USER_SYSTEM, 0, 0, TEE_ZONE_RSV}},

	{"system_2",	RC_SYSTEM_2_START, RC_SYSTEM_2_SIZE,
		MR_ATTR(TZ_SRW_NRW, 0, 1, MT_CACHED, 0), 0,
		{TEE_MR_USER_SYSTEM, 0, 0, TEE_ZONE_RSV}},

	{"register",	RC_REGISTER_START, RC_REGISTER_SIZE,
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

const uint32_t tz_boot_mode_param = TZ_BOOT_MODE_RECOVERY;

const struct tz_boot_param_list bp_list[] = {
	{TZ_BOOT_PARAM_MEMREGION, mem_region_param, sizeof(mem_region_param)},
	{TZ_BOOT_PARAM_TA_INFO, ta_info_param, sizeof(ta_info_param)},
	{TZ_BOOT_PARAM_BOOT_MODE, &tz_boot_mode_param, sizeof(tz_boot_mode_param)},
};

const uint32_t bp_num = ARRAY_SIZE(bp_list);
