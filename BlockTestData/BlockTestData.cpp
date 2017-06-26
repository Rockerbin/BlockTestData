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

static const UINT FLASH_MAX_ECC = 68;
static const UINT FLASH_BAD_COL_DATA_LENTH = 4096 * 4 * 4 * 4;
static const UINT FLASH_SLC_PAGE_CNT = 288;
static const UINT FLASH_TLC_PAGE_CNT = 576;
static const UINT FLASH_CE_CNT = 2;
static const UINT FLASH_PLANE_CNT = 2;
static const UINT FLASH_BLOCK_CNT = 1880;
static const UINT GOOD_BLK_ECC = 0;
static const UINT BAD_BLK_ECC = 0xFF;
vector<vector<BYTE>> g_vecBlkEcc;
set<UINT> g_setMixBindBlk;
set<UINT> g_setEnReplacebadBlk;
UINT g_uMaxBadBlkCeNo = 0, g_uMaxBadBlkPlaneNo = 0, g_uMaxBadBlkCnt = 0;
#define DIR_PATH "C:\\Users\\Administrator\\Desktop\\8293NFTL\\BinCard\\8293\\log"
#define SRC_DID_PATH "C:\\Users\\Administrator\\Desktop\\8293NFTL\\BinCard\\8293\\log\\20170623170641_Q"

BOOL IsAllNFBadBlk(UINT _uAFBlkNo)
{
	BOOL bAllNFBadBlk = TRUE;
	for (UINT uCeNo = 0; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		for (UINT uPlaneNo = 0; uPlaneNo < FLASH_PLANE_CNT; uPlaneNo++)
		{
			if (!(g_vecBlkEcc[uCeNo][_uAFBlkNo*FLASH_PLANE_CNT+uPlaneNo] > FLASH_MAX_ECC))
			{
				bAllNFBadBlk = FALSE;
				break;
			}
		}

		if (!bAllNFBadBlk)
		{
			break;
		}
	}
	return bAllNFBadBlk;
}

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

BOOL GenerateFormData(const UINT _uMixedBindBlkCnt)
{
	if (_uMixedBindBlkCnt < g_setMixBindBlk.size() || _uMixedBindBlkCnt < g_setEnReplacebadBlk.size())
	{
		cout <<"创建混合绑定块块数小于坏块数 无法生成数据"<< endl;
		return FALSE;
	}

	UINT uSpecialCeNo, uSpecialPlaneNo;
	UINT uCnt= 0;
	UINT uPer = 0, uLastPer = 1000;

	srand((unsigned int)time(NULL));

	uSpecialCeNo = g_uMaxBadBlkCeNo;
	uSpecialPlaneNo = g_uMaxBadBlkPlaneNo;

	while (1)
	{
		uCnt++;
		UINT uRandBlkNo = RandU32()%(FLASH_BLOCK_CNT/FLASH_PLANE_CNT);
		if ((g_setMixBindBlk.end() == g_setMixBindBlk.find(uRandBlkNo)) 
			&& (g_setEnReplacebadBlk.end() == g_setEnReplacebadBlk.find(uRandBlkNo)))
		{
			// 置坏快
			for (UINT uCeNo = 0; uCeNo < FLASH_CE_CNT; uCeNo++)
			{
				UINT uTmpBlk = uRandBlkNo*FLASH_PLANE_CNT;
				for (UINT uPlaneNo = 0; uPlaneNo < FLASH_PLANE_CNT; uPlaneNo++)
				{
					if (((uTmpBlk+uPlaneNo)%FLASH_PLANE_CNT != uSpecialPlaneNo) || (uCeNo != uSpecialCeNo))
					{
						g_vecBlkEcc[uCeNo][uTmpBlk+uPlaneNo] = BAD_BLK_ECC;
					}
				}
			}
	
			g_setMixBindBlk.insert(uRandBlkNo);
		}
		uPer = g_setMixBindBlk.size()*100/ _uMixedBindBlkCnt;
		if (uLastPer != uPer)
		{
			uLastPer = uPer;
			cout << "次数:" << uCnt << " 混合绑定块:" << g_setMixBindBlk.size() <<" 完成百分比 "<< uPer << "%" << "\r";
			if (g_setMixBindBlk.size() == _uMixedBindBlkCnt)
			{
				break;
			}
		}
	}

	cout << endl;

	// 构造特殊列的个数要和特殊绑定块一样
	uCnt = 0;
	UINT uSpecialBlkCnt = 0;
	if (_uMixedBindBlkCnt <= (FLASH_BLOCK_CNT/FLASH_PLANE_CNT)/2)
	{
		uSpecialBlkCnt = _uMixedBindBlkCnt;
	}
	else
	{
		uSpecialBlkCnt = (FLASH_BLOCK_CNT/FLASH_PLANE_CNT) - g_setMixBindBlk.size();
	}

	uPer = 0; 
	uLastPer = 1000;
	while(1)
	{
		uCnt++;
		UINT uRandBlkNo = RandU32()%(FLASH_BLOCK_CNT/FLASH_PLANE_CNT);
		UINT uTmpBlk = uRandBlkNo*FLASH_PLANE_CNT;
		if ((g_setMixBindBlk.end() == g_setMixBindBlk.find(uRandBlkNo)) 
			&& (g_setEnReplacebadBlk.end() == g_setEnReplacebadBlk.find(uRandBlkNo)))
		{
			// 置坏快
			g_vecBlkEcc[uSpecialCeNo][uTmpBlk+uSpecialPlaneNo] = BAD_BLK_ECC;
			g_setEnReplacebadBlk.insert(uRandBlkNo);
		}


		if (uSpecialBlkCnt != 0)
		{
			uPer = g_setEnReplacebadBlk.size() *100/ uSpecialBlkCnt;
		}
		else
		{
			uPer = 100;
		}

		if (uLastPer != uPer)
		{
			uLastPer = uPer;
			cout << "次数:" << uCnt << " 基准块:" << g_setEnReplacebadBlk.size() << " 完成百分比 "<< uPer << "%" << "\r";
			if (g_setEnReplacebadBlk.size() == uSpecialBlkCnt)
			{
				break;
			}
		}
	}

	cout << endl;
	return TRUE;
}

