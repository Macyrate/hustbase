#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include "RM_Manager.h"
#include "IX_Manager.h"
#include <iostream>
#include "string.h"
#include "DebugLogger.h"
#include <string>
#include <algorithm>

using namespace std;

void ExecuteAndMessage(char* sql, CEditArea* editArea) {//����ִ�е���������ڽ�������ʾִ�н�����˺������޸�
	std::string s_sql = sql;
	//if(s_sql.find("select") == 0){
	//	SelResult res;
	//	Init_Result(&res);
	//	//rc = Query(sql,&res);
	//	//����ѯ�������һ�£����������������ʽ
	//	//����editArea->ShowSelResult(col_num,row_num,fields,rows);
	//	int col_num = 5;
	//	int row_num = 3;
	//	char ** fields = new char *[5];
	//	for(int i = 0;i<col_num;i++){
	//		fields[i] = new char[20];
	//		memset(fields[i],0,20);
	//		fields[i][0] = 'f';
	//		fields[i][1] = i+'0';
	//	}
	//	char *** rows = new char**[row_num];
	//	for(int i = 0;i<row_num;i++){
	//		rows[i] = new char*[col_num];
	//		for(int j = 0;j<col_num;j++){
	//			rows[i][j] = new char[20];
	//			memset(rows[i][j],0,20);
	//			rows[i][j][0] = 'r';
	//			rows[i][j][1] = i + '0';
	//			rows[i][j][2] = '+';
	//			rows[i][j][3] = j + '0';
	//		}
	//	}
	//	editArea->ShowSelResult(col_num,row_num,fields,rows);
	//	for(int i = 0;i<5;i++){
	//		delete[] fields[i];
	//	}
	//	delete[] fields;
	//	Destory_Result(&res);
	//	return;
	//}
	/*--------------------���´���Ϊ����RM_Manager����ʵ������--------------------*/
	//LogMessage("����һ����Ϣ��Log");
	//LogMessage("����һ������Log", LOG_ERROR);
	//RM_CreateFile("abc", 32);
	//RM_FileHandle* handle = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	//RM_OpenFile("abc", handle);
	//char dummyData[32] = "Hello, world!\0";
	//char dummyData2[32] = "Hakurei Reimu\0";
	//char dummyData3[32] = "Kochiya Sanae\0";
	//RID* rid = (RID*)malloc(sizeof(RID));
	//RC insertRC;
	//for (int i = 0; i < 10; i++)
	//{
	//	insertRC = InsertRec(handle, dummyData, rid);
	//}
	//free(rid);
	//rid = (RID*)malloc(sizeof(RID));
	//for (int i = 0; i < 10; i++)
	//{
	//	insertRC = InsertRec(handle, dummyData2, rid);
	//}
	//free(rid);
	//rid = (RID*)malloc(sizeof(RID));
	//for (int i = 5; i < 11; i++)
	//{
	//	RM_Record* rec = (RM_Record*)malloc(sizeof(RM_Record));
	//	rec->bValid = false;
	//	rec->pData = (char*)dummyData3;
	//	rec->rid.pageNum = 2;
	//	rec->rid.slotNum = i;
	//	UpdateRec(handle, rec);
	//	free(rec);
	//}
	//free(rid);
	//RM_FileScan* scan = (RM_FileScan*)malloc(sizeof(RM_FileScan));
	//OpenScan(scan, handle, 0, NULL);
	//RC rc1 = SUCCESS;
	//while (rc1 == SUCCESS)
	//{
	//	RM_Record* rec = (RM_Record*)malloc(sizeof(RM_Record));
	//	rc1 = GetNextRec(scan, rec);
	//	AfxMessageBox(rec->pData);
	//	free(rec);
	//}
	//free(scan);
	//RM_CloseFile(handle);

	/*--------------------------------------------------------------------*/
	RC rc = execute(sql);
	int row_num = 0;
	char** messages;
	switch (rc) {
	case SUCCESS:
		row_num = 1;
		messages = new char* [row_num];
		messages[0] = "�����ɹ�";
		editArea->ShowMessage(row_num, messages);
		delete[] messages;
		break;
	case SQL_SYNTAX:
		row_num = 1;
		messages = new char* [row_num];
		messages[0] = "���﷨����";
		editArea->ShowMessage(row_num, messages);
		delete[] messages;
		break;
	default:
		row_num = 1;
		messages = new char* [row_num];
		messages[0] = "����δʵ��";
		editArea->ShowMessage(row_num, messages);
		delete[] messages;
		break;
	}
}

