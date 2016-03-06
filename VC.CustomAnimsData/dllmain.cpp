#include <Windows.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <stdint.h>
#include "anim.h"
#include "CFileMgr.h"
#include "injector/injector.hpp"

std::fstream outFile;

//#define _TEST

struct CustomAnimationGroup 
{
	char pszGroupName[50]; 		
	char pszBlockName[50]; 					
	int nInitModelIndex; 				
	int nGroupAnimCount; 	
	std::vector<std::string> szAnimationNames;
	const char* pszAnimationNames[500];
	std::vector<StaticAnimInfoList> AnimInfos;
};

std::vector<CustomAnimationGroup> CustomAnimGroupArray;
StaticAnimationGroup* newAnimsGroup;

void ExportData()
{
	outFile.open("VC.CustomAnimsData.dat", std::ios::app | std::ios::ate);
	if (outFile.tellg())
		return;

	outFile << "# Usage: %man ped    1  173" << std::endl;
	outFile << "# man - GroupName, ped - BlockName, 1 - InitModelIndex, 173 - GroupAnimCount" << std::endl;
	outFile << "# then there's a list of animations with id and flags(173 lines, ids from 0 to 172)." << std::endl;
	outFile << "# To add another animation, increase GroupAnimCount and add animation name at the end of the list." << std::endl;
	outFile << "# For example: " << std::endl;
	outFile << "# %man ped    1  174" << std::endl;
	outFile << "# .................." << std::endl;
	outFile << "# walk_civi 173 354" << std::endl;
	outFile << "# now you can use id 173 in game." << std::endl;
	outFile << "# DON'T FORGET TO INCREASE GroupAnimCount!" << std::endl;
	outFile << std::endl;

	StaticAnimationGroup* AnimGroupArray = (StaticAnimationGroup*)0x6857B0;

	for (size_t i = 0; i < 61; i++)
	{
		outFile << "%" << AnimGroupArray[i].pszGroupName << " " << AnimGroupArray[i].pszBlockName << std::setw(5) << AnimGroupArray[i].nInitModelIndex << std::setw(5) << AnimGroupArray[i].nGroupAnimCount << std::endl;
		for (size_t k = 0; k < AnimGroupArray[i].nGroupAnimCount; k++)
		{
			const char** AnimName = AnimGroupArray[i].pszAnimationNames;
			StaticAnimInfoList* pAnimInfos = AnimGroupArray[i].pAnimationInfos;
			outFile << AnimName[k] << std::setw(45 - strlen(AnimName[k])) << pAnimInfos[k].nAnimationId << std::setw(45) << pAnimInfos[k].wAnimationFlags << std::endl;
		}
		outFile << std::endl;
	}
	outFile.close();
}

void LoadDatFile()
{
	if (FILE* hFile = CFileMgr::OpenFile("VC.CustomAnimsData.dat", "r"))
	{
		while (const char* pLine = CFileMgr::LoadLine(hFile))
		{
			if (pLine[0] && pLine[0] != '#')
			{
				if (pLine[0] == '%')
				{
					CustomAnimationGroup AnimGroup;
					sscanf(pLine + 1, "%s %s %d %d", &AnimGroup.pszGroupName, &AnimGroup.pszBlockName, &AnimGroup.nInitModelIndex, &AnimGroup.nGroupAnimCount);
					
					for (size_t i = 0; i < AnimGroup.nGroupAnimCount; i++)
					{
						pLine = CFileMgr::LoadLine(hFile);

						char name[50];
						StaticAnimInfoList info;
						sscanf(pLine, "%s %d %d", &name, &info.nAnimationId, &info.wAnimationFlags);
						AnimGroup.szAnimationNames.push_back(name);
						AnimGroup.AnimInfos.push_back(info);
					}
					CustomAnimGroupArray.push_back(AnimGroup);
				}
			}
		}
		CFileMgr::CloseFile(hFile);
	}


#ifdef _TEST
	std::ofstream outFile;
	outFile.open("VC.CustomAnimsData1.txt");
	for (auto it = CustomAnimGroupArray.cbegin(); it != CustomAnimGroupArray.cend(); it++)
	{
		outFile << "%" << it->pszGroupName << " " << it->pszBlockName << " " << it->nInitModelIndex << " " << it->nGroupAnimCount << " " << std::endl;
		for (size_t k = 0; k < it->nGroupAnimCount; k++)
		{
			outFile << it->szAnimationNames[k] << " " << it->AnimInfos[k].nAnimationId << " " << it->AnimInfos[k].wAnimationFlags << std::endl;
		}
		outFile << std::endl;
	}
	outFile.close();
#endif // _TEST
}

