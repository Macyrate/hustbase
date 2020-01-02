#ifndef IX_HELPER_H_H
#define IX_HELPER_H_H
#include "IX_Manager.h"

int addKey(char *key, RID *val, int *effectiveLength, char *keyInsert,
           const RID *valInsert, AttrType type, int attrLength);
int removeKey(char *key, RID *val, int *eLength, char *keyDelete, AttrType type,
              int attrLength);
int getNodeByKey(IX_IndexHandle *indexHandle, void *targetKey);
int addKeyShift(int keyOffset, char *key, RID *val, int *effectiveLength,
                char *keyInsert, const RID *valInsert, int attrLength);
void removeKeyShift(int keyOffset, char *key, RID *val, int *eLength,
                    int attrLength);
void _insert(IX_IndexHandle *indexHandle, void *pData, const RID *rid,
             PF_PageHandle *pageInsert);
RC _delete(IX_IndexHandle *indexHandle, void *pData, const RID *rid,
           PF_PageHandle *pageDelete);
void getRightPageData(PF_PageHandle *pageHandle, PF_PageHandle *rightHandle,
                      int order, AttrType attrType, int attrLength,
                      const int threshold, int &status);
void getFromLeft(PF_PageHandle *pageHandle, PF_PageHandle *leftHandle,
                 int order, AttrType attrType, int attrLength,
                 const int threshold, int &status);
void getPageDataFromBrother(PF_PageHandle *pageHandle, PF_FileHandle *fileHandle,
                    const int order, const AttrType attrType,
                    const int attrLength, const int threshold);
void getLeftBrother(PF_PageHandle *pageHandle, PF_FileHandle *fileHandle,
                     const int order, const AttrType attrType,
                     const int attrLength, PageNum &leftBrother);
void deleteOrAlterParentNode(PF_PageHandle *parentPageHandle,
                             PF_FileHandle *fileHandle, int order,
                             AttrType attrType, int attrLength,
                             PageNum nodePageNum, void *pData, int parentOrder,
                             bool isDelete);
bool compareLeafValue(float left, float right, CompOp oper);
bool compareLeafString(char *left, char *right, CompOp oper);
#endif