RC execute(char* sql) {
	sqlstr* sql_str = NULL;
	RC rc;
	sql_str = get_sqlstr();//Ϊsql_str����ռ�
	rc = parse(sql, sql_str);//�﷨�������������sql_str�С�ֻ�����ַ��ؽ��SUCCESS��SQL_SYNTAX���﷨����

	if (rc == SUCCESS)
	{
		int i = 0;
		switch (sql_str->flag)
		{
			//case 1:
			////�ж�SQL���Ϊselect���

			//break;

		case 2:
			//�ж�SQL���Ϊinsert���
			Insert(sql_str->sstr.ins.relName, sql_str->sstr.ins.nValues, sql_str->sstr.ins.values);
			break;
		case 3:
			//�ж�SQL���Ϊupdate���
			Update(sql_str->sstr.upd.relName, sql_str->sstr.upd.attrName, &sql_str->sstr.upd.value, sql_str->sstr.upd.nConditions, sql_str->sstr.upd.conditions);
			break;

		case 4:
			//�ж�SQL���Ϊdelete���
			Delete(sql_str->sstr.del.relName, sql_str->sstr.del.nConditions, sql_str->sstr.del.conditions);
			break;

		case 5:
			//�ж�SQL���ΪcreateTable���
			CreateTable(sql_str->sstr.cret.relName, sql_str->sstr.cret.attrCount, sql_str->sstr.cret.attributes);
			//pDoc->m_pTreeView->PopulateTree();	//������ͼ
			break;

		case 6:
			//�ж�SQL���ΪdropTable���
			DropTable(sql_str->sstr.drt.relName);
			break;

		case 7:
			//�ж�SQL���ΪcreateIndex���
			CreateIndex(sql_str->sstr.crei.indexName, sql_str->sstr.crei.relName, sql_str->sstr.crei.attrName);
			break;

		case 8:
			//�ж�SQL���ΪdropIndex���
			DropIndex(sql_str->sstr.dri.indexName);
			break;

		case 9:
			//�ж�Ϊhelp��䣬���Ը���������ʾ
			AfxMessageBox("RTFM!");
			system("start https://gitee.com/strangenamebc/hustbase");
			break;

		case 10:
			//�ж�Ϊexit��䣬�����ɴ˽����˳�����
			AfxGetMainWnd()->SendMessage(WM_CLOSE);		//�رմ���
			break;
		}
	}
	else {
		AfxMessageBox(sql_str->sstr.errors);//���������sql���ʷ�����������Ϣ
		return rc;
	}
}

////////////////////////////////////////////////////////////////////////////
//�������ݿ�
//������ʱ�䣺2019/12/19 17:57
//������״̬������Ԥ��
//�������ˣ�Macyrate
RC CreateDB(char* dbpath, char* dbname) {
	RC rc;
	SetCurrentDirectory(dbpath);//ת�����ݿ�Ŀ¼
	char newdbpath[256];
	strcpy(newdbpath, dbpath);
	strcat(newdbpath, "\\");
	strcat(newdbpath, dbname);
	if (PathIsDirectoryA(newdbpath))	//���Ҫ���������ݿ����Ѿ����ڣ�����SQL_SYNTAX
		return SQL_SYNTAX;
	CreateDirectory(newdbpath, NULL);	//�������ݿ��ļ���
	SetCurrentDirectory(newdbpath);
	rc = RM_CreateFile("SYSTABLES", 25);	//����SYSTABLESϵͳ���ļ���ÿ����¼����Ϊ21+4=25
	if (rc != SUCCESS)
		return SQL_SYNTAX;
	rc = RM_CreateFile("SYSCOLUMNS", 76);	//����SYSCOLUMNSϵͳ���ļ���ÿ����¼����Ϊ21*3+4*3+1=76
	if (rc != SUCCESS)
		return SQL_SYNTAX;
	SetCurrentDirectory("..");	//������ɣ��ص��ϼ��ļ���
	return SUCCESS;
}

