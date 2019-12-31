#include "StdAfx.h"
#include "QU_Manager.h"
#include "SYS_Manager.h"
#include "RM_Manager.h"

//ָ������������Ӧ���������ƴӻ��ӵ�selAttrs�з�����������õ���Offset
//TODO: δ����
RC GetAttrsByRelName(char* relName, int nInputSelAttrs, RelAttr** selAttrs, int* nOutputAttrs, Attr* attrs)
{

	RC rc;
	RM_FileHandle* hSyscolumns = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	RM_FileScan* FileScan = (RM_FileScan*)calloc(1, sizeof(RM_FileScan));
	RM_Record* syscolumnsRec = (RM_Record*)calloc(1, sizeof(syscolumnsRec));
	Con* checkerCons = (Con*)calloc(2, sizeof(Con));
	hSyscolumns->bOpen = false;
	FileScan->bOpen = false;
	*nOutputAttrs = 0;

	//��ϵͳ���ļ�

	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS)return rc;

	int i = 0;
	for (int i = 0; i < nInputSelAttrs; i++)
	{
		if (selAttrs[i]->relName != NULL && strcmp(selAttrs[i]->relName, relName) != 0)		//�б����ұ�������������
			continue;

		//�Ա������ϵ�����,��SYSCOLUMNS��ɨ��

		//ɨ������1������ΪrelName
		checkerCons[0].bLhsIsAttr = 1;
		checkerCons[0].LattrLength = 21;
		checkerCons[0].LattrOffset = 0;
		checkerCons[0].compOp = CompOp::EQual;
		checkerCons[0].attrType = chars;
		checkerCons[0].bRhsIsAttr = 0;
		checkerCons[0].Rvalue = (void*)calloc(1, 21);
		strcpy((char*)checkerCons[0].Rvalue, relName);

		//ɨ������2��������ΪselAttrs[i]->attrName
		checkerCons[1].bLhsIsAttr = 1;
		checkerCons[1].LattrLength = 21;
		checkerCons[1].LattrOffset = 21;
		checkerCons[1].compOp = CompOp::EQual;
		checkerCons[1].bRhsIsAttr = 0;
		checkerCons[1].Rvalue = (void*)calloc(1, 21);
		strcpy((char*)checkerCons[1].Rvalue, selAttrs[i]->attrName);

		//ɨ��SYSCOLUMNS����ȡ���Զ�Ӧ��¼
		rc = OpenScan(FileScan, hSyscolumns, 2, checkerCons);
		if (rc != SUCCESS)return rc;
		rc = GetNextRec(FileScan, syscolumnsRec);
		if (rc != SUCCESS)return rc;

		//��ȡ������Ϣ
		strcpy(attrs[*nOutputAttrs].attrName, selAttrs[i]->attrName);						//��ȡ������
		attrs[*nOutputAttrs].type = (AttrType) * (int*)(syscolumnsRec->pData + 42);			//��ȡ��������
		attrs[*nOutputAttrs].size = *(int*)(syscolumnsRec->pData + 42 + sizeof(int));		//��ȡ���Գ���
		attrs[*nOutputAttrs].offset = *(int*)(syscolumnsRec->pData + 42 + sizeof(int) * 2);	//��ȡ���Գ���

		(*nOutputAttrs)++;

		rc = CloseScan(FileScan);
		if (rc != SUCCESS)return rc;
	}

	//��β
	free(FileScan);

	rc = RM_CloseFile(hSyscolumns);
	if (rc != SUCCESS)return rc;
	free(hSyscolumns);

	//free(syscolumnsRec);
	free(checkerCons);

	return SUCCESS;
}

