#include "stdafx.h"
#include "RM_Manager.h"
#include "str.h"
#include "PF_Manager.h"

//δ����
RC OpenScan(RM_FileScan* rmFileScan, RM_FileHandle* fileHandle, int conNum, Con* conditions)//��ʼ��ɨ��
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//�����޸�rmFileScan�ĵ�ֵַ

	RM_FileScan* retRMFileScan = rmFileScan;

	retRMFileScan->pRMFileHandle = fileHandle;
	retRMFileScan->conNum = conNum;
	retRMFileScan->conditions = conditions;

	retRMFileScan->pn = 2;
	retRMFileScan->sn = 0;

	//��Ǵ�״̬

	retRMFileScan->bOpen = true;

}

//δ�깤
RC GetNextRec(RM_FileScan* rmFileScan, RM_Record* rec)
{

}

//������ʱ�䣺2019/12/13 8:26
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC GetRec(RM_FileHandle* fileHandle, RID* rid, RM_Record* rec)
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//����λ��

	if (rid->slotNum >= fileHandle->recordPerPage || rid->slotNum < 0)
	{
		return RM_INVALIDRID;
	}

	//�����޸�rec�ĵ�ֵַ

	RM_Record* retRec = rec;

	//��rid��ָʾ��ҳ��

	PF_PageHandle* targetPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	targetPageHandle->bOpen = false;

	RC getTargetPageRC = GetThisPage(fileHandle->pPFFileHandle, rid->pageNum, targetPageHandle);

	if (getTargetPageRC == SUCCESS)
	{

		//��ȡҳ��ɹ�
		//����λ�Ƿ�Ϊ��

		char* targetSlot = (char*)targetPageHandle->pFrame->page.pData + rid->slotNum * (fileHandle->recordSize + 8) + 2;
		short* targetSlotPrevious = (short*)targetSlot;

		if (targetSlotPrevious[0] == 0)
		{

			//���Ǹ��ղ�λ���޷���ȡ
			//Unpinҳ��

			UnpinPage(targetPageHandle);

			//�ͷ���Դ�����ش�����Ϣ

			free(targetPageHandle);
			return RM_INVALIDRID;

		}
		else
		{

			//�ⲻ�ǿղ�λ�����Ի�ȡ

			char* targetSlotData = targetSlot + 4;
			char* targetSlotNext = targetSlot + fileHandle->recordSize + 4;

			//��䴫���Ķ���

			retRec->rid.pageNum = rid->pageNum;
			retRec->rid.slotNum = rid->slotNum;
			retRec->rid.bValid = true;

			retRec->nextRid.pageNum = targetSlotNext[0];
			retRec->nextRid.slotNum = targetSlotNext[1];
			retRec->nextRid.bValid = true;

			retRec->pData = (char*)malloc(fileHandle->recordSize * sizeof(char));

			for (int i = 0; i < fileHandle->recordSize; i++)
			{
				retRec->pData[i] = targetSlotData[i];
			}

			//Unpinҳ��

			UnpinPage(targetPageHandle);

			//�ͷ���Դ������

			free(targetPageHandle);
			return SUCCESS;

		}
	}
	else
	{

		//�ͷ���Դ�����ش�����Ϣ

		free(targetPageHandle);
		return getTargetPageRC;

	}
}

