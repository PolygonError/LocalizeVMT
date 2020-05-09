/* 
 * LocalizeVMT
 * MIT License
 * Copyright (c) 2020 polygonerror
 *
 * I'M NOT GOOD AT VARIABLE NAMES I'M SORRY
 */ 

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "VTFLib.h"
#include "utility.hpp"

namespace fs = std::filesystem;
std::vector<std::string> unknownVMFs;

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		std::cout << "Error: need material path" << std::endl;
		return 1;
	}
	std::string temp = argv[1];
	std::replace(temp.begin(), temp.end(), '\\', '/');
	std::filesystem::path argPath = temp;
	
	std::vector<std::string> argFolderNames = Utility::split(argPath.string(), "/");
	std::vector<std::string>::iterator iArgMaterial = std::find(argFolderNames.begin(), argFolderNames.end(), "materials");

	if (!std::filesystem::is_directory(argPath) || iArgMaterial == argFolderNames.end()) {
		std::cout << "Error: provided path isn't valid" << std::endl;
		return 1;
	}

	//build base materials path
	std::string materialsPath;
	for (std::vector<std::string>::const_iterator i = argFolderNames.begin(); i != argFolderNames.end(); ++i) {
		if (i == iArgMaterial + 1) {
			break;
		}
		materialsPath += *i + "/";
	}

	//get all vmts in directory
	std::vector<std::string> vmtFiles;
	for (auto& p: fs::recursive_directory_iterator(argPath)) {
		if (p.path().string().rfind(".vmt") != std::string::npos) {
			vmtFiles.push_back(p.path().string());
		}
	}

	for (auto& vmt : vmtFiles) {
		std::replace(vmt.begin(), vmt.end(), '\\', '/');

		bool unknownPath = true; //if we can't solve where the texture is, we save it for later
		bool changed = false; //only save if there are changes

		VTFLib::CVMTFile* VMTFile = new VTFLib::CVMTFile();
		try {
			VMTFile->Load(vmt.c_str());
		} catch (const char* msg) {
			std::cout << std::endl << "Error: exception raised in " << vmt << " (" << msg << ")" << std::endl << std::endl;
			delete VMTFile;
 
			continue;
		}

		for (size_t i = 0; i < VMTFile->GetRoot()->GetNodeCount(); i++)
		{
			VTFLib::Nodes::CVMTNode* pVMTChild = VMTFile->GetRoot()->GetNode(i);
			if (pVMTChild->GetType() == NODE_TYPE_STRING)
			{
				std::string curString = (char*)static_cast<VTFLib::Nodes::CVMTStringNode*>(pVMTChild)->GetValue();
				std::replace(curString.begin(), curString.end(), '\\', '/');

				//I'M JUST ASSUMING ITS A FILE PATH IF IT HAS A SLASH
				//IF THERE ARE PARAMS THAT USE SLASHES OTHER THAN PATHS THEY CAN GET FUCKED (or just skipped over hopefully)
				if (curString.find('/') != std::string::npos) {
					std::vector<std::string> nodePathNames = Utility::split(curString, "/");
					//std::vector<std::string> VMTPathNames = Utility::split(vmt, "/");
					std::string curNodePath = materialsPath;

					//get the vtf path FROM the node
					//couldn't this be cleaned up with +=, add the og string then just put .vtf at the end
					for (std::vector<std::string>::const_iterator i = nodePathNames.begin(); i != nodePathNames.end(); ++i) {
						if (i == std::next(nodePathNames.end(), -1)) {
							curNodePath += *i + ".vtf";
							break;
						}
						curNodePath += *i + "/";
					}
					//Check if the material this node is asking for exists,
					//If it does, we can just skip it
					if (std::filesystem::is_regular_file(static_cast<std::filesystem::path>(curNodePath))) {
						continue;
					}
					std::vector<std::string> VMTPathNames = Utility::split(vmt.substr(vmt.find("materials/") + sizeof("materials/") - 1), "/");
					VMTPathNames.erase(VMTPathNames.end() - 1); //don't want the VMT itself
					
					for (int i = 1; i < VMTPathNames.size() + 1; i++) {
						curNodePath = materialsPath;
						for (int j = 0; j < i; j++)
						{
							curNodePath += VMTPathNames.at(j) + "/";
						}
						curNodePath += curString + ".vtf";
						if (std::filesystem::is_regular_file(static_cast<std::filesystem::path>(curNodePath))) {
							unknownPath = false;
							changed = true;
							std::string newString = curNodePath.substr(curNodePath.find("materials/") + sizeof("materials/") - 1);
							newString.erase(newString.find(".vtf")); //get rid of extension, VMT file doesn't need it

							static_cast<VTFLib::Nodes::CVMTStringNode*>(pVMTChild)->SetValue(newString.c_str());
							
							break;
						}
					}

					if (unknownPath) {
						std::cout << "Warning: couldn't solve paths in \"" << vmt << "\"" << std::endl;
						unknownVMFs.push_back(vmt);
						unknownPath = false; //make sure we only add this file once..
					}	
				}
			}
		}

		if (changed) {
			std::cout << "Saving \"" << vmt << "\"" << std::endl;
			VMTFile->Save(vmt.c_str());
		}

		delete VMTFile;
	}

	return 0;
}