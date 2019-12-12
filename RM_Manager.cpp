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

//未测试
RC InsertRec(RM_FileHandle* fileHandle, char* pData, RID* rid)
{

	//检查fileHandle的状态

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//避免更改rid的地址值

	RID* retRID = rid;

	//fileHandle处于打开状态
	//检查第一个空槽位所在的页

	if (fileHandle->firstEmptyPage == -1)
	{

		//未分配任何页面或已分配的页面全满
		//分配新的页面

		PF_PageHandle* pgHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
		pgHandle->bOpen = false;

		RC allocateRC = AllocatePage(fileHandle->pPFFileHandle, pgHandle);

		if (allocateRC == SUCCESS)
		{

			//分配页面成功
			//记录首个空槽位为0

			short* firstEmptyOfPage = (short*)(char*)pgHandle->pFrame->page.pData;
			*firstEmptyOfPage = 0;

			//完成空槽位串联

			char* firstSlot = (char*)pgHandle->pFrame->page.pData + 2;

			//串联第一个空槽位

			((short*)firstSlot)[0] = 0;
			((short*)firstSlot)[1] = 0;
			((short*)(firstSlot + fileHandle->recordSize + 4))[0] = pgHandle->pFrame->page.pageNum;
			((short*)(firstSlot + fileHandle->recordSize + 4))[1] = 1;

			//串联中间的槽位

			for (int i = 1; i < fileHandle->recordPerPage - 1; i++)
			{
				char* currentSlot = firstSlot + i * (fileHandle->recordSize + 8);
				((short*)currentSlot)[0] = 0;
				((short*)currentSlot)[1] = 0;
				((short*)(currentSlot + fileHandle->recordSize + 4))[0] = pgHandle->pFrame->page.pageNum;
				((short*)(currentSlot + fileHandle->recordSize + 4))[1] = i + 1;
			}

			//串联最后的空槽位

			char* lastSlot = firstSlot + (fileHandle->recordPerPage - 1) * (fileHandle->recordSize + 8);
			((short*)lastSlot)[0] = 0;
			((short*)lastSlot)[1] = 0;
			((short*)(lastSlot + fileHandle->recordSize + 4))[0] = -1;
			((short*)(lastSlot + fileHandle->recordSize + 4))[1] = -1;

			//在句柄中记录首个有空槽的页面

			fileHandle->firstEmptyPage = pgHandle->pFrame->page.pageNum;

			//标记脏页面并Unpin

			MarkDirty(pgHandle);
			UnpinPage(pgHandle);

			//销毁页面句柄

			free(pgHandle);

		}
		else
		{

			//释放资源并返回

			free(pgHandle);
			return allocateRC;

		}
	}

	//记录插入记录的页面位置

	retRID->pageNum = fileHandle->firstEmptyPage;

	//获取对应的页面

	PF_PageHandle* pfPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	pfPageHandle->bOpen = false;

	RC getPageRC = GetThisPage(fileHandle->pPFFileHandle, fileHandle->firstEmptyPage, pfPageHandle);

	if (getPageRC == SUCCESS)
	{

		//页面获取成功
		//读取页面首个空槽位

		short* firstEmptySlotOfPage = (short*)(char*)pfPageHandle->pFrame->page.pData;
		retRID->slotNum = *firstEmptySlotOfPage;

		//将数据拷贝入此槽位中

		char* targetSlot = (char*)pfPageHandle->pFrame->page.pData + retRID->slotNum * (fileHandle->recordSize + 8) + 2;
		char* targetSlotData = targetSlot + 4;

		for (int i = 0; i < fileHandle->recordSize; i++)
		{
			targetSlotData[i] = pData[i];
		}

		//读取最后的有效记录

		if (fileHandle->pLastRecord->pageNum == -1)
		{

			//未储存任何数据

			fileHandle->pLastRecord->pageNum = retRID->pageNum;
			fileHandle->pLastRecord->slotNum = retRID->slotNum;
			fileHandle->pLastRecord->bValid = true;

			fileHandle->pFirstRecord->pageNum = retRID->pageNum;
			fileHandle->pFirstRecord->slotNum = retRID->slotNum;
			fileHandle->pFirstRecord->bValid = true;

		}
		else
		{

			//获取原先最后记录所在的页面

			PF_PageHandle* lastRecordPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
			lastRecordPageHandle->bOpen = false;

			RC getThisPageRC = GetThisPage(fileHandle->pPFFileHandle, fileHandle->pLastRecord->pageNum, lastRecordPageHandle);

			if (getThisPageRC == SUCCESS)
			{

				//页面获取成功
				//将原先最后的有效记录连接至新纪录

				char* oldLastRecord = (char*)lastRecordPageHandle->pFrame->page.pData + fileHandle->pLastRecord->slotNum * (fileHandle->recordSize + 8) + 2;
				short* oldLastRecordNext = (short*)(oldLastRecord + fileHandle->recordSize + 4);
				oldLastRecordNext[0] = retRID->pageNum;
				oldLastRecordNext[1] = retRID->slotNum;

				//将新记录的父亲设置为原先原先最后的有效记录

				short* targetSlotPrevious = (short*)targetSlot;
				targetSlotPrevious[0] = fileHandle->pLastRecord->pageNum;
				targetSlotPrevious[1] = fileHandle->pLastRecord->slotNum;

				//将空位指针指向下一个空位

				short* targetSlotNext = (short*)(targetSlot + fileHandle->recordSize + 4);

				if (targetSlotNext[0] != pfPageHandle->pFrame->page.pageNum)
				{

					//下一个空位不再位于此页面上

					*firstEmptySlotOfPage = -1;
					fileHandle->firstEmptyPage = targetSlotNext[0];

				}
				else if (targetSlotNext[0] != -1)
				{

					//下一个空位仍然位于此页面上

					*firstEmptySlotOfPage = targetSlotNext[1];

				}
				else
				{

					//没有空位了

					*firstEmptySlotOfPage = -1;
					fileHandle->firstEmptyPage = -1;

				}

				//将新纪录的下一个指针指向（-1，-1）

				targetSlotNext[0] = -1;
				targetSlotNext[1] = -1;

				//更新最后有效记录信息

				fileHandle->pLastRecord->pageNum = retRID->pageNum;
				fileHandle->pLastRecord->slotNum = retRID->slotNum;
				fileHandle->pLastRecord->bValid = true;

				//标记藏页面并Unpin

				MarkDirty(lastRecordPageHandle);
				UnpinPage(lastRecordPageHandle);

				//销毁页面句柄

				free(lastRecordPageHandle);

			}
			else
			{

				//释放资源并返回

				free(pfPageHandle);
				free(lastRecordPageHandle);
				return getThisPageRC;

			}
		}

		//标记藏页面，Unpin

		MarkDirty(pfPageHandle);
		UnpinPage(pfPageHandle);

		//销毁页面句柄并返回

		free(pfPageHandle);
		return SUCCESS;

	}
	else
	{

		//释放资源并返回

		free(pfPageHandle);
		return getPageRC;

	}
}

RC DeleteRec(RM_FileHandle* fileHandle, const RID* rid)
{
	return SUCCESS;
}

RC UpdateRec(RM_FileHandle* fileHandle, const RM_Record* rec)
{
	return SUCCESS;
}

//最后测试时间：2019/12/11 10:12
//最后测试状态：符合预期
//最后测试人：strangenameBC
RC RM_CreateFile(char* fileName, int recordSize)
{

	//检查recordSize

	if (recordSize <= 0 || recordSize > 4082)
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

//最后测试时间：2019/12/11 10:12
//最后测试状态：符合预期
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

			//获取第一个有空槽位页码

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

//最后测试时间：2019/12/11 10:12
//最后测试状态：符合预期
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
		//更新第一个有空槽位页信息

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