//������ʱ�䣺2019/12/12 10:55
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC InsertRec(RM_FileHandle* fileHandle, char* pData, RID* rid)
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//�������rid�ĵ�ֵַ

	RID* retRID = rid;

	//fileHandle���ڴ�״̬
	//����һ���ղ�λ���ڵ�ҳ

	if (fileHandle->firstEmptyPage == -1)
	{

		//δ�����κ�ҳ����ѷ����ҳ��ȫ��
		//�����µ�ҳ��

		PF_PageHandle* pgHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
		pgHandle->bOpen = false;

		RC allocateRC = AllocatePage(fileHandle->pPFFileHandle, pgHandle);

		if (allocateRC == SUCCESS)
		{

			//����ҳ��ɹ�
			//��¼�׸��ղ�λΪ0

			short* firstEmptyOfPage = (short*)(char*)pgHandle->pFrame->page.pData;
			*firstEmptyOfPage = 0;

			//��ɿղ�λ����

			char* firstSlot = (char*)pgHandle->pFrame->page.pData + 2;

			//������һ���ղ�λ

			((short*)firstSlot)[0] = 0;
			((short*)firstSlot)[1] = 0;
			((short*)(firstSlot + fileHandle->recordSize + 4))[0] = pgHandle->pFrame->page.pageNum;
			((short*)(firstSlot + fileHandle->recordSize + 4))[1] = 1;

			//�����м�Ĳ�λ

			for (int i = 1; i < fileHandle->recordPerPage - 1; i++)
			{
				char* currentSlot = firstSlot + i * (fileHandle->recordSize + 8);
				((short*)currentSlot)[0] = 0;
				((short*)currentSlot)[1] = 0;
				((short*)(currentSlot + fileHandle->recordSize + 4))[0] = pgHandle->pFrame->page.pageNum;
				((short*)(currentSlot + fileHandle->recordSize + 4))[1] = i + 1;
			}

			//�������Ŀղ�λ

			char* lastSlot = firstSlot + (fileHandle->recordPerPage - 1) * (fileHandle->recordSize + 8);
			((short*)lastSlot)[0] = 0;
			((short*)lastSlot)[1] = 0;
			((short*)(lastSlot + fileHandle->recordSize + 4))[0] = -1;
			((short*)(lastSlot + fileHandle->recordSize + 4))[1] = -1;

			//�ھ���м�¼�׸��пղ۵�ҳ��

			fileHandle->firstEmptyPage = pgHandle->pFrame->page.pageNum;

			//�����ҳ�沢Unpin

			MarkDirty(pgHandle);
			UnpinPage(pgHandle);

			//����ҳ����

			free(pgHandle);

		}
		else
		{

			//�ͷ���Դ������

			free(pgHandle);
			return allocateRC;

		}
	}

	//��¼�����¼��ҳ��λ��

	retRID->pageNum = fileHandle->firstEmptyPage;

	//��ȡ��Ӧ��ҳ��

	PF_PageHandle* pfPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	pfPageHandle->bOpen = false;

	RC getPageRC = GetThisPage(fileHandle->pPFFileHandle, fileHandle->firstEmptyPage, pfPageHandle);

	if (getPageRC == SUCCESS)
	{

		//ҳ���ȡ�ɹ�
		//��ȡҳ���׸��ղ�λ

		short* firstEmptySlotOfPage = (short*)(char*)pfPageHandle->pFrame->page.pData;
		retRID->slotNum = *firstEmptySlotOfPage;

		//�����ݿ�����˲�λ��

		char* targetSlot = (char*)pfPageHandle->pFrame->page.pData + retRID->slotNum * (fileHandle->recordSize + 8) + 2;
		char* targetSlotData = targetSlot + 4;

		for (int i = 0; i < fileHandle->recordSize; i++)
		{
			targetSlotData[i] = pData[i];
		}

		//��ȡ������Ч��¼

		if (fileHandle->pLastRecord->pageNum == -1)
		{

			//δ�����κ�����

			fileHandle->pLastRecord->pageNum = retRID->pageNum;
			fileHandle->pLastRecord->slotNum = retRID->slotNum;
			fileHandle->pLastRecord->bValid = true;

			fileHandle->pFirstRecord->pageNum = retRID->pageNum;
			fileHandle->pFirstRecord->slotNum = retRID->slotNum;
			fileHandle->pFirstRecord->bValid = true;

			//���¼�¼�ĸ�������Ϊ��-1��-1��

			short* targetSlotPrevious = (short*)targetSlot;
			targetSlotPrevious[0] = -1;
			targetSlotPrevious[1] = -1;

		}
		else
		{

			//��ȡԭ������¼���ڵ�ҳ��

			PF_PageHandle* lastRecordPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
			lastRecordPageHandle->bOpen = false;

			RC getThisPageRC = GetThisPage(fileHandle->pPFFileHandle, fileHandle->pLastRecord->pageNum, lastRecordPageHandle);

			if (getThisPageRC == SUCCESS)
			{

				//ҳ���ȡ�ɹ�
				//��ԭ��������Ч��¼�������¼�¼

				char* oldLastRecord = (char*)lastRecordPageHandle->pFrame->page.pData + fileHandle->pLastRecord->slotNum * (fileHandle->recordSize + 8) + 2;
				short* oldLastRecordNext = (short*)(oldLastRecord + fileHandle->recordSize + 4);
				oldLastRecordNext[0] = retRID->pageNum;
				oldLastRecordNext[1] = retRID->slotNum;

				//���¼�¼�ĸ�������Ϊԭ��������Ч��¼

				short* targetSlotPrevious = (short*)targetSlot;
				targetSlotPrevious[0] = fileHandle->pLastRecord->pageNum;
				targetSlotPrevious[1] = fileHandle->pLastRecord->slotNum;

				//���������Ч��¼��Ϣ

				fileHandle->pLastRecord->pageNum = retRID->pageNum;
				fileHandle->pLastRecord->slotNum = retRID->slotNum;
				fileHandle->pLastRecord->bValid = true;

				//��ǲ�ҳ�沢Unpin

				MarkDirty(lastRecordPageHandle);
				UnpinPage(lastRecordPageHandle);

				//����ҳ����

				free(lastRecordPageHandle);

			}
			else
			{

				//Unpin

				UnpinPage(pfPageHandle);

				//�ͷ���Դ������

				free(pfPageHandle);
				free(lastRecordPageHandle);
				return getThisPageRC;

			}
		}

		//����λָ��ָ����һ����λ

		short* targetSlotNext = (short*)(targetSlot + fileHandle->recordSize + 4);

		if (targetSlotNext[0] != pfPageHandle->pFrame->page.pageNum)
		{

			//��һ����λ����λ�ڴ�ҳ����

			*firstEmptySlotOfPage = -1;
			fileHandle->firstEmptyPage = targetSlotNext[0];

		}
		else if (targetSlotNext[0] != -1)
		{

			//��һ����λ��Ȼλ�ڴ�ҳ����

			*firstEmptySlotOfPage = targetSlotNext[1];

		}
		else
		{

			//û�п�λ��

			*firstEmptySlotOfPage = -1;
			fileHandle->firstEmptyPage = -1;

		}

		//���¼�¼����һ��ָ��ָ��-1��-1��

		targetSlotNext[0] = -1;
		targetSlotNext[1] = -1;

		//��ǲ�ҳ�棬Unpin

		MarkDirty(pfPageHandle);
		UnpinPage(pfPageHandle);

		//����ҳ��������ӱ�ǲ�����

		free(pfPageHandle);
		retRID->bValid = true;
		return SUCCESS;

	}
	else
	{

		//�ͷ���Դ������

		free(pfPageHandle);
		return getPageRC;

	}
}

