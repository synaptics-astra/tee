#ifndef _TEE_INTERNAL_PRIVATE_API_H_
#define _TEE_INTERNAL_PRIVATE_API_H_

#include "tee_internal_core_common.h"
#include "tee_internal_storage_generic_api.h"
#include "mem_region.h"

enum {
	RPMB_QUERY_STATE,
	RPMB_WRITE_KEY,
};

typedef struct mem_region	TEE_MemRegion;
typedef struct reg_region	TEE_RegRegion;

#define TEE_MR_ATTR(perm, reg, zone, mattr, ctrl)		\
	MR_ATTR(perm, reg, zone, mattr, ctrl)

#define TEE_MR_M_PERM			MR_M_PERM
#define TEE_MR_M_ZONE			MR_M_ZONE
#define TEE_MR_M_TYPE			MR_M_TYPE
#define TEE_MR_M_MEM_ATTR		MR_M_MEM_ATTR
#define TEE_MR_M_CACHEABLE		MR_M_CACHEABLE
#define TEE_MR_M_DATA_ATTR		MR_M_DATA_ATTR
#define TEE_MR_M_PREMAPPED		MR_M_PREMAPPED

#define TEE_MR_PERM(mr)			MR_PERM(mr)
#define TEE_MR_ZONE(mr)			MR_ZONE(mr)
#define TEE_MR_WINDOW(mr)		MR_WINDOW(mr)
#define TEE_MR_MEM_ATTR(mr)		MR_MEM_ATTR(mr)
#define TEE_MR_IS_RESTRICTED(mr)	MR_IS_RESTRICTED(mr)
#define TEE_MR_IS_SECURE(mr)		MR_IS_SECURE(mr)
#define TEE_MR_IS_NONSECURE(mr)		MR_IS_NONSECURE(mr)
#define TEE_MR_IS_PREMAPPED(mr)		MR_IS_PREMAPPED(mr)
#define TEE_MR_IS_CACHEABLE(mr)		MR_IS_CACHEABLE(mr)
#define TEE_MR_IS_DATA_ATTR(mr)		MR_IS_DATA_ATTR(mr)
#define TEE_MR_IS_REGISTER(mr)		MR_IS_REGISTER(mr)
#define TEE_MR_IS_MEMORY(mr)		MR_IS_MEMORY(mr)

/** Get the memory region count
 *
 * @param attrMask	attribute mask to filter memory regions.
 * 			can be the flags in TEE_MR_M_*. 0 to get all.
 * 			only (region.attr & attrMask == attrVal) is filtered.
 * @param attrVal	attribute value to filter memory regions.
 * 			use TEE_MR_ATTR() to generate it. ignored if attrMask=0.
 *
 * @return	region number.
 */
uint32_t TEE_GetMemRegionCount(uint32_t attrMask, uint32_t attrVal);

/** Retrieve the memory region list.
 *
 * @param region	buffer to store the retrieved regions.
 *			NULL to return region number, same as
 *			TEE_GetMemRegionCount().
 * @param maxNum	max count can be retrieved (count region).
 * @param attrMask	attribute mask to filter memory regions.
 * 			can be the flags in TEE_MR_M_*. 0 to get all.
 * 			only (region.attr & attrMask == attrVal) is filtered.
 * @param attrVal	attribute value to filter memory regions.
 * 			use TEE_MR_ATTR() to generate it. ignored if attrMask=0.
 *
 * @return	retrieved region number. if maxNum < total matched region
 * 		number, then only maxNum would be copied, and return total
 * 		matched region_number.
 */
uint32_t TEE_GetMemRegionList(TEE_MemRegion *region, uint32_t maxNum,
		uint32_t attrMask, uint32_t attrVal);

/** Find memory region based on the input address.
 *
 * @param region	buffer to store the retrieved region.
 * @param buffer	buffer address to check.
 * @param size		size of the buffer, must not cross 2 regions.
 *
 * @retval TEE_SUCCESS			Pass the check.
 * @retval TEE_ERROR_BAD_PARAMETERS	Parameter error.
 */
TEE_Result TEE_FindMemRegion(TEE_MemRegion *region,
		void *buffer, uint32_t size);

/** Find Read register region based on the input address.
 *
 * @param region	buffer to store the register region.
 * @param addr      address to check.
 *
 * @retval TEE_SUCCESS			    Pass the check.
 * @retval TEE_ERROR_BAD_PARAMETERS	Parameter error.
 * @retval TEE_ERROR_ACCESS_DENIED  address is not in the register region
 */
