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
#ifndef _MEM_REGION_H_
#define _MEM_REGION_H_

#include "types.h"
#include "tz_perm.h"
#include "mem_type.h"

struct master_setting {
	uint32_t		id;
	uint32_t		attr;	/* stage + perm */
	uint32_t		reserved[2];
};

#define MST_M_STAGE			(0xff000000)
#define MST_M_PERM			(0x000000ff)
#define MST_S_STAGE			(24)
#define MST_GET_STAGE(attr)		((attr) >> MST_S_STAGE)
#define MST_GET_PERM(attr)		((attr) & MST_M_PERM)
#define MST_ATTR(stage, perm)		(((stage) << MST_S_STAGE) | \
					((perm) & MST_M_PERM))

/* memory zone */
#define MAX_MASTER_NUM      32
// ns
#define NS_ZONE             1
#define S_ZONE              0

struct mem_zone {
	uint32_t		id;
	char			name[32];
	uint32_t		ns; // 1: non-secure zone, 0: secure zone
	uint32_t		ns_rd;
	uint32_t		ns_wr;
	uint32_t		s_rd;
	uint32_t		s_wr;
	uint32_t		start;
	uint32_t		size;
};

/* register firewall window */
struct rf_window {
	uint32_t		id;
	uint32_t		base;
	uint32_t		mask;
	uint32_t		attr;	/* stage + perm */
	uint32_t		mst_group_ids;
};

#define RF_M_STAGE			(0xff000000)
#define RF_M_PERM			(0x000000ff)
#define RF_S_STAGE			(24)
#define RF_GET_STAGE(attr)		((attr) >> RF_S_STAGE)
#define RF_GET_PERM(attr)		((attr) & RF_M_PERM)
#define RF_ATTR(stage, perm)		(((stage) << RF_S_STAGE) | \
					((perm) & RF_M_PERM))

struct rf_master_group {
	uint32_t		rf_master_id;
	uint32_t		group_id;
};

/*
 * Memory Region.
 *
 * Attributes Definitions:
 *
 * bit 31:24: control info
 * bit    31: 0 not premapped, 1 premapped.
 * bit    30: 0 fixed data attributes, 1 support changeable data attributes
 * bit 29:24: reserved
 * bit 23:16: mapping attributes. can be
 *            - MT_STRONGORDER. only used in debugging.
 *            - MT_DEVICE
 *            - MT_UNCACHED
 *            - MT_CACHED
 *            note: for non-secure memory, it's the default attribute.
 *                  non-secure world should pass actual attributes when
 *                  map memory.
 * bit    15: type. 0 memory, 1 register.
 * bit 14: 8: memory zone id or window id (0-127)
 * bit  7: 0: permission, see TZ_Sxx_Nxx
 *
 * == Note: ARM Defined Memory types ==
 *
 * Memory Types (TR):
 * 00b - Strong Order (in-order access)
 *       a. it would make *all* access in order too. like every access in
 *          device + DSB.
 *       b. it tends to come with quite large performance penalty.
 *       c. it's highly recommended to use Device Memory. If necessary, insert
 *          DMB instructions ensure write buffer gets drained out to the
 *          peripheral.
 * 01b - Device (in-order access)
 * 10b - Normal (may out-of-order)
 *
 * example 1: load-norm-A, load-dev-B, load-dev-C, load-norm-D could be
 *            performed as ADBC, DBCA, DBAC... (only B->C is guanteed)
 * example 2: load-norm-A, load-so-B, load-so-C, load-norm-D must always
 *            be performed as ABCD.
 * example 3: load-norm-A, load-dev-B, load-dev-C, DMB, load-norm-D can
 *            only be performed as ABCD, BACD, BCAD
 *
 * Normal memory cache attributes (IR/OR):
 * 00b - Non-cacheable
 * 01b - Write-Back, Write-Allocate
 * 10b - Write-Through, no Write-Allocate
 * 11b - Write-Back, no Write-Allocate
 */

/* mask */
#define MR_M_PERM		(0x000000ff)
#define MR_M_ZONE		(0x00007f00)
#define MR_M_TYPE		(0x00008000)
#define MR_M_MEM_ATTR		(0x00ff0000)
#define MR_M_CTRL		(0xff000000)
#define MR_M_SHM		(0x20000000)
#define MR_M_DATA_ATTR		(0x40000000)
#define MR_M_PREMAPPED		(0x80000000)

/* shift */
#define MR_S_PERM		(0)
#define MR_S_ZONE		(8)
#define MR_S_TYPE		(15)
#define MR_S_MEM_ATTR		(16)
#define MR_S_CTRL		(24)
#define MR_S_SHM		(29)
#define MR_S_DATA_ATTR		(30)
#define MR_S_PREMAPPED		(31)