//������ʱ�䣺2019/12/12 10:55
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC DeleteRec(RM_FileHandle* fileHandle, const RID* rid)
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//����λ��

	if (rid->slotNum >= fileHandle->recordPerPage || rid->slotNum < 0)
	{
		return RM_INVALIDRID;
	}

	//��rid��ָʾ��ҳ��

	PF_PageHandle* targetPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	targetPageHandle->bOpen = false;

	RC getTargetPageRC = GetThisPage(fileHandle->pPFFileHandle, rid->pageNum, targetPageHandle);

	if (getTargetPageRC == SUCCESS)
	{

		//ҳ���ȡ�ɹ�

		char* targetSlot = (char*)targetPageHandle->pFrame->page.pData + rid->slotNum * (fileHandle->recordSize + 8) + 2;
		char* targetSlotData = targetSlot + 4;

		short* targetSlotPrevious = (short*)targetSlot;
		short* targetSlotNext = (short*)(targetSlot + fileHandle->recordSize + 4);

		if (targetSlotPrevious[0] == 0)
		{

			//�˼�¼ѹ�����ǿռ�¼
			//Unpin

			UnpinPage(targetPageHandle);

			//�ͷ���Դ�����ش�����Ϣ

			free(targetPageHandle);
			return RM_INVALIDRID;

		}

		//Ĩ����λ����

		for (int i = 0; i < fileHandle->recordSize; i++)
		{
			targetSlotData[i] = 0;
		}

		//���˲�λǰ����

		if (!(fileHandle->pFirstRecord->pageNum == rid->pageNum &&
			fileHandle->pFirstRecord->slotNum == rid->slotNum &&
			fileHandle->pLastRecord->pageNum == rid->pageNum &&
			fileHandle->pLastRecord->slotNum == rid->slotNum))
		{

			//�˼�¼����ΨһҪɾ���ļ�¼

			if (fileHandle->pFirstRecord->pageNum == rid->pageNum &&
				fileHandle->pFirstRecord->slotNum == rid->slotNum)
			{

				//�˼�¼��������¼

				fileHandle->pFirstRecord->pageNum = targetSlotNext[0];
				fileHandle->pFirstRecord->slotNum = targetSlotNext[1];

			}
			else if (fileHandle->pLastRecord->pageNum == rid->pageNum &&
				fileHandle->pLastRecord->slotNum == rid->slotNum)
			{

				//�˼�¼��ĩ����¼

				fileHandle->pLastRecord->pageNum = targetSlotPrevious[0];
				fileHandle->pLastRecord->slotNum = targetSlotPrevious[1];

			}
			else
			{

				//�˼�¼���м��¼

				int previousPage = targetSlotPrevious[0];
				int previousSlot = targetSlotPrevious[1];
				int nextPage = targetSlotNext[0];
				int nextSlot = targetSlotNext[1];

				//��ǰһ����¼��������һ��

				if (previousPage == rid->pageNum)
				{

					//ǰһ����¼���ڴ�ҳ

					short* previousNext = (short*)((char*)targetPageHandle->pFrame->page.pData + (previousSlot + 1) * (fileHandle->recordSize + 8) - 2);
					previousNext[0] = nextPage;
					previousNext[1] = nextSlot;

				}
				else
				{

					//ǰһ����¼���ڴ�ҳ

					PF_PageHandle* previousPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
					previousPageHandle->bOpen = false;

					RC getPreviousPageRC = GetThisPage(fileHandle->pPFFileHandle, previousPage, previousPageHandle);

					if (getPreviousPageRC == SUCCESS)
					{

						//ǰһ����¼���ڵ�ҳ�򿪳ɹ�

						short* previousNext = (short*)((char*)previousPageHandle->pFrame->page.pData + (previousSlot + 1) * (fileHandle->recordSize + 8) - 2);
						previousNext[0] = nextPage;
						previousNext[1] = nextSlot;

						//Unpin�������ҳ��

						MarkDirty(previousPageHandle);
						UnpinPage(previousPageHandle);

						//����ҳ����

						free(previousPageHandle);

					}
					else
					{

						//Unpin

						UnpinPage(targetPageHandle);

						//�ͷ���Դ�����ش�����Ϣ

						free(previousPageHandle);
						free(targetPageHandle);
						return getPreviousPageRC;

					}
				}

				//����һ����¼������ǰһ��

				if (nextPage == rid->pageNum)
				{

					//��һ����¼���ڴ�ҳ

					short* nextPrevious = (short*)((char*)targetPageHandle->pFrame->page.pData + nextSlot * (fileHandle->recordSize + 8) + 2);
					nextPrevious[0] = previousPage;
					nextPrevious[1] = previousSlot;

				}
				else
				{

					//��һ����¼���ڴ�ҳ

					PF_PageHandle* nextPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
					nextPageHandle->bOpen = false;

					RC getNextPageRC = GetThisPage(fileHandle->pPFFileHandle, nextPage, nextPageHandle);

					if (getNextPageRC == SUCCESS)
					{

						//��һ����¼���ڵ�ҳ��򿪳ɹ�

						short* nextPrevious = (short*)((char*)nextPageHandle->pFrame->page.pData + nextSlot * (fileHandle->recordSize + 8) + 2);
						nextPrevious[0] = previousPage;
						nextPrevious[1] = previousSlot;

						//�����ҳ�沢Unpin

						MarkDirty(nextPageHandle);
						UnpinPage(nextPageHandle);

						//����ҳ����

						free(nextPageHandle);

					}
					else
					{

						//Unpin�Ա�����̭

						UnpinPage(targetPageHandle);

						//�ͷ���Դ�����ش�����Ϣ

						free(nextPageHandle);
						free(targetPageHandle);
						return getNextPageRC;

					}
				}
			}
		}
		else
		{

			//�˼�¼��ΨһҪɾ���ļ�¼

			fileHandle->pFirstRecord->pageNum = -1;
			fileHandle->pFirstRecord->slotNum = -1;
			fileHandle->pFirstRecord->bValid = false;

			fileHandle->pLastRecord->pageNum = -1;
			fileHandle->pLastRecord->slotNum = -1;
			fileHandle->pLastRecord->bValid = false;

		}

		//���˲�λ��Ϊ�׸��ղ�λ���Ա���һ���������
		//�˲�λ�ĸ�����Ϊ��0��0��

		targetSlotPrevious[0] = 0;
		targetSlotPrevious[1] = 0;

		//�˲�λ����һ����λ����Ϊԭ�ȵ��׸��ղ�λ
		//��ȡԭ�ȵĿղ�λ��Ϣ

		if (fileHandle->firstEmptyPage != rid->pageNum)
		{

			//ԭ�ȵ��׸��ղ�λ������ҳ��

			PF_PageHandle* oldFirstEmptyPage = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
			oldFirstEmptyPage->bOpen = false;

			RC getOldFirstEmptyPageRC = GetThisPage(fileHandle->pPFFileHandle, fileHandle->firstEmptyPage, oldFirstEmptyPage);

			if (getOldFirstEmptyPageRC == SUCCESS)
			{

				//��ȡҳ��ɹ�
				//���¿ղ�λ�������ɵ��׸��ղ�λ

				short* oldFirstEmptySlot = (short*)(char*)oldFirstEmptyPage->pFrame->page.pData;
				targetSlotNext[0] = fileHandle->firstEmptyPage;
				targetSlotNext[1] = *oldFirstEmptySlot;

				//Unpin�Ա�����̭

				UnpinPage(oldFirstEmptyPage);

				//����ҳ����

				free(oldFirstEmptyPage);

			}
			else
			{

				//Unpin

				UnpinPage(targetPageHandle);

				//�ͷ���Դ�����ش�����Ϣ

				free(targetPageHandle);
				free(oldFirstEmptyPage);
				return getOldFirstEmptyPageRC;

			}
		}
		else
		{

			//ԭ�ȵĿղ�λ�ڴ�ҳ����

			short* firstEmptySlot = (short*)(char*)targetPageHandle->pFrame->page.pData;
			targetSlotNext[0] = rid->pageNum;
			targetSlotNext[1] = *firstEmptySlot;

		}

		//���׸��п�λ��ҳ������Ϊ��ҳ��

		fileHandle->firstEmptyPage = rid->pageNum;

		//����ҳ����׸��ղ�λ����Ϊ������Ĳ�λ

		short* firstEmptySlot = (short*)(char*)targetPageHandle->pFrame->page.pData;
		*firstEmptySlot = rid->slotNum;

		//�����ҳ�沢Unpin

		MarkDirty(targetPageHandle);
		UnpinPage(targetPageHandle);

		//����ҳ���������سɹ���Ϣ

		free(targetPageHandle);
		return SUCCESS;

	}
	else
	{

		//�ͷ���Դ�����ش�����Ϣ

		free(targetPageHandle);
		return getTargetPageRC;

	}
}