void ReadSrcBlkData()
{
	UINT uBlkDataLenth = FLASH_BLOCK_CNT*FLASH_CE_CNT;
	HANDLE handleSLCFile, handleTLCFile;
    BYTE* pSlcBlkData = new BYTE[FLASH_BAD_COL_DATA_LENTH];
	BYTE* pTlcBlkData = new BYTE[FLASH_BAD_COL_DATA_LENTH];
	// slc_bad_blk_tab.dat && tlc_bad_blk_tab.dat
	//////////////////////////////////////////////////////////////////////////////////////////////////
	string strSLCFilePath = SRC_DID_PATH;
	string strTLCFilePath = SRC_DID_PATH;
	strSLCFilePath += "\\slc_bad_blk_tab.dat";
	strTLCFilePath += "\\tlc_bad_blk_tab.dat";
	handleSLCFile = CreateFile(strSLCFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleSLCFile)
	{
		cout << "创建" << strSLCFilePath.c_str() << " 文件句柄失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		return;
	}
	handleTLCFile = CreateFile(strTLCFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handleTLCFile)
	{
		cout << "创建" << strTLCFilePath.c_str() << " 文件句柄失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		return;
	}

	DWORD uRealWriteNum = 0;
	if (!ReadFile(handleSLCFile, pSlcBlkData, uBlkDataLenth, &uRealWriteNum, NULL))
	{
		cout << "读" << strSLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		SAFE_DELETE_ARRAY(pSlcBlkData);
		return;
	}
	cout <<strSLCFilePath.c_str() <<endl;
	assert(uBlkDataLenth == uRealWriteNum);
	if (!ReadFile(handleTLCFile, pTlcBlkData, uBlkDataLenth, &uRealWriteNum, NULL))
	{
		cout << "读" << strTLCFilePath.c_str() << " 文件失败" << endl;
		// 关闭资源
		CloseHandle(handleSLCFile);
		CloseHandle(handleTLCFile);
		SAFE_DELETE_ARRAY(pSlcBlkData);
		return;
	}
	assert(uBlkDataLenth == uRealWriteNum);
	cout <<strTLCFilePath.c_str() <<endl;

	// 初始化FlashEcc结构
	g_vecBlkEcc.clear();
	g_vecBlkEcc.resize(FLASH_CE_CNT);
	for (UINT uCeNo =0 ; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		g_vecBlkEcc[uCeNo].resize(FLASH_BLOCK_CNT, GOOD_BLK_ECC);
		for (UINT uBlkNo = 0; uBlkNo < FLASH_BLOCK_CNT; uBlkNo++)
		{
			if ((pSlcBlkData[uCeNo*FLASH_BLOCK_CNT+uBlkNo] > FLASH_MAX_ECC) 
				&& (pTlcBlkData[uCeNo*FLASH_BLOCK_CNT+uBlkNo] > FLASH_MAX_ECC))
			{
				g_vecBlkEcc[uCeNo][uBlkNo] = BAD_BLK_ECC;
			}
		}
	}

	// 关闭资源
	CloseHandle(handleSLCFile);
	CloseHandle(handleTLCFile);
	SAFE_DELETE_ARRAY(pSlcBlkData);

	return;
}

