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

//未完成
RC Query(char* sql, SelResult* res)
{

	//储存语法分析结果

	sqlstr* sql_str = NULL;
	RC rc;

	//初始化sql_str

	sql_str = get_sqlstr();

	//语法分析

	rc = parse(sql, sql_str);

	if (rc == SUCCESS && sql_str->flag == 1)
	{

		//是合规的语法并且是select语句

		

	}
	else
	{

		//有语法错误或者不是select语句

		return SQL_SYNTAX;

	}
}

//未完成
RC Select(int nSelAttrs, RelAttr** selAttrs, int nRelations, char** relations, int nConditions, Condition* conditions, SelResult* res)
{
	
}
