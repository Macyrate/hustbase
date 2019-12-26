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
			Insert(sql_str->sstr.ins.relName, sql_str->sstr.ins.nValues, sql_str->sstr.ins.values);
			break;
		case 3:
			//判断SQL语句为update语句
			Update(sql_str->sstr.upd.relName, sql_str->sstr.upd.attrName, &sql_str->sstr.upd.value, sql_str->sstr.upd.nConditions, sql_str->sstr.upd.conditions);
			break;

		case 4:
			//判断SQL语句为delete语句
			Delete(sql_str->sstr.del.relName, sql_str->sstr.del.nConditions, sql_str->sstr.del.conditions);
			break;

		case 5:
			//判断SQL语句为createTable语句
			CreateTable(sql_str->sstr.cret.relName, sql_str->sstr.cret.attrCount, sql_str->sstr.cret.attributes);
			//pDoc->m_pTreeView->PopulateTree();	//更新视图
			break;

		case 6:
			//判断SQL语句为dropTable语句
			DropTable(sql_str->sstr.drt.relName);
			break;

		case 7:
			//判断SQL语句为createIndex语句
			CreateIndex(sql_str->sstr.crei.indexName, sql_str->sstr.crei.relName, sql_str->sstr.crei.attrName);
			break;

		case 8:
			//判断SQL语句为dropIndex语句
			DropIndex(sql_str->sstr.dri.indexName);
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

//关闭数据库，并不知道怎么写好
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

//最后测试时间：2019/12/20 13:38
//最后测试状态：符合预期
//最后测试人：Macyrate
//创建一个名为relName的表。
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
	const int attrCountC = attrCount;
	int recordSize = 0;//每条记录的大小
	int* attrOffset = (int*)malloc(sizeof(int) * attrCount);
	for (int i = 0; i < attrCount; i++) {
		*(attrOffset + i) = recordSize;		//预先计算属性的offset
		recordSize += (attributes + i)->attrLength;		//计算整条记录的长度
	}

	//打开系统表文件
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

	char* pSystableRecord = (char*)calloc(1, sizeof(SysTable));		//构造SYSTABLES记录
	strcpy(pSystableRecord, relName);								//填充表名
	memcpy(pSystableRecord + 21, &attrCount, sizeof(int));			//填充列数
	rc = InsertRec(hSystables, pSystableRecord, rid);				//向SYSTABLES插入记录
	if (rc != SUCCESS) return rc;
	rc = RM_CloseFile(hSystables);									//关闭文件
	if (rc != SUCCESS) return rc;
	free(hSystables);
	free(rid);

	for (int i = 0; i < attrCount; i++) {
		char* pSyscolumnRecord = (char*)calloc(1, sizeof(SysColumn));									//构造SYSCOLUMNS记录
		strcpy(pSyscolumnRecord, relName);																//填充表名
		strcpy(pSyscolumnRecord + 21, (attributes + i)->attrName);										//填充属性名
		memcpy(pSyscolumnRecord + 42, &((attributes + i)->attrType), sizeof(int));						//填充属性类型
		memcpy(pSyscolumnRecord + 42 + sizeof(int), &((attributes + i)->attrLength), sizeof(int));		//填充属性长度
		memcpy(pSyscolumnRecord + 42 + 2 * sizeof(int), (attrOffset + i), sizeof(int));					//填充属性偏移量
		memcpy(pSyscolumnRecord + 42 + 3 * sizeof(int), "0", sizeof(char));								//填充索引标志
		rid = (RID*)calloc(1, sizeof(RID));
		rid->bValid = false;
		rc = InsertRec(hSyscolumns, pSyscolumnRecord, rid);		//向SYSCOLUMNS插入记录
		if (rc != SUCCESS) return rc;
		free(pSyscolumnRecord);
		free(rid);
	}
	RM_CloseFile(hSyscolumns);		//关闭文件
	free(hSyscolumns);

	rc = RM_CreateFile(relName, recordSize);		//创建数据表
	free(attrOffset);
	return rc;
}