//ɾ�����ݿ�
//������ʱ�䣺2019/12/17 10:11
//������״̬������Ԥ��
//�������ˣ�Macyrate
RC DropDB(char* dbname) {
	RC rc;
	if (PathIsDirectoryA(dbname)) {		//�ж��ļ����Ƿ����
		char systablespath[256];
		strcpy(systablespath, dbname);
		if (PathFileExistsA(strcat(systablespath, "\\SYSTABLES"))) {	//ͨ���ļ������Ƿ����SYSTABLES�ж��Ƿ������ݿ��ļ���
			char removestr[265] = "rd /s /q ";
			strcat(removestr, dbname);
			system(removestr);		//����һ���ݹ�ɾ�����ɾ�����ݿ��ļ���
			return SUCCESS;
		}
	}
	return SQL_SYNTAX;	//�������������㣬�򷵻�SQL_SYNTAX
}

//�����ݿ⣬δ���
RC OpenDB(char* dbname) {
	RC rc;
	if (PathIsDirectoryA(dbname)) {		//�ж��ļ����Ƿ����
		char systablespath[256];
		strcpy(systablespath, dbname);
		if (PathFileExistsA(strcat(systablespath, "\\SYSTABLES"))) {	//ͨ���ļ������Ƿ����SYSTABLES�ж��Ƿ������ݿ��ļ���
			SetCurrentDirectory(dbname);	//�л�����Ŀ¼�����ݿ��ļ���
			return SUCCESS;
		}
	}
	return SQL_SYNTAX;
}

//�ر����ݿ⣬����֪����ôд��
RC CloseDB() {
	//RC rc;
	//char* dbname[256];
	////GetCurrentDirectory();
	//if (PathIsDirectoryA(dbname)) {		//�ж��ļ����Ƿ����
	//	char systablespath[256];
	//	strcpy(systablespath, dbname);
	//	if (PathFileExistsA(strcat(systablespath, "\\SYSTABLES"))) {	//ͨ���ļ������Ƿ����SYSTABLES�ж��Ƿ������ݿ��ļ���
	//		SetCurrentDirectory(dbname);	//�л�����Ŀ¼�����ݿ��ļ���
	//		return SUCCESS;
	//	}
	//}
	//return SQL_SYNTAX;
	return SUCCESS;
}

//������ʱ�䣺2019/12/20 13:38
//������״̬������Ԥ��
//�������ˣ�Macyrate
//����һ����ΪrelName�ı�
//����attrCount��ʾ��ϵ�����Ե�������ȡֵΪ1��MAXATTRS֮�䣩��
//����attributes��һ������ΪattrCount�����顣
//�����¹�ϵ�е�i�����ԣ�attributes�����еĵ�i��Ԫ�ذ������ơ����ͺ����Եĳ��ȣ���AttrInfo�ṹ���壩��
RC CreateTable(char* relName, int attrCount, AttrInfo* attributes)
{
	//typedef struct _ AttrInfo AttrInfo;
	//struct _AttrInfo {
	//char			*attrName;			// ������
	//AttrType	attrType;			// ��������
	//int			attrLength;			// ���Գ���
	//};

	RC rc;
	RM_FileHandle* hSystables, * hSyscolumns;
	RID* rid;
	const int attrCountC = attrCount;
	int recordSize = 0;//ÿ����¼�Ĵ�С
	int* attrOffset = (int*)malloc(sizeof(int) * attrCount);
	for (int i = 0; i < attrCount; i++) {
		*(attrOffset + i) = recordSize;		//Ԥ�ȼ������Ե�offset
		recordSize += (attributes + i)->attrLength;		//����������¼�ĳ���
	}

	//��ϵͳ���ļ�
	hSystables = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	hSystables->bOpen = false;
	hSyscolumns = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	hSyscolumns->bOpen = false;
	rid = (RID*)calloc(1, sizeof(RID));
	rid->bValid = false;

	rc = RM_OpenFile("SYSTABLES", hSystables);
	if (rc != SUCCESS) {
		free(hSystables);
		return rc;
	}
	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS) {
		free(hSyscolumns);
		return rc;
	}

	char* pSystableRecord = (char*)calloc(1, sizeof(SysTable));		//����SYSTABLES��¼
	strcpy(pSystableRecord, relName);								//������
	memcpy(pSystableRecord + 21, &attrCount, sizeof(int));			//�������
	rc = InsertRec(hSystables, pSystableRecord, rid);				//��SYSTABLES�����¼
	if (rc != SUCCESS) return rc;
	rc = RM_CloseFile(hSystables);									//�ر��ļ�
	if (rc != SUCCESS) return rc;
	free(hSystables);
	free(rid);

	for (int i = 0; i < attrCount; i++) {
		char* pSyscolumnRecord = (char*)calloc(1, sizeof(SysColumn));									//����SYSCOLUMNS��¼
		strcpy(pSyscolumnRecord, relName);																//������
		strcpy(pSyscolumnRecord + 21, (attributes + i)->attrName);										//���������
		memcpy(pSyscolumnRecord + 42, &((attributes + i)->attrType), sizeof(int));						//�����������
		memcpy(pSyscolumnRecord + 42 + sizeof(int), &((attributes + i)->attrLength), sizeof(int));		//������Գ���
		memcpy(pSyscolumnRecord + 42 + 2 * sizeof(int), (attrOffset + i), sizeof(int));					//�������ƫ����
		memcpy(pSyscolumnRecord + 42 + 3 * sizeof(int), "0", sizeof(char));								//���������־
		rid = (RID*)calloc(1, sizeof(RID));
		rid->bValid = false;
		rc = InsertRec(hSyscolumns, pSyscolumnRecord, rid);		//��SYSCOLUMNS�����¼
		if (rc != SUCCESS) return rc;
		free(pSyscolumnRecord);
		free(rid);
	}
	RM_CloseFile(hSyscolumns);		//�ر��ļ�
	free(hSyscolumns);

	rc = RM_CreateFile(relName, recordSize);		//�������ݱ�
	free(attrOffset);
	return rc;
}

