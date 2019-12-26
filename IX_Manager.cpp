#include "stdafx.h"
#include "IX_Manager.h"

int threshold = 0;

/**
 * 此函数创建一个名为fileName的索引。
 * attrType描述被索引属性的类型，attrLength描述被索引属性的长度。
 */
RC CreateIndex(char* fileName, AttrType attrType, int attrLength) {
	PF_FileHandle* fileHandle = NULL;
	PageNum pageNum;
	//创建索引文件
	CreateFile(fileName);
	fileHandle = (PF_FileHandle*)malloc(sizeof(PF_FileHandle));
	//打开索引文件
	openFile(fileName, fileHandle);

	PF_PageHandle* firstPageHandle = NULL;
	firstPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	//分配索引文件的第一个页面
	AllocatePage(fileHandle, firstPageHandle);
	GetPageNum(firstPageHandle, &pageNum);

	//生成索引控制信息
	IX_FileHeader index_FileHeader;
	index_FileHeader.attrLength = attrLength;
	index_FileHeader.keyLength = attrLength + sizeof(RID);
	index_FileHeader.attrType = attrType;
	index_FileHeader.rootPage = pageNum;
	index_FileHeader.first_leaf = pageNum;
	int order = (PF_PAGE_SIZE - (sizeof(IX_FileHeader) + sizeof(IX_Node))) /
		(2 * sizeof(RID) + attrLength);
	index_FileHeader.order = order;
	threshold = order >> 1;
	//获取第一页的数据区
	char* pData;
	GetData(firstPageHandle, &pData);
	memcpy(pData, &index_FileHeader, sizeof(IX_FileHeader));

	//初始化节点控制信息，将根节点的置为叶子节点，关键字数为0，
	IX_Node index_NodeControl;
	index_NodeControl.keynum = 0;
	index_NodeControl.parent = 0;
	index_NodeControl.brother = 0;
	index_NodeControl.is_leaf = 1;
	memcpy(pData + sizeof(IX_FileHeader), &index_NodeControl, sizeof(IX_Node));
	MarkDirty(firstPageHandle);

	UnpinPage(firstPageHandle);
	//关闭索引文件
	CloseFile(fileHandle);
	free(firstPageHandle);
	free(fileHandle);

	return SUCCESS;
}

RC OpenIndex(const char* fileName, IX_IndexHandle* indexHandle) {
	RC rc;
	PF_FileHandle pfFileHandle;
	PF_PageHandle pfPageHandle;
	char* data;

	memset(indexHandle, 0, sizeof(IX_IndexHandle));

	if ((rc = openFile((char*)fileName, &pfFileHandle)) ||
		(rc = GetThisPage(&pfFileHandle, 1, &pfPageHandle))) {
		return rc;
	}

	indexHandle->bOpen = true;
	if (rc = GetData(&pfPageHandle, &data)) {
		return rc;
	}
	indexHandle->fileHeader = *(IX_FileHeader*)data;
	indexHandle->fileHandle = pfFileHandle;
	if (rc = UnpinPage(&pfPageHandle)) {
		return rc;
	}

	return SUCCESS;
}

// TODO: 可能存在bug
//关闭索引文件
RC CloseIndex(IX_IndexHandle* indexHandle) {
	PF_FileHandle fileHandle = indexHandle->fileHandle;
	CloseFile(&fileHandle);
	return SUCCESS;
}

