#include "IX_Manager.h"
#include "stdafx.h"

int threshold = 0;

/**
 * 此函数创建一个名为fileName的索引。
 * attrType描述被索引属性的类型，attrLength描述被索引属性的长度。
 */
RC CreateIndex(char *fileName, AttrType attrType, int attrLength) {
  PF_FileHandle *fileHandle = NULL;
  PageNum pageNum;
  //创建索引文件
  CreateFile(fileName);
  fileHandle = (PF_FileHandle *)malloc(sizeof(PF_FileHandle));
  //打开索引文件
  openFile(fileName, fileHandle);

  PF_PageHandle *firstPageHandle = NULL;
  firstPageHandle = (PF_PageHandle *)malloc(sizeof(PF_PageHandle));
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
  char *pData;
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

RC OpenIndex(const char *fileName, IX_IndexHandle *indexHandle) {
  RC rc;
  PF_FileHandle pfFileHandle;
  PF_PageHandle pfPageHandle;
  char *data;

  memset(indexHandle, 0, sizeof(IX_IndexHandle));

  if ((rc = openFile((char *)fileName, &pfFileHandle)) ||
      (rc = GetThisPage(&pfFileHandle, 1, &pfPageHandle))) {
    return rc;
  }

  indexHandle->bOpen = true;
  if (rc = GetData(&pfPageHandle, &data)) {
    return rc;
  }
  indexHandle->fileHeader = *(IX_FileHeader *)data;
  indexHandle->fileHandle = pfFileHandle;
  if (rc = UnpinPage(&pfPageHandle)) {
    return rc;
  }

  return SUCCESS;
}

//关闭索引文件
RC CloseIndex(IX_IndexHandle *indexHandle) {
  PF_FileHandle fileHandle = indexHandle->fileHandle;
  CloseFile(&fileHandle);
  return SUCCESS;
}

RC OpenIndexScan(IX_IndexScan *indexScan, IX_IndexHandle *indexHandle,
                 CompOp compOp, char *value) {
  return SUCCESS;
}

RC IX_GetNextEntry(IX_IndexScan *indexScan, RID *rid) { return SUCCESS; }

RC CloseIndexScan(IX_IndexScan *indexScan) { return SUCCESS; }

RC GetIndexTree(char *fileName, Tree *index) { return SUCCESS; }
