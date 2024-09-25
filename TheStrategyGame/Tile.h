#pragma once
class Tile
{
private:
    int* mValue;
    int mNumLayers;
    float* mRotX;
    float* mSize;
public:
    Tile();
    ~Tile();
    int GetValue(int layer);
    void SetValue(int value, int layer);
    float GetRot(int layer);
    void SetRotation(float rot, int layer);
    float GetSize(int layer);
    void SetSize(float size, int layer);
    void SetNumLayers(int layers);
};

