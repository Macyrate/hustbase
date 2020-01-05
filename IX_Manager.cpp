#include "stdafx.h"
#include "IX_Manager.h"

int threshold = 0;

/**
 * 此函数创建一个名为fileName的索引。
 * attrType描述被索引属性的类型，attrLength描述被索引属性的长度。
 */
RC CreateIndex(char* fileName, AttrType attrType, int attrLength) {
	//PF_FileHandle* fileHandle = NULL;
	//PageNum pageNum;
	////创建索引文件
	//CreateFile(fileName);
	//fileHandle = (PF_FileHandle*)malloc(sizeof(PF_FileHandle));
	////打开索引文件
	//openFile(fileName, fileHandle);

	//PF_PageHandle* firstPageHandle = NULL;
	//firstPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	////分配索引文件的第一个页面
	//AllocatePage(fileHandle, firstPageHandle);
	//GetPageNum(firstPageHandle, &pageNum);

	////生成索引控制信息
	//IX_FileHeader index_FileHeader;
	//index_FileHeader.attrLength = attrLength;
	//index_FileHeader.keyLength = attrLength + sizeof(RID);
	//index_FileHeader.attrType = attrType;
	//index_FileHeader.rootPage = pageNum;
	//index_FileHeader.first_leaf = pageNum;
	//int order = (PF_PAGE_SIZE - (sizeof(IX_FileHeader) + sizeof(IX_Node))) /
	//	(2 * sizeof(RID) + attrLength);
	//index_FileHeader.order = order;
	//threshold = order >> 1;
	////获取第一页的数据区
	//char* pData;
	//GetData(firstPageHandle, &pData);
	//memcpy(pData, &index_FileHeader, sizeof(IX_FileHeader));

	////初始化节点控制信息，将根节点的置为叶子节点，关键字数为0，
	//IX_Node index_NodeControl;
	//index_NodeControl.keynum = 0;
	//index_NodeControl.parent = 0;
	//index_NodeControl.brother = 0;
	//index_NodeControl.is_leaf = 1;
	//memcpy(pData + sizeof(IX_FileHeader), &index_NodeControl, sizeof(IX_Node));
	//MarkDirty(firstPageHandle);

	//UnpinPage(firstPageHandle);
	////关闭索引文件
	//CloseFile(fileHandle);
	//free(firstPageHandle);
	//free(fileHandle);

	return SUCCESS;
}

RC OpenIndex(const char* fileName, IX_IndexHandle* indexHandle) {
	//RC rc;
	//PF_FileHandle pfFileHandle;
	//PF_PageHandle pfPageHandle;
	//char* data;

	//memset(indexHandle, 0, sizeof(IX_IndexHandle));

	//if ((rc = openFile((char*)fileName, &pfFileHandle)) ||
	//	(rc = GetThisPage(&pfFileHandle, 1, &pfPageHandle))) {
	//	return rc;
	//}

	//indexHandle->bOpen = true;
	//if (rc = GetData(&pfPageHandle, &data)) {
	//	return rc;
	//}
	//indexHandle->fileHeader = *(IX_FileHeader*)data;
	//indexHandle->fileHandle = pfFileHandle;
	//if (rc = UnpinPage(&pfPageHandle)) {
	//	return rc;
	//}

	return SUCCESS;
}

// TODO: 可能存在bug
//关闭索引文件
RC CloseIndex(IX_IndexHandle* indexHandle) {
	//PF_FileHandle fileHandle = indexHandle->fileHandle;
	//CloseFile(&fileHandle);
	return SUCCESS;
}

