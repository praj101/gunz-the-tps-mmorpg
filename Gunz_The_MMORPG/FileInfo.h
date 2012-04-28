#pragma once

#include <windows.h>
#include <time.h>

//Get the file's last updated time
BOOL GetLastUpdate(const char *pFileName, FILETIME *ft);

//File been updated?
//PFileName
//Ot: comparison of time (usually hours when open)
BOOL IsModifiedOutside(const char *pFileName, FILETIME ot);

//Extension Removal
BOOL RemoveExtension(char *pRemoveExt, const char *pFileName);

//Extension Replacement
void ReplaceExtension(char *pTargetName, const char *pSourceName, char *pExt);

//Get the relative path
void GetRelativePath(char *pRelativePath, const char *pBasePath, const char *pPath);

//Get the absolute path
void GetFullPath(char *pFullPath, const char *pBasePath, const char *pRelativePath);
//Get absolute path to another. relative to the current directory.
void GetFullPath(char *pFullPath, const char *pRelativePath);

//Path ten thousand.
void GetPurePath(char *pPurePath,const char *pFilename);
//Path and file name extension, removed to obtain pure.
void GetPureFilename(char *pPureFilename,const char *pFilename);
//Extension gets the
void GetPureExtension(char *pPureExtension,const char *pFilename);

//Is the absolute path? (The network path is not supported)
BOOL IsFullPath(const char *pPath);

//Header read (int start of the header has a total size of the header.
BOOL ReadHeader(HANDLE hFile, void *pHeader, int nHeaderSize);

//File of the CheckSum is obtained. (Date, size, and the contents of the file)
DWORD GetFileCheckSum(char* pszFileName);

//File exist?
bool IsExist(const char *filename);

//Get the parent directory name.
void GetParentDirectory(char* pszFileName);

//The given path to the directory is created.
bool MakePath(const char* pszFileName);

//Time_t to convert FILETIME Code from MSDN
void time_tToFILETIME(time_t t, LPFILETIME pft);

//Set the file WriteTime
BOOL MSetFileTime(LPCTSTR lpszPath, FILETIME ft);

bool GetMyDocumentsPath(char* path);

bool CreatePath(char* path);

