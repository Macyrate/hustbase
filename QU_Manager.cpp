#include "StdAfx.h"
#include "QU_Manager.h"
#include "SYS_Manager.h"
#include "RM_Manager.h"

//指定表名，将对应的属性名称从混杂的selAttrs中分离出来，并得到其Offset
//TODO: 未测试
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

	//打开系统列文件

	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS)return rc;

	int i = 0;
	for (int i = 0; i < nInputSelAttrs; i++)
	{
		if (selAttrs[i]->relName != NULL && strcmp(selAttrs[i]->relName, relName) != 0)		//有表名且表名不符，跳过
			continue;

		//对表名符合的属性,在SYSCOLUMNS中扫描

		//扫描条件1：表名为relName
		checkerCons[0].bLhsIsAttr = 1;
		checkerCons[0].LattrLength = 21;
		checkerCons[0].LattrOffset = 0;
		checkerCons[0].compOp = CompOp::EQual;
		checkerCons[0].attrType = chars;
		checkerCons[0].bRhsIsAttr = 0;
		checkerCons[0].Rvalue = (void*)calloc(1, 21);
		strcpy((char*)checkerCons[0].Rvalue, relName);

		//扫描条件2：属性名为selAttrs[i]->attrName
		checkerCons[1].bLhsIsAttr = 1;
		checkerCons[1].LattrLength = 21;
		checkerCons[1].LattrOffset = 21;
		checkerCons[1].compOp = CompOp::EQual;
		checkerCons[1].bRhsIsAttr = 0;
		checkerCons[1].Rvalue = (void*)calloc(1, 21);
		strcpy((char*)checkerCons[1].Rvalue, selAttrs[i]->attrName);

		//扫描SYSCOLUMNS，获取属性对应记录
		rc = OpenScan(FileScan, hSyscolumns, 2, checkerCons);
		if (rc != SUCCESS)return rc;
		rc = GetNextRec(FileScan, syscolumnsRec);
		if (rc != SUCCESS)return rc;

		//提取属性信息
		strcpy(attrs[*nOutputAttrs].attrName, selAttrs[i]->attrName);						//提取属性名
		attrs[*nOutputAttrs].type = (AttrType) * (int*)(syscolumnsRec->pData + 42);			//提取属性类型
		attrs[*nOutputAttrs].size = *(int*)(syscolumnsRec->pData + 42 + sizeof(int));		//提取属性长度
		attrs[*nOutputAttrs].offset = *(int*)(syscolumnsRec->pData + 42 + sizeof(int) * 2);	//提取属性长度

		(*nOutputAttrs)++;

		rc = CloseScan(FileScan);
		if (rc != SUCCESS)return rc;
	}

	//收尾
	free(FileScan);

	rc = RM_CloseFile(hSyscolumns);
	if (rc != SUCCESS)return rc;
	free(hSyscolumns);

	//free(syscolumnsRec);
	free(checkerCons);

	return SUCCESS;
}

//用于朝记录集里添加若干条记录
//未测试
RC AddResult(SelResult* res, int nData, char** data)
{

	//寻找整条链的尾部

	SelResult* current = res;
	while (current->next_res != NULL)
	{
		current = current->next_res;
	}

	//计算每条记录的长度

	int totalLength = 0;
	for (int i = 0; i < current->col_num; i++)
	{
		totalLength += current->length[i];
	}

	//对于每一条要添加的记录

	for (int i = 0; i < nData; i++)
	{

		//若已经到达100条记录，则开辟新的SelResult

		if (current->row_num == 100)
		{
			SelResult* nextResult = (SelResult*)malloc(sizeof(SelResult));
			Init_Result(nextResult, current);
			current = nextResult;
		}

		//为current->res[current->row_num]开辟空间

		current->res[current->row_num] = (char**)malloc(sizeof(char*));
		*current->res[current->row_num] = (char*)malloc(totalLength * sizeof(char));

		//添加记录

		memcpy(*(current->res[current->row_num++]), data[i], totalLength * sizeof(char));

	}

	return SUCCESS;

}

