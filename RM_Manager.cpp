#include "stdafx.h"
#include "RM_Manager.h"
#include "str.h"
#include "PF_Manager.h"

RC OpenScan(RM_FileScan* rmFileScan, RM_FileHandle* fileHandle, int conNum, Con* conditions)//��ʼ��ɨ��
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

//δ���
RC InsertRec(RM_FileHandle* fileHandle, char* pData, RID* rid)
{

	//���fileHandle��״̬

	if (fileHandle->bOpen == false)
	{
		return RM_FHCLOSED;
	}

	//fileHandle���ڴ�״̬
	//����һ���ղ�λ���ڵ�ҳ

	if (fileHandle->firstEmptyPage == -1)
	{

		//δ�����κ�ҳ����ѷ����ҳ��ȫ��



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

//������ʱ�䣺2019/12/11 8:58
//������״̬������Ԥ�ڣ��Ѹ��£�
//�������ˣ�strangenameBC
RC RM_CreateFile(char* fileName, int recordSize)
{

	//���recordSize

	if (recordSize <= 0 || recordSize > 4092)
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

//������ʱ�䣺2019/12/11 8:58
//������״̬������Ԥ�ڣ��Ѹ��£�
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

			//��ȡ��һ���հ�ҳ��

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

//������ʱ�䣺2019/12/11 8:58
//������״̬������Ԥ�ڣ��Ѹ��£�
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
		//���µ�һ���հ�ҳ��Ϣ

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
