#ifndef IX_MANAGER_H_H
#define IX_MANAGER_H_H

#include <cmath>
#include <cstdlib>
#include "PF_Manager.h"
#include "RM_Manager.h"

typedef struct {
  int attrLength;
  int keyLength;
  AttrType attrType;
  PageNum rootPage;
  PageNum first_leaf;
  int order;
} IX_FileHeader;

typedef struct {
  bool bOpen;
  PF_FileHandle fileHandle;
  IX_FileHeader fileHeader;
} IX_IndexHandle;

typedef struct {
  int is_leaf;
  int keynum;
  PageNum parent;
  int parentOrder;
  PageNum brother;
  char *keys;
  RID *rids;
} IX_Node;

typedef struct {
  bool bOpen;
  IX_IndexHandle *pIXIndexHandle;
  CompOp compOp;
  char *value;
  PF_PageHandle *pfPageHandle;
  PageNum pnNext;
  int ridIx;
  IX_Node *currentPageControl;
} IX_IndexScan;

typedef struct Tree_Node {
  int keyNum;
  char **keys;
  Tree_Node *parent;
  Tree_Node *sibling;
  Tree_Node *firstChild;
} Tree_Node;

typedef struct {
  AttrType attrType;
  int attrLength;
  int order;
  Tree_Node *root;
} Tree;

RC CreateIndex(const char *fileName, AttrType attrType, int attrLength);
RC OpenIndex(const char *fileName, IX_IndexHandle *indexHandle);
RC CloseIndex(IX_IndexHandle *indexHandle);
RC InsertEntry(IX_IndexHandle *indexHandle, void *pData, const RID *rid);
RC DeleteEntry(IX_IndexHandle *indexHandle, void *pData, const RID *rid);
RC OpenIndexScan(IX_IndexScan *indexScan, IX_IndexHandle *indexHandle,
                 CompOp compOp, char *value);
RC IX_GetNextEntry(IX_IndexScan *indexScan, RID *rid);
RC CloseIndexScan(IX_IndexScan *indexScan);
RC GetIndexTree(char *fileName, Tree *index);
#endif
