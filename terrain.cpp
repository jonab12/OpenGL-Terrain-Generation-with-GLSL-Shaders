

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "opengl.h"
#include "terrain.h"

//-----------------------------------------------------------------------------
// HeightMap.
//-----------------------------------------------------------------------------

HeightMap::HeightMap() : m_size(0), m_gridSpacing(0), m_heightScale(1.0f)
{
}

HeightMap::~HeightMap()
{
    destroy();
}

bool HeightMap::create(int size, int gridSpacing, float scale)
{
    m_heightScale = scale;
    m_size = size;
    m_gridSpacing = gridSpacing;

    try
    {
        m_heights.resize(m_size * m_size);
    }
    catch (const std::bad_alloc &)
    {
        return false;
    }

    memset(&m_heights[0], 0, m_heights.size());
    return true;
}

void HeightMap::destroy()
{
    m_heightScale = 1.0f;
    m_size = 0;
    m_gridSpacing = 0;
    m_heights.clear();
}

void HeightMap::generateDiamondSquareFractal(float roughness)
{
    // Generates a fractal height field using the diamond-square (midpoint
    // displacement) algorithm. Note that only square height fields work with
    // this algorithm.
    //
    // Based on article and associated code:
    // "Fractal Terrain Generation - Midpoint Displacement" by Jason Shankel
    // (Game Programming Gems I, pp.503-507).

    srand(static_cast<unsigned int>(time(0)));

    std::fill(m_heights.begin(), m_heights.end(), 0.0f);

    int p1, p2, p3, p4, mid;
    float dH = m_size * 0.5f;
    float dHFactor = powf(2.0f, -roughness);
    float minH = 0.0f, maxH = 0.0f;

    for (int w = m_size; w > 0; dH *= dHFactor, w /= 2)
    {
        // Diamond Step.
        for (int z = 0; z < m_size; z += w)
        {
            for (int x = 0; x < m_size; x += w)
            {
                p1 = heightIndexAt(x, z);
                p2 = heightIndexAt(x + w, z);
                p3 = heightIndexAt(x + w, z + w);
                p4 = heightIndexAt(x, z + w);
                mid = heightIndexAt(x + w / 2, z + w / 2);

                m_heights[mid] = Math::random(-dH, dH) + (m_heights[p1] + m_heights[p2] + m_heights[p3] + m_heights[p4]) * 0.25f;

                minH = min(minH, m_heights[mid]);
                maxH = max(maxH, m_heights[mid]);
            }
        }

        // Square step.
        for (int z = 0; z < m_size; z += w)
        {
            for (int x = 0; x < m_size; x += w)
            {
                p1 = heightIndexAt(x, z);
                p2 = heightIndexAt(x + w, z);
                p3 = heightIndexAt(x + w / 2, z - w / 2);
                p4 = heightIndexAt(x + w / 2, z + w / 2);
                mid = heightIndexAt(x + w / 2, z);

                m_heights[mid] = Math::random(-dH, dH) + (m_heights[p1] + m_heights[p2] + m_heights[p3] + m_heights[p4]) * 0.25f;

                minH = min(minH, m_heights[mid]);
                maxH = max(maxH, m_heights[mid]);

                p1 = heightIndexAt(x, z);
                p2 = heightIndexAt(x, z + w);
                p3 = heightIndexAt(x + w / 2, z + w / 2);
                p3 = heightIndexAt(x - w / 2, z + w / 2);
                mid = heightIndexAt(x, z + w / 2);

                m_heights[mid] = Math::random(-dH, dH) + (m_heights[p1] + m_heights[p2] + m_heights[p3] + m_heights[p4]) * 0.25f;

                minH = min(minH, m_heights[mid]);
                maxH = max(maxH, m_heights[mid]);
            }
        }
    }

    smooth();

    // Normalize height field so altitudes fall into range [0,255].
    for (int i = 0; i < m_size * m_size; ++i)
        m_heights[i] = 255.0f * (m_heights[i] - minH) / (maxH - minH);
}

float HeightMap::heightAt(float x, float z) const
{
    // Given a (x, z) position on the rendered height map this method
    // calculates the exact height of the height map at that (x, z)
    // position using bilinear interpolation.

    x /= static_cast<float>(m_gridSpacing);
    z /= static_cast<float>(m_gridSpacing);

    assert(x >= 0.0f && x < float(m_size));
    assert(z >= 0.0f && z < float(m_size));

    long ix = Math::floatToLong(x);
    long iz = Math::floatToLong(z);
    float topLeft = m_heights[heightIndexAt(ix, iz)] * m_heightScale;
    float topRight = m_heights[heightIndexAt(ix + 1, iz)] * m_heightScale;
    float bottomLeft = m_heights[heightIndexAt(ix, iz + 1)] * m_heightScale;
    float bottomRight = m_heights[heightIndexAt(ix + 1, iz + 1)] * m_heightScale;
    float percentX = x - static_cast<float>(ix);
    float percentZ = z - static_cast<float>(iz);

    return Math::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
}

