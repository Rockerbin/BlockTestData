// AutoGenerateBadBlkInfo_Emmc_8291.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif

static const UINT FLASH_BAD_COL_DATA_LENTH = 4096 * 4 * 4 * 4;
static const UINT FLASH_SLC_PAGE_CNT = 128;
static const UINT FLASH_TLC_PAGE_CNT = 384;
static const UINT FLASH_CE_CNT = 1;
static const UINT FLASH_PLANE_CNT = 2;
static const UINT FLASH_BLOCK_CNT = 2892;
static const UINT GOOD_BLK_ECC = 0;
static const UINT BAD_BLK_ECC = 0xFF;
vector<vector<BYTE>> g_vecBlkEcc;
#define DIR_PATH "C:\\Users\\Administrator\\Desktop\\395\\BinCard\\8291\\log"

UINT RandU32()
{
	return (rand()<<30) + (rand()<<15) + rand();
}

void SaveDataToBlkFile()
{
	UINT uBlkDataLenth = FLASH_BLOCK_CNT*FLASH_CE_CNT;
	HANDLE handleSLCFile, handleTLCFile;

	// slc_bad_blk_tab.dat && tlc_bad_blk_tab.dat
	//////////////////////////////////////////////////////////////////////////////////////////////////
	string strSLCFilePath = DIR_PATH;
	string strTLCFilePath = DIR_PATH;
	strSLCFilePath += "\\slc_bad_blk_tab.dat";
	strTLCFilePath += "\\tlc_bad_blk_tab.dat";
	handleSLCFile = CreateFile(strSLCFilePath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleSLCFile)
	{
		cout << "创建" << strSLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		return;
	}
	handleTLCFile = CreateFile(strTLCFilePath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleTLCFile)
	{
		cout << "创建" << strTLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		return;
	}
	BYTE *pBlkData = new BYTE[uBlkDataLenth];
	UINT ui = 0;
	for (UINT uCeNo = 0 ; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		for (UINT uBlkNo = 0; uBlkNo < FLASH_BLOCK_CNT; uBlkNo++)
		{
			pBlkData[ui++] = g_vecBlkEcc[uCeNo][uBlkNo];
		}
	}
	
	DWORD uRealWriteNum = 0;
	if (!WriteFile(handleSLCFile, pBlkData, uBlkDataLenth, &uRealWriteNum, NULL))
	{
		cout << "写" << strSLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		SAFE_DELETE_ARRAY(pBlkData);
		return;
	}
	cout <<strSLCFilePath.c_str() <<endl;
 	assert(uBlkDataLenth == uRealWriteNum);
 	if (!WriteFile(handleTLCFile, pBlkData, uBlkDataLenth, &uRealWriteNum, NULL))
	{
		cout << "写" << strTLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		SAFE_DELETE_ARRAY(pBlkData);
		return;
	}
 	assert(uBlkDataLenth == uRealWriteNum);
	cout <<strTLCFilePath.c_str() <<endl;
	// 关闭资源
	CloseHandle(handleSLCFile);
	CloseHandle(handleTLCFile);
	SAFE_DELETE_ARRAY(pBlkData);

	return;
}