//索引的插入
RC InsertEntry(IX_IndexHandle* indexHandle, void* pData, RID* rid) {
	PF_FileHandle fileHandle = indexHandle->fileHandle;
	IX_FileHeader fileHeader = indexHandle->fileHeader;
	PF_PageHandle* pageHandle = NULL;
	//获取根节点页面
	pageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	pageHandle->bOpen = false;
	GetThisPage(&fileHandle, fileHeader.rootPage, pageHandle);
	//获取根节点页面的数据区
	char* pageData;
	GetData(pageHandle, &pageData);
	//获取根节点页面得节点控制信息
	IX_Node* index_NodeControlInfo =
		(IX_Node*)(pageData + sizeof(IX_FileHeader));
	//索引文件页面的序数
	int order = fileHeader.order;
	//索引关键字的长度
	int attrLength = fileHeader.attrLength;

	//判断节点如果是叶子节点
	while (index_NodeControlInfo->is_leaf != 1) {
		RID tempRid;
		setInfoToPage(pageHandle, order, fileHeader.attrType, fileHeader.attrLength,
			pData, &tempRid, false);  //查找将要插入关键字的页面
		GetThisPage(&fileHandle, tempRid.pageNum, pageHandle);
		GetData(pageHandle, &pageData);
		index_NodeControlInfo = (IX_Node*)(pageData + sizeof(IX_FileHeader));
	}
	setInfoToPage(pageHandle, order, fileHeader.attrType, fileHeader.attrLength,
		pData, rid, true);
	while (index_NodeControlInfo->keynum == order) {
		int keynum = index_NodeControlInfo->keynum;
		//获取关键字区
		char* keys = pageData + sizeof(IX_FileHeader) + sizeof(IX_Node);
		//获取指针区
		char* rids = keys + order * attrLength;

		PageNum nodePage;
		GetPageNum(pageHandle, &nodePage);

		//新叶子节点页面
		PF_PageHandle* newLeafPageHandle = NULL;
		newLeafPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
		newLeafPageHandle->bOpen = false;
		AllocatePage(&fileHandle, newLeafPageHandle);
		PageNum newLeafPage;
		GetPageNum(newLeafPageHandle, &newLeafPage);

		int divide1 = keynum >> 1;
		int divide2 = keynum - divide1;

		if (index_NodeControlInfo->parent == 0)
		{
			//生成新的根页面
			PF_PageHandle* newRootPageHandle = NULL;
			newRootPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
			newRootPageHandle->bOpen = false;
			AllocatePage(&fileHandle, newRootPageHandle);
			PageNum newRootPage;
			GetPageNum(newRootPageHandle, &newRootPage);
			cpNewToPage(newRootPageHandle, 0, 0, 0, 0);  //设置新根节点的节点控制信息

			cpNewToPage(newLeafPageHandle, index_NodeControlInfo->brother,
				newRootPage, index_NodeControlInfo->is_leaf,
				divide2);  //设置新分裂节点的节点控制信息
			cpNewToPage(pageHandle, newLeafPage, newRootPage,
				index_NodeControlInfo->is_leaf,
				divide1);  //设置原节点控制信息

			cpInfoToPage(
				newLeafPageHandle, keys + divide1 * attrLength, attrLength, divide2,
				order,
				rids + divide1 * sizeof(RID));  //复制关键字和指针到分裂后新的页面中

			char* tempData;
			GetData(pageHandle, &tempData);
			RID tempRid;
			tempRid.bValid = false;
			tempRid.pageNum = nodePage;
			setInfoToPage(newRootPageHandle, order, fileHeader.attrType,
				fileHeader.attrLength, tempData, &tempRid,
				true);  //向新的根节点插入子节点的关键字和指针

			GetData(newLeafPageHandle, &tempData);
			tempRid.pageNum = newLeafPage;
			setInfoToPage(newRootPageHandle, order, fileHeader.attrType,
				fileHeader.attrLength, tempData, &tempRid,
				true);  //向新的根节点插入子节点的关键字和指针

			indexHandle->fileHeader.rootPage =
				newRootPage;  //修改索引控制信息中的根节点页面
			free(newRootPageHandle);
		}
		else  //说明当前分裂的节点不是根节点
		{
			PageNum parentPage = index_NodeControlInfo->parent;
			cpNewToPage(newLeafPageHandle, nodePage, parentPage,
				index_NodeControlInfo->is_leaf,
				divide2);  //设置新分裂节点的节点控制信息
			cpNewToPage(pageHandle, newLeafPage, parentPage,
				index_NodeControlInfo->is_leaf,
				divide1);  //设置原节点控制信息
			cpInfoToPage(
				newLeafPageHandle, keys + divide1 * attrLength, attrLength, divide2,
				order,
				rids + divide1 * sizeof(RID));  //复制关键字和指针到分裂后新的页面中

			char* tempData;
			GetData(newLeafPageHandle, &tempData);

			RID tempRid;
			tempRid.bValid = false;
			tempRid.pageNum = newLeafPage;

			GetThisPage(&fileHandle, parentPage,
				pageHandle);  //令pageHandle指向其父节点页面
			setInfoToPage(pageHandle, order, fileHeader.attrType,
				fileHeader.attrLength, tempData, &tempRid,
				true);  //向父节点插入新子节点的关键字和指针

			GetData(pageHandle, &pageData);  //令pageData指向父节点的数据区
			index_NodeControlInfo =
				(IX_Node*)(pageData + sizeof(IX_FileHeader));  //父节点的节点控制信息
		}
		free(newLeafPageHandle);
	}

	free(pageHandle);

	return SUCCESS;
}