//���ڳ���¼���������������¼
//δ����
RC AddResult(SelResult* res, int nData, char** data)
{

	//Ѱ����������β��

	SelResult* current = res;
	while (current->next_res != NULL)
	{
		current = current->next_res;
	}

	//����ÿ����¼�ĳ���

	int totalLength = 0;
	for (int i = 0; i < current->col_num; i++)
	{
		totalLength += current->length[i];
	}

	//����ÿһ��Ҫ��ӵļ�¼

	for (int i = 0; i < nData; i++)
	{

		//���Ѿ�����100����¼���򿪱��µ�SelResult

		if (current->row_num == 100)
		{
			SelResult* nextResult = (SelResult*)malloc(sizeof(SelResult));
			Init_Result(nextResult, current);
			current = nextResult;
		}

		//Ϊcurrent->res[current->row_num]���ٿռ�

		current->res[current->row_num] = (char**)malloc(sizeof(char*));
		*current->res[current->row_num] = (char*)malloc(totalLength * sizeof(char));

		//��Ӽ�¼

		memcpy(*(current->res[current->row_num++]), data[i], totalLength * sizeof(char));

	}

	return SUCCESS;

}

//���ڹ���һ���Ͷ�Ӧ������sel�Ӿ��Ӧ�Ŀյ�SelResult*
//δ����
RC Init_Result(SelResult* res, char* relName, int nAttrs, Attr* attrs)
{

	RC rc;

	//�ɹ��õ����ڴ˱������
	//��ʼ��SelResult

	res->col_num = nAttrs;
	res->row_num = 0;
	res->next_res = NULL;

	int currentOffset = 0;
	for (int i = 0; i < nAttrs; i++)
	{

		//����ÿ���У������Ϣ

		strcpy(res->fields[i], attrs[i].attrName);
		res->length[i] = attrs[i].size;
		res->offset[i] = currentOffset;
		res->type[i] = attrs[i].type;

		currentOffset += res->length[i];

	}

	//���سɹ�

	return SUCCESS;

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

//����������ĵѿ�����
//δ����
RC Join(SelResult* resA, SelResult* resB, SelResult* outRes)
{

	//ƴ����������У������µ�ÿ���е�ƫ����

	outRes->col_num = resA->col_num + resB->col_num;
	outRes->row_num = 0;

	for (int i = 0; i < resA->col_num; i++)
	{

		//��ǰa�����resA��ƫ������Ϣ���г���

		outRes->length[i] = resA->length[i];
		outRes->offset[i] = resA->offset[i];

	}

	int currentOffset = resA->offset[resA->col_num - 1] + resA->length[resA->col_num - 1];

	for (int i = 0; i < resB->col_num; i++)
	{

		//���b�����resB�ĳ�����Ϣ��������Ӧ�е�ƫ����

		outRes->length[i + resA->col_num] = resB->length[i];
		outRes->offset[i + resA->col_num] = resB->offset[i] + currentOffset;

	}

	//���ˣ��µ�outRes�ṹ�������
	//�������
	//�������ڴ�����ʱ�е��ֶ�

	char** data = (char**)malloc(MAX_SINGLE_REL_RES_NUM * sizeof(char*));
	int nData = 0;
	int totalLengthA = resA->length[resA->col_num - 1] + resA->offset[resA->col_num - 1];
	int totalLengthB = resB->length[resB->col_num - 1] + resB->offset[resB->col_num - 1];
	int totalLength = outRes->length[outRes->col_num - 1] + outRes->offset[outRes->col_num - 1];
	SelResult* currentA = resA;
	while (currentA != NULL)
	{

		//���ѭ����˳���ó�resA�����������������ݵ�����

		for (int i = 0; i < currentA->row_num; i++)
		{

			SelResult* currentB = resB;
			while (currentB != NULL)
			{

				//�ڲ�ѭ����˳���ó�resB�����������������ݵ�����

				for (int j = 0; j < currentB->row_num; j++)
				{

					data[nData] = (char*)malloc(totalLength * sizeof(char));

					//��ǰ�벿���Ϊa�е�����

					memcpy(data[nData], *(currentA->res[i]), totalLengthA * sizeof(char));

					//����벿���Ϊb�е�����

					memcpy(data[nData++], *(currentB->res[j]), totalLengthB * sizeof(char));

				}

				currentB = currentB->next_res;

			}

		}

		currentA = currentA->next_res;

	}

	//�������ӽ�outRes��

	AddResult(outRes, nData, data);

	//��ʰ����

	for (int i = 0; i < nData; i++)
	{
		free(data[i]);
	}
	free(data);

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

//��ɲ�ѯ
//δ����
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

		rc = Select(sql_str->sstr.sel.nSelAttrs, sql_str->sstr.sel.selAttrs, sql_str->sstr.sel.nRelations, sql_str->sstr.sel.relations, sql_str->sstr.sel.nConditions, sql_str->sstr.sel.conditions, res);
		return rc;

	}
	else
	{

		//���﷨������߲���select���

		return SQL_SYNTAX;

	}
}