void SaveDataToPageFile()
{
	UINT uSlcPageDataLenth = FLASH_SLC_PAGE_CNT*FLASH_BLOCK_CNT*FLASH_CE_CNT;
	UINT uTlcPageDataLenth = FLASH_TLC_PAGE_CNT*FLASH_BLOCK_CNT*FLASH_CE_CNT;
	HANDLE handleSLCFile, handleTLCFile;

	// slc_bad_page_tab.dat && tlc_bad_page_tab.dat
	//////////////////////////////////////////////////////////////////////////////////////////////////
	string strSLCFilePath = DIR_PATH;
	string strTLCFilePath = DIR_PATH;
	strSLCFilePath += "\\slc_bad_page_tab.dat";
	strTLCFilePath += "\\tlc_bad_page_tab.dat";
	handleSLCFile = CreateFile(strSLCFilePath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleSLCFile)
	{
		cout << "创建" << strSLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		return;
	}
	handleTLCFile = CreateFile(strTLCFilePath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleTLCFile)
	{
		cout << "创建" << strTLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		return;
	}
	BYTE *pSlcPageData = new BYTE[uSlcPageDataLenth];
	BYTE *pTLlcageData = new BYTE[uTlcPageDataLenth];
	UINT ui = 0;
	UINT uj = 0;
	for (UINT uCeNo = 0 ; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		for (UINT uBlkNo = 0; uBlkNo < FLASH_BLOCK_CNT; uBlkNo++)
		{
				
			for (UINT uPageNo = 0; uPageNo < FLASH_SLC_PAGE_CNT; uPageNo++)
			{
				pSlcPageData[ui++] = g_vecBlkEcc[uCeNo][uBlkNo];
			}

			for (UINT uPageNo = 0; uPageNo < FLASH_TLC_PAGE_CNT; uPageNo++)
			{
				pTLlcageData[uj++] = g_vecBlkEcc[uCeNo][uBlkNo];
			}
		}	
	}

	DWORD uRealWriteNum = 0;
	if (!WriteFile(handleSLCFile, pSlcPageData, uSlcPageDataLenth, &uRealWriteNum, NULL))
	{
		cout << "写" << strSLCFilePath.c_str() << " 文件失败" << endl;
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		SAFE_DELETE_ARRAY(pSlcPageData);
		SAFE_DELETE_ARRAY(pTLlcageData);
		return;
	}
	assert(uSlcPageDataLenth == uRealWriteNum);
	cout <<strSLCFilePath.c_str() <<endl;
	if (!WriteFile(handleTLCFile, pTLlcageData, uTlcPageDataLenth, &uRealWriteNum, NULL))
	{
		cout << "写" << strTLCFilePath.c_str() << " 文件失败" << endl;
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		SAFE_DELETE_ARRAY(pSlcPageData);
		SAFE_DELETE_ARRAY(pTLlcageData);
		return;
	}
	assert(uTlcPageDataLenth == uRealWriteNum);
	cout <<strTLCFilePath.c_str() <<endl;

	// 关闭资源
	CloseHandle(handleSLCFile);
	CloseHandle(handleTLCFile);
	SAFE_DELETE_ARRAY(pSlcPageData);
	SAFE_DELETE_ARRAY(pTLlcageData);

	return;
}

void SaveBadColData()
{
	BYTE *pBadColData = new BYTE[FLASH_BAD_COL_DATA_LENTH];
	memset(pBadColData, 0, FLASH_BAD_COL_DATA_LENTH);

	// Bad_Column_Info.dat
	//////////////////////////////////////////////////////////////////////////////////////////////////
	string strFilePath = DIR_PATH;
	strFilePath += "\\Bad_Column_Info.dat";
	HANDLE handleFile = CreateFile(strFilePath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleFile)
	{
		cout << "创建" << strFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleFile);
		return;
	}

	DWORD uRealWriteNum = 0;
	if (!WriteFile(handleFile, pBadColData, FLASH_BAD_COL_DATA_LENTH, &uRealWriteNum, NULL))
	{
		cout << "写" << strFilePath.c_str() << " 文件失败" << endl;
		CloseHandle(handleFile);
		SAFE_DELETE_ARRAY(pBadColData);
		return;
	}
	assert(FLASH_BAD_COL_DATA_LENTH == uRealWriteNum);
	cout <<strFilePath.c_str() <<endl;
	CloseHandle(handleFile);
	SAFE_DELETE_ARRAY(pBadColData);
}

