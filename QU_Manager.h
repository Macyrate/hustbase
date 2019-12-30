#ifndef __QUERY_MANAGER_H_
#define __QUERY_MANAGER_H_
#include "str.h"

typedef struct SelResult {
	int col_num;
	int row_num;
	AttrType type[20];	//��������ֶε���������
	int offset[20];		//��������ֶ��ڼ�¼�е�ƫ��	xmyadd20191224��Ϊ����AutoTest����һ��
	int length[20];		//��������ֶ�ֵ�ĳ���
	char fields[20][20];//����ʮ���ֶ���������ÿ���ֶεĳ��Ȳ�����20
	char** res[100];	//���һ������¼
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