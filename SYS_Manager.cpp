#include "stdafx.h"
#include "EditArea.h"
#include "SYS_Manager.h"
#include "QU_Manager.h"
#include "RM_Manager.h"
#include <iostream>
#include "string.h"
#include "DebugLogger.h"

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
	//LogMessage("这是一条消息级Log");
	//LogMessage("这是一条错误级Log", LOG_ERROR);
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
			CreateTable(sql_str->sstr.cret.relName, sql_str->sstr.cret.attrCount, sql_str->sstr.cret.attributes);
			//pDoc->m_pTreeView->PopulateTree();	//更新视图
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
			AfxMessageBox("RTFM!");
			system("start https://gitee.com/strangenamebc/hustbase");
			break;

		case 10:
			//判断为exit语句，可以由此进行退出操作
			AfxGetMainWnd()->SendMessage(WM_CLOSE);		//关闭窗口
			break;
		}
	}
	else {
		AfxMessageBox(sql_str->sstr.errors);//弹出警告框，sql语句词法解析错误信息
		return rc;
	}
}

////////////////////////////////////////////////////////////////////////////
//创建数据库
//最后测试时间：2019/12/19 17:57
//最后测试状态：符合预期
//最后测试人：Macyrate
RC CreateDB(char* dbpath, char* dbname) {
	RC rc;
	SetCurrentDirectory(dbpath);//转到数据库目录
	char newdbpath[256];
	strcpy(newdbpath, dbpath);
	strcat(newdbpath, "\\");
	strcat(newdbpath, dbname);
	if (PathIsDirectoryA(newdbpath))	//如果要创建的数据库名已经存在，返回SQL_SYNTAX
		return SQL_SYNTAX;
	CreateDirectory(newdbpath, NULL);	//创建数据库文件夹
	SetCurrentDirectory(newdbpath);
	rc = RM_CreateFile("SYSTABLES", 25);	//创建SYSTABLES系统表文件，每条记录长度为21+4=25
	if (rc != SUCCESS)
		return SQL_SYNTAX;
	rc = RM_CreateFile("SYSCOLUMNS", 76);	//创建SYSCOLUMNS系统表文件，每条记录长度为21*3+4*3+1=76
	if (rc != SUCCESS)
		return SQL_SYNTAX;
	SetCurrentDirectory("..");	//操作完成，回到上级文件夹
	return SUCCESS;
}

//删除数据库
//最后测试时间：2019/12/17 10:11
//最后测试状态：符合预期
//最后测试人：Macyrate
RC DropDB(char* dbname) {
	RC rc;
	if (PathIsDirectoryA(dbname)) {		//判断文件夹是否存在
		char systablespath[256];
		strcpy(systablespath, dbname);
		if (PathFileExistsA(strcat(systablespath, "\\SYSTABLES"))) {	//通过文件夹里是否存在SYSTABLES判断是否是数据库文件夹
			char removestr[265] = "rd /s /q ";
			strcat(removestr, dbname);
			system(removestr);		//构造一条递归删除命令，删除数据库文件夹
			return SUCCESS;
		}
	}
	return SQL_SYNTAX;	//任意条件不满足，则返回SQL_SYNTAX
}

//打开数据库，未完成
RC OpenDB(char* dbname) {
	RC rc;
	if (PathIsDirectoryA(dbname)) {		//判断文件夹是否存在
		char systablespath[256];
		strcpy(systablespath, dbname);
		if (PathFileExistsA(strcat(systablespath, "\\SYSTABLES"))) {	//通过文件夹里是否存在SYSTABLES判断是否是数据库文件夹
			SetCurrentDirectory(dbname);	//切换工作目录到数据库文件夹
			return SUCCESS;
		}
	}
	return SQL_SYNTAX;
}

//关闭数据库，未完成
RC CloseDB() {
	//RC rc;
	//char* dbname[256];
	////GetCurrentDirectory();
	//if (PathIsDirectoryA(dbname)) {		//判断文件夹是否存在
	//	char systablespath[256];
	//	strcpy(systablespath, dbname);
	//	if (PathFileExistsA(strcat(systablespath, "\\SYSTABLES"))) {	//通过文件夹里是否存在SYSTABLES判断是否是数据库文件夹
	//		SetCurrentDirectory(dbname);	//切换工作目录到数据库文件夹
	//		return SUCCESS;
	//	}
	//}
	//return SQL_SYNTAX;
	return SUCCESS;
}

//创建一个名为relName的表。未完成
//参数attrCount表示关系中属性的数量（取值为1到MAXATTRS之间）。
//参数attributes是一个长度为attrCount的数组。
//对于新关系中第i个属性，attributes数组中的第i个元素包含名称、类型和属性的长度（见AttrInfo结构定义）。
RC CreateTable(char* relName, int attrCount, AttrInfo* attributes)
{
	//typedef struct _ AttrInfo AttrInfo;
	//struct _AttrInfo {
	//char			*attrName;			// 属性名
	//AttrType	attrType;			// 属性类型
	//int			attrLength;			// 属性长度
	//};

	RC rc;
	RM_FileHandle* hSystables, * hSyscolumns;
	RID* rid;
	int recordSize = 0;//每条记录的大小
	int* attrOffset = new int[attrCount];
	for (int i = 0; i < attrCount; i++) {
		attrOffset[i] = recordSize;		//预先计算属性的offset
		recordSize += (attributes + i)->attrLength;		//计算整条记录的长度
	}

	//打开系统表文件
	hSystables = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	hSystables->bOpen = false;
	hSyscolumns = (RM_FileHandle*)malloc(sizeof(RM_FileHandle));
	hSyscolumns->bOpen = false;
	rid = (RID*)malloc(sizeof(RID));
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

	char* pSystableRecord = (char*)malloc(sizeof(SysTable));		//构造SYSTABLES记录
	memcpy(pSystableRecord, relName, 21);							//填充表名
	memcpy(pSystableRecord + 21, &attrCount, sizeof(int));			//填充列数
	rc = InsertRec(hSystables, pSystableRecord, rid);
	if (rc != SUCCESS) {
		free(hSystables);
		return rc;
	}
	else {
		RM_CloseFile(hSystables);
		free(hSystables);
	}

	for (int i = 0; i < attrCount; i++) {
		char* pSyscolumnRecord = (char*)malloc(sizeof(SysColumn));									//构造SYSCOLUMNS记录
		memcpy(pSyscolumnRecord, relName, 21);														//填充表名
		memcpy(pSyscolumnRecord + 21, (attributes + i)->attrName, 21);								//填充属性名
		memcpy(pSyscolumnRecord + 42, &((attributes + i)->attrType), sizeof(int));					//填充属性类型
		memcpy(pSyscolumnRecord + 42 + sizeof(int), &(attributes + i)->attrLength, sizeof(int));	//填充属性长度
		memcpy(pSyscolumnRecord + 42 + 2 * sizeof(int), &attrOffset[i], sizeof(int));				//填充属性偏移量
		memcpy(pSyscolumnRecord + 42 + 2 * sizeof(int) + 1, "0", sizeof(char));						//填充索引标志
		rid = (RID*)malloc(sizeof(RID));
		rid->bValid = false;
		rc = InsertRec(hSyscolumns, pSystableRecord, rid);
		free(pSyscolumnRecord);
		free(rid);
		return rc;
	}
	RM_CloseFile(hSyscolumns);
	free(hSyscolumns);

	rc = RM_CreateFile(relName, recordSize);
	delete attrOffset;
	return rc;
}

bool CanButtonClick() {//需要重新实现
	//如果当前有数据库已经打开
	return true;
	//如果当前没有数据库打开
	//return false;
}