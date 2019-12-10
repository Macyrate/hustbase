#include "stdafx.h"
#include "RM_Manager.h"
#include "str.h"
#include "PF_Manager.h"

RC OpenScan(RM_FileScan* rmFileScan, RM_FileHandle* fileHandle, int conNum, Con* conditions)//初始化扫描
{
	return SUCCESS;
}

RC GetNextRec(RM_FileScan* rmFileScan, RM_Record* rec)
{
	return SUCCESS;
}

RC GetRec(RM_FileHandle* fileHandle, RID* rid, RM_Record* rec)
{
	return SUCCESS;
}

RC InsertRec(RM_FileHandle* fileHandle, char* pData, RID* rid)
{
	return SUCCESS;
}

RC DeleteRec(RM_FileHandle* fileHandle, const RID* rid)
{
	return SUCCESS;
}

RC UpdateRec(RM_FileHandle* fileHandle, const RM_Record* rec)
{
	return SUCCESS;
}

RC RM_CreateFile(char* fileName, int recordSize)
{

	//创建新文件

	RC createRC = CreateFile(fileName);

	if (createRC == SUCCESS)
	{

		//文件创建成功

		PF_FileHandle* fileHandle = getPF_FileHandle();
		RC openRC = openFile(fileName, fileHandle);

		if (openRC == SUCCESS)
		{

			//文件打开成功
			//创建记录信息控制页

			PF_PageHandle* pgHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
			pgHandle->bOpen = false;

			RC allocateRC = AllocatePage(fileHandle, pgHandle);

			if (allocateRC == SUCCESS)
			{

				//记录信息控制页创建成功
				//页面信息清零

				for (int i = 0; i < PF_PAGE_SIZE; i++)
				{
					pgHandle->pFrame->page.pData[i] = 0;
				}

				//填充记录信息控制页信息：记录长度

				int* headOfPage = (int*)(char*)pgHandle->pFrame->page.pData;
				headOfPage[0] = recordSize;

				//关闭文件

				RC closeRC = CloseFile(fileHandle);

				//释放资源，返回相关信息

				free(pgHandle);
				free(fileHandle);
				return closeRC;

			}
			else
			{

				//关闭文件

				CloseFile(fileHandle);

				//释放资源，返回错误信息

				free(pgHandle);
				free(fileHandle);
				return allocateRC;

			}
		}
		else
		{

			//释放资源，返回错误信息

			free(fileHandle);
			return openRC;

		}
	}
	else
	{

		//返回错误信息

		return createRC;

	}
}

RC RM_OpenFile(char* fileName, RM_FileHandle* fileHandle)
{

	//防止修改fileHandle本身的地址值

	RM_FileHandle* retHandle = fileHandle;

	//打开页面文件

	PF_FileHandle* pfFileHandle = getPF_FileHandle();
	RC openRC = openFile(fileName, pfFileHandle);

	if (openRC == SUCCESS)
	{

		//打开页面文件成功

		retHandle->pPFFileHandle = pfFileHandle;
		retHandle->bOpen = true;

		//TODO:读取记录长度

		return SUCCESS;

	}
	else
	{

		//释放资源，返回错误信息

		free(pfFileHandle);
		return openRC;
	}
}

RC RM_CloseFile(RM_FileHandle* fileHandle)
{
	return SUCCESS;
}