//������ʱ�䣺2019/12/12 11:20
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC UpdateRec(RM_FileHandle* fileHandle, const RM_Record* rec)
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//����λ��

	if (rec->rid.slotNum >= fileHandle->recordPerPage || rec->rid.slotNum < 0)
	{
		return RM_INVALIDRID;
	}

	//�򿪼�¼���ڵ�ҳ��

	PF_PageHandle* targetPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	targetPageHandle->bOpen = false;

	RC getTargetPageRC = GetThisPage(fileHandle->pPFFileHandle, rec->rid.pageNum, targetPageHandle);

	if (getTargetPageRC == SUCCESS)
	{

		//��ȡҳ��ɹ�
		//����λ�Ƿ�Ϊ��

		char* targetSlot = (char*)targetPageHandle->pFrame->page.pData + rec->rid.slotNum * (fileHandle->recordSize + 8) + 2;
		short* targetSlotPrevious = (short*)targetSlot;

		if (targetSlotPrevious[0] == 0)
		{

			//���Ǹ��ղ�λ���޷�����
			//Unpinҳ��

			UnpinPage(targetPageHandle);

			//�ͷ���Դ�����ش�����Ϣ

			free(targetPageHandle);
			return RM_INVALIDRID;

		}
		else
		{

			//�ⲻ�ǿղ�λ�����Ը���
			//��������

			char* targetSlotData = targetSlot + 4;

			for (int i = 0; i < fileHandle->recordSize; i++)
			{
				targetSlotData[i] = rec->pData[i];
			}

			//�����ҳ�沢Unpin

			MarkDirty(targetPageHandle);
			UnpinPage(targetPageHandle);

			//���سɹ���Ϣ

			return SUCCESS;

		}
	}
	else
	{

		//�ͷ���Դ�����ش�����Ϣ

		free(targetPageHandle);
		return getTargetPageRC;

	}
}

