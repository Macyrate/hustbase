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
				headOfPage[0] = recordSize;

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

		//TODO:��ȡ��¼����

		return SUCCESS;

	}
	else
	{

		//�ͷ���Դ�����ش�����Ϣ

		free(pfFileHandle);
		return openRC;
	}
}

RC RM_CloseFile(RM_FileHandle* fileHandle)
{
	return SUCCESS;
}
