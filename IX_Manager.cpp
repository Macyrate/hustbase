#include "StdAfx.h"
#include "IX_Manager.h"
#include "RM_Manager.h"
#include "IX_Helper.h"

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