//������ʱ�䣺2019/12/11 10:12
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC RM_CreateFile(char* fileName, int recordSize)
{

	//���recordSize

	if (recordSize <= 0 || recordSize > 4082)
	{
		return RM_INVALIDRECSIZE;
	}

	//�������ļ�

	RC createRC = CreateFile(fileName);

	if (createRC == SUCCESS)
	{

		//�ļ������ɹ�

		PF_FileHandle* fileHandle = getPF_FileHandle();
		RC openRC = openFile(fileName, fileHandle);

		if (openRC == SUCCESS)
		{

			//�ļ��򿪳ɹ�
			//������¼��Ϣ����ҳ

			PF_PageHandle* pgHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
			pgHandle->bOpen = false;

			RC allocateRC = AllocatePage(fileHandle, pgHandle);

			if (allocateRC == SUCCESS)
			{

				//��¼��Ϣ����ҳ�����ɹ�
				//ҳ����Ϣ����

				for (int i = 0; i < PF_PAGE_SIZE; i++)
				{
					pgHandle->pFrame->page.pData[i] = 0;
				}

				//����¼��Ϣ����ҳ��Ϣ����¼����

				int* headOfPage = (int*)(char*)pgHandle->pFrame->page.pData;
				*headOfPage = recordSize;

				//��¼��һ���п�λ��ҳ�棬-1��ʾδ�����κ�����ҳ����ѷ����ҳ��ȫ��

				short* ptrFirstEmpty = (short*)(((char*)pgHandle->pFrame->page.pData) + 4);
				*ptrFirstEmpty = -1;

				//��¼��һ����Ч���ݵ�λ�ã���-1��-1����ʾδ�����κ�����

				short* ptrFirstRecord = (short*)(((char*)pgHandle->pFrame->page.pData) + 6);
				ptrFirstRecord[0] = -1;
				ptrFirstRecord[1] = -1;

				//��¼���һ����Ч���ݵ�λ�ã���-1��-1����ʾδ�����κ�����

				short* ptrLastRecord = (short*)(((char*)pgHandle->pFrame->page.pData) + 10);
				ptrLastRecord[0] = -1;
				ptrLastRecord[1] = -1;

				//���ҳ��Ϊ��ҳ��Unpin

				MarkDirty(pgHandle);
				UnpinPage(pgHandle);

				//�ر��ļ�

				RC closeRC = CloseFile(fileHandle);

				//�ͷ���Դ�����������Ϣ

				free(pgHandle);
				free(fileHandle);
				return closeRC;

			}
			else
			{

				//�ر��ļ�

				CloseFile(fileHandle);

				//�ͷ���Դ�����ش�����Ϣ

				free(pgHandle);
				free(fileHandle);
				return allocateRC;

			}
		}
		else
		{

			//�ͷ���Դ�����ش�����Ϣ

			free(fileHandle);
			return openRC;

		}
	}
	else
	{

		//���ش�����Ϣ

		return createRC;

	}
}

