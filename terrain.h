

#if !defined(TERRAIN_H)
#define TERRAIN_H

#include <vector>
#include "bitmap.h"
#include "mathlib.h"

class HeightMap
{
public:
    HeightMap();
    ~HeightMap();

    float getHeightScale() const
    { return m_heightScale; }

    int getSize() const
    { return m_size; }

    int getGridSpacing() const
    { return m_gridSpacing; }

    const float *getHeights() const
    { return &m_heights[0]; }

    bool create(int size, int gridSpacing, float scale);
    void destroy();

    void generateDiamondSquareFractal(float roughness);

    float heightAt(float x, float z) const;

    float heightAtPixel(int x, int z) const
    { return m_heights[z * m_size + x]; };

    void normalAt(float x, float z, Vector3 &n) const;
    void normalAtPixel(int x, int z, Vector3 &n) const;

private:
    void blur(float amount);
    unsigned int heightIndexAt(int x, int z) const;
    void smooth();

    int m_size;
    int m_gridSpacing;
    float m_heightScale;
    std::vector<float> m_heights;
};


class Terrain
{
public:
    Terrain();
    virtual ~Terrain();

    bool create(int size, int gridSpacing, float scale);
    void destroy();
    void draw();
    bool generateUsingDiamondSquareFractal(float roughness);
    void update(const Vector3 &cameraPos);

    const HeightMap &getHeightMap() const
    { return m_heightMap; }

    HeightMap &getHeightMap()
    { return m_heightMap; }

protected:
    virtual bool terrainCreate(int size, int gridSpacing, float scale);
    virtual void terrainDestroy();
    virtual void terrainDraw();
    virtual void terrainUpdate(const Vector3 &cameraPos);

private:
    struct Vertex
    {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
    };

    bool generateIndices();
    bool generateVertices();
    
    bool use16BitIndices() const
    { return m_totalVertices <= 65536; }

    unsigned int m_vertexBuffer;
    unsigned int m_indexBuffer;
    int m_totalVertices;
    int m_totalIndices;
    HeightMap m_heightMap;
};

#endif