//最后测试时间：2019/12/23 16:20
//最后测试状态：符合预期
//最后测试人：Macyrate
//删除表名为relName的数据表，以及所有对应的索引
RC DropTable(char* relName) {
	RC rc;
	RM_FileHandle* hSystables, * hSyscolumns;
	RM_FileScan* FileScan;

	//打开要操作的表文件，获取句柄
	hSystables = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hSystables->bOpen = false;
	hSyscolumns = (RM_FileHandle*)calloc(1, sizeof(RM_FileHandle));
	hSyscolumns->bOpen = false;
	rc = RM_OpenFile("SYSTABLES", hSystables);
	if (rc != SUCCESS) return rc;
	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS) return rc;

	//构造暂存搜索结果
	RM_Record* systablesRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	RM_Record* syscolumnsRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	systablesRec->bValid = false;
	syscolumnsRec->bValid = false;

	//对SYSTABLES启动扫描器，不加条件以直接进行遍历
	FileScan = (RM_FileScan*)malloc(sizeof(RM_FileScan));
	FileScan->bOpen = false;
	rc = OpenScan(FileScan, hSystables, 0, NULL);
	if (rc != SUCCESS) return rc;

	//循环查找SYSTABLES中"表名"为relName的记录,对其进行删除
	while (GetNextRec(FileScan, systablesRec) == SUCCESS) {
		if (strcmp(relName, systablesRec->pData) == 0) {
			DeleteRec(hSystables, &(systablesRec->rid));
			break;
		}
	}
	CloseScan(FileScan);
	FileScan->bOpen = false;

	//对SYSCOLUMNS启动扫描器，不加条件以直接进行遍历
	rc = OpenScan(FileScan, hSyscolumns, 0, NULL);
	if (rc != SUCCESS) return rc;

	//循环查找SYSCOLUMNS中"表名"为relName的记录，对其进行删除
	while (GetNextRec(FileScan, syscolumnsRec) == SUCCESS) {
		if (strcmp(relName, syscolumnsRec->pData) == 0) {
			if (*((syscolumnsRec->pData) + 42 + 3 * sizeof(int)) == '1') {								//检查是否有索引
				char* pIndexName = (syscolumnsRec->pData) + 42 + 3 * sizeof(int) + sizeof(char);		//提取索引名称
				DropIndex(pIndexName);																	//删除索引
			}
			DeleteRec(hSyscolumns, &(syscolumnsRec->rid));												//从表中删除记录
		}
	}
	CloseScan(FileScan);
	free(FileScan);

	//关闭文件
	rc = RM_CloseFile(hSystables);
	if (rc != SUCCESS) return rc;
	rc = RM_CloseFile(hSyscolumns);
	if (rc != SUCCESS) return rc;

	//释放空间
	free(hSystables);
	free(hSyscolumns);
	free(systablesRec);
	free(syscolumnsRec);

	//删除数据表文件
	DeleteFile((LPCTSTR)relName);

	return SUCCESS;
}

//未完成
//该函数在关系relName的属性attrName上创建名为indexName的索引。
//函数首先检查在标记属性上是否已经存在一个索引，如果存在，则返回一个非零的错误码。否则，创建该索引。
//创建索引的工作包括：
//①创建并打开索引文件；
//②逐个扫描被索引的记录，并向索引文件中插入索引项；
//③关闭索引。
RC CreateIndex(char* indexName, char* relName, char* attrName) {
	return SUCCESS;
}

//仅删除索引文件的基础暴力完成，待修改
//该函数用来删除名为indexName的索引。
//函数首先检查索引是否存在，如果不存在，则返回一个非零的错误码。否则，销毁该索引。
RC DropIndex(char* indexName) {
	if (DeleteFile((LPCTSTR)indexName))
		return SUCCESS;
	else
		return SQL_SYNTAX;
}

//用于对属性列表按偏移量升序排列
bool ColCmp(SysColumn& col1, SysColumn& col2) {
	return col1.attroffset < col2.attroffset;
}