void cpInfoToPage(PF_PageHandle* pageHandle, void* keySrc, int attrLength,
	int num, int order, void* ridSrc) {
	char* pData;
	GetData(pageHandle, &pData);
	pData = pData + sizeof(IX_FileHeader) + sizeof(IX_Node);
	memcpy(pData, keySrc, num * attrLength);
	pData = pData + order * attrLength;
	memcpy(pData, ridSrc, num * sizeof(RID));
}

void cpNewToPage(PF_PageHandle* pageHandle, PageNum brother, PageNum parent,
	int is_leaf, int keynum) {
	IX_Node newNodeInfo;
	newNodeInfo.brother = brother;
	newNodeInfo.parent = parent;
	newNodeInfo.is_leaf = is_leaf;
	newNodeInfo.keynum = keynum;
	char* pData;
	GetData(pageHandle, &pData);
	memcpy(pData + sizeof(IX_FileHeader), &newNodeInfo, sizeof(IX_Node));
}

//将给定的关键字和指针写到指定的页面
void setInfoToPage(PF_PageHandle* pageHandle, int order, AttrType attrType,
	int attrLength, void* pData, RID* rid, bool insertIfTrue) {
	char* parentData;
	char* parentKeys;
	char* parentRids;

	GetData(pageHandle, &parentData);

	//获取根节点页面得节点控制信息
	IX_Node* nodeControlInfo = (IX_Node*)(parentData + sizeof(IX_FileHeader));
	int keynum = nodeControlInfo->keynum;

	//获取关键字区
	parentKeys = parentData + sizeof(IX_FileHeader) + sizeof(IX_Node);
	//获取指针区
	parentRids = parentKeys + order * attrLength;

	int position = 0;
	switch (attrType) {
	case chars:
		for (; position < keynum; position++) {
			if (strcmp(parentKeys + position * attrLength, (char*)pData) > 0)
				break;
		}
		break;
	case ints:
		int data1;
		data1 = *((int*)pData);
		for (; position < keynum; position++) {
			int data2 = *((int*)parentKeys + position * attrLength);
			if (data2 > data1) break;
		}
		break;
	case floats:
		float data_floats = *((float*)pData);
		for (; position < keynum; position++) {
			float data2 = *((float*)parentKeys + position * attrLength);
			if (data2 > data_floats) break;
		}
		break;
	}
	if (insertIfTrue) {
		memcpy(parentKeys + (position + 1) * attrLength,
			parentKeys + position * attrLength,
			(keynum - position) * attrLength);
		memcpy(parentKeys + position * attrLength, pData, attrLength);
		//插入关键字的指针
		memcpy(parentRids + (position + 1) * sizeof(RID),
			parentRids + position * sizeof(RID),
			(keynum - position) * sizeof(RID));
		memcpy(parentRids + position * sizeof(RID), rid, sizeof(RID));
		keynum++;
		nodeControlInfo->keynum = keynum;
	}
	else {
		position--;
		//关键字将会插入到第一个关键字处
		if (position < 0) {
			//插入到最前面的页面
			position = 0;
			//修改所指向页面的最小关键字
			memcpy(parentKeys, pData, attrLength);
		}
		memcpy(rid, parentRids + position * sizeof(RID), sizeof(RID));
	}
}

//索引的删除
RC DeleteEntry(IX_IndexHandle* indexHandle, void* pData, RID* rid) {
	// TODO: 完成索引删除
	return SUCCESS;
}

RC OpenIndexScan(IX_IndexScan* indexScan, IX_IndexHandle* indexHandle,
	CompOp compOp, char* value) {
	return SUCCESS;
}

RC IX_GetNextEntry(IX_IndexScan* indexScan, RID* rid) { return SUCCESS; }

RC CloseIndexScan(IX_IndexScan* indexScan) { return SUCCESS; }

RC GetIndexTree(char* fileName, Tree* index) { return SUCCESS; }
