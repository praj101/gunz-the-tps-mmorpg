// 특정 메모리 블럭의 배열과 그 정보를 하나의 메모리 블럭에 넣기 위한 함수들
#pragma once
/// nOneBlobSize만큼 nBlobCount갯수만큼 배열한 블럭 만들기
void* CCMakeBlobArray(int nOneBlobSize, int nBlobCount);
/// 블럭 지우기
void CCEraseBlobArray(void* pBlob);
/// 블럭에서 각 원소 얻어내기
void* CCGetBlobArrayElement(void* pBlob, int i);
/// 블럭의 갯수 얻어내기
int CCGetBlobArrayCount(void* pBlob);
/// 블걱 전체 사이즈 얻어내기
int CCGetBlobArraySize(void* pBlob);
/// Array Block으로 보고 맨 처음 포인터 얻어내기
void* CCGetBlobArrayPointer(void* pBlob);
