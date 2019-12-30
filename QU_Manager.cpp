#include "StdAfx.h"
#include "QU_Manager.h"
#include "SYS_Manager.h"
#include "RM_Manager.h"

//指定表名，将对应的属性名称从混杂的selAttrs中分离出来，并得到其Offset
//TODO: 等待老司机编写
RC GetAttrsByRelName(char* relName, int nInputSelAttrs, RelAttr* selAttrs, int nOutputAttrs, Attr* attrs)
{
	return SUCCESS;
}

//用于构建一个和对应表名与sel子句对应的空的SelResult*
//未测试
RC Init_Result(SelResult* res, char* relName, int nSelAttrs, RelAttr* selAttrs)
{

	RC rc;

	//获取属于relName的属性信息

	int nAttrs = 0;
	Attr* attrs = (Attr*)malloc(sizeof(Attr));
	rc = GetAttrsByRelName(relName, nSelAttrs, selAttrs, nAttrs, attrs);

	if (rc == SUCCESS)
	{

		//成功拿到属于此表的属性
		//初始化SelResult

		res->col_num = nAttrs;
		res->row_num = 0;
		res->next_res = NULL;

		for (int i = 0; i < nAttrs; i++)
		{

			//对于每个列，填充信息

			strcpy(res->fields[i], attrs[i].attrName);
			res->length[i] = attrs[i].size;
			res->offset[i] = attrs[i].offset;
			res->type[i] = attrs[i].type;

		}

		//释放资源，返回成功

		free(attrs);
		return SUCCESS;

	}
	else
	{

		//释放资源并返回错误信息

		free(attrs);
		return rc;

	}
}

//用于构建超过100条记录后的下一个SelResult
//未测试
RC Init_Result(SelResult* res, SelResult* father)
{

	father->next_res = res;
	res->col_num = father->col_num;

	//按照father填充child

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

		//TODO

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

	RC rc;

	//首先分表扫描，然后投影并拼接

	for (int i = 0; i < nRelations; i++)
	{

		//当前表名

		char* currentRelName = relations[i];

		//对于每一张表（关系）构造扫描条件

		Con* cons = (Con*)malloc(nConditions * sizeof(Con));
		rc = GetScanCons(currentRelName, nConditions, conditions, cons);

		if (rc == SUCCESS)
		{

			//成功构造扫描条件
			//使用条件进行扫描
			//打开对应的表文件

			RM_FileHandle* relHandle = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
			relHandle->bOpen = false;
			rc = RM_OpenFile(currentRelName, relHandle);

			if (rc == SUCCESS)
			{

				//打开表文件成功
				//构造并开启扫描句柄

				RM_FileScan* scanner = (RM_FileScan*)malloc(sizeof(RM_FileScan));
				scanner->bOpen = false;
				rc = OpenScan(scanner, relHandle, nConditions, cons);

				if (rc == SUCCESS)
				{

					//启动扫描成功
					//对当前表进行扫描拼接

					//TODO

				}
				else
				{

					//释放资源并返回错误信息

					free(scanner);
					free(relHandle);
					free(cons);
					return rc;

				}

			}
			else
			{

				//释放资源并返回错误信息

				free(relHandle);
				free(cons);
				return rc;

			}

		}
		else
		{

			//释放资源，返回错误信息

			free(cons);
			return rc;

		}

	}
	return SUCCESS;
}