TEE_Result TEE_FindRegRegion(TEE_RegRegion *region,
		void *addr);

/*
 * Data attributes support to save memory in TrustZone.
 *
 * Data attributes are used to identify the content in secure memory is secure
 * or not.
 * With data attributes, we can store all the secure & non-secure content in
 * secure memory, so the memory size can be saved.
 *
 * In order to use it, the user must set the data buffer to be secure or
 * non-secure before use them.
 *
 * And in order to improve the access performance, read/write lock is used to
 * synchornize the data attributes list.
 */

/** Retrieve back data attributes.
 *
 * @param buffer	start address of the data.
 * @param size		size of the data.
 * @param attr		buffer to return attributes of the data.
 *
 * @retval TEE_SUCCESS			Pass the check.
 * @retval TEE_ERROR_BAD_PARAMETERS	Parameter error.
 * @retval TEE_ERROR_BAD_STATE		Data attributes is not set.
 */
TEE_Result TEE_GetDataAttribute(const void *buffer, size_t size, uint32_t *attr);

/* Memory Transfer Operation */
typedef enum {
	TEE_MT_COPY,
	TEE_MT_ENCRYPT,
	TEE_MT_DECRYPT
} TEE_MemoryOperation;

/** check memory transfer rights.
 *
 * @param dst		destination address.
 * @param dstLen	destination data length
 * @param src		source address.
 * @param srcLen	source data length
 * @param op		Operation, see TEE_MemoryOperation.
 *
 * @retval TEE_SUCCESS			Pass the check.
 * @retval TEE_ERROR_ACCESS_CONFLICT	Can't pass the check, master must not
 *					issue the transfer.
 *
 * Examples:
 * - memory to memory copy:
 *	res = TEE_CheckMemoryTransferRights(dst, dstLen, src, srcLen,
 *			TEE_MT_COPY);
 * - memory to memory decrypt:
 *	res = TEE_CheckMemoryTransferRights(dst, dstLen, src, srcLen,
 *			TEE_MT_DECRYPT);
 */
TEE_Result TEE_CheckMemoryTransferRights(
		const void*		dst,
		uint32_t		dstLen,
		const void*		src,
		uint32_t		srcLen,
		TEE_MemoryOperation	op);

/* memory access permision, see TZ_Sxx_Nxx in tz_perm.h */
#define TEE_NONSECURE				TZ_SRW_NRW
#define TEE_SECURE				TZ_SRW_NNA
#define TEE_RESTRICTED				TZ_SNA_NNA

/** check memory input rights.
 *
 * @param src		source address.
 * @param srcLen	source data length
 * @param dstPerm	desitination memory access permission, can be
 *			TEE_NONSECURE, TEE_SECURE.
 * @param op		Operation, see TEE_MemoryOperation.
 *
 * @retval TEE_SUCCESS			Pass the check.
 * @retval TEE_ERROR_ACCESS_CONFLICT	Can't pass the check, master must not
 *					issue the transfer.
 *
 * Examples:
 * - memory to memory copy:
 *	res = TEE_CheckMemoryInputRights(src, srcLen,
 *			TEE_NONSECURE, TEE_MT_COPY);
 * - memory to memory decrypt:
 *	res = TEE_CheckMemoryTransferRights(src, srcLen,
 *			TEE_SECURE, TEE_MT_DECRYPT);
 */
TEE_Result TEE_CheckMemoryInputRights(
		const void*		src,
		uint32_t		srcLen,
		uint32_t		dstPerm,
		TEE_MemoryOperation	op);

/** check memory output rights.
 *
 * @param dst		destination address.
 * @param dstLen	destination data length
 * @param srcPerm	source memory access permission, can be
 *			TEE_NONSECURE, TEE_SECURE.
 * @param op		Operation, see TEE_MemoryOperation.
 *
 * @retval TEE_SUCCESS			Pass the check.
 * @retval TEE_ERROR_ACCESS_CONFLICT	Can't pass the check, master must not
 *					issue the transfer.
 *
 * Examples:
 * - memory to memory copy:
 *	res = TEE_CheckMemoryOutputRights(dst, dstLen,
 *			TEE_NONSECURE, TEE_MT_COPY);
 * - memory to memory decrypt:
 *	res = TEE_CheckMemoryTransferRights(dst, dstLen,
 *			TEE_SECURE, TEE_MT_DECRYPT);
 */
