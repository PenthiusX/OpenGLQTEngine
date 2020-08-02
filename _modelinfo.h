#ifndef _MODELINFO_H
#define _MODELINFO_H
#include <Qstring>
#include <vector>
#include <glm/glm.hpp>

struct Vertex {//added Aug.2020
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class _ModelInfo
{
public:
    _ModelInfo();

    void setName(QString name);
    void setPath(QString path);
    void setIsLoaded(bool isLoaded);
    void setVertexArray(std::vector<float> vertexArray);
    void setVertexInfoArray(std::vector<Vertex> vertexInfoArray);
    void setIndexArray(std::vector<uint> indexAarray);
    void setNormalArray(std::vector<float>norArray);
    void setMaxExtents(glm::vec4 max);
    void setMinExtents(glm::vec4 min);
    void setCentroid(glm::vec4 cent);
    //
    QString getName() const;
    QString getPath() const;
    std::vector<float> getVertexArray()const;
    std::vector<Vertex> getVertexInfoArray();
    std::vector<uint> getIndexArray()const;
    std::vector<float> getNormalArray()const;
    glm::vec4 getMaxExtent() const;
    glm::vec4 getMinExtent() const;
    glm::vec4 getCentroid() const;
    bool getIsLoaded() const;

    void calcMinMaxExtents();
    glm::vec4 calcCentroidFromMinMax();
    glm::vec4 calcCentroidFromMinMax(glm::vec3 min, glm::vec3 max);

private:
    QString name;//the tag
    QString path;//the relative path of the model data file
    std::vector<Vertex> vertexInfoArray;//Holds vertex,normal and texcordData
    std::vector<float> vertexArray;// vertices read from modelFile into memory
    std::vector<uint> indexAarray;// indeices read from modelFile into memory
    std::vector<float> normalArray;// normal array
    //
    glm::vec4 max;//max extent.
    glm::vec4 min;//min extent.
    glm::vec4 cent;//centroid from min & max.
    bool isLoaded;
};

#endif // _MODELINFO_H
