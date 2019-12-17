#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include "RM_Manager.h"
#include <iostream>
#include "string.h"

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
	//RM_CreateFile("abc", 32);
	RM_FileHandle* handle = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	RM_OpenFile("abc", handle);
	char dummyData[32] = "Hello, world!\0";
	char dummyData2[32] = "Hakurei Reimu\0";
	char dummyData3[32] = "Kochiya Sanae\0";
	RID* rid = (RID*)malloc(sizeof(RID));
	RC insertRC;
	for (int i = 0; i < 10; i++)
	{
		insertRC = InsertRec(handle, dummyData, rid);
	}
	free(rid);
	rid = (RID*)malloc(sizeof(RID));
	for (int i = 0; i < 10; i++)
	{
		insertRC = InsertRec(handle, dummyData2, rid);
	}
	free(rid);
	rid = (RID*)malloc(sizeof(RID));
	for (int i = 5; i < 11; i++)
	{
		RM_Record* rec = (RM_Record*)malloc(sizeof(RM_Record));
		rec->bValid = false;
		rec->pData = (char*)dummyData3;
		rec->rid.pageNum = 2;
		rec->rid.slotNum = i;
		UpdateRec(handle, rec);
		free(rec);
	}
	free(rid);
	RM_FileScan* scan = (RM_FileScan*)malloc(sizeof(RM_FileScan));
	OpenScan(scan, handle, 0, NULL);
	RC rc1 = SUCCESS;
	while (rc1 == SUCCESS)
	{
		RM_Record* rec = (RM_Record*)malloc(sizeof(RM_Record));
		rc1 = GetNextRec(scan, rec);
		AfxMessageBox(rec->pData);
		free(rec);
	}
	free(scan);
	RM_CloseFile(handle);
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

		case 3:
			//�ж�SQL���Ϊupdate���
			break;

		case 4:
			//�ж�SQL���Ϊdelete���
			break;

		case 5:
			//�ж�SQL���ΪcreateTable���
			break;

		case 6:
			//�ж�SQL���ΪdropTable���
			break;

		case 7:
			//�ж�SQL���ΪcreateIndex���
			break;

		case 8:
			//�ж�SQL���ΪdropIndex���
			break;

		case 9:
			//�ж�Ϊhelp��䣬���Ը���������ʾ
			break;

		case 10:
			//�ж�Ϊexit��䣬�����ɴ˽����˳�����
			break;
		}
	}
	else {
		AfxMessageBox(sql_str->sstr.errors);//���������sql���ʷ�����������Ϣ
		return rc;
	}
}

//������Щ��Ҫ��HustBase.cpp����ã�δ���
//�������ݿ�
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

bool CanButtonClick() {//��Ҫ����ʵ��
	//�����ǰ�����ݿ��Ѿ���
	return true;
	//�����ǰû�����ݿ��
	//return false;
}

//����һ����ΪrelName�ı�
//����attrCount��ʾ��ϵ�����Ե�������ȡֵΪ1��MAXATTRS֮�䣩��
//����attributes��һ������ΪattrCount�����顣
//�����¹�ϵ�е�i�����ԣ�attributes�����еĵ�i��Ԫ�ذ������ơ����ͺ����Եĳ��ȣ���AttrInfo�ṹ���壩��
RC CreateTable(char* relName, int attrCount, AttrInfo* attributes) {
	return SUCCESS;
}