//基本完成，未测试
//该函数用来在relName表中插入具有指定属性值的新元组，nValues为属性值个数，values为对应的属性值数组。
//函数根据给定的属性值构建元组，调用记录管理模块的函数插入该元组，然后在该表的每个索引中为该元组创建合适的索引项
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

	//打开系统表文件、列文件和数据表文件
	rc = RM_OpenFile("SYSTABLES", hSystables);
	if (rc != SUCCESS) return rc;
	rc = RM_OpenFile("SYSCOLUMNS", hSyscolumns);
	if (rc != SUCCESS) return rc;
	rc = RM_OpenFile(relName, hTable);
	if (rc != SUCCESS) return rc;
	hSystables->bOpen = false;
	hSyscolumns->bOpen = false;
	hTable->bOpen = false;

	//构造暂存搜索结果
	RM_Record* systablesRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	RM_Record* syscolumnsRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	RM_Record* tableRec = (RM_Record*)calloc(1, sizeof(RM_Record));
	systablesRec->bValid = false;
	syscolumnsRec->bValid = false;
	tableRec->bValid = false;

	//检查nValues是否正确
	rc = OpenScan(FileScan, hSystables, 0, NULL);					//扫描系统表文件
	if (rc != SUCCESS) return rc;
	int columnsNum = 0;
	while (GetNextRec(FileScan, systablesRec) == SUCCESS) {
		if (strcmp(relName, syscolumnsRec->pData) == 0) {
			columnsNum = *(int*)((syscolumnsRec->pData) + 21);		//提取属性数量
			break;
		}
	}
	FileScan->bOpen = false;										//关闭扫描
	RM_CloseFile(hSystables);
	free(hSystables);

	if (columnsNum != nValues) return SQL_SYNTAX;					//若INSERT语句中列数与表的列数不符，报错

	//提取要插入目的表的属性列表
	rc = OpenScan(FileScan, hSyscolumns, 0, NULL);
	if (rc != SUCCESS) return rc;
	int recordSize = 0;
	for (int i = 0; i < nValues && GetNextRec(FileScan, syscolumnsRec) == SUCCESS; i++) {
		memcpy((rgstSyscolumns + i)->tablename, syscolumnsRec->pData, 21);														//提取表名
		memcpy((rgstSyscolumns + i)->attrname, syscolumnsRec->pData + 21, 21);													//提取属性名
		memcpy(&((rgstSyscolumns + i)->attrtype), syscolumnsRec->pData + 42, sizeof(int));										//提取属性类型
		memcpy(&((rgstSyscolumns + i)->attrlength), syscolumnsRec->pData + 42 + sizeof(int), sizeof(int));													//提取属性类型
		memcpy(&((rgstSyscolumns + i)->attroffset), syscolumnsRec->pData + 42 + sizeof(int) * 2, sizeof(int));					//提取属性偏移量
		memcpy(&((rgstSyscolumns + i)->ix_flag), syscolumnsRec->pData + 42 + sizeof(int) * 3, sizeof(char));					//提取索引标志
		memcpy((rgstSyscolumns + i)->indexname, syscolumnsRec->pData + 42 + sizeof(int) * 3 + sizeof(char), sizeof(char));		//提取索引标志

		recordSize += (rgstSyscolumns + i)->attrlength;			//计算要构造的元组总长度
	}
	CloseScan(FileScan);
	free(FileScan);

	//检查values是否与目的表属性列表一致，构造元组，添加索引记录
	std::sort((SysColumn*)rgstSyscolumns, (SysColumn*)rgstSyscolumns + nValues, ColCmp);				//按偏移量升序整理提取的属性列表
	char* columntoInsert = (char*)calloc(1, sizeof(char) * recordSize);
	for (int i; i < nValues; i++) {
		if ((values + i)->type != (rgstSyscolumns + i)->attrtype) return SQL_SYNTAX;					//检查属性类型是否一致
		if ((values + i)->type == AttrType::chars) {													//如果属性类型是字符串	
			if (strlen((char*)(values + i)->data) > (rgstSyscolumns + i)->attrlength)					//检查要插入的字符串是否过长
				return SQL_SYNTAX;
			memcpy(columntoInsert + (rgstSyscolumns + i)->attroffset, (values + i)->data, strlen((char*)(values + i)->data));		//复制字符串长度的内容
		}
		else
			memcpy(columntoInsert + (rgstSyscolumns + i)->attroffset, (values + i)->data, (rgstSyscolumns + i)->attrlength);		//复制内容

		rid->bValid = false;
		rc = InsertRec(hTable, columntoInsert, rid);				//插入记录
		if (rc != SUCCESS)return rc;

		if ((rgstSyscolumns + i)->ix_flag == '0')					//有索引则尝试打开索引
			rc = OpenIndex((rgstSyscolumns + i)->indexname, hIndex);
		if (rc != SUCCESS)return rc;
		rc = InsertEntry(hIndex, columntoInsert, rid);				//插入索引项
		if (rc != SUCCESS)return rc;
	}

	free(rid);
	free(rgstSyscolumns);
	free(columntoInsert);

	rc = RM_CloseFile(hSyscolumns);			//关闭文件
	if (rc != SUCCESS)return rc;
	rc = RM_CloseFile(hTable);
	if (rc != SUCCESS)return rc;
	rc = CloseIndex(hIndex);
	if (rc != SUCCESS)return rc;

	free(hSyscolumns);						//释放句柄
	free(hTable);
	free(hIndex);

	return SUCCESS;
}

//未完成
//该函数用来删除relName表中所有满足指定条件的元组以及该元组对应的索引项。
//如果没有指定条件，则此方法删除relName关系中所有元组。
//如果包含多个条件，则这些条件之间为与关系。
RC Delete(char* relName, int nConditions, Condition* conditions) {
	return SUCCESS;
}

//未完成
//该函数用于更新relName表中所有满足指定条件的元组，在每一个更新的元组中将属性attrName的值设置为一个新的值。
//如果没有指定条件，则此方法更新relName中所有元组。
//如果要更新一个被索引的属性，应当先删除每个被更新元组对应的索引条目，然后插入一个新的索引条目。
RC Update(char* relName, char* attrName, Value* Value, int nConditions, Condition* conditions) {
	return SUCCESS;
}


bool CanButtonClick() {//需要重新实现
	//如果当前有数据库已经打开
	return true;
	//如果当前没有数据库打开
	//return false;
}