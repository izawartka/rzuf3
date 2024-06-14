#pragma once
#include "common.h"
#include "assets_manager.h"

class RZUF3_AssetsManager;

class RZUF3_Asset {
public:
	static RZUF3_Asset* getInstance(std::string filepath);

	virtual bool load() = 0;
	virtual void unload() = 0;

	virtual void addReference() final;
	virtual void removeReference() final;
	virtual int getReferences() const final;

	virtual std::string getFilepath() const final;
	virtual void* getContent() = 0;
protected:
	std::string m_filepath;
	int m_references = 0;
};