//用于构建一个和对应表名与sel子句对应的空的SelResult*
//未测试
RC Init_Result(SelResult* res, char* relName, int nAttrs, Attr* attrs)
{

	RC rc;

	//成功拿到属于此表的属性
	//初始化SelResult

	res->col_num = nAttrs;
	res->row_num = 0;
	res->next_res = NULL;

	int currentOffset = 0;
	for (int i = 0; i < nAttrs; i++)
	{

		//对于每个列，填充信息

		strcpy(res->fields[i], attrs[i].attrName);
		res->length[i] = attrs[i].size;
		res->offset[i] = currentOffset;
		res->type[i] = attrs[i].type;

		currentOffset += res->length[i];

	}

	//返回成功

	return SUCCESS;

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

//计算两个表的笛卡尔积
//未测试
RC Join(SelResult* resA, SelResult* resB, SelResult* outRes)
{

	//拼接两个表的列，计算新的每个列的偏移量

	outRes->col_num = resA->col_num + resB->col_num;
	outRes->row_num = 0;

	for (int i = 0; i < resA->col_num; i++)
	{

		//向前a列填充resA的偏移量信息和列长度

		outRes->length[i] = resA->length[i];
		outRes->offset[i] = resA->offset[i];

	}

	int currentOffset = resA->offset[resA->col_num - 1] + resA->length[resA->col_num - 1];

	for (int i = 0; i < resB->col_num; i++)
	{

		//向后b列填充resB的长度信息，并计算应有的偏移量

		outRes->length[i + resA->col_num] = resB->length[i];
		outRes->offset[i + resA->col_num] = resB->offset[i] + currentOffset;

	}

	//至此，新的outRes结构构建完成
	//填充数据
	//声明用于储存临时行的字段

	char** data = (char**)malloc(MAX_SINGLE_REL_RES_NUM * sizeof(char*));
	int nData = 0;
	int totalLengthA = resA->length[resA->col_num - 1] + resA->offset[resA->col_num - 1];
	int totalLengthB = resB->length[resB->col_num - 1] + resB->offset[resB->col_num - 1];
	int totalLength = outRes->length[outRes->col_num - 1] + outRes->offset[outRes->col_num - 1];
	SelResult* currentA = resA;
	while (currentA != NULL)
	{

		//外层循环：顺序拿出resA整条链中所有有数据的链结

		for (int i = 0; i < currentA->row_num; i++)
		{

			SelResult* currentB = resB;
			while (currentB != NULL)
			{

				//内层循环：顺序拿出resB整条链中所有有数据的链结

				for (int j = 0; j < currentB->row_num; j++)
				{

					data[nData] = (char*)malloc(totalLength * sizeof(char));

					//将前半部填充为a中的数据

					memcpy(data[nData], *(currentA->res[i]), totalLengthA * sizeof(char));

					//将后半部填充为b中的数据

					memcpy(data[nData++], *(currentB->res[j]), totalLengthB * sizeof(char));

				}

				currentB = currentB->next_res;

			}

		}

		currentA = currentA->next_res;

	}

	//将数据扔进outRes中

	AddResult(outRes, nData, data);

	//收拾垃圾

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

//完成查询
//未测试
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

		rc = Select(sql_str->sstr.sel.nSelAttrs, sql_str->sstr.sel.selAttrs, sql_str->sstr.sel.nRelations, sql_str->sstr.sel.relations, sql_str->sstr.sel.nConditions, sql_str->sstr.sel.conditions, res);
		return rc;

	}
	else
	{

		//有语法错误或者不是select语句

		return SQL_SYNTAX;

	}
}

//支持多表的查询
//未测试
RC Select(int nSelAttrs, RelAttr** selAttrs, int nRelations, char** relations, int nConditions, Condition* conditions, SelResult* res)
{

	RC rc;

	//首先分表扫描，然后投影并拼接
	//声明一个用于储存各表扫描结果的数组

	SelResult* singleResults = (SelResult*)malloc(nRelations * sizeof(SelResult));

	for (int i = 0; i < nRelations; i++)
	{

		//当前表名

		char* currentRelName = relations[i];
		SelResult* currentResult = singleResults + i;;

		//初始化当前表的扫描结果集合

		int nAttrs = 0;
		Attr* currentAttrs = (Attr*)malloc(20 * sizeof(Attr));
		rc = GetAttrsByRelName(currentRelName, nSelAttrs, selAttrs, &nAttrs, currentAttrs);

		if (rc == SUCCESS)
		{

			Init_Result(currentResult, currentRelName, nAttrs, currentAttrs);

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

						char** data = (char**)malloc(MAX_SINGLE_REL_RES_NUM * sizeof(char*));
						int nData = 0;
						int totalLength = currentResult->offset[currentResult->col_num - 1] + currentResult->length[currentResult->col_num - 1];
						RM_Record* currentRec = (RM_Record*)malloc(sizeof(RM_Record));
						while (GetNextRec(scanner, currentRec) == SUCCESS)
						{

							//对于每条满足条件的记录，在拼接时完成投影

							data[nData] = (char*)malloc(totalLength * sizeof(char));

							for (int j = 0; j < currentResult->col_num; j++)
							{

								//对于每一个属于该表的列

								memcpy(data[nData] + currentResult->offset[j], currentRec->pData + currentAttrs[j].offset, currentResult->length[j] * sizeof(char));

							}

							nData++;

						}

						//朝当前表的扫描记录集合内添加

						AddResult(currentResult, nData, data);

						//至此，扫描拼接投影完成
						//收拾垃圾

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

						//释放资源并返回错误信息

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

					//释放资源并返回错误信息

					free(relHandle);
					free(cons);
					free(currentAttrs);
					free(singleResults);
					return rc;

				}

			}
			else
			{

				//释放资源，返回错误信息

				free(cons);
				free(currentAttrs);
				free(singleResults);
				return rc;

			}
		}
		else
		{

			//释放资源，返回错误信息

			free(currentAttrs);
			free(singleResults);
			return rc;

		}

	}

	//进行笛卡尔积的计算

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
