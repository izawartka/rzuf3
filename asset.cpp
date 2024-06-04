#include "asset.h"

RZUF3_Asset* RZUF3_Asset::getInstance(std::string filepath)
{
    return nullptr;
}

void RZUF3_Asset::addReference()
{
    m_references++;
}

void RZUF3_Asset::removeReference()
{
    m_references--;
}

int RZUF3_Asset::getReferences() const
{
    return m_references;
}

std::string RZUF3_Asset::getFilepath() const
{
    return m_filepath;
}