//������ʱ�䣺2019/12/11 10:12
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC RM_OpenFile(char* fileName, RM_FileHandle* fileHandle)
{

	//��ֹ�޸�fileHandle����ĵ�ֵַ

	RM_FileHandle* retHandle = fileHandle;

	//��ҳ���ļ�

	PF_FileHandle* pfFileHandle = getPF_FileHandle();
	RC openRC = openFile(fileName, pfFileHandle);

	if (openRC == SUCCESS)
	{

		//��ҳ���ļ��ɹ�

		retHandle->pPFFileHandle = pfFileHandle;
		retHandle->bOpen = true;

		//��ȡ��¼������Ϣҳ

		PF_PageHandle* pfPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
		pfPageHandle->bOpen = false;

		RC getPageRC = GetThisPage(pfFileHandle, 1, pfPageHandle);

		if (getPageRC == SUCCESS)
		{

			//��ȡ��¼������Ϣҳ�ɹ�
			//��ȡ��¼����

			int* headOfPage = (int*)(char*)pfPageHandle->pFrame->page.pData;
			retHandle->recordSize = *headOfPage;

			//��ȡ��һ���пղ�λҳ��

			short* ptrFirstEmpty = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 4);
			retHandle->firstEmptyPage = *ptrFirstEmpty;

			//��ȡ��һ����Ч��¼��λ��

			RID* firstRecordRID = (RID*)malloc(sizeof(RID));
			short* ptrFirstRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 6);

			if (ptrFirstRecord[0] == -1)
			{

				//δ�����κ�����

				firstRecordRID->pageNum = -1;
				firstRecordRID->slotNum = -1;
				firstRecordRID->bValid = false;

			}
			else
			{

				//���������ݣ���¼��һ�����ݵ�λ��

				firstRecordRID->pageNum = ptrFirstRecord[0];
				firstRecordRID->slotNum = ptrFirstRecord[1];
				firstRecordRID->bValid = true;

			}

			retHandle->pFirstRecord = firstRecordRID;

			//��ȡ���һ����Ч��¼��λ��

			RID* lastRecordRID = (RID*)malloc(sizeof(RID));
			short* ptrLastRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 10);

			if (ptrLastRecord[0] == -1)
			{

				//δ�����κ�����

				lastRecordRID->pageNum = -1;
				lastRecordRID->slotNum = -1;
				lastRecordRID->bValid = false;

			}
			else
			{

				//���������ݣ���¼���һ�����ݵ�λ��

				lastRecordRID->pageNum = ptrLastRecord[0];
				lastRecordRID->slotNum = ptrLastRecord[1];
				lastRecordRID->bValid = true;

			}

			retHandle->pLastRecord = lastRecordRID;

			//����ÿҳ��¼����ÿ����¼ǰ����8B��ָ����Ϣ
			//ÿҳǰ��4Bҳ�������Ϣ��2Bָ���һ���ղ�λ�ļ�¼

			retHandle->recordPerPage = (PF_PAGESIZE - 6) / (retHandle->recordSize + 8);

			//��ҳ��Unpin������̭

			UnpinPage(pfPageHandle);

			//�ͷ�PageHandle�������ͷ�FileHandle

			free(pfPageHandle);
			return SUCCESS;

		}
		else
		{

			//�ͷ���Դ�����ش�����Ϣ

			free(pfPageHandle);
			free(pfFileHandle);
			return getPageRC;

		}
	}
	else
	{

		//�ͷ���Դ�����ش�����Ϣ

		free(pfFileHandle);
		return openRC;

	}
}