//֧�ֶ��Ĳ�ѯ
//δ����
RC Select(int nSelAttrs, RelAttr** selAttrs, int nRelations, char** relations, int nConditions, Condition* conditions, SelResult* res)
{

	RC rc;

	//���ȷֱ�ɨ�裬Ȼ��ͶӰ��ƴ��
	//����һ�����ڴ������ɨ����������

	SelResult* singleResults = (SelResult*)malloc(nRelations * sizeof(SelResult));

	for (int i = 0; i < nRelations; i++)
	{

		//��ǰ����

		char* currentRelName = relations[i];
		SelResult* currentResult = singleResults + i;;

		//��ʼ����ǰ���ɨ��������

		int nAttrs = 0;
		Attr* currentAttrs = (Attr*)malloc(20 * sizeof(Attr));
		rc = GetAttrsByRelName(currentRelName, nSelAttrs, selAttrs, &nAttrs, currentAttrs);

		if (rc == SUCCESS)
		{

			Init_Result(currentResult, currentRelName, nAttrs, currentAttrs);

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

						char** data = (char**)malloc(MAX_SINGLE_REL_RES_NUM * sizeof(char*));
						int nData = 0;
						int totalLength = currentResult->offset[currentResult->col_num - 1] + currentResult->length[currentResult->col_num - 1];
						RM_Record* currentRec = (RM_Record*)malloc(sizeof(RM_Record));
						while (GetNextRec(scanner, currentRec) == SUCCESS)
						{

							//����ÿ�����������ļ�¼����ƴ��ʱ���ͶӰ

							data[nData] = (char*)malloc(totalLength * sizeof(char));

							for (int j = 0; j < currentResult->col_num; j++)
							{

								//����ÿһ�����ڸñ����

								memcpy(data[nData] + currentResult->offset[j], currentRec->pData + currentAttrs[j].offset, currentResult->length[j] * sizeof(char));

							}

							nData++;

						}

						//����ǰ���ɨ���¼���������

						AddResult(currentResult, nData, data);

						//���ˣ�ɨ��ƴ��ͶӰ���
						//��ʰ����

						for (int i = 0; i < nData; i++)
						{
							free(data[i]);
						}
						free(data);

						CloseScan(scanner);
						RM_CloseFile(relHandle);
						free(scanner);
						free(relHandle);
						free(cons);

					}
					else
					{

						//�ͷ���Դ�����ش�����Ϣ

						RM_CloseFile(relHandle);
						free(scanner);
						free(relHandle);
						free(cons);
						free(currentAttrs);
						free(singleResults);
						return rc;

					}

				}
				else
				{

					//�ͷ���Դ�����ش�����Ϣ

					free(relHandle);
					free(cons);
					free(currentAttrs);
					free(singleResults);
					return rc;

				}

			}
			else
			{

				//�ͷ���Դ�����ش�����Ϣ

				free(cons);
				free(currentAttrs);
				free(singleResults);
				return rc;

			}
		}
		else
		{

			//�ͷ���Դ�����ش�����Ϣ

			free(currentAttrs);
			free(singleResults);
			return rc;

		}

	}

	//���еѿ������ļ���

	SelResult* tmpResult = (SelResult*)malloc(sizeof(SelResult));
	memcpy(tmpResult, singleResults, sizeof(SelResult));
	memcpy(res, singleResults, sizeof(SelResult));
	for (int i = 1; i < nRelations; i++)
	{
		Join(tmpResult, singleResults + i, res);
		memcpy(tmpResult, res, sizeof(SelResult));
	}

	return SUCCESS;
}