void GenerateFormData(const UINT _uMixedBindBlkCnt)
{
	set<UINT> setBadBlk;
	set<UINT> setMixBlk;
	UINT uSpecialCeNo, uSpecialPlaneNo;
	UINT uCnt= 0, uBlkCnt = 0;
	UINT uPer = 0, uLastPer = 1000;

	srand((unsigned int)time(NULL));

	uSpecialPlaneNo = RandU32()%FLASH_PLANE_CNT;
	uSpecialCeNo = RandU32()%FLASH_CE_CNT;

	g_vecBlkEcc.resize(FLASH_CE_CNT);
	for (UINT uCeNo =0 ; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		g_vecBlkEcc[uCeNo].resize(FLASH_BLOCK_CNT, GOOD_BLK_ECC);
	}

	while (1)
	{
		uCnt++;
		UINT uRandBlkNo = RandU32()%FLASH_BLOCK_CNT;

		if (setBadBlk.end() == setBadBlk.find(uRandBlkNo))
		{
			// 置坏快
			for (UINT uCeNo = 0; uCeNo < FLASH_CE_CNT; uCeNo++)
			{
				UINT uTmpBlk = uRandBlkNo/FLASH_PLANE_CNT*FLASH_PLANE_CNT;
				for (UINT uPlaneNo = 0; uPlaneNo < FLASH_PLANE_CNT; uPlaneNo++)
				{
					if (((uTmpBlk+uPlaneNo)%FLASH_PLANE_CNT != uSpecialPlaneNo) || (uCeNo != uSpecialCeNo))
					{
						g_vecBlkEcc[uCeNo][uTmpBlk+uPlaneNo] = BAD_BLK_ECC;
					}
				}
			}
	
			uBlkCnt++;
			setBadBlk.insert(uRandBlkNo);
		}
		uPer =uBlkCnt*100/ _uMixedBindBlkCnt;
		if (uLastPer != uPer)
		{
			uLastPer = uPer;
			cout << "次数:" << uCnt << " 混合绑定块:" << uBlkCnt <<" 完成百分比 "<< uPer << "%" << "\r";
			if (uBlkCnt == _uMixedBindBlkCnt)
			{
				break;
			}
		}
	}

	cout << endl;

	// 构造特殊列的个数要和特殊绑定块一样
	uCnt = 0;
	UINT uSpecialBlkCnt = 0;
	if (_uMixedBindBlkCnt <= FLASH_BLOCK_CNT/2)
	{
		uSpecialBlkCnt = _uMixedBindBlkCnt;
	}
	else
	{
		uSpecialBlkCnt = FLASH_BLOCK_CNT - uBlkCnt;
	}
	uBlkCnt = 0;
	uPer = 0; 
	uLastPer = 1000;
	while(1)
	{
		uCnt++;
		UINT uRandBlkNo = RandU32()%FLASH_BLOCK_CNT;
		BOOL bExistBadBlkQueue = FALSE;

		for (UINT uCeNo = 0; uCeNo < FLASH_CE_CNT; uCeNo++)
		{
			for (UINT uPlaneNo = 0; uPlaneNo < FLASH_PLANE_CNT; uPlaneNo++)
			{
				UINT uTmpBlk = uRandBlkNo/FLASH_PLANE_CNT*FLASH_PLANE_CNT;
				if (BAD_BLK_ECC == g_vecBlkEcc[uCeNo][uTmpBlk+uPlaneNo])
				{
					bExistBadBlkQueue = TRUE;
					break;
				}
			}
		}

		if (!bExistBadBlkQueue)
		{
			if (setMixBlk.end() == setMixBlk.find(uRandBlkNo))
			{
				if ((uRandBlkNo%FLASH_PLANE_CNT) == uSpecialPlaneNo)
				{
					// 置坏快
					g_vecBlkEcc[uSpecialCeNo][uRandBlkNo] = BAD_BLK_ECC;
					setMixBlk.insert(uRandBlkNo);
					uBlkCnt++;
				}
			}
		}

		if (uSpecialBlkCnt != 0)
		{
			uPer = uBlkCnt *100/ uSpecialBlkCnt;
		}
		else
		{
			uPer = 100;
		}

		if (uLastPer != uPer)
		{
			uLastPer = uPer;
			cout << "次数:" << uCnt << " 基准列块:" << uBlkCnt << " 完成百分比 "<< uPer << "%" << "\r";
			if (uBlkCnt == uSpecialBlkCnt)
			{
				break;
			}
		}
		}

	cout << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	while (1)
	{
		cout << "=============================================================" << endl;
		UINT uMixedBindBlkCnt = 0;
		while ((uMixedBindBlkCnt <= 0) || (uMixedBindBlkCnt > FLASH_BLOCK_CNT))
		{
			cout << "请输入要生成的混合块数:";
			cin >> uMixedBindBlkCnt;
		}
		GenerateFormData(uMixedBindBlkCnt);

		SaveDataToBlkFile();
		SaveDataToPageFile();
		SaveBadColData();

		cout << "=============================================================" << endl;
	}

	return 0;
}