void HeightMap::normalAt(float x, float z, Vector3 &n) const
{
    // Given a (x, z) position on the rendered height map this method
    // calculates the exact normal of the height map at that (x, z) position
    // using bilinear interpolation.

    x /= static_cast<float>(m_gridSpacing);
    z /= static_cast<float>(m_gridSpacing);

    assert(x >= 0.0f && x < float(m_size));
    assert(z >= 0.0f && z < float(m_size));

    long ix = Math::floatToLong(x);
    long iz = Math::floatToLong(z);

    float percentX = x - static_cast<float>(ix);
    float percentZ = z - static_cast<float>(iz);

    Vector3 topLeft;
    Vector3 topRight;
    Vector3 bottomLeft;
    Vector3 bottomRight;
    Vector3 normal;

    normalAtPixel(ix, iz, topLeft);
    normalAtPixel(ix + 1, iz, topRight);
    normalAtPixel(ix, iz + 1, bottomLeft);
    normalAtPixel(ix + 1, iz + 1, bottomRight);

    n = Math::bilerp(topLeft, topRight, bottomLeft, bottomRight, percentX, percentZ);
    n.normalize();
}

void HeightMap::normalAtPixel(int x, int z, Vector3 &n) const
{
    // Returns the normal at the specified location on the height map.
    // The normal is calculated using the properties of the height map.
    // This approach is much quicker and more elegant than triangulating the
    // height map and averaging triangle surface normals.

    if (x > 0 && x < m_size - 1)
        n.x = heightAtPixel(x - 1, z) - heightAtPixel(x + 1, z);
    else if (x > 0)
        n.x = 2.0f * (heightAtPixel(x - 1, z) - heightAtPixel(x, z));
    else
        n.x = 2.0f * (heightAtPixel(x, z) - heightAtPixel(x + 1, z));

    if (z > 0 && z < m_size - 1)
        n.z = heightAtPixel(x, z - 1) - heightAtPixel(x, z + 1);
    else if (z > 0)
        n.z = 2.0f * (heightAtPixel(x, z - 1) - heightAtPixel(x, z));
    else
        n.z = 2.0f * (heightAtPixel(x, z) - heightAtPixel(x, z + 1));

    n.y = 2.0f * m_gridSpacing;
    n.normalize();
}

void HeightMap::blur(float amount)
{
    // Applies a simple FIR (Finite Impulse Response) filter across the height
    // map to blur it. 'amount' is in range [0,1]. 0 is no blurring, and 1 is
    // very strong blurring.

    float *pPixel = 0;
    float *pPrevPixel = 0;
    int width = m_size;
    int height = m_size;
    int pitch = m_size;

    // Blur horizontally. Both left-to-right, and right-to-left.
    for (int i = 0; i < height; ++i)
    {
        pPrevPixel = &m_heights[i * pitch];

        // Left-to-right.
        for (int j = 1; j < width; ++j)
        {
            pPixel = &m_heights[(i * pitch) + j];
            *pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
            pPrevPixel = pPixel;
        }

        pPrevPixel = &m_heights[(i * pitch) + (width - 1)];

        // Right-to-left.
        for (int j = width - 2; j >= 0; --j)
        {
            pPixel = &m_heights[(i * pitch) + j];
            *pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
            pPrevPixel = pPixel;
        }
    }

    // Blur vertically. Both top-to-bottom, and bottom-to-top.
    for (int i = 0; i < width; ++i)
    {
        pPrevPixel = &m_heights[i];

        // Top-to-bottom.
        for (int j = 1; j < height; ++j)
        {
            pPixel = &m_heights[(j * pitch) + i];
            *pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
            pPrevPixel = pPixel;
        }

        pPrevPixel = &m_heights[((height - 1) * pitch) + i];

        // Bottom-to-top.
        for (int j = height - 2; j >= 0; --j)
        {
            pPixel = &m_heights[(j * pitch) + i];
            *pPixel = (*pPrevPixel * amount) + (*pPixel * (1.0f - amount));
            pPrevPixel = pPixel;
        }
    }
}

unsigned int HeightMap::heightIndexAt(int x, int z) const
{
    // Given a 2D height map coordinate, this method returns the index
    // into the height map. This method wraps around for coordinates larger
    // than the height map size.
    return (((x + m_size) % m_size) + ((z + m_size) % m_size) * m_size);
}

void HeightMap::smooth()
{
    // Applies a box filter to the height map to smooth it out.

    std::vector<float> source(m_heights);
    float value = 0.0f;
    float cellAverage = 0.0f;
    int i = 0;
    int bounds = m_size * m_size;

    for (int y = 0; y < m_size; ++y)
    {
        for (int x = 0; x < m_size; ++x)
        {
            value = 0.0f;
            cellAverage = 0.0f;

            i = (y - 1) * m_size + (x - 1);
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = (y - 1) * m_size + x;
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = (y - 1) * m_size + (x + 1);
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = y * m_size + (x - 1);
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = y * m_size + x;
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = y * m_size + (x + 1);
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = (y + 1) * m_size + (x - 1);
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = (y + 1) * m_size + x;
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            i = (y + 1) * m_size + (x + 1);
            if (i >= 0 && i < bounds)
            {
                value += source[i];
                cellAverage += 1.0f;
            }

            m_heights[y * m_size + x] = value / cellAverage;
        }
    }
}

