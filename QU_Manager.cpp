#include "StdAfx.h"
#include "QU_Manager.h"
#include "SYS_Manager.h"
#include "RM_Manager.h"

//ָ������������Ӧ���������ƴӻ��ӵ�selAttrs�з�����������õ���Offset
//TODO: �ȴ���˾����д
RC GetAttrsByRelName(char* relName, int nInputSelAttrs, RelAttr* selAttrs, int nOutputAttrs, Attr* attrs)
{
	return SUCCESS;
}

//���ڹ���һ���Ͷ�Ӧ������sel�Ӿ��Ӧ�Ŀյ�SelResult*
//δ����
RC Init_Result(SelResult* res, char* relName, int nSelAttrs, RelAttr* selAttrs)
{

	RC rc;

	//��ȡ����relName��������Ϣ

	int nAttrs = 0;
	Attr* attrs = (Attr*)malloc(sizeof(Attr));
	rc = GetAttrsByRelName(relName, nSelAttrs, selAttrs, nAttrs, attrs);

	if (rc == SUCCESS)
	{

		//�ɹ��õ����ڴ˱������
		//��ʼ��SelResult

		res->col_num = nAttrs;
		res->row_num = 0;
		res->next_res = NULL;

		for (int i = 0; i < nAttrs; i++)
		{

			//����ÿ���У������Ϣ

			strcpy(res->fields[i], attrs[i].attrName);
			res->length[i] = attrs[i].size;
			res->offset[i] = attrs[i].offset;
			res->type[i] = attrs[i].type;

		}

		//�ͷ���Դ�����سɹ�

		free(attrs);
		return SUCCESS;

	}
	else
	{

		//�ͷ���Դ�����ش�����Ϣ

		free(attrs);
		return rc;

	}
}

//���ڹ�������100����¼�����һ��SelResult
//δ����
RC Init_Result(SelResult* res, SelResult* father)
{

	father->next_res = res;
	res->col_num = father->col_num;

	//����father���child

	for (int i = 0; i < res->col_num; i++)
	{

		strcpy(res->fields[i], father->fields[i]);
		res->length[i] = father->length[i];
		res->offset[i] = father->offset[i];
		res->row_num = 0;
		res->type[i] = father->type[i];

	}

	return SUCCESS;

}

void Destory_Result(SelResult* res)
{
	for (int i = 0; i < res->row_num; i++)
	{
		for (int j = 0; j < res->col_num; j++)
		{
			delete[] res->res[i][j];
		}
		delete[] res->res[i];
	}
	if (res->next_res != NULL)
	{
		Destory_Result(res->next_res);
	}
}

//δ���
RC Query(char* sql, SelResult* res)
{

	//�����﷨�������

	sqlstr* sql_str = NULL;
	RC rc;

	//��ʼ��sql_str

	sql_str = get_sqlstr();

	//�﷨����

	rc = parse(sql, sql_str);

	if (rc == SUCCESS && sql_str->flag == 1)
	{

		//�ǺϹ���﷨������select���

		//TODO

	}
	else
	{

		//���﷨������߲���select���

		return SQL_SYNTAX;

	}
}

//δ���
RC Select(int nSelAttrs, RelAttr** selAttrs, int nRelations, char** relations, int nConditions, Condition* conditions, SelResult* res)
{

	RC rc;

	//���ȷֱ�ɨ�裬Ȼ��ͶӰ��ƴ��

	for (int i = 0; i < nRelations; i++)
	{

		//��ǰ����

		char* currentRelName = relations[i];

		//����ÿһ�ű���ϵ������ɨ������

		Con* cons = (Con*)malloc(nConditions * sizeof(Con));
		rc = GetScanCons(currentRelName, nConditions, conditions, cons);

		if (rc == SUCCESS)
		{

			//�ɹ�����ɨ������
			//ʹ����������ɨ��
			//�򿪶�Ӧ�ı��ļ�

			RM_FileHandle* relHandle = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
			relHandle->bOpen = false;
			rc = RM_OpenFile(currentRelName, relHandle);

			if (rc == SUCCESS)
			{

				//�򿪱��ļ��ɹ�
				//���첢����ɨ����

				RM_FileScan* scanner = (RM_FileScan*)malloc(sizeof(RM_FileScan));
				scanner->bOpen = false;
				rc = OpenScan(scanner, relHandle, nConditions, cons);

				if (rc == SUCCESS)
				{

					//����ɨ��ɹ�
					//�Ե�ǰ�����ɨ��ƴ��

					//TODO

				}
				else
				{

					//�ͷ���Դ�����ش�����Ϣ

					free(scanner);
					free(relHandle);
					free(cons);
					return rc;

				}

			}
			else
			{

				//�ͷ���Դ�����ش�����Ϣ

				free(relHandle);
				free(cons);
				return rc;

			}

		}
		else
		{

			//�ͷ���Դ�����ش�����Ϣ

			free(cons);
			return rc;

		}

	}
	return SUCCESS;
}