//������ʱ�䣺2019/12/11 10:12
//������״̬������Ԥ��
//�������ˣ�strangenameBC
RC RM_CloseFile(RM_FileHandle* fileHandle)
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//��ȡ��¼������Ϣҳ

	PF_PageHandle* pfPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	pfPageHandle->bOpen = false;

	RC getPageRC = GetThisPage(fileHandle->pPFFileHandle, 1, pfPageHandle);

	if (getPageRC == SUCCESS)
	{

		//��ȡ��¼������Ϣҳ�ɹ�
		//���µ�һ���пղ�λҳ��Ϣ

		short* ptrFirstEmpty = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 4);
		*ptrFirstEmpty = fileHandle->firstEmptyPage;

		//����������¼��Ϣ

		short* ptrFirstRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 6);
		ptrFirstRecord[0] = fileHandle->pFirstRecord->pageNum;
		ptrFirstRecord[1] = fileHandle->pFirstRecord->slotNum;

		//����ĩ����¼��Ϣ

		short* ptrLastRecord = (short*)(((char*)pfPageHandle->pFrame->page.pData) + 10);
		ptrLastRecord[0] = fileHandle->pLastRecord->pageNum;
		ptrLastRecord[1] = fileHandle->pLastRecord->slotNum;

		//���Ϊ��ҳ��Unpin

		MarkDirty(pfPageHandle);
		UnpinPage(pfPageHandle);

		//����ҳ����

		free(pfPageHandle);

	}
	else
	{

		//�ͷ���Դ������

		free(pfPageHandle);
		return getPageRC;

	}

	//�رն�Ӧ��ҳ���ļ�

	RC closeRC = CloseFile(fileHandle->pPFFileHandle);

	if (closeRC == SUCCESS)
	{

		//����ҳ���ļ����

		free(fileHandle->pPFFileHandle);
		fileHandle->pPFFileHandle = NULL;

		//����������ĩ����¼RID

		free(fileHandle->pFirstRecord);
		free(fileHandle->pLastRecord);
		fileHandle->pFirstRecord = NULL;
		fileHandle->pLastRecord = NULL;

		//��ӱ�ǲ�����

		fileHandle->bOpen = false;
		return SUCCESS;

	}
	else
	{
		return closeRC;
	}
}