//������ʱ�䣺2019/12/23 16:20
//������״̬������Ԥ��
//�������ˣ�Macyrate
//ɾ������ΪrelName�����ݱ��Լ����ж�Ӧ������
RC DropTable(char* relName) {
	RC rc;
	RM_FileHandle* hSystables, * hSyscolumns;
	RM_FileScan* FileScan;

	//��Ҫ�����ı��ļ�����ȡ���
	hSystables = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hSystables->bOpen = false;
	hSyscolumns = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hSyscolumns->bOpen = false;
	rc = RM_OpenFile("SYSTABLES", hSystables);
	if (rc != SUCCESS) return rc;
	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS) return rc;

	//�����ݴ��������
	RM_Record* systablesRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	RM_Record* syscolumnsRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	systablesRec->bValid = false;
	syscolumnsRec->bValid = false;

	//��SYSTABLES����ɨ����������������ֱ�ӽ��б���
	FileScan = (RM_FileScan*)malloc(sizeof(RM_FileScan));
	FileScan->bOpen = false;
	rc = OpenScan(FileScan, hSystables, 0, NULL);
	if (rc != SUCCESS) return rc;

	//ѭ������SYSTABLES��"����"ΪrelName�ļ�¼,�������ɾ��
	while (GetNextRec(FileScan, systablesRec) == SUCCESS) {
		if (strcmp(relName, systablesRec->pData) == 0) {
			DeleteRec(hSystables, &(systablesRec->rid));
			break;
		}
	}
	CloseScan(FileScan);
	FileScan->bOpen = false;

	//��SYSCOLUMNS����ɨ����������������ֱ�ӽ��б���
	rc = OpenScan(FileScan, hSyscolumns, 0, NULL);
	if (rc != SUCCESS) return rc;

	//ѭ������SYSCOLUMNS��"����"ΪrelName�ļ�¼���������ɾ��
	while (GetNextRec(FileScan, syscolumnsRec) == SUCCESS) {
		if (strcmp(relName, syscolumnsRec->pData) == 0) {
			if (*((syscolumnsRec->pData) + 42 + 3 * sizeof(int)) == '1') {								//����Ƿ�������
				char* pIndexName = (syscolumnsRec->pData) + 42 + 3 * sizeof(int) + sizeof(char);		//��ȡ��������
				DropIndex(pIndexName);																	//ɾ������
			}
			DeleteRec(hSyscolumns, &(syscolumnsRec->rid));												//�ӱ���ɾ����¼
		}
	}
	CloseScan(FileScan);
	free(FileScan);

	//�ر��ļ�
	rc = RM_CloseFile(hSystables);
	if (rc != SUCCESS) return rc;
	rc = RM_CloseFile(hSyscolumns);
	if (rc != SUCCESS) return rc;

	//�ͷſռ�
	free(hSystables);
	free(hSyscolumns);
	free(systablesRec);
	free(syscolumnsRec);

	//ɾ�����ݱ��ļ�
	DeleteFile((LPCTSTR)relName);

	return SUCCESS;
}