TEE_Result TEE_CheckMemoryOutputRights(
		const void*		dst,
		uint32_t		dstLen,
		uint32_t		srcPerm,
		TEE_MemoryOperation	op);

/** User level callback to client.
 * Callback to client and execute user registered callback
 *
 * @param commandID	command ID.
 * @param paramTypes	parameter types, see TEE_PARAM_TYPE.
 * @param params	the 4 parameters. here, for memref, it won't
 * 			be converted to client virtual address.
 * @param cancellationRequestTimeout	timeout in ms.
 * @param returnOrigin	return origin of the result.
 *
 * @retval TEE_SUCCESS	Succeed.
 * @retval TEE_ERROR_*	Error code, need check returnOrigin too.
 */
TEE_Result TEE_Callback(
	uint32_t		commandID,
	uint32_t		paramTypes,
	TEE_Param		params[4],
	uint32_t		cancellationRequestTimeout,
	uint32_t*		returnOrigin);

/**System level callback to client.
 * Callback to client and execute system registered callback
 *
 * @param commandID	command ID.
 * @param paramTypes	parameter types, see TEE_PARAM_TYPE.
 * @param params	the 4 parameters. here, for memref, it won't
 * 			be converted to client virtual address.
 * @param param_ext	extent param.
 * @param param_ext_size extent param size.
 * @param returnOrigin	return origin of the result.
 *
 * @retval TEE_SUCCESS	Succeed.
 * @retval TEE_ERROR_*	Error code, need check returnOrigin too.
 */
TEE_Result TEE_SysCallback(
	uint32_t		commandID,
	uint32_t		paramTypes,
	TEE_Param		params[4],
	void			*param_ext,
	uint32_t		param_ext_size,
	uint32_t*		returnOrigin);

/*
 * Physical/Virtual Address Conversion API
 */
void *TEE_PhysToVirt(paddr_t pa);
void *TEE_VirtToPhys(void *va);

/*
 * Cache Operation
 */

/** invalidate dcache by range.
 *
 * @param start		virtual start address of region
 * @param size		size of the region
 */
TEE_Result TEE_InvalidateCache(void *start, size_t size);

/** clean dcache by range.
 *
 * @param start		virtual start address of region
 * @param size		size of region
 */
TEE_Result TEE_CleanCache(void *start, size_t size);

/** clean & invalidate dcache by range.
 *
 * Ensure that the data held in the page addr is written back
 * to the page in question.
 * @param start		virtual start address of region
 * @param size		size of region
 */
TEE_Result TEE_FlushCache(void *start, size_t size);

/** create mutex.
 *
 * @param lock		return the lock handle.
 * @param name		name of the mutex.
 * 			if different modules use same mutex name,
 * 			they will share same mutex handle.
 * 			if name==NULL, then it would create a new
 * 			anonymous mutex. to avoid duplicate name,
 *			it is suggested to use UUID.
 *
 * @retval TEE_SUCCESS	Success to create the mutex.
 * @retval TEE_ERROR_*	Error code if fail to create the mutex.
 */
TEE_Result TEE_MutexCreate(void **lock, const char *name);

/** destroy mutex.
 *
 * @param lock		mutex handle.
 *
 * @retval TEE_SUCCESS	success to destroy the lock.
 * @retval TEE_ERROR_*	Error code if fail to destroy the mutex.
 */
TEE_Result TEE_MutexDestroy(void *lock);

/** lock mutex.
 *
 * @param lock		mutex handle.
 *
 * @retval TEE_SUCCESS	success to lock the mutex.
 * @retval TEE_ERROR_*	Error code if fail to lock the mutex.
 */
TEE_Result TEE_MutexLock(void *lock);

/** try to lock mutex.
 *
 * @param lock		mutex handle.
 *
 * @retval TEE_SUCCESS	success to trylock the mutex.
 * @retval TEE_ERROR_BAD_STATE	lock is owned by others.
 * @retval TEE_ERROR_*	Error code if fail to trylock the mutex.
 */
TEE_Result TEE_MutexTryLock(void *lock);

/** unlock mutex.
 *
 * @param lock		mutex handle.
 *
 * @retval TEE_SUCCESS	success to unlock the mutex.
 * @retval TEE_ERROR_*	Error code if fail to unlock the mutex.
 */