void Init()
{
	ExportData();
	LoadDatFile();

	newAnimsGroup = new StaticAnimationGroup[CustomAnimGroupArray.size()];

	for (size_t i = 0; i < CustomAnimGroupArray.size(); i++)
	{
		newAnimsGroup[i].pszGroupName = CustomAnimGroupArray[i].pszGroupName;
		newAnimsGroup[i].pszBlockName = CustomAnimGroupArray[i].pszBlockName;
		newAnimsGroup[i].nInitModelIndex = CustomAnimGroupArray[i].nInitModelIndex;
		newAnimsGroup[i].nGroupAnimCount = CustomAnimGroupArray[i].nGroupAnimCount;

		for (size_t k = 0; k < newAnimsGroup[i].nGroupAnimCount; k++)
		{
			CustomAnimGroupArray[i].pszAnimationNames[k] = CustomAnimGroupArray[i].szAnimationNames[k].c_str();
		}
		newAnimsGroup[i].pszAnimationNames = &CustomAnimGroupArray[i].pszAnimationNames[0];
		newAnimsGroup[i].pAnimationInfos = &CustomAnimGroupArray[i].AnimInfos[0];
	}

	injector::WriteMemory(0x40598A, &newAnimsGroup->pszGroupName, true); // = 0x6857B0 + 0x0 
	injector::WriteMemory(0x40545F, &newAnimsGroup->pszBlockName, true); // = 0x6857B0 + 0x4 
	injector::WriteMemory(0x405531, &newAnimsGroup->pszBlockName, true); // = 0x6857B0 + 0x4 
	injector::WriteMemory(0x4054C3, &newAnimsGroup->nInitModelIndex, true); // = 0x6857B0 + 0x8 
	injector::WriteMemory(0x405519, &newAnimsGroup->nGroupAnimCount, true); // = 0x6857B0 + 0xC 
	injector::WriteMemory(0x40551F, &newAnimsGroup->pszAnimationNames, true); // = 0x6857B0 + 0x10
	injector::WriteMemory(0x4054FF, &newAnimsGroup->pAnimationInfos, true); // = 0x6857B0 + 0x14
	injector::WriteMemory(0x405559, &newAnimsGroup->pAnimationInfos, true); // = 0x6857B0 + 0x14

#ifdef _TEST
	std::ofstream outFile;
	outFile.open("VC.CustomAnimsData2.txt");
	StaticAnimationGroup* AnimGroupArray = (StaticAnimationGroup*)newAnimsGroup;
	for (size_t i = 0; i < 61; i++)
	{
		outFile << "%" << AnimGroupArray[i].pszGroupName << " " << AnimGroupArray[i].pszBlockName << " " << AnimGroupArray[i].nInitModelIndex << " " << AnimGroupArray[i].nGroupAnimCount << " " << std::endl;
		for (size_t k = 0; k < AnimGroupArray[i].nGroupAnimCount; k++)
		{
			const char** AnimName = AnimGroupArray[i].pszAnimationNames;
			StaticAnimInfoList* pAnimInfos = AnimGroupArray[i].pAnimationInfos;
			outFile << AnimName[k] << " " << pAnimInfos[k].nAnimationId << " " << pAnimInfos[k].wAnimationFlags << std::endl;
		}
		outFile << std::endl;
	}
	outFile.close();
#endif // _TEST
}


BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Init();
	}
	return TRUE;
}