#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include "RM_Manager.h"
#include <iostream>
#include "string.h"

void ExecuteAndMessage(char* sql, CEditArea* editArea) {//根据执行的语句类型在界面上显示执行结果。此函数需修改
	std::string s_sql = sql;
	//if(s_sql.find("select") == 0){
	//	SelResult res;
	//	Init_Result(&res);
	//	//rc = Query(sql,&res);
	//	//将查询结果处理一下，整理成下面这种形式
	//	//调用editArea->ShowSelResult(col_num,row_num,fields,rows);
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
	/*--------------------以下代码为测试RM_Manager，无实际意义--------------------*/
	//RM_CreateFile("abc", 32);
	RM_FileHandle* handle = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	RM_OpenFile("abc", handle);
	char dummyData[32] = "Hello, world!\0";
	RID* rid = (RID*)malloc(sizeof(RID));
	RC insertRC;
	for (int i = 0; i < 200; i++)
	{
		insertRC = InsertRec(handle, dummyData, rid);
	}
	RM_CloseFile(handle);
	/*--------------------------------------------------------------------*/
	RC rc = execute(sql);
	int row_num = 0;
	char** messages;
	switch (rc) {
	case SUCCESS:
		row_num = 1;
		messages = new char* [row_num];
		messages[0] = "操作成功";
		editArea->ShowMessage(row_num, messages);
		delete[] messages;
		break;
	case SQL_SYNTAX:
		row_num = 1;
		messages = new char* [row_num];
		messages[0] = "有语法错误";
		editArea->ShowMessage(row_num, messages);
		delete[] messages;
		break;
	default:
		row_num = 1;
		messages = new char* [row_num];
		messages[0] = "功能未实现";
		editArea->ShowMessage(row_num, messages);
		delete[] messages;
		break;
	}
}

RC execute(char* sql) {
	sqlstr* sql_str = NULL;
	RC rc;
	sql_str = get_sqlstr();//为sql_str分配空间
	rc = parse(sql, sql_str);//语法分析，结果放在sql_str中。只有两种返回结果SUCCESS和SQL_SYNTAX（语法错误）

	if (rc == SUCCESS)
	{
		int i = 0;
		switch (sql_str->flag)
		{
			//case 1:
			////判断SQL语句为select语句

			//break;

		case 2:
			//判断SQL语句为insert语句

		case 3:
			//判断SQL语句为update语句
			break;

		case 4:
			//判断SQL语句为delete语句
			break;

		case 5:
			//判断SQL语句为createTable语句
			break;

		case 6:
			//判断SQL语句为dropTable语句
			break;

		case 7:
			//判断SQL语句为createIndex语句
			break;

		case 8:
			//判断SQL语句为dropIndex语句
			break;

		case 9:
			//判断为help语句，可以给出帮助提示
			break;

		case 10:
			//判断为exit语句，可以由此进行退出操作
			break;
		}
	}
	else {
		AfxMessageBox(sql_str->sstr.errors);//弹出警告框，sql语句词法解析错误信息
		return rc;
	}
}

//以下这些需要在HustBase.cpp里调用，未完成
RC CreateDB(char* dbpath, char* dbname) {
	RC rc;
	SetCurrentDirectory(dbpath);//转到数据库目录
	char newdbpath[256];
	strcpy(newdbpath, dbpath);
	strcat(newdbpath, "\\");
	strcat(newdbpath, dbname);
	CreateDirectory(newdbpath, NULL);
	SetCurrentDirectory(newdbpath);
	rc = RM_CreateFile("SYSTABLES", 25);//创建SYSTABLES系统表文件，每条记录长度为21+4=25
	if (rc != SUCCESS) {
		return SQL_SYNTAX;
	}
	rc = RM_CreateFile("SYSCOLUMNS", 76);//创建SYSCOLUMNS系统表文件，每条记录长度为21*3+4*3+1=76
	if (rc != SUCCESS) {
		return SQL_SYNTAX;
	}
	return SUCCESS;

}

RC DropDB(char* dbname) {
	return SUCCESS;
}

RC OpenDB(char* dbname) {
	return SUCCESS;
}


RC CloseDB() {
	return SUCCESS;
}

bool CanButtonClick() {//需要重新实现
	//如果当前有数据库已经打开
	return true;
	//如果当前没有数据库打开
	//return false;
}