TEE_Result TEE_MutexUnlock(void *lock);


/** create semaphore.
 *
 * @param sem		return the sem handle.
 * @param value		initial value of the semaphore.
 * @param name		name of the semaphore.
 * 			if different modules use same semaphore name,
 * 			they will share same semaphore handle. and the initial
 * 			value is ignored. to avoid duplicate name, it is suggested
 *			to use UUID.
 * 			if name==NULL, then it would create a new
 * 			anonymous semaphore.
 *
 * @retval TEE_SUCCESS	Success to create the semaphore.
 * @retval TEE_ERROR_*	Error code if fail to create the semaphore.
 */
TEE_Result TEE_SemaphoreCreate(void **sem, int value, const char *name);

/** destroy semaphore.
 *
 * @param sem		semaphore handle.
 *
 * @retval TEE_SUCCESS	success to destroy the sem.
 * @retval TEE_ERROR_*	Error code if fail to destroy the semaphore.
 */
TEE_Result TEE_SemaphoreDestroy(void *sem);

/** wait semaphore, and decrease the count
 *
 * @param sem		semaphore handle.
 *
 * @retval TEE_SUCCESS	success to wait the semaphore.
 * @retval TEE_ERROR_*	Error code if fail to wait the semaphore.
 */
TEE_Result TEE_SemaphoreWait(void *sem);

/** wait semaphore until timeout.
 *
 * @param sem		semaphore handle.
 * @param timeout	timeout in milliseconds.
 *
 * @retval TEE_SUCCESS	success to wait the semaphore.
 * @retval TEE_ERROR_TIMEOUT	timeout reached.
 * @retval TEE_ERROR_*	Error code if fail to wait the semaphore.
 */
TEE_Result TEE_SemaphoreTimedWait(void *sem, uint32_t timeout);

/** try to wait semaphore.
 *
 * it's similar as TEE_SemaphoreTimedWait(sem, 0).
 *
 * @param sem		semaphore handle.
 *
 * @retval TEE_SUCCESS	success to trywait the semaphore.
 * @retval TEE_ERROR_BAD_STATE	sem is owned by others.
 * @retval TEE_ERROR_*	Error code if fail to trywait the semaphore.
 */
TEE_Result TEE_SemaphoreTryWait(void *sem);

/** post semaphore, and increase the count.
 *
 * @param sem		semaphore handle.
 *
 * @retval TEE_SUCCESS	success to post the semaphore.
 * @retval TEE_ERROR_*	Error code if fail to post the semaphore.
 */
TEE_Result TEE_SemaphorePost(void *sem);

typedef enum {
	TEE_BOOT_MODE_NORMAL	= 0,	/* Normal Boot */
	TEE_BOOT_MODE_RECOVERY	= 1,	/* Recovery Boot */
	TEE_BOOT_MODE_MAX
} TEE_BootMode;

/** Get boot mode.
 *
 * @param mode		buffer to return boot mode, see TEE_BootMode.
 *
 * @retval TEE_SUCCESS	success to get the info.
 */
TEE_Result TEE_GetBootMode(TEE_BootMode *mode);

typedef struct {
	uint32_t commVer;		/* Common Version */
	uint32_t custVer;		/* Customer Version */
} TEE_AntiRollbackInfo;

/** Get anti-rollback info.
 *
 * @param info		buffer to return anti-rollback versions.
 *
 * @retval TEE_SUCCESS	success to get the info.
 */
TEE_Result TEE_GetAntiRollbackInfo(TEE_AntiRollbackInfo *info);

/** Verify BCM image.
 *
 * @param in		buffer to keep the enced image.
 * @param inLen		length of the enced image
 * @param out		buffer to keep the decrypted image
 * @param outLen	buffer to keep the return length
 *
 * @retval TEE_SUCCESS			success to get the info.
 * @retval TZ_ERROR_BAD_PARAMETERS	invalid parameters.
 */
TEE_Result TEE_VerifyImage(const void *in, uint32_t inLen, void *out,
		uint32_t *outLen, uint32_t codeType);

/** send command to secure processor and wait until its completion
 *
 * @param cmd_id	command ID
 * @param param		buffer of the input param
 * @param param_len	parameter length
 * @param result	buffer to keep the result
 * @param result_len	result length
 *
 * @retval TEE_SUCCESS	success to get the info.
 */