//索引的插入
RC InsertEntry(IX_IndexHandle* indexHandle, void* pData, RID* rid) {
	//PF_FileHandle fileHandle = indexHandle->fileHandle;
	//IX_FileHeader fileHeader = indexHandle->fileHeader;
	//PF_PageHandle* pageHandle = NULL;
	////获取根节点页面
	//pageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	//pageHandle->bOpen = false;
	//GetThisPage(&fileHandle, fileHeader.rootPage, pageHandle);
	////获取根节点页面的数据区
	//char* pageData;
	//GetData(pageHandle, &pageData);
	////获取根节点页面得节点控制信息
	//IX_Node* index_NodeControlInfo =
	//	(IX_Node*)(pageData + sizeof(IX_FileHeader));
	////索引文件页面的序数
	//int order = fileHeader.order;
	////索引关键字的长度
	//int attrLength = fileHeader.attrLength;

	////判断节点如果是叶子节点
	//while (index_NodeControlInfo->is_leaf != 1) {
	//	RID tempRid;
	//	setInfoToPage(pageHandle, order, fileHeader.attrType, fileHeader.attrLength,
	//		pData, &tempRid, false);  //查找将要插入关键字的页面
	//	GetThisPage(&fileHandle, tempRid.pageNum, pageHandle);
	//	GetData(pageHandle, &pageData);
	//	index_NodeControlInfo = (IX_Node*)(pageData + sizeof(IX_FileHeader));
	//}
	//setInfoToPage(pageHandle, order, fileHeader.attrType, fileHeader.attrLength,
	//	pData, rid, true);
	//while (index_NodeControlInfo->keynum == order) {
	//	int keynum = index_NodeControlInfo->keynum;
	//	//获取关键字区
	//	char* keys = pageData + sizeof(IX_FileHeader) + sizeof(IX_Node);
	//	//获取指针区
	//	char* rids = keys + order * attrLength;

	//	PageNum nodePage;
	//	GetPageNum(pageHandle, &nodePage);

	//	//新叶子节点页面
	//	PF_PageHandle* newLeafPageHandle = NULL;
	//	newLeafPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	//	newLeafPageHandle->bOpen = false;
	//	AllocatePage(&fileHandle, newLeafPageHandle);
	//	PageNum newLeafPage;
	//	GetPageNum(newLeafPageHandle, &newLeafPage);

	//	int divide1 = keynum >> 1;
	//	int divide2 = keynum - divide1;

	//	if (index_NodeControlInfo->parent == 0)
	//	{
	//		//生成新的根页面
	//		PF_PageHandle* newRootPageHandle = NULL;
	//		newRootPageHandle = (PF_PageHandle*)malloc(sizeof(PF_PageHandle));
	//		newRootPageHandle->bOpen = false;
	//		AllocatePage(&fileHandle, newRootPageHandle);
	//		PageNum newRootPage;
	//		GetPageNum(newRootPageHandle, &newRootPage);
	//		cpNewToPage(newRootPageHandle, 0, 0, 0, 0);  //设置新根节点的节点控制信息

	//		cpNewToPage(newLeafPageHandle, index_NodeControlInfo->brother,
	//			newRootPage, index_NodeControlInfo->is_leaf,
	//			divide2);  //设置新分裂节点的节点控制信息
	//		cpNewToPage(pageHandle, newLeafPage, newRootPage,
	//			index_NodeControlInfo->is_leaf,
	//			divide1);  //设置原节点控制信息

	//		cpInfoToPage(
	//			newLeafPageHandle, keys + divide1 * attrLength, attrLength, divide2,
	//			order,
	//			rids + divide1 * sizeof(RID));  //复制关键字和指针到分裂后新的页面中

	//		char* tempData;
	//		GetData(pageHandle, &tempData);
	//		RID tempRid;
	//		tempRid.bValid = false;
	//		tempRid.pageNum = nodePage;
	//		setInfoToPage(newRootPageHandle, order, fileHeader.attrType,
	//			fileHeader.attrLength, tempData, &tempRid,
	//			true);  //向新的根节点插入子节点的关键字和指针

	//		GetData(newLeafPageHandle, &tempData);
	//		tempRid.pageNum = newLeafPage;
	//		setInfoToPage(newRootPageHandle, order, fileHeader.attrType,
	//			fileHeader.attrLength, tempData, &tempRid,
	//			true);  //向新的根节点插入子节点的关键字和指针

	//		indexHandle->fileHeader.rootPage =
	//			newRootPage;  //修改索引控制信息中的根节点页面
	//		free(newRootPageHandle);
	//	}
	//	else  //说明当前分裂的节点不是根节点
	//	{
	//		PageNum parentPage = index_NodeControlInfo->parent;
	//		cpNewToPage(newLeafPageHandle, nodePage, parentPage,
	//			index_NodeControlInfo->is_leaf,
	//			divide2);  //设置新分裂节点的节点控制信息
	//		cpNewToPage(pageHandle, newLeafPage, parentPage,
	//			index_NodeControlInfo->is_leaf,
	//			divide1);  //设置原节点控制信息
	//		cpInfoToPage(
	//			newLeafPageHandle, keys + divide1 * attrLength, attrLength, divide2,
	//			order,
	//			rids + divide1 * sizeof(RID));  //复制关键字和指针到分裂后新的页面中

	//		char* tempData;
	//		GetData(newLeafPageHandle, &tempData);

	//		RID tempRid;
	//		tempRid.bValid = false;
	//		tempRid.pageNum = newLeafPage;

	//		GetThisPage(&fileHandle, parentPage,
	//			pageHandle);  //令pageHandle指向其父节点页面
	//		setInfoToPage(pageHandle, order, fileHeader.attrType,
	//			fileHeader.attrLength, tempData, &tempRid,
	//			true);  //向父节点插入新子节点的关键字和指针

	//		GetData(pageHandle, &pageData);  //令pageData指向父节点的数据区
	//		index_NodeControlInfo =
	//			(IX_Node*)(pageData + sizeof(IX_FileHeader));  //父节点的节点控制信息
	//	}
	//	free(newLeafPageHandle);
	//}

	//free(pageHandle);

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

/**
 * 
 * #include "StdAfx.h"
#include "IX_Manager.h"
#include "RM_Manager.h"

  RC CreateIndex(const char *fileName, AttrType attrType, int attrLength) {
    if (CreateFile(fileName)) return FAIL;

    PF_FileHandle *file = new PF_FileHandle;
    if (openFile((char *)fileName, file)) return FAIL;

    PF_PageHandle *firstPage = new PF_PageHandle;

    if (AllocatePage(file, firstPage)) {
      free(firstPage);
      return FAIL;
    }

    IX_FileHeader *fileHeader = (IX_FileHeader *)firstPage->pFrame->page.pData;
    fileHeader->attrLength = attrLength;
    fileHeader->attrType = attrType;
    fileHeader->first_leaf = 1;
    fileHeader->keyLength = attrLength + sizeof(RID);

    fileHeader->order = (PF_PAGE_SIZE - sizeof(IX_FileHeader) - sizeof(IX_Node)) /
                            (2 * sizeof(RID) + attrLength) -
                        1;
    fileHeader->rootPage = 1;

    IX_Node *ixNode =
        (IX_Node *)(firstPage->pFrame->page.pData + sizeof(IX_FileHeader));
    ixNode->is_leaf = 1;
    ixNode->keynum = 0;
    ixNode->parent = 0;
    ixNode->parentOrder = 0;
    ixNode->brother = -1;
    ixNode->keys = (char *)(firstPage->pFrame->page.pData +
                            sizeof(IX_FileHeader) + sizeof(IX_Node));
    ixNode->rids =
        (RID *)(ixNode->keys + (fileHeader->order + 1) * fileHeader->keyLength);
    MarkDirty(firstPage);
    UnpinPage(firstPage);
    free(firstPage);
    CloseFile(file);
    return SUCCESS;
  }

  RC OpenIndex(const char *fileName, IX_IndexHandle *indexHandle) {
    if (indexHandle->bOpen) return RM_FHOPENNED;
    indexHandle->bOpen = TRUE;

    if (openFile((char *)fileName, &indexHandle->fileHandle)) return FAIL;

    PF_PageHandle *ctrPage = NULL;
    if (GetThisPage(&indexHandle->fileHandle, 1, ctrPage)) {
      CloseFile(&indexHandle->fileHandle);
      return FAIL;
    }

    IX_FileHeader *headerInfo;

    headerInfo = (IX_FileHeader *)ctrPage->pFrame->page.pData;
    indexHandle->fileHeader.keyLength = headerInfo->keyLength;
    indexHandle->fileHeader.order = headerInfo->order;
    indexHandle->fileHeader.rootPage = headerInfo->rootPage;
    indexHandle->fileHeader.attrLength = headerInfo->attrLength;
    indexHandle->fileHeader.attrType = headerInfo->attrType;
    indexHandle->fileHeader.first_leaf = headerInfo->first_leaf;

    return SUCCESS;
  }

  RC CloseIndex(IX_IndexHandle *indexHandle) {
    if (!indexHandle->bOpen) return IX_ISCLOSED;

    if (CloseFile(&indexHandle->fileHandle)) return FAIL;
    indexHandle->bOpen = FALSE;

    return SUCCESS;
  }

  RC InsertEntry(IX_IndexHandle *indexHandle, void *pData, const RID *rid) {
    int pageInsertNumber = getNodeByKey(indexHandle, pData);
    PF_PageHandle *pageInsert = new PF_PageHandle;
    GetThisPage(&indexHandle->fileHandle, pageInsertNumber, pageInsert);
    _insert(indexHandle, pData, rid, pageInsert);
    return FAIL;
  }

  RC DeleteEntry(IX_IndexHandle *indexHandle, void *pData, const RID *rid) {
    PF_PageHandle *pageDelete = new PF_PageHandle;
    int pageNum = getNodeByKey(indexHandle, pData);
    GetThisPage(&indexHandle->fileHandle, pageNum, pageDelete);

    RC rtn = _delete(indexHandle, pData, rid, pageDelete);
    free(pageDelete);
    return rtn;
  }

  RC OpenIndexScan(IX_IndexScan *indexScan, IX_IndexHandle *indexHandle,
                  CompOp compOp, char *value) {
    indexScan->bOpen = true;
    indexScan->compOp = compOp;
    indexScan->pIXIndexHandle = indexHandle;
    indexScan->value = value;
    switch (compOp) {
      case NO_OP:
      case LEqual:
      case LessT:
        indexScan->pnNext = indexHandle->fileHeader.first_leaf;
        indexScan->ridIx = 0;
        GetThisPage(&indexHandle->fileHandle, indexScan->pnNext,
                    indexScan->pfPageHandle);
        indexScan->currentPageControl =
            (IX_Node *)(indexScan->pfPageHandle->pFrame->page.pData +
                        sizeof(IX_FileHeader));
        return SUCCESS;
      default:
        break;
    }
    int startPageNumber = getNodeByKey(indexHandle, value);
    GetThisPage(&indexHandle->fileHandle, startPageNumber,
                indexScan->pfPageHandle);
    IX_Node *startPageControl =
        (IX_Node *)(indexScan->pfPageHandle->pFrame->page.pData +
                    sizeof(IX_FileHeader));
    int indexOffset, rtn;
    float targetVal, indexVal;
    for (indexOffset = 0; indexOffset < startPageControl->keynum; indexOffset++) {
      switch (indexHandle->fileHeader.attrType) {
        case 0:
          rtn = strcmp((char *)value,
                      startPageControl->keys +
                          indexOffset * indexHandle->fileHeader.keyLength +
                          sizeof(RID));
        case 1:
        case 2:
          targetVal = *(float *)value;
          indexVal = *(float *)(startPageControl->keys +
                                indexOffset * indexHandle->fileHeader.keyLength +
                                sizeof(RID));
          rtn = (targetVal < indexVal) ? -1 : ((targetVal == indexVal) ? 0 : 1);
          break;
        default:
          break;
      }
      if (rtn == 0) {
        indexScan->pnNext = indexScan->pfPageHandle->pFrame->page.pageNum;
        indexScan->ridIx =
            (compOp == EQual || compOp == GEqual) ? indexOffset : indexOffset + 1;

        indexScan->currentPageControl = startPageControl;
        return SUCCESS;
      } else if (rtn < 0) {
        if (compOp == EQual)
          return FAIL;
        else {
          indexScan->pnNext = indexScan->pfPageHandle->pFrame->page.pageNum;
          indexScan->ridIx = indexOffset;

          indexScan->currentPageControl = startPageControl;
          return SUCCESS;
        }
      }
    }
    if (indexOffset == startPageControl->keynum) {
      if (compOp == EQual)
        return FAIL;
      else {
        indexScan->pnNext = startPageControl->brother;
        indexScan->ridIx = 0;
        GetThisPage(&indexHandle->fileHandle, startPageControl->brother,
                    indexScan->pfPageHandle);
        indexScan->currentPageControl =
            (IX_Node *)(indexScan->pfPageHandle->pFrame->page.pData +
                        sizeof(IX_FileHeader));
        return SUCCESS;
      }
    }
    return SUCCESS;
  }

  RC IX_GetNextEntry(IX_IndexScan *indexScan, RID *rid) {
    if (indexScan->ridIx == indexScan->currentPageControl->keynum) {
      if (indexScan->currentPageControl->brother == -1)
        return FAIL;
      else {
        indexScan->pnNext = indexScan->currentPageControl->brother;
        GetThisPage(&indexScan->pIXIndexHandle->fileHandle, indexScan->pnNext,
                    indexScan->pfPageHandle);
        indexScan->currentPageControl =
            (IX_Node *)(indexScan->pfPageHandle->pFrame->page.pData +
                        sizeof(IX_FileHeader));
        indexScan->ridIx = 0;
      }
    }
    if (indexScan->compOp != NO_OP) {
      switch (indexScan->pIXIndexHandle->fileHeader.attrType) {
        case chars:
          if (!compareLeafString(
                  indexScan->currentPageControl->keys +
                      indexScan->ridIx *
                          indexScan->pIXIndexHandle->fileHeader.keyLength +
                      sizeof(RID),
                  indexScan->value, indexScan->compOp))
            return FAIL;
        case ints:
        case floats:
          if (!compareLeafValue(
                  *(float *)(indexScan->currentPageControl->keys +
                            indexScan->ridIx *
                                indexScan->pIXIndexHandle->fileHeader.keyLength +
                            sizeof(RID)),
                  *(float *)indexScan->value, indexScan->compOp))
            return FAIL;
          break;
      }
    }
    rid->bValid = true;
    rid->pageNum = indexScan->pnNext;
    rid->slotNum = indexScan->ridIx;
    indexScan->ridIx++;
    return SUCCESS;
  }

  RC CloseIndexScan(IX_IndexScan *indexScan) {
    indexScan->bOpen = false;
    indexScan->pfPageHandle->pFrame->bDirty = false;
    CloseIndex(indexScan->pIXIndexHandle);
    return SUCCESS;
  }

  RC GetIndexTree(char *fileName, Tree *index) {
    char *pageData = NULL;
    char *pageKeys = NULL;
    char *pageRids = NULL;
    IX_Node *nodeControlInfo = NULL;

    IX_IndexHandle *indexHandle = new IX_IndexHandle;
    if (openFile(fileName, &indexHandle->fileHandle)) return FAIL;
    
    Tree_Node *treeNode = new Tree_Node;
    Tree_Node *bnode = new Tree_Node;
    Tree_Node *firstChild = NULL;

    index->attrLength = indexHandle->fileHeader.attrLength;
    index->attrType = indexHandle->fileHeader.attrType;
    index->order = indexHandle->fileHeader.order;
    PF_PageHandle *pageHandle = new PF_PageHandle;
    GetThisPage(&indexHandle->fileHandle, indexHandle->fileHeader.rootPage,
                pageHandle);
    GetData(pageHandle, &pageData);
    nodeControlInfo = (IX_Node *)(pageData + sizeof(IX_FileHeader));
    pageKeys = pageData + sizeof(IX_FileHeader) + sizeof(IX_Node);
    pageRids = pageKeys +
              indexHandle->fileHeader.order * indexHandle->fileHeader.attrLength;

    index->root->keyNum = nodeControlInfo->keynum;
    memcpy(index->root->keys, pageKeys,
          nodeControlInfo->keynum * indexHandle->fileHeader.attrLength);
    index->root->sibling = NULL;
    index->root->parent = NULL;

    return SUCCESS;
  }

  bool compareLeafString(char *left, char *right, CompOp oper) {
    int cmpResult = strcmp(left, right);
    switch (oper) {
      case NEqual:
        return (cmpResult == 0) ? false : true;
      case LEqual:
        return (cmpResult == 0 || cmpResult < 0) ? true : false;
      case EQual:
        return (cmpResult == 0) ? true : false;
      case GreatT:
        return (cmpResult > 0) ? true : false;
      case LessT:
        return (cmpResult < 0) ? true : false;
      case GEqual:
        return (cmpResult == 0 || cmpResult > 0) ? true : false;
      default:
        return false;
    }
  }

  bool compareLeafValue(float left, float right, CompOp oper) {
    switch (oper) {
      case LEqual:
        return (left <= right);
      case GEqual:
        return (left >= right);
      case EQual:
        return (left == right);
      case LessT:
        return (left < right);
      case NEqual:
        return (left != right);
      case GreatT:
        return (left > right);
      default:
        return false;
    }
  }

  void _insert(IX_IndexHandle *indexHandle, void *pData, const RID *rid,
              PF_PageHandle *pageInsert) {
    IX_Node *pageC =
        (IX_Node *)(pageInsert->pFrame->page.pData + sizeof(IX_FileHeader));
    int posInsert = addKey(pageC->keys, pageC->rids, &pageC->keynum,
                          (char *)pData, rid, indexHandle->fileHeader.attrType,
                          indexHandle->fileHeader.keyLength);
    if (pageC->keynum < indexHandle->fileHeader.order) {
      if (0 == posInsert) {
        PageNum pageNum;
        PF_PageHandle *parentPageHandle = new PF_PageHandle;

        GetPageNum(pageInsert, &pageNum);
        GetThisPage(&indexHandle->fileHandle, pageC->parent, parentPageHandle);
        deleteOrAlterParentNode(parentPageHandle, &indexHandle->fileHandle,
                                indexHandle->fileHeader.order,
                                indexHandle->fileHeader.attrType,
                                indexHandle->fileHeader.attrLength, pageNum,
                                pData, pageC->parentOrder, false);
        free(parentPageHandle);
      }
    } else {
      int offset = int(pageC->keynum / 2 + 1);
      PF_PageHandle *bnode = new PF_PageHandle;
      AllocatePage(&indexHandle->fileHandle, bnode);
      pageC->brother = bnode->pFrame->page.pageNum;

      IX_Node *bPageControl =
          (IX_Node *)(bnode->pFrame->page.pData + sizeof(IX_FileHeader));
      bPageControl->keys =
          bnode->pFrame->page.pData + sizeof(IX_FileHeader) + sizeof(IX_Node);
      bPageControl->rids =
          (RID *)(bPageControl->keys + (indexHandle->fileHeader.order + 1) *
                                          indexHandle->fileHeader.keyLength);
      bPageControl->keynum = (pageC->keynum - offset);

      memcpy(bPageControl->keys, pageC->keys + offset,
            bPageControl->keynum * indexHandle->fileHeader.keyLength);
      pageC->keynum = offset;
      memcpy(bPageControl->rids, pageC->rids + offset,
            bPageControl->keynum * sizeof(RID));

      bPageControl->is_leaf = pageC->is_leaf;
      bPageControl->brother = -1;
      MarkDirty(bnode);
      UnpinPage(bnode);
      free(bnode);

      if (pageC->parent == 0) {
        PF_PageHandle *parentNode = new PF_PageHandle;
        AllocatePage(&indexHandle->fileHandle, parentNode);
        IX_Node *parentPageControl =
            (IX_Node *)(parentNode->pFrame->page.pData + sizeof(IX_FileHeader));
        ;
        // 初始化
        parentPageControl->keynum = 2;
        parentPageControl->is_leaf = 0;
        parentPageControl->parent = -1;
        parentPageControl->brother = -1;
        parentPageControl->keys = parentNode->pFrame->page.pData +
                                  sizeof(IX_FileHeader) + sizeof(IX_Node);

        parentPageControl->rids = (RID *)(parentPageControl->keys +
                                          (indexHandle->fileHeader.order + 1) *
                                              indexHandle->fileHeader.keyLength);
        indexHandle->fileHeader.rootPage = parentNode->pFrame->page.pageNum;
        memcpy(parentPageControl->keys, pageC->keys,
              indexHandle->fileHeader.keyLength);
        parentPageControl->rids->bValid = true;
        parentPageControl->rids->pageNum = pageInsert->pFrame->page.pageNum;
        parentPageControl->rids->slotNum = 0;

        memcpy(parentPageControl->keys, bPageControl->keys,
              indexHandle->fileHeader.keyLength);
        parentPageControl->rids->bValid = true;
        parentPageControl->rids->pageNum = bnode->pFrame->page.pageNum;
        parentPageControl->rids->slotNum = 0;
        pageC->parent = parentNode->pFrame->page.pageNum;
        bPageControl->parent = parentNode->pFrame->page.pageNum;
        MarkDirty(parentNode);
        UnpinPage(parentNode);
        free(parentNode);
        return;
      } else {
        bPageControl->parent = pageC->parent;

        RID *broPointer;
        broPointer->bValid = true;
        broPointer->pageNum = bnode->pFrame->page.pageNum;
        broPointer->slotNum = 0;
        PF_PageHandle *parentPage = new PF_PageHandle;
        GetThisPage(&indexHandle->fileHandle, pageC->parent, parentPage);
        if (posInsert != 0)
          memcpy(parentPage->pFrame->page.pData, pageC->keys,
                indexHandle->fileHeader.keyLength);
        MarkDirty(pageInsert);
        UnpinPage(pageInsert);
        _insert(indexHandle, bPageControl->keys, broPointer, parentPage);
      }
    }
  }

  RC _delete(IX_IndexHandle *indexHandle, void *pData, const RID *rid,
            PF_PageHandle *pageDelete) {
    IX_Node *pageC =
        (IX_Node *)(pageDelete->pFrame->page.pData + sizeof(IX_FileHeader));
    PF_FileHandle *fileHandle = &indexHandle->fileHandle;

    int offset = removeKey(pageC->keys, pageC->rids, &pageC->keynum,
                          (char *)pData, indexHandle->fileHeader.attrType,
                          indexHandle->fileHeader.keyLength);
    if (-1 == offset) return FAIL;
    int threshold = ceil((float)indexHandle->fileHeader.order / 2);

    if (pageC->keynum >= threshold) {
      if (offset == 0) {
        PageNum pageNum;
        PF_PageHandle *parentPageHandle = new PF_PageHandle;

        GetPageNum(pageDelete, &pageNum);
        GetThisPage(fileHandle, pageC->parent, parentPageHandle);
        deleteOrAlterParentNode(
            parentPageHandle, fileHandle, indexHandle->fileHeader.order,
            indexHandle->fileHeader.attrType, indexHandle->fileHeader.attrLength,
            pageNum, &pageC->keynum, pageC->parentOrder, false);
        free(parentPageHandle);
      }
    } else {
      getPageDataFromBrother(pageDelete, fileHandle,
                            indexHandle->fileHeader.order,
                            indexHandle->fileHeader.attrType,
                            indexHandle->fileHeader.attrLength, threshold);
    }
    return SUCCESS;
  }

  void getPageDataFromBrother(PF_PageHandle *pageHandle,
                              PF_FileHandle *fileHandle, const int order,
                              const AttrType attrType, const int attrLength,
                              const int threshold) {
    int status = 0;
    PageNum leftPageNum;
    PageNum pageNum;

    getLeftBrother(pageHandle, fileHandle, order, attrType, attrLength,
                  leftPageNum);
    char *tempData = nullptr;
    char *tempKeys = nullptr;
    IX_Node *tempNodeControlInfo = nullptr;
    PF_PageHandle *parentPageHandle = new PF_PageHandle;

    if (-1 != leftPageNum) {
      PF_PageHandle *leftHandle = new PF_PageHandle;
      GetThisPage(fileHandle, leftPageNum, leftHandle);
      getFromLeft(pageHandle, leftHandle, order, attrType, attrLength, threshold,
                  status);

      if (1 == status) {
        GetPageNum(pageHandle, &pageNum);
        GetData(pageHandle, &tempData);
        tempNodeControlInfo = (IX_Node *)(tempData + sizeof(IX_FileHeader));
        tempKeys = tempData + sizeof(IX_FileHeader) + sizeof(IX_Node);
        GetThisPage(fileHandle, tempNodeControlInfo->parent, parentPageHandle);

        deleteOrAlterParentNode(parentPageHandle, fileHandle, order, attrType,
                                attrLength, pageNum, tempKeys,
                                tempNodeControlInfo->parentOrder, false);
      } else if (2 == status) {
        GetData(leftHandle, &tempData);
        tempNodeControlInfo = (IX_Node *)(tempData + sizeof(IX_FileHeader));

        GetThisPage(fileHandle, tempNodeControlInfo->parent, parentPageHandle);

        deleteOrAlterParentNode(parentPageHandle, fileHandle, order, attrType,
                                attrLength, leftPageNum, nullptr,
                                tempNodeControlInfo->parentOrder, true);
      }
      free(leftHandle);
    } else {
      PF_PageHandle *rightHandle = new PF_PageHandle;
      GetData(pageHandle, &tempData);
      tempNodeControlInfo = (IX_Node *)(tempData + sizeof(IX_FileHeader));
      GetThisPage(fileHandle, tempNodeControlInfo->brother, rightHandle);
      getRightPageData(pageHandle, rightHandle, order, attrType, attrLength,
                      threshold, status);

      GetData(rightHandle, &tempData);
      tempNodeControlInfo = (IX_Node *)(tempData + sizeof(IX_FileHeader));
      tempKeys = tempData + sizeof(IX_FileHeader) + sizeof(IX_Node);
      GetThisPage(fileHandle, tempNodeControlInfo->parent, parentPageHandle);
      GetPageNum(rightHandle, &pageNum);

      if (3 == status) {
        deleteOrAlterParentNode(parentPageHandle, fileHandle, order, attrType,
                                attrLength, pageNum, tempKeys,
                                tempNodeControlInfo->parentOrder, false);
      } else if (4 == status) {
        deleteOrAlterParentNode(parentPageHandle, fileHandle, order, attrType,
                                attrLength, pageNum, nullptr,
                                tempNodeControlInfo->parentOrder, true);
      }
      free(rightHandle);
    }
    free(parentPageHandle);
  }

  void getRightPageData(PF_PageHandle *pageHandle, PF_PageHandle *rightHandle,
                        int order, AttrType attrType, int attrLength,
                        const int threshold, int &status) {
    char *pageData;
    char *pageKeys;
    char *pageRids;
    GetData(pageHandle, &pageData);
    IX_Node *pageNodeInfo = (IX_Node *)(pageData + sizeof(IX_FileHeader));
    int pageKeynum = pageNodeInfo->keynum;
    pageKeys = pageData + sizeof(IX_FileHeader) + sizeof(IX_Node);
    pageRids = pageKeys + order * attrLength;

    char *rightData;
    char *rightKeys;
    char *rightRids;
    GetData(rightHandle, &rightData);

    IX_Node *rightNodeControlInfo =
        (IX_Node *)(rightData + sizeof(IX_FileHeader));
    rightKeys = rightData + sizeof(IX_FileHeader) + sizeof(IX_Node);
    rightRids = rightKeys + order * attrLength;

    int rightKeynum = rightNodeControlInfo->keynum;
    if (rightKeynum > threshold) {
      memcpy(pageKeys + pageKeynum * attrLength, rightKeys, attrLength);
      memcpy(pageRids + pageKeynum * sizeof(RID), rightRids, sizeof(RID));

      memcpy(rightKeys, rightKeys + attrLength, (rightKeynum - 1) * attrLength);
      memcpy(rightRids, rightRids + sizeof(RID), (rightKeynum - 1) * sizeof(RID));

      rightNodeControlInfo->keynum = rightKeynum - 1;
      pageNodeInfo->keynum = pageKeynum + 1;
      status = 3;
    } else {
      memcpy(pageKeys + pageKeynum * attrLength, rightKeys,
            rightKeynum * attrLength);
      memcpy(pageRids + pageKeynum * sizeof(RID), rightRids,
            rightKeynum * sizeof(RID));

      rightNodeControlInfo->keynum = 0;
      pageNodeInfo->keynum = pageKeynum + rightKeynum;
      status = 4;

      pageNodeInfo->brother = rightNodeControlInfo->brother;
    }
    MarkDirty(pageHandle);
    UnpinPage(pageHandle);
    MarkDirty(rightHandle);
    UnpinPage(rightHandle);
  }

  void getLeftBrother(PF_PageHandle *pageHandle, PF_FileHandle *fileHandle,
                      const int order, const AttrType attrType,
                      const int attrLength, PageNum &leftBrother) {
    char *data;
    PageNum nowPage;
    GetPageNum(pageHandle, &nowPage);
    GetData(pageHandle, &data);
    IX_Node *nodeInfo = (IX_Node *)(data + sizeof(IX_FileHeader));

    PF_PageHandle *parentPageHandle = new PF_PageHandle;
    GetThisPage(fileHandle, nodeInfo->parent, parentPageHandle);
    char *parentData;
    char *parentKeys;
    char *parentRids;

    GetData(parentPageHandle, &parentData);

    parentKeys = parentData + sizeof(IX_FileHeader) + sizeof(IX_Node);

    parentRids = parentKeys + order * attrLength;
    for (int offset = 0;; offset++) {
      RID *tempRid = (RID *)parentRids + offset * sizeof(RID);
      if (tempRid->pageNum == nowPage) {
        if (offset != 0) {
          offset--;
          tempRid = (RID *)parentRids + offset * sizeof(RID);
          leftBrother = tempRid->pageNum;
        } else
          leftBrother = -1;
        return;
      }
    }
    free(parentPageHandle);
  }

  void getFromLeft(PF_PageHandle *pageHandle, PF_PageHandle *leftHandle,
                  int order, AttrType attrType, int attrLength,
                  const int threshold, int &status) {
    char *leftData;
    char *leftKeys;
    char *leftRids;

    GetData(leftHandle, &leftData);

    IX_Node *lNodeInfo = (IX_Node *)(leftData + sizeof(IX_FileHeader));

    leftKeys = leftData + sizeof(IX_FileHeader) + sizeof(IX_Node);

    leftRids = leftKeys + order * attrLength;

    char *pageData;
    char *pageKeys;
    char *pageRids;

    GetData(pageHandle, &pageData);
    IX_Node *pageNodeInfo = (IX_Node *)(pageData + sizeof(IX_FileHeader));
    int pageKeynum = pageNodeInfo->keynum;

    pageKeys = pageData + sizeof(IX_FileHeader) + sizeof(IX_Node);

    pageRids = pageKeys + order * attrLength;

    int leftKeynum = lNodeInfo->keynum;
    if (leftKeynum > threshold) {
      memcpy(pageKeys + attrLength, pageKeys, pageKeynum * attrLength);
      memcpy(pageRids + sizeof(RID), pageRids, pageKeynum * sizeof(RID));

      memcpy(pageKeys, leftKeys + (leftKeynum - 1) * attrLength, attrLength);
      memcpy(pageRids, leftRids + (leftKeynum - 1) * sizeof(RID), sizeof(RID));

      lNodeInfo->keynum = leftKeynum - 1;
      pageNodeInfo->keynum = pageKeynum + 1;
      status = 1;

    } else {
      memcpy(leftKeys + leftKeynum * attrLength, pageKeys,
            pageKeynum * attrLength);
      memcpy(leftRids + leftKeynum * sizeof(RID), pageRids,
            pageKeynum * sizeof(RID));

      lNodeInfo->keynum = leftKeynum + pageKeynum;
      pageNodeInfo->keynum = 0;
      lNodeInfo->brother = pageNodeInfo->brother;
      status = 2;
    }
    MarkDirty(pageHandle);
    UnpinPage(pageHandle);
    MarkDirty(leftHandle);
    UnpinPage(leftHandle);
  }

  void deleteOrAlterParentNode(PF_PageHandle *parentPageHandle,
                              PF_FileHandle *fileHandle, int order,
                              AttrType attrType, int attrLength, PageNum pageNum,
                              void *pData, int parentOrder, bool isDelete) {
    IX_Node *nodeInfo;
    char *parentData;
    char *parentKeys;
    char *parentRids;
    int offset = parentOrder;

    while (1) {
      GetData(parentPageHandle, &parentData);
      nodeInfo = (IX_Node *)(parentData + sizeof(IX_FileHeader));
      int keynum = nodeInfo->keynum;
      parentKeys = parentData + sizeof(IX_FileHeader) + sizeof(IX_Node);
      parentRids = parentKeys + order * attrLength;

      if (isDelete) {
        memcpy(parentRids + offset * sizeof(RID),
              parentRids + (offset + 1) * sizeof(RID),
              (keynum - offset - 1) * sizeof(RID));
        memcpy(parentKeys + offset * attrLength,
              parentKeys + (offset + 1) * attrLength,
              (keynum - offset - 1) * attrLength);

        nodeInfo->keynum = keynum - 1;
        break;
      } else {
        memcpy(parentKeys + offset * attrLength, pData, attrLength);
        if (offset == 0 && nodeInfo->parent != 0) {
          MarkDirty(parentPageHandle);
          UnpinPage(parentPageHandle);

          GetPageNum(parentPageHandle, &pageNum);
          GetThisPage(fileHandle, nodeInfo->parent, parentPageHandle);
        } else
          break;
      }
      offset = nodeInfo->parentOrder;
    }
    MarkDirty(parentPageHandle);
    UnpinPage(parentPageHandle);
  }

  int addKey(char *key, RID *val, int *effectiveLength, char *keyInsert,
            const RID *valInsert, AttrType type, int attrLength) {
    int keyOffset, rtn;
    float newValue, valueInIndex;

    for (keyOffset = 0; keyOffset < (*effectiveLength); keyOffset++) {
      switch (type) {
        case 0:
          rtn = strcmp(keyInsert + sizeof(RID),
                      key + keyOffset * attrLength + sizeof(RID));
          break;
        case 1:
        case 2:
          newValue = *((float *)keyInsert + sizeof(RID));
          valueInIndex = *((float *)(key + keyOffset * attrLength + sizeof(RID)));
          rtn = (newValue < valueInIndex) ? -1
                                          : ((newValue == valueInIndex) ? 0 : 1);
          break;
        default:
          break;
      }
      if (rtn <= 0) {
        if (rtn == 0) {
          if (((RID *)keyInsert)->pageNum ==
              ((RID *)key + keyOffset * attrLength)->pageNum) {
            if (((RID *)keyInsert)->slotNum ==
                ((RID *)key + keyOffset * attrLength)->slotNum) {
              *((RID *)(val + keyOffset * sizeof(RID))) = *valInsert;
              return keyOffset;
            } else if (((RID *)keyInsert)->slotNum >
                      ((RID *)key + keyOffset * attrLength)->slotNum)
              continue;
          } else if (((RID *)keyInsert)->pageNum >
                    ((RID *)key + keyOffset * attrLength)->pageNum)
            continue;
        }
        *effectiveLength = addKeyShift(keyOffset, key, val, effectiveLength,
                                      keyInsert, valInsert, attrLength);
        return keyOffset;
      }
    }
  }

  int removeKey(char *key, RID *val, int *eLength, char *keyDelete, AttrType type,
                int attrLength) {
    int keyOffset = 0;
    switch (type) {
      case ints:
      case floats:
        for (keyOffset = 0; keyOffset < (*eLength); keyOffset++) {
          int sub = *((float *)keyDelete + sizeof(RID)) -
                    *((float *)(key + keyOffset * attrLength + sizeof(RID)));
          if (sub < 0)
            break;
          else if (sub == 0) {
            if (((RID *)keyDelete)->pageNum ==
                ((RID *)key + keyOffset * attrLength)->pageNum) {
              if (((RID *)keyDelete)->slotNum ==
                  ((RID *)key + keyOffset * attrLength)->slotNum) {
                removeKeyShift(keyOffset, key, val, eLength, attrLength);
                return keyOffset;
              }

              else if (((RID *)keyDelete)->slotNum <
                      ((RID *)key + keyOffset * attrLength)->slotNum)
                return -1;

            }

            else if (((RID *)keyDelete)->pageNum <
                    ((RID *)key + keyOffset * attrLength)->pageNum)
              return -1;
          }
        }
        break;
      case chars:
        for (keyOffset = 0; keyOffset < (*eLength); keyOffset++) {
          int rtn = strcmp(keyDelete + sizeof(RID),
                          key + keyOffset * attrLength + sizeof(RID));
          if (rtn < 0)
            break;
          else if (rtn == 0) {
            if (((RID *)keyDelete)->pageNum ==
                ((RID *)key + keyOffset * attrLength)->pageNum) {
              if (((RID *)keyDelete)->slotNum ==
                  ((RID *)key + keyOffset * attrLength)->slotNum) {
                removeKeyShift(keyOffset, key, val, eLength, attrLength);
                return keyOffset;
              }

              else if (((RID *)keyDelete)->slotNum <
                      ((RID *)key + keyOffset * attrLength)->slotNum)
                return -1;

            }

            else if (((RID *)keyDelete)->pageNum <
                    ((RID *)key + keyOffset * attrLength)->pageNum)
              return -1;
          }
        }
        break;
      default:
        break;
    }
  }

  int addKeyShift(int keyOffset, char *key, RID *val, int *effectiveLength,
                  char *keyInsert, const RID *valInsert, int attrLength) {
    char *buffer =
        (char *)malloc((*effectiveLength - keyOffset - 1) * attrLength);
    memcpy(buffer, key + keyOffset * attrLength,
          (*effectiveLength - keyOffset - 1) * attrLength);
    memset(key + keyOffset * attrLength, 0,
          (*effectiveLength - keyOffset - 1) * attrLength);
    memcpy(key + (keyOffset + 1) * attrLength, buffer,
          (*effectiveLength - keyOffset - 1) * attrLength);

    strcpy(key + keyOffset * attrLength, keyInsert);
    free(buffer);

    RID *valBuffer =
        (RID *)malloc((*effectiveLength - keyOffset - 1) * sizeof(RID));
    memcpy(buffer, val + keyOffset * sizeof(RID),
          (*effectiveLength - keyOffset - 1) * sizeof(RID));
    memset(val + keyOffset * sizeof(RID), 0,
          (*effectiveLength - keyOffset - 1) * sizeof(RID));
    memcpy(val + (keyOffset + 1) * sizeof(RID), buffer,
          (*effectiveLength - keyOffset - 1) * sizeof(RID));

    *((RID *)(val + keyOffset * sizeof(RID))) = *valInsert;
    free(valBuffer);

    return ++(*effectiveLength);
  }

  void removeKeyShift(int keyOffset, char *key, RID *val, int *eLength,
                      int attrLength) {
    char *buffer = (char *)malloc((*eLength - keyOffset - 1) * attrLength);
    memcpy(buffer, key + (keyOffset + 1) * attrLength,
          (*eLength - keyOffset - 1) * attrLength);
    memcpy(key + keyOffset * attrLength, buffer,
          (*eLength - keyOffset - 1) * attrLength);
    free(buffer);

    RID *valBuffer = (RID *)malloc((*eLength - keyOffset - 1) * sizeof(RID));
    memcpy(buffer, val + (keyOffset + 1) * sizeof(RID),
          (*eLength - keyOffset - 1) * sizeof(RID));
    memcpy(val + keyOffset * sizeof(RID), buffer,
          (*eLength - keyOffset - 1) * sizeof(RID));
    free(valBuffer);
  }

  int getNodeByKey(IX_IndexHandle *indexHandle, void *targetKey) {
    int rootPage = indexHandle->fileHeader.rootPage;
    PF_PageHandle *currentPage = new PF_PageHandle;
    int rtn;
    float targetVal, iVal;
    GetThisPage(&indexHandle->fileHandle, rootPage, currentPage);
    IX_Node *nodeInfo;
    nodeInfo =
        (IX_Node *)(currentPage->pFrame->page.pData[sizeof(IX_FileHeader)]);
    int isLeaf = nodeInfo->is_leaf;

    int offset;
    while (isLeaf == 0) {
      for (offset = 0; offset < nodeInfo->keynum; offset++) {
        switch (indexHandle->fileHeader.attrType) {
          case 0:
            rtn = strcmp((char *)targetKey + sizeof(RID),
                        nodeInfo->keys +
                            offset * indexHandle->fileHeader.keyLength +
                            sizeof(RID));
          case 1:
          case 2:
            targetVal = *((float *)targetKey + sizeof(RID));
            iVal = *(float *)(nodeInfo->keys +
                              offset * indexHandle->fileHeader.keyLength +
                              sizeof(RID));
            rtn = (targetVal < iVal) ? -1 : ((targetVal == iVal) ? 0 : 1);
            break;
          default:
            break;
        }
        if (rtn == 0) {
          int currentPageNum =
              ((RID *)(nodeInfo->keys +
                      offset * indexHandle->fileHeader.keyLength))
                  ->pageNum;
          if (((RID *)targetKey)->pageNum > currentPageNum)
            continue;
          else if (((RID *)targetKey)->pageNum == currentPageNum) {
            int currentSlotNum =
                ((RID *)(nodeInfo->keys +
                        offset * indexHandle->fileHeader.keyLength))
                    ->slotNum;
            if (((RID *)targetKey)->slotNum > currentSlotNum) continue;
            if (((RID *)targetKey)->slotNum) offset++;
          }
        } else if (rtn > 0) {
          continue;
        }
        RID child = (RID)nodeInfo->rids[offset == 0 ? 0 : offset - 1];
        UnpinPage(currentPage);
        GetThisPage(&indexHandle->fileHandle, child.pageNum, currentPage);
        nodeInfo =
            (IX_Node *)(currentPage->pFrame->page.pData[sizeof(IX_FileHeader)]);
        int isLeaf = nodeInfo->is_leaf;
        break;
      }
    }
    UnpinPage(currentPage);
    return currentPage->pFrame->page.pageNum;
  }
 * 
 * /