//δ���
//�ú����ڹ�ϵrelName������attrName�ϴ�����ΪindexName��������
//�������ȼ���ڱ���������Ƿ��Ѿ�����һ��������������ڣ��򷵻�һ������Ĵ����롣���򣬴�����������
//���������Ĺ���������
//�ٴ������������ļ���
//�����ɨ�豻�����ļ�¼�����������ļ��в��������
//�۹ر�������
RC CreateIndex(char* indexName, char* relName, char* attrName) {
	return SUCCESS;
}

//��ɾ�������ļ��Ļ���������ɣ����޸�
//�ú�������ɾ����ΪindexName��������
//�������ȼ�������Ƿ���ڣ���������ڣ��򷵻�һ������Ĵ����롣�������ٸ�������
RC DropIndex(char* indexName) {
	if (DeleteFile((LPCTSTR)indexName))
		return SUCCESS;
	else
		return SQL_SYNTAX;
}

//���ڶ������б�ƫ������������
bool ColCmp(SysColumn& col1, SysColumn& col2) {
	return col1.attroffset < col2.attroffset;
}

//������ɣ�δ����
//�ú���������relName���в������ָ������ֵ����Ԫ�飬nValuesΪ����ֵ������valuesΪ��Ӧ������ֵ���顣
//�������ݸ���������ֵ����Ԫ�飬���ü�¼����ģ��ĺ��������Ԫ�飬Ȼ���ڸñ��ÿ��������Ϊ��Ԫ�鴴�����ʵ�������
RC Insert(char* relName, int nValues, Value* values) {
	RC rc;
	RM_FileHandle* hSystables, * hSyscolumns, * hTable;
	IX_IndexHandle* hIndex;
	RM_FileScan* FileScan;
	SysColumn* rgstSyscolumns;
	RID* rid;

	hSystables = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hSyscolumns = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hTable = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hIndex = (IX_IndexHandle*)calloc(1, sizeof(IX_IndexHandle));
	rid = (RID*)malloc(sizeof(RID));

	FileScan = (RM_FileScan*)calloc(1, sizeof(FileScan));
	FileScan->bOpen = false;

	rgstSyscolumns = (SysColumn*)calloc(nValues, sizeof(SysColumn));

	//��ϵͳ���ļ������ļ������ݱ��ļ�
	rc = RM_OpenFile("SYSTABLES", hSystables);
	if (rc != SUCCESS) return rc;
	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS) return rc;
	rc = RM_OpenFile(relName, hTable);
	if (rc != SUCCESS) return rc;
	hSystables->bOpen = false;
	hSyscolumns->bOpen = false;
	hTable->bOpen = false;

	//�����ݴ��������
	RM_Record* systablesRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	RM_Record* syscolumnsRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	RM_Record* tableRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	systablesRec->bValid = false;
	syscolumnsRec->bValid = false;
	tableRec->bValid = false;

	//���nValues�Ƿ���ȷ
	rc = OpenScan(FileScan, hSystables, 0, NULL);					//ɨ��ϵͳ���ļ�
	if (rc != SUCCESS) return rc;
	int columnsNum = 0;
	while (GetNextRec(FileScan, systablesRec) == SUCCESS) {
		if (strcmp(relName, syscolumnsRec->pData) == 0) {
			columnsNum = *(int*)((syscolumnsRec->pData) + 21);		//��ȡ��������
			break;
		}
	}
	FileScan->bOpen = false;										//�ر�ɨ��
	RM_CloseFile(hSystables);
	free(hSystables);

	if (columnsNum != nValues) return SQL_SYNTAX;					//��INSERT���������������������������

	//��ȡҪ����Ŀ�ı�������б�
	rc = OpenScan(FileScan, hSyscolumns, 0, NULL);
	if (rc != SUCCESS) return rc;
	int recordSize = 0;
	for (int i = 0; i < nValues && GetNextRec(FileScan, syscolumnsRec) == SUCCESS; i++) {
		memcpy((rgstSyscolumns + i)->tablename, syscolumnsRec->pData, 21);														//��ȡ����
		memcpy((rgstSyscolumns + i)->attrname, syscolumnsRec->pData + 21, 21);													//��ȡ������
		memcpy(&((rgstSyscolumns + i)->attrtype), syscolumnsRec->pData + 42, sizeof(int));										//��ȡ��������
		memcpy(&((rgstSyscolumns + i)->attrlength), syscolumnsRec->pData + 42 + sizeof(int), sizeof(int));													//��ȡ��������
		memcpy(&((rgstSyscolumns + i)->attroffset), syscolumnsRec->pData + 42 + sizeof(int) * 2, sizeof(int));					//��ȡ����ƫ����
		memcpy(&((rgstSyscolumns + i)->ix_flag), syscolumnsRec->pData + 42 + sizeof(int) * 3, sizeof(char));					//��ȡ������־
		memcpy((rgstSyscolumns + i)->indexname, syscolumnsRec->pData + 42 + sizeof(int) * 3 + sizeof(char), sizeof(char));		//��ȡ������־

		recordSize += (rgstSyscolumns + i)->attrlength;			//����Ҫ�����Ԫ���ܳ���
	}
	CloseScan(FileScan);
	free(FileScan);

	//���values�Ƿ���Ŀ�ı������б�һ�£�����Ԫ�飬���������¼
	std::sort((SysColumn*)rgstSyscolumns, (SysColumn*)rgstSyscolumns + nValues, ColCmp);				//��ƫ��������������ȡ�������б�
	char* columntoInsert = (char*)calloc(1, sizeof(char) * recordSize);
	for (int i; i < nValues; i++) {
		if ((values + i)->type != (rgstSyscolumns + i)->attrtype) return SQL_SYNTAX;					//������������Ƿ�һ��
		if ((values + i)->type == AttrType::chars) {													//��������������ַ���	
			if (strlen((char*)(values + i)->data) > (rgstSyscolumns + i)->attrlength)					//���Ҫ������ַ����Ƿ����
				return SQL_SYNTAX;
			memcpy(columntoInsert + (rgstSyscolumns + i)->attroffset, (values + i)->data, strlen((char*)(values + i)->data));		//�����ַ������ȵ�����
		}
		else
			memcpy(columntoInsert + (rgstSyscolumns + i)->attroffset, (values + i)->data, (rgstSyscolumns + i)->attrlength);		//��������

		rid->bValid = false;
		rc = InsertRec(hTable, columntoInsert, rid);				//�����¼
		if (rc != SUCCESS)return rc;

		if ((rgstSyscolumns + i)->ix_flag == '0')					//���������Դ�����
			rc = OpenIndex((rgstSyscolumns + i)->indexname, hIndex);
		if (rc != SUCCESS)return rc;
		rc = InsertEntry(hIndex, columntoInsert, rid);				//����������
		if (rc != SUCCESS)return rc;
	}

	free(rid);
	free(rgstSyscolumns);
	free(columntoInsert);

	rc = RM_CloseFile(hSyscolumns);			//�ر��ļ�
	if (rc != SUCCESS)return rc;
	rc = RM_CloseFile(hTable);
	if (rc != SUCCESS)return rc;
	rc = CloseIndex(hIndex);
	if (rc != SUCCESS)return rc;

	free(hSyscolumns);						//�ͷž��
	free(hTable);
	free(hIndex);

	return SUCCESS;
}

//δ���
//�ú�������ɾ��relName������������ָ��������Ԫ���Լ���Ԫ���Ӧ�������
//���û��ָ����������˷���ɾ��relName��ϵ������Ԫ�顣
//��������������������Щ����֮��Ϊ���ϵ��
RC Delete(char* relName, int nConditions, Condition* conditions) {
	return SUCCESS;
}

//δ���
//�ú������ڸ���relName������������ָ��������Ԫ�飬��ÿһ�����µ�Ԫ���н�����attrName��ֵ����Ϊһ���µ�ֵ��
//���û��ָ����������˷�������relName������Ԫ�顣
//���Ҫ����һ�������������ԣ�Ӧ����ɾ��ÿ��������Ԫ���Ӧ��������Ŀ��Ȼ�����һ���µ�������Ŀ��
RC Update(char* relName, char* attrName, Value* Value, int nConditions, Condition* conditions) {
	return SUCCESS;
}


bool CanButtonClick() {//��Ҫ����ʵ��
	//�����ǰ�����ݿ��Ѿ���
	return true;
	//�����ǰû�����ݿ��
	//return false;
}