TEE_Result TEE_ExecCmd(size_t cmd_id, const void *param, size_t param_len,
		const void *result, size_t result_len);

/** send command to secure processor and wait until its completion
 *  can set the delay number
 *
 * @param cmd_id	command ID
 * @param param		buffer of the input param
 * @param param_len	parameter length
 * @param result	buffer to keep the result
 * @param result_len	result length
 * @param delay		delay ms number
 *
 * @retval TEE_SUCCESS	success to get the info.
 */
TEE_Result TEE_ExecCmdEx(size_t cmd_id, const void *param, size_t param_len,
		const void *result, size_t result_len, uint32_t delay);

/* TA-TA Secure Shared Memory (SHM).
 *
 * the SHM is used to share info between 2 or multiple TAs.
 * the SHM is designed to be
 * * highly security control
 *   - access/visible scope control: can bind to current session
 *   - assess permission control: readwrite/readonly/writeonly
 *   - owner authentication: owner can set access permission for TAs
 * * easy to use
 *   - TA can map shm to peer, so peer doesn't need to be aware
 */

#define TEE_SHM_ACCESS_SCOPE_MASK	(0x0000000f)

/* indicate the SHM is session private.
 * session private SHM is only visible for current session in the 2 TAs.
 * once the TA serve other session, the TA can't see the SHM any more.
 */
#define TEE_SHM_SESSION_PRIVATE		(0x00000001)
/* indicate the SHM is private for the 2 TAs of curren session.
 * the difference from TEE_SHM_SESSION_PRIVATE is that the SHM is always
 * visible for the 2 TAs even they serve other sessions until it release
 * the SHM.
 */
#define TEE_SHM_SESSION_TA_PRIVATE	(0x00000002)
/* indidate the SHM is only accessible by TA. it can't be accessed by other
 * secure masters.
 */
#define TEE_SHM_RESTRICTED		(0x00000003)
/* indicate the SHM is accessible in whole secure domain */
#define TEE_SHM_SECURE			(0x00000004)
/* indidate the SHM is assessible by nonsecure domain too. (non-secure shm) */
#define TEE_SHM_NONSECURE		(0x00000005)

/* read write permisions */
#define TEE_SHM_PEER_READ		(0x10000000)
#define TEE_SHM_PEER_WRITE		(0x20000000)
#define TEE_SHM_PEER_READWRITE		(0x30000000)
#define TEE_SHM_PEER_GRANT		(0x08000000)

/* indidate the SHM is non-cacheable memory */
#define TEE_SHM_UNCACHED		(0x40000000)
/* indicate the SHM is physical continuous which for hardare access.
 * for physical continuous memory, can always use TEE_VirtToPhys()
 * to get the physical address, or use TEE_PhysToVirt() to convert
 * physical address to virtual address.
 */
#define TEE_SHM_PHYSICAL_CONTINUOUS	(0x80000000)

/** allocate secure shared memory, the memory is page aligned.
 *
 * @param len		shared memory size in bytes.
 * @param shm_id	return the shm handle.
 * @param va		return shm virtual address.
 *
 * @retval TEE_SUCCESS			success.
 * @retval TEE_ERROR_GENERIC	fail to allocate shared memory
 */
TEE_Result TEE_AllocateSharedMemory2(size_t len, uint32_t *shmid, void **va);

/** grant read/write permission to other ta
 *
 * @param shm_id	the shm handle.
 * @param uuid		uuid of TA which will be granted permission
 * @param permission_flag		read/write permission and grant permission
 *								read/write is a must
 *
 * @retval TEE_SUCCESS			success.
 * @retval TEE_ERROR_GENERIC	fail to grant permission
 */

TEE_Result TEE_AddSharedMemoryAccess(uint32_t shmid,
								TEE_UUID uuid, uint32_t permission_flag);

/** map shared memory to running ta
 *
 * @param shm_id	the shm handle.
 * @param va		return the va address
 * @param size		return size of shared memory
 *
 * @retval TEE_SUCCESS			success.
 * @retval TEE_ERROR_ACCESS_DENIED	no permission
 */
TEE_Result TEE_MapSharedMemory(uint32_t shmid, void **va, size_t *size);

/** unmap shared memory from running ta
 *
 * @param shm_id	the shm handle.
 *
 * @retval TEE_SUCCESS			success.
 * @retval TEE_ERROR_BAD_PARAMETERS	wrong parameter
 */
