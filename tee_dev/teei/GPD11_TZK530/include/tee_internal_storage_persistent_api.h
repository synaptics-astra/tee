/*
 * Copyright (c) 2013-2014 Marvell International Ltd. and its affiliates.
 * All rights reserved.
 *
 * This software file (the "File") is owned and distributed by Marvell
 * International Ltd. and/or its affiliates ("Marvell") under the following
 * licensing terms.
 *
 * If you received this File from Marvell and you have entered into a
 * commercial license agreement (a "Commercial License") with Marvell, the
 * File is licensed to you under the terms of the applicable Commercial
 * License.
 */
#ifndef _TEE_INTERNAL_STORAGE_PERSISTENT_API_H_
#define _TEE_INTERNAL_STORAGE_PERSISTENT_API_H_

#include "tee_internal_common.h"
#include "tee_internal_storage_generic_api.h"

typedef enum
{
	TEE_DATA_SEEK_SET = 0,
	TEE_DATA_SEEK_CUR,
	TEE_DATA_SEEK_END
} TEE_Whence;

/** Handle on a persistent object enumerator */
typedef void* TEE_ObjectEnumHandle;

enum TEE_StorageType {
	TEE_STORAGE_PRIVATE = 0x00000001,
	TEE_STORAGE_REE_FS = 0x00000002,
};

enum TEE_DataFlag {
	TEE_DATA_FLAG_ACCESS_READ = 0x00000001,
	TEE_DATA_FLAG_ACCESS_WRITE = 0x00000002,
	TEE_DATA_FLAG_ACCESS_WRITE_META = 0x00000004,
	TEE_DATA_FLAG_SHARE_READ = 0x00000010,
	TEE_DATA_FLAG_SHARE_WRITE = 0x00000020,
	TEE_DATA_FLAG_CREATE = 0x00000200,
	TEE_DATA_FLAG_OVERWRITE = 0x00000400,
};

enum TEE_Miscellaneous {
	TEE_DATA_MAX_POSITION = 0xFFFFFFFF,
	TEE_OBJECT_ID_MAX_LEN = 64,
};

typedef struct {
	bool		isdir;
	char		name[TEE_OBJECT_ID_MAX_LEN * 2];
} TEE_Dirent;

typedef int (*GenTSKeyFunc)(
		const uint8_t *seedBuf,
		size_t seedLen,
		uint8_t* uniqueKey,
		size_t uniqueKeyLen);

TEE_Result TEE_OpenPersistentObject(
	uint32_t storageID,
	void* objectID,
	size_t objectIDLen,
	uint32_t flags,
	TEE_ObjectHandle* object);

TEE_Result TEE_CreatePersistentObject(
	uint32_t storageID,
	void* objectID,
	size_t objectIDLen,
	uint32_t flags,
	TEE_ObjectHandle attributes,
	void* initialData,
	size_t initialDataLen,
	TEE_ObjectHandle* object);

void TEE_CloseAndDeletePersistentObject(TEE_ObjectHandle object);

TEE_Result TEE_CloseAndDeletePersistentObject1(TEE_ObjectHandle object);

TEE_Result TEE_AllocatePersistentObjectEnumerator(
	TEE_ObjectEnumHandle* objectEnumerator);

void TEE_FreePersistentObjectEnumerator(TEE_ObjectEnumHandle
	objectEnumerator);

void TEE_ResetPersistentObjectEnumerator(TEE_ObjectEnumHandle
	objectEnumerator);

TEE_Result TEE_StartPersistentObjectEnumerator(
	TEE_ObjectEnumHandle objectEnumerator,
	uint32_t storageID);

TEE_Result TEE_GetNextPersistentObject(
	TEE_ObjectEnumHandle objectEnumerator,
	TEE_ObjectInfo* objectInfo,
	void* objectID,
	size_t* objectIDLen);

TEE_Result TEE_ReadObjectData(
	TEE_ObjectHandle object,
	void* buffer,
	size_t size,
	uint32_t* count);

TEE_Result TEE_WriteObjectData(
	TEE_ObjectHandle object,
	void* buffer,
	size_t size);

TEE_Result TEE_SeekObjectData(
	TEE_ObjectHandle object,
	int32_t offset,
	TEE_Whence whence);

void TEE_RegisterGenKey(GenTSKeyFunc GenFigoTSKey);

#endif /* _TEE_INTERNAL_STORAGE_PERSISTENT_API_H_ */
