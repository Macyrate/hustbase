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

//未完成
RC InsertRec(RM_FileHandle* fileHandle, char* pData, RID* rid)
{

	//检查fileHandle的状态

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//fileHandle处于打开状态
	//检查第一个空槽位所在的页

	if (fileHandle->firstEmptyPage == -1)
	{

		//未分配任何页面或已分配的页面全满



	}

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

//最后测试时间：2019/12/11 8:58
//最后测试状态：符合预期（已更新）
//最后测试人：strangenameBC
RC RM_CreateFile(char* fileName, int recordSize)
{

	//检查recordSize

	if (recordSize <= 0 || recordSize > 4092)
	{
		return RM_INVALIDRECSIZE;
	}

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
				*headOfPage = recordSize;

				//记录第一个有空位的页面，-1表示未分配任何数据页面或已分配的页面全满

				short* ptrFirstEmpty = (short*)(((char*)pgHandle->pFrame->page.pData) + 4);
				*ptrFirstEmpty = -1;

				//记录第一条有效数据的位置，（-1，-1）表示未储存任何数据

				short* ptrFirstRecord = (short*)(((char*)pgHandle->pFrame->page.pData) + 6);
				ptrFirstRecord[0] = -1;
				ptrFirstRecord[1] = -1;

				//记录最后一条有效数据的位置，（-1，-1）表示未储存任何数据

				short* ptrLastRecord = (short*)(((char*)pgHandle->pFrame->page.pData) + 10);
				ptrLastRecord[0] = -1;
				ptrLastRecord[1] = -1;

				//标记页面为脏页并Unpin

				MarkDirty(pgHandle);
				UnpinPage(pgHandle);

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

//最后测试时间：2019/12/11 8:58
//最后测试状态：符合预期（已更新）
//最后测试人：strangenameBC
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

		//获取记录控制信息页

		PF_PageHandle* pfPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
		pfPageHandle->bOpen = false;

		RC getPageRC = GetThisPage(pfFileHandle, 1, pfPageHandle);

		if (getPageRC == SUCCESS)
		{

			//获取记录控制信息页成功
			//获取记录长度

			int* headOfPage = (int*)(char*)pfPageHandle->pFrame->page.pData;
			retHandle->recordSize = *headOfPage;

			//获取第一个空白页码

			short* ptrFirstEmpty = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 4);
			retHandle->firstEmptyPage = *ptrFirstEmpty;

			//获取第一条有效记录的位置

			RID* firstRecordRID = (RID*)malloc(sizeof(RID));
			short* ptrFirstRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 6);

			if (ptrFirstRecord[0] == -1)
			{

				//未储存任何数据

				firstRecordRID->pageNum = -1;
				firstRecordRID->slotNum = -1;
				firstRecordRID->bValid = false;

			}
			else
			{

				//储存有数据，记录第一条数据的位置

				firstRecordRID->pageNum = ptrFirstRecord[0];
				firstRecordRID->slotNum = ptrFirstRecord[1];
				firstRecordRID->bValid = true;

			}

			retHandle->pFirstRecord = firstRecordRID;

			//获取最后一条有效记录的位置

			RID* lastRecordRID = (RID*)malloc(sizeof(RID));
			short* ptrLastRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 10);

			if (ptrLastRecord[0] == -1)
			{

				//未储存任何数据

				lastRecordRID->pageNum = -1;
				lastRecordRID->slotNum = -1;
				lastRecordRID->bValid = false;

			}
			else
			{

				//储存有数据，记录最后一条数据的位置

				lastRecordRID->pageNum = ptrLastRecord[0];
				lastRecordRID->slotNum = ptrLastRecord[1];
				lastRecordRID->bValid = true;

			}

			retHandle->pLastRecord = lastRecordRID;

			//计算每页记录数：每条记录前后有8B的指向信息
			//每页前有4B页面控制信息，2B指向第一个空槽位的记录

			retHandle->recordPerPage = (PF_PAGESIZE - 6) / (retHandle->recordSize + 8);

			//将页面Unpin便于淘汰

			UnpinPage(pfPageHandle);

			//释放PageHandle，不能释放FileHandle

			free(pfPageHandle);
			return SUCCESS;

		}
		else
		{

			//释放资源，返回错误信息

			free(pfPageHandle);
			free(pfFileHandle);
			return getPageRC;

		}
	}
	else
	{

		//释放资源，返回错误信息

		free(pfFileHandle);
		return openRC;

	}
}

//最后测试时间：2019/12/11 8:58
//最后测试状态：符合预期（已更新）
//最后测试人：strangenameBC
RC RM_CloseFile(RM_FileHandle* fileHandle)
{

	//检查fileHandle的状态

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//获取记录控制信息页

	PF_PageHandle* pfPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	pfPageHandle->bOpen = false;

	RC getPageRC = GetThisPage(fileHandle->pPFFileHandle, 1, pfPageHandle);

	if (getPageRC == SUCCESS)
	{

		//获取记录控制信息页成功
		//更新第一个空白页信息

		short* ptrFirstEmpty = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 4);
		*ptrFirstEmpty = fileHandle->firstEmptyPage;

		//更新首条记录信息

		short* ptrFirstRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 6);
		ptrFirstRecord[0] = fileHandle->pFirstRecord->pageNum;
		ptrFirstRecord[1] = fileHandle->pFirstRecord->slotNum;

		//更新末条记录信息

		short* ptrLastRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 10);
		ptrLastRecord[0] = fileHandle->pLastRecord->pageNum;
		ptrLastRecord[1] = fileHandle->pLastRecord->slotNum;

		//标记为脏页并Unpin

		MarkDirty(pfPageHandle);
		UnpinPage(pfPageHandle);

		//销毁页面句柄

		free(pfPageHandle);

	}
	else
	{

		//释放资源并返回

		free(pfPageHandle);
		return getPageRC;

	}

	//关闭对应的页面文件

	RC closeRC = CloseFile(fileHandle->pPFFileHandle);

	if (closeRC == SUCCESS)
	{

		//销毁页面文件句柄

		free(fileHandle->pPFFileHandle);
		fileHandle->pPFFileHandle = NULL;

		//销毁首条、末条记录RID

		free(fileHandle->pFirstRecord);
		free(fileHandle->pLastRecord);
		fileHandle->pFirstRecord = NULL;
		fileHandle->pLastRecord = NULL;

		//添加标记并返回

		fileHandle->bOpen = false;
		return SUCCESS;

	}
	else
	{
		return closeRC;
	}
}