//-----------------------------------------------------------------------------
// Terrain.
//-----------------------------------------------------------------------------

Terrain::Terrain()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_totalVertices = 0;
    m_totalIndices = 0;
}

Terrain::~Terrain()
{
    destroy();
}

bool Terrain::create(int size, int gridSpacing, float scale)
{
    if (!m_heightMap.create(size, gridSpacing, scale))
        return false;

    return terrainCreate(size, gridSpacing, scale);
}

void Terrain::destroy()
{
    m_heightMap.destroy();
    terrainDestroy();
}

void Terrain::draw()
{
    terrainDraw();
}

bool Terrain::generateUsingDiamondSquareFractal(float roughness)
{
    m_heightMap.generateDiamondSquareFractal(roughness);
    return generateVertices();
}

void Terrain::update(const Vector3 &cameraPos)
{
    terrainUpdate(cameraPos);
}

bool Terrain::terrainCreate(int size, int gridSpacing, float scale)
{
    // Initialize the vertex buffer object.

    m_totalVertices = size * size;
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_totalVertices,0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialize the index buffer object.

    m_totalIndices = (size - 1) * (size * 2 + 1);
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    int indexSize = use16BitIndices() ? sizeof(unsigned short) : sizeof(unsigned int);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * m_totalIndices, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return generateIndices();
}

void Terrain::terrainDestroy()
{
    if (m_vertexBuffer)
    {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
        m_totalVertices = 0;
    }

    if (m_indexBuffer)
    {
        glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
        m_totalIndices = 0;
    }
}

void Terrain::terrainDraw()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(6 * sizeof(float)));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

    if (use16BitIndices())
        glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    else
        glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::terrainUpdate(const Vector3 &cameraPos)
{

}

bool Terrain::generateIndices()
{
    void *pBuffer = 0;
    int size = m_heightMap.getSize();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    
    if (!(pBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)))
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return false;
    }

    if (use16BitIndices())
    {
        unsigned short *pIndex = static_cast<unsigned short *>(pBuffer);

        for (int z = 0; z < size - 1; ++z)
        {
            if (z % 2 == 0)
            {
                for (int x = 0; x < size; ++x)
                {
                    *pIndex++ = static_cast<unsigned short>(x + z * size);
                    *pIndex++ = static_cast<unsigned short>(x + (z + 1) * size);
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = static_cast<unsigned short>((size - 1) + (z + 1) * size);
            }
            else
            {
                for (int x = size - 1; x >= 0; --x)
                {
                    *pIndex++ = static_cast<unsigned short>(x + z * size);
                    *pIndex++ = static_cast<unsigned short>(x + (z + 1) * size);
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = static_cast<unsigned short>((z + 1) * size);
            }
        }
    }
    else
    {
        unsigned int *pIndex = static_cast<unsigned int *>(pBuffer);

        for (int z = 0; z < size - 1; ++z)
        {
            if (z % 2 == 0)
            {
                for (int x = 0; x < size; ++x)
                {
                    *pIndex++ = x + z * size;
                    *pIndex++ = x + (z + 1) * size;
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = (size - 1) + (z + 1) * size;
            }
            else
            {
                for (int x = size - 1; x >= 0; --x)
                {
                    *pIndex++ = x + z * size;
                    *pIndex++ = x + (z + 1) * size;
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = (z + 1) * size;
            }
        }
    }   

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

bool Terrain::generateVertices()
{
    Vertex *pVertices = 0;
    Vertex *pVertex = 0;
    int currVertex = 0;
    int size = m_heightMap.getSize();
    int gridSpacing = m_heightMap.getGridSpacing();
    float heightScale = m_heightMap.getHeightScale();
    Vector3 normal;

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    pVertices = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    if (!pVertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return false;
    }

    for (int z = 0; z < size; ++z)
    {
        for (int x = 0; x < size; ++x)
        {
            currVertex = z * size + x;
            pVertex = &pVertices[currVertex];

            pVertex->x = static_cast<float>(x * gridSpacing);
            pVertex->y = m_heightMap.heightAtPixel(x, z) * heightScale;
            pVertex->z = static_cast<float>(z * gridSpacing);

            m_heightMap.normalAtPixel(x, z, normal);
            pVertex->nx = normal.x;
            pVertex->ny = normal.y;
            pVertex->nz = normal.z;
            
            pVertex->s = static_cast<float>(x) / static_cast<float>(size);
            pVertex->t = static_cast<float>(z) / static_cast<float>(size);
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}