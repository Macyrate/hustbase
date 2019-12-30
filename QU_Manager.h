#ifndef __QUERY_MANAGER_H_
#define __QUERY_MANAGER_H_
#include "str.h"

typedef struct SelResult {
	int col_num;
	int row_num;
	AttrType type[20];	//结果集各字段的数据类型
	int offset[20];		//结果集各字段在记录中的偏移	xmyadd20191224，为了与AutoTest保持一致
	int length[20];		//结果集各字段值的长度
	char fields[20][20];//最多二十个字段名，而且每个字段的长度不超过20
	char** res[100];	//最多一百条记录
	SelResult* next_res;
}SelResult;

typedef struct
{
	char attrName[21];
	AttrType type;
	int size;
	int offset;
} Attr;

RC GetAttrsByRelName(char* relName, int nInputSelAttrs, RelAttr* selAttrs, int nOutputAttrs, Attr* attrs);
RC AddResult(SelResult* res, int nData, char** data);
RC Init_Result(SelResult* res, char* relName, int nSelAttrs, RelAttr* selAttrs);
RC Init_Result(SelResult* res, SelResult* father);

void Destory_Result(SelResult* res);

RC Query(char* sql, SelResult* res);

RC Select(int nSelAttrs, RelAttr** selAttrs, int nRelations, char** relations, int nConditions, Condition* conditions, SelResult* res);
#endif