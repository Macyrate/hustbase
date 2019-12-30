#include "StdAfx.h"
#include "QU_Manager.h"

void Init_Result(SelResult* res)
{
	res->next_res = NULL;
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
	
}