TEE_Result TEE_UnmapSharedMemory(uint32_t shmid);

/** RPMB set key
 *
 * @param key	the rpmb key
 * @retval TEE_SUCCESS	return TEE_SUCCESS
 */
TEE_Result TEE_RPMB_SetKey(uint8_t *key);

/** RPMB read block
 *
 * @param devName	name of rpmb device, eg: /dev/block/rpmb
 * @param addr		the address of rpmb write,unit is 256B
 * @param data		the data read from rpmb
 * @param len		size of data in bytes
 * @retval TEE_SUCCESS	return TEE_SUCCESS
 */
TEE_Result TEE_RPMB_Read(const char *devName,
			uint16_t addr,
			uint8_t *data,
			uint32_t len);
/** RPMB write block
 *
 * @param addr		the address of rpmb write,unit is 256B
 * @param data		the data write to rpmb
 * @parm len		the len of data, size should be <= 256
 * @retval TEE_SUCCESS	return TEE_SUCCESS
 */
TEE_Result TEE_RPMB_Write(const char *devName,
			    uint16_t addr,
			    uint8_t *data,
			    uint32_t len);

/** Get raw data from OTP
 *
 * @param row		row of data in OTP
 * @parm buffer		raw data, should be uint8_t buf[8]
 * @retval TEE_SUCCESS	return TEE_SUCCESS
 */
TEE_Result TEE_GetOtpRawData(uint32_t row, uint8_t *buffer);

/** Get TA's secure store data
 *
 * @param[IN] ss_buf	secure store data
 * @param[IN] ss_len	the len of secure store data
 * @parm[IO] clear_buf	the clear secure store data
 * @parm[IN] clear_len	the len of clear secure store data
 * @parm[OUT] out_len	the real len of output clear secure store data
 * @retval TEE_SUCCESS	return TEE_SUCCESS
 */
TEE_Result TEE_GetCustomSecureStoreData(void *ss_buf,
					uint32_t ss_len,
					void *clear_buf,
					uint32_t clear_len,
					void *outlen);

/** pair RPMB authentication key
 * note: only call by factory TA when MP flow
 *
 * @param[INOUT] paired_flag	OTP flag to indicate if the pair has already
 * been done
 * @retval TEE_SUCCESS	if paired return TEE_SUCCESS else TEE_ERROR_XXX
 */
TEE_Result TEE_RPMB_PairKey(int *paired_flag, int *rpmb_state, int op_mode);


/** Get the SZC rules count
 *
 * @param[OUT] cnt		value of szc rule count
 *
 * @retval TEE_SUCCESS		successfully get the szc rules count
 */

TEE_Result TEE_SZC_GetRulesCnt(uint32_t *cnt);

/** Get the SZC rules
 *
 * @param[OUT] rules	buffer to store Rules Setting
 * @param[INOUT] size	return the real size of rules
 *
 * @retval TEE_SUCCESS		successfully get the szc rules
 */

TEE_Result TEE_SZC_GetRules(void *rules, size_t *size);

/** Get the SZC zones count
 *
 * @param[OUT] cnt		value of szc zone count
 *
 * @retval TEE_SUCCESS		successfully get the szc zone count
 */

TEE_Result TEE_SZC_GetZonesCnt(uint32_t *cnt);

/** Get the SZC zones
 *
 * @param[OUT] zones	buffer to store Zones Setting
 * @param[INOUT] size	return the real size of zones
 *
 * @retval TEE_SUCCESS		successfully get the szc zone
 */

TEE_Result TEE_SZC_GetZones(void *zones, size_t *size);

/** Get the SZC master setting count
 *
 * @param[OUT] cnt		value of szc master setting count
 *
 * @retval TEE_SUCCESS		successfully get the szc master setting count
 */

TEE_Result TEE_SZC_GetMstSettingCnt(uint32_t *cnt);

/** Get the SZC Master Setting count
 *
 * @param[OUT] master_setting	buffer to store master Settings
 * @param[INOUT] size	return the real size of settings
 *
 * @retval TEE_SUCCESS		successfully get the szc master settings
 */
TEE_Result TEE_SZC_GetMstSettings(void *master_setting, size_t *size);
#endif /* _TEE_INTERNAL_PRIVATE_API_H_ */