typedef struct _BAD_BLK_DIS
{
	UINT uCeNo;
	UINT uPlaneNo;
	UINT uBadBlkCnt;
	UINT uBadBlkAfNo[FLASH_BLOCK_CNT/FLASH_CE_CNT];
}BAD_BLK_DIS, *PBAD_BLK_DIS;

BOOL Compare(const BAD_BLK_DIS &tmpBadBlkInfo1, const BAD_BLK_DIS &tmpBadBlkInfo2)
{
	return  tmpBadBlkInfo1.uBadBlkCnt>tmpBadBlkInfo2.uBadBlkCnt ? 1 :0;
}

void AnalysisBadBlkDataFromFile()
{
	g_setMixBindBlk.clear();
	g_setEnReplacebadBlk.clear();
	vector<BAD_BLK_DIS> flashBadBlkDis;

	UINT uBadBlkCnt[FLASH_CE_CNT*FLASH_PLANE_CNT] = {0};

	for (UINT uCeNo = 0; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		BAD_BLK_DIS tmpBADBlkDis = {0};
		tmpBADBlkDis.uCeNo = uCeNo;
		for (UINT uPlaneNo = 0; uPlaneNo < FLASH_PLANE_CNT; uPlaneNo++)
		{
			tmpBADBlkDis.uPlaneNo = uPlaneNo;
			flashBadBlkDis.push_back(tmpBADBlkDis);
		}

		for (UINT uBlkNo = 0; uBlkNo < FLASH_BLOCK_CNT; uBlkNo++)
		{
			if (g_vecBlkEcc[uCeNo][uBlkNo] > FLASH_MAX_ECC)
			{
				UINT uPlaneNo = uBlkNo%FLASH_PLANE_CNT;
				flashBadBlkDis[uCeNo*FLASH_PLANE_CNT+uPlaneNo].uBadBlkCnt++;
			}
		}
	}

	// 降序排序
	sort(flashBadBlkDis.begin(), flashBadBlkDis.end(), Compare);

	// 记录特殊绑定块队列
	g_uMaxBadBlkCeNo =flashBadBlkDis[0].uCeNo;
	g_uMaxBadBlkPlaneNo = flashBadBlkDis[0].uPlaneNo;
	g_uMaxBadBlkCnt = flashBadBlkDis[0].uBadBlkCnt;

	UINT uMixedBadBlkCnt = 0, uSpecialBlkCnt = 0;
	for (UINT uCeNo = 0; uCeNo < FLASH_CE_CNT; uCeNo++)
	{
		for (UINT uAFBlkNo = 0; uAFBlkNo < FLASH_BLOCK_CNT/FLASH_PLANE_CNT; uAFBlkNo++)
		{
			BOOL bBadAFBlk = FALSE, bBenchBadBlk = FALSE;
			for (UINT uPlaneNo = 0; uPlaneNo < FLASH_PLANE_CNT; uPlaneNo++)
			{
				if (g_vecBlkEcc[uCeNo][uAFBlkNo*FLASH_PLANE_CNT+uPlaneNo] > FLASH_MAX_ECC)
				{
					bBadAFBlk = TRUE;
					// 判断是否为基准坏块
					if ((uCeNo == g_uMaxBadBlkCeNo) && (uPlaneNo == g_uMaxBadBlkPlaneNo))
					{
						bBenchBadBlk = TRUE;
					}
				}
			}

			// 置对应的坏块队列
			if (bBadAFBlk)
			{
				if (!IsAllNFBadBlk(uAFBlkNo))
				{
					if (bBenchBadBlk)
					{
						g_setEnReplacebadBlk.insert(uAFBlkNo);
					}
					else
					{
						g_setMixBindBlk.insert(uAFBlkNo);
					}
				}
			}
		}
	}

	cout << "从坏块中整理的混合绑定块个数：" << g_setMixBindBlk.size() << endl;
	cout << "从坏块中整理的基准块个数：" << g_setEnReplacebadBlk.size() << endl;
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
		cout << "读取原始数据..." << endl;
		ReadSrcBlkData();

		cout << "开始分析并创建混合绑定块数据..." << endl;
		AnalysisBadBlkDataFromFile();
		BOOL bRet = GenerateFormData(uMixedBindBlkCnt);
		if (bRet)
		{
			cout << "保存数据文件..." << endl;
			SaveDataToBlkFile();
			SaveDataToPageFile();
			SaveBadColData();
		}

		cout << "=============================================================" << endl;
	}

	return 0;
}
