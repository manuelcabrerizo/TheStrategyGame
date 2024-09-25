#include "Tile.h"

#include <memory>

Tile::Tile()
{
    // Initialize internal vars
    mNumLayers = 0;
    mValue = nullptr;
    mRotX = nullptr;
    mSize = nullptr;
}

Tile::~Tile()
{
    if (mValue) delete[] mValue;
    if (mRotX)  delete[] mRotX;
    if (mSize)  delete[] mSize;
}

void Tile::SetNumLayers(int layers)
{
    if (mValue) delete[] mValue;
    if (mRotX)  delete[] mRotX;
    if (mSize)  delete[] mSize;

    mValue = new int[layers];
    memset(mValue, 0, layers * sizeof(int));
    mRotX = new float[layers];
    memset(mRotX, 0, layers * sizeof(float));
    mSize = new float[layers];
    memset(mSize, 0, layers * sizeof(float));

    mNumLayers = layers;
}

int Tile::GetValue(int layer)
{
    if (layer >= mNumLayers) 
    {
        return -1;
    }
    return mValue[layer];
}

void Tile::SetValue(int value, int layer)
{
    if (layer >= mNumLayers)
    {
        return;
    }
    mValue[layer] = value;
}

float Tile::GetRot(int layer)
{
    if (layer >= mNumLayers)
    {
        return -1.0f;
    }
    return mRotX[layer];
}

void Tile::SetRotation(float rot, int layer)
{
    if (layer >= mNumLayers) 
    {
        return;
    }
    mRotX[layer] = rot;
}

float Tile::GetSize(int layer)
{
    if (layer >= mNumLayers)
    {
        return -1.0f;
    }
    return mSize[layer];
}

void Tile::SetSize(float size, int layer)
{
    if (layer >= mNumLayers)
    {
        return;
    }
    mSize[layer] = size;
}