/* utils */
#define MR_ATTR(perm, reg, zone, mattr, ctrl)			\
	((perm) | ((zone) << MR_S_ZONE)				\
	 | ((reg) ? MR_M_TYPE : 0)				\
	 | ((mattr) << MR_S_MEM_ATTR)				\
	 | (ctrl))

#define MR_PERM(mr)		(((mr)->attr & MR_M_PERM) >> MR_S_PERM)
#define MR_ZONE(mr)		(((mr)->attr & MR_M_ZONE) >> MR_S_ZONE)
#define MR_WINDOW(mr)		MR_ZONE(mr)
#define MR_MEM_ATTR(mr)		(((mr)->attr & MR_M_MEM_ATTR) >> MR_S_MEM_ATTR)

#define MR_IS_RESTRICTED(mr)	TZ_IS_SNA(MR_PERM(mr))
#define MR_IS_SECURE(mr)	TZ_IS_NNA(MR_PERM(mr))
#define MR_IS_NONSECURE(mr)	(!MR_IS_SECURE(mr))
#define MR_IS_CACHEABLE(mr)	(MR_MEM_ATTR(mr) == MT_CACHED)
#define MR_IS_UNCACHEABLE(mr)	(MR_MEM_ATTR(mr) == MT_UNCACHED)
#define MR_IS_SHM(mr)		((mr)->attr & MR_M_SHM)
#define MR_IS_PREMAPPED(mr)	((mr)->attr & MR_M_PREMAPPED)
#define MR_IS_DEVICE(mr)	(MR_MEM_ATTR(mr) == MR_MEM_DEVICE)
#define MR_IS_STRONGORDER(mr)	(MR_MEM_ATTR(mr) == MR_MEM_STRONGORDER)
#define MR_IS_DATA_ATTR(mr)	((mr)->attr & MR_M_DATA_ATTR)
#define MR_IS_REGISTER(mr)	((mr)->attr & MR_M_TYPE)
#define MR_IS_MEMORY(mr)	(!MR_IS_REGISTER(mr))

/* memory region */
struct mem_region {
	char			name[32];
	uintptr_t		base;	/* base address */
	size_t			size;
	uint32_t		attr;	/* attributes */
	uint32_t		reserved;

	/* bellow fields are user defined data.
	 * it's to support multiple platforms by unified structure.
	 * different platform can use them for different purpose.
	 */
	uint32_t		userdata[4];
};

/*
 * anti-rollback info
 */
struct antirollback
{
	uint32_t comm_ver;	/* common version */
	uint32_t cust_ver;	/* customer version */
	uint32_t reserved[2];
};

/*register region*/
struct reg_region {
	char			name[32];
	uint32_t		base;
	uint32_t		size;
	uint32_t		attr;	/* attributes */
};

#define RULE_S_RD	1
#define RULE_S_WR	(1 << 1)
#define RULE_NS_RD	(1 << 2)
#define RULE_NS_WR	(1 << 3)

#define MST_RULE_SRW_NRW	(RULE_S_RD | RULE_S_WR | RULE_NS_RD | RULE_NS_WR)
#define MST_RULE_SRW_NRO	(RULE_S_RD | RULE_S_WR | RULE_NS_RD)
#define MST_RULE_SRW_NNA	(RULE_S_RD | RULE_S_WR)
#define MST_RULE_SRO_NRW	(RULE_S_RD | RULE_NS_RD | RULE_NS_WR)
#define MST_RULE_SWO_NNA	(RULE_S_WR)
#define MST_RULE_SNA_NRW	(RULE_NS_RD | RULE_NS_WR)
#define MST_RULE_SNA_NNA	(0)

#define RULE_M_ID		(0xff000000)
#define RULE_M_MST		(0x00ff0000)
#define RULE_M_PERM		(0x000000ff)
#define RULE_S_ID		(24)
#define RULE_S_MST		(16)
#define RULE_S_PERM		(0)

#define RULE_GET_ID(attr)			(((attr) & RULE_M_ID) >> RULE_S_ID)
#define RULE_GET_MST(attr)			(((attr) & RULE_M_MST) >> RULE_S_MST)
#define RULE_GET_PERM(attr)			(((attr) & RULE_M_PERM) >> RULE_S_PERM)
#define RULE_ATTR(id, master, perm)	((((id) << RULE_S_ID) & RULE_M_ID) | \
					(((master) << RULE_S_MST)  & RULE_M_MST) | \
					((perm) << RULE_S_PERM))

#endif	/* _MEM_REGION_H_ */
