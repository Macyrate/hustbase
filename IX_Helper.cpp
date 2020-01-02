#include "StdAfx.h"
#include "IX_Helper.h"

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
