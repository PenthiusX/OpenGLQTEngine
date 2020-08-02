#include "_renderer.h"
#include <iostream>
#include <_tools.h>
#include <qquaternion.h>


/*
 * The Renderer class
 *
 * Author: Aditya
*/
/*
 * The "QOpenGLExtraFunctions(QOpenGLContext::currentContext())" is passed by parameter
*/
_Renderer::_Renderer() : QOpenGLExtraFunctions(QOpenGLContext::currentContext())
{
    glEnable(GL_FRONT_AND_BACK);//shows bot back and front of the model
    glEnable(GL_BLEND);//for transparency in alpha values
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    glClearColor(0.1f, 0.1f, 0.3f, 1.0);//sets the bckground color of the openglContext.
    //
    shdr = new _Shader();//initialising the _shader() class * object.
    fboShader = new _Shader();
    setShader();//will run this shader by default.
    timer.start();
    //
    projectionMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    viewMatrix = glm::mat4(1.0f);
    pivotTmat = glm::mat4(1.0f);
    qDebug() << "render initialised ";
}
/*
 *Distructor: _Renderer Class
*/
_Renderer::~_Renderer()
{
    delete fboShader;
    delete shdr;
}
/*
*
* returns the current scene entity object.
*/
_SceneEntity _Renderer::getSceneEntity() const
{
    return sceneEntity;
}
/*
  ▪   ▐ ▄ ▪  ▄▄▄▄▄▪   ▄▄▄· ▄▄▌  ▪  ·▄▄▄▄•▄▄▄ .
  ██ •█▌▐███ •██  ██ ▐█ ▀█ ██•  ██ ▪▀·.█▌▀▄.▀·
  ▐█·▐█▐▐▌▐█· ▐█.▪▐█·▄█▀▀█ ██▪  ▐█·▄█▀▀▀•▐▀▀▪▄
  ▐█▌██▐█▌▐█▌ ▐█▌·▐█▌▐█ ▪▐▌▐█▌▐▌▐█▌█▌▪▄█▀▐█▄▄▌
  ▀▀▀▀▀ █▪▀▀▀ ▀▀▀ ▀▀▀ ▀  ▀ .▀▀▀ ▀▀▀·▀▀▀ • ▀▀▀
*/

/*
* Sets the sceen entity object locally and sets the
* shader ,Model data , projection matrix, texture,
* and initialises the modelMatrix.
*/
void _Renderer::initSceneEntityInRenderer(_SceneEntity s)
{
    sceneEntity = s;
    actualColor = sceneEntity.getColor();
    setShader(sceneEntity.getVertexShaderPath(), sceneEntity.getFragmentShaderPath());
    setupTexture(sceneEntity.getTexturePath());
    setModelMatrix(sceneEntity.getPostion(), sceneEntity.getScale(), sceneEntity.getRotation());

    //setModelDataInBuffers(sceneEntity.getModelInfo().getVertexArray(),sceneEntity.getModelInfo().getNormalArray(), sceneEntity.getModelInfo().getIndexArray());
    setModelDataInBuffers(sceneEntity.getModelInfo().getVertexArray(), sceneEntity.getModelInfo().getIndexArray());
}
/*
 sets a copy of sceneEntity in the renderer
*/
void _Renderer::setSceneEntityInRenderer(_SceneEntity s){
    sceneEntity = s;
}
/*
 *
*/
void _Renderer::keepSceneEntityUpdated(){
    //Keeps a copy of the current matrix info in the respective sceneEntity
    sceneEntity.setTranslationMatrix(translationMatrix);
    sceneEntity.setRotationmatrix(rotationMatrix);
    sceneEntity.setScaleingMatrix(scalingMatrix);
    sceneEntity.setProjectionMatrix(projectionMatrix);
    sceneEntity.setViewMatrix(viewMatrix);
    sceneEntity.setModelMatrix(modelMatrix);

    //get the real position values from the modelMatrix
    glm::mat4x4 tmat4 = modelMatrix * glm::inverse(rotationMatrix) * glm::inverse(scalingMatrix);
    sceneEntity.setPosition(glm::vec3(tmat4[3][0],
            tmat4[3][1],
            tmat4[3][2]));
    //    qDebug()<< tmat4[3][0] <<tmat4[3][1] << tmat4[3][2];
}

/*
 * Sets a dafault hard-fed shader
 * on the render object
 * Is being used by the _glWidget class
 * Create:11_02_2019
*/
void _Renderer::setShader()
{
    shdr->attachShaders(":/shaders/vshader.glsl", ":/shaders/fshader.glsl");
    qDebug() << "default Shader attached for entity" << sceneEntity.getTag();
}
/*
 * Takes the path to the relative qrc aided directory
 * to set shader paths externaly on the render object.
 * Is being used by the _glWidget class
 * Create:11_02_2019
*/
void _Renderer::setShader(QString vSh, QString fSh)
{
    shdr->attachShaders(vSh,fSh);
    qDebug() << "setShader(QString"<<vSh<<", QString"<<fSh<<")" << sceneEntity.getTag();;
}
/*
 * set Vertex and Index data into
 * the GPU buffers to use for the current model.
 * May have extended implementation for inclusion of UV for texture and Normals for
 * lighting.
 * Used by: the _glWidget class initializeGL().
*/
void _Renderer::setModelDataInBuffers(std::vector<float> vertexArray, std::vector<uint> indexArray)//Pass normals
{
    // Copy the vertex and index data locally for use in the current drawcall.
    vertices = vertexArray;
    indices = indexArray;
    // Initialization code (done once (unless your object frequently changes))
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    qDebug() << "VBO-" << VBO << "VAO-" << VAO << "EBO-" << EBO;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

//     position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


    setuniformLocations();//sets all uniform locations to respective variables.
    qDebug() << "setModelDataInBuffers() for entity" << sceneEntity.getTag();
}


void _Renderer::setModelDataInBuffers(std::vector<float>vertexArray,std::vector<float> normalsArray ,std::vector<uint> indexArray)
{
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    qDebug() << "VBO-" << VBO << "VAO-" << VAO << "EBO-" << EBO;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    //Dump the data into the Buffer.
    glBufferData(GL_ARRAY_BUFFER,sizeof (float)* vertexArray.size()+normalsArray.size()/*+sizeof(uv)*/, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexArray.size(), vertexArray.data());
//    glBufferSubData(GL_ARRAY_BUFFER, vertexArray.size(), normalsArray.size(), normalsArray.data());
//    glBufferSubData(GL_ARRAY_BUFFER, sizeof(position)+sizeof(normal), sizeof(uv), uv);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size() * sizeof(uint), &indexArray[0], GL_STATIC_DRAW);

    //Get the location of the attributes from the shader
    GLuint positionLocation = glGetAttribLocation(shdr->getShaderProgram(),"aPos");
    GLuint normalLocation = glGetAttribLocation(shdr->getShaderProgram(),"normal");
    // GLuint uvLocation = glGetAttribLocation(shdr->getShaderProgram(),"texCoord");

    //enable the attribute locations
    glEnableVertexAttribArray(positionLocation);
//    glEnableVertexAttribArray(normalLocation);
    // glEnableVertexAttribArray(uvLocation);

    //Link buffer data to position attribute location
    glVertexAttribPointer(positionLocation,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
    //Link buffer data to normal attribute location
//    glVertexAttribPointer(normalLocation,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)vertexArray.size());
    //Link buffer data to texture attribute location
//   glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(position) + sizeof(normal));

    setuniformLocations();//sets all uniform locations to respective variables.
    qDebug() << "setModelDataInBuffers() with normals for entity" << sceneEntity.getTag();
}
/*
 * sets the unform location uints into there respectively
 * named variables. These variables are used based on definition in shader.
*/
void _Renderer::setuniformLocations()
{
    qInfo() << "---------------UNIFORM INFO------------------------";
    qDebug() <<"Tag ->"<<sceneEntity.getTag();
    colorUniform = shdr->getUniformLocation("aColor");
    qDebug() << "colorUniform ->" << colorUniform;
    modelUnifrom = shdr->getUniformLocation("model");
    qDebug() << "modelUnifrom ->" << modelUnifrom;
    viewUniform  = shdr->getUniformLocation("view");
    qDebug() << "viewUniform ->" << viewUniform;
    projectionUniform = shdr->getUniformLocation("projection");
    qDebug() << "projectionUniform ->" << projectionUniform;
    mousePosUniform = shdr->getUniformLocation("iMouse");
    qDebug() << "mousePosUniform ->" << mousePosUniform;
    qDebug() <<"---------------------------------------------------";
    qDebug() <<"---------------------------------------------------";
}
/*
 * creates new texture and adds into list(vector) of textures
 * set a default 8bit single color texture of size 1360 x 1024
 */
void _Renderer::setupTexture()
{
    char* img = new char[1360*1024];
    _Texture texture(img,1360,1024);
    texture.load(GL_RED,GL_UNSIGNED_BYTE);
    textures.push_back(texture);
    qDebug() << "setupTexture() on entity" << sceneEntity.getTag();
}

void _Renderer::setupTexture(QString texfile)
{
    QImage img = QImage(texfile);
    _Texture texture(img);
    texture.load(GL_RGBA,GL_UNSIGNED_BYTE);
    textures.push_back(texture);
    qDebug() << "setupTexture(QString texfile) on entity" << sceneEntity.getTag();
}
/*
 * Contributor : saurabh
 * updates the first texture image from char pointer array
 * resolution of previous image is used
 * current context should be active while calling this function

 */
void _Renderer::setTexture(char* texBitmap)
{
    if(!textures.empty())
        textures[0].setImage(texBitmap);
    qDebug() << "setTexture(char* texBitmap) on entity" << sceneEntity.getTag();
}
void _Renderer::setTexture(char* texBitmap,uint iwidth,uint iheight)
{
    if(!textures.empty())
        textures[0].setImage(texBitmap,iwidth,iheight);
    qDebug() << "setTexture(char* texBitmap,uint iwidth,uint iheight) on entity" << sceneEntity.getTag();
}
void _Renderer::setTexture(QString pathtoTexture)
{
    if(!textures.empty())
        textures[0].setImage(pathtoTexture);
    qDebug() << "setTexture(QString pathtoTexture) on entity" << sceneEntity.getTag();
}

/*
* Sets the values matrices for the model matrix
* works in implementing translation , rotation and scaling
* Used by: the _glWidget class initialiseGl() or paintGl().
*/
void _Renderer::setModelMatrix(glm::vec3 position,float scale,glm::vec3 rotation)
{
    modelMatrix = glm::mat4(1.0f);
    translationMatrix = glm::mat4(1.f);
    rotationMatrix = glm::mat4(1.f);
    scalingMatrix = glm::mat4(1.f);

    scalingMatrix = glm::scale(scalingMatrix, glm::vec3(scale, scale, scale));//scale equally on all sides
    glm::vec3 EulerAngles(rotation.x,rotation.y,rotation.z);
    glm::quat quat = glm::quat(EulerAngles);
    rotationMatrix = glm::mat4_cast(quat);
    translationMatrix = glm::translate(translationMatrix,position);

    modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
    qDebug() << "setModelMatrix() on entity" << sceneEntity.getTag();
    keepSceneEntityUpdated();
}
/*
* sets the camera view for the scene through this matrix
* helps set the camera , eye positon , rotation, lookat.
* Used by: the _glWidget class initialiseGl() or paintGl().
* depending if the camra needs to update its position in  realtime.
*/
void _Renderer::setCamViewMatrix(QVector3D eyePos,glm::vec3 focalPoint,QVector3D upVector)
{
    camposForLight = camposForLight;//temp
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::lookAt(
                glm::vec3(eyePos.x(), eyePos.y(), eyePos.z()),
                glm::vec3(focalPoint.x, focalPoint.y, focalPoint.z),
                glm::vec3(upVector.x(), upVector.y(), upVector.z()));
    keepSceneEntityUpdated();
}
/*
        ▐ ▄     ▄▄▄  ▄▄▄ ..▄▄ · ▪  ·▄▄▄▄•▄▄▄ .
 ▪     •█▌▐█    ▀▄ █·▀▄.▀·▐█ ▀. ██ ▪▀·.█▌▀▄.▀·
  ▄█▀▄ ▐█▐▐▌    ▐▀▀▄ ▐▀▀▪▄▄▀▀▀█▄▐█·▄█▀▀▀•▐▀▀▪▄
 ▐█▌.▐▌██▐█▌    ▐█•█▌▐█▄▄▌▐█▄▪▐█▐█▌█▌▪▄█▀▐█▄▄▌
  ▀█▄▀▪▀▀ █▪    .▀  ▀ ▀▀▀  ▀▀▀▀ ▀▀▀·▀▀▀ • ▀▀▀
*/
/*
* takes thew width and height of the window and sets the relative
* field of view and the aspect ration bindings. will update itself each time the
* window is resized.and needs to be called in the resizeGl function.
* Used by: the _glWidget class resizeGL().
*/
void _Renderer::setProjectionMatrix(int resW, int resH, float fov, float zNear, float zFar)
{
    // Calculate aspect ratio
    float aspect = float(resW) / float(resH ? resH : 1);
    projectionMatrix = glm::perspective(glm::radians(fov), float(aspect), zNear, zFar);
    //qDebug() << "setProjectionMatrix() on entity" << sceneEntity.getTag();
}
/*
 ▄▄▄▄▄▄▄▄   ▄▄▄·  ▐ ▄ .▄▄ · ·▄▄▄      ▄▄▄  • ▌ ▄ ·.
 •██  ▀▄ █·▐█ ▀█ •█▌▐█▐█ ▀. ▐▄▄·▪     ▀▄ █··██ ▐███▪
  ▐█.▪▐▀▀▄ ▄█▀▀█ ▐█▐▐▌▄▀▀▀█▄██▪  ▄█▀▄ ▐▀▀▄ ▐█ ▌▐▌▐█·
  ▐█▌·▐█•█▌▐█ ▪▐▌██▐█▌▐█▄▪▐███▌.▐█▌.▐▌▐█•█▌██ ██▌▐█▌
  ▀▀▀ .▀  ▀ ▀  ▀ ▀▀ █▪ ▀▀▀▀ ▀▀▀  ▀█▄▀▪.▀  ▀▀▀  █▪▀▀▀
*/
/*
 * updates the specific trasformations that affect the model matrix
 * of the matrices of the individual object.In this case the positions
 * Used by: _render class in draw()
*/
void _Renderer::setPosition(glm::vec3 pos)
{
    if(sceneEntity.getIsTransformationAllowed())
    {
        if(sceneEntity.getIsTransformationLocal())
        {
            modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix,pos);
        }
        else if(!sceneEntity.getIsTransformationLocal())
        {
            translationMatrix = glm::mat4(1.f);
            translationMatrix = glm::translate(translationMatrix,pos);
            modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        }
        keepSceneEntityUpdated();
    }
}
/*
 *
*/
void _Renderer::translate(glm::vec3 pos)
{
    if(sceneEntity.getIsTransformationAllowed())
    {
        //update the traformation matrix with the current values
        setPosition(sceneEntity.getPostion());
        if(sceneEntity.getIsTransformationLocal())
        {
            //        modelMatrix *= translationMatrix;
            modelMatrix = glm::translate(modelMatrix,pos);
        }
        else if(!sceneEntity.getIsTransformationLocal())
        {
            translationMatrix = glm::translate(translationMatrix,pos);
            modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        }
        keepSceneEntityUpdated();
    }
}
/*
 * updates the specific trasformations that affect the model matrix
 * of the matrices of the individual object.In this case the rotation
 * Used by: _glwidgetClass on mousemovement

*/
void _Renderer::setRotation(glm::vec3 rot)
{
    if(sceneEntity.getIsTransformationAllowed())
    {
        if(sceneEntity.getIsPivotSet() == false)
        {
            sceneEntity.setRotation(rot);
            if(sceneEntity.getIsTransformationLocal())
            {
                glm::vec3 eulerAngles(sceneEntity.getRotation());
                glm::quat quat = glm::quat(eulerAngles);
                modelMatrix *= glm::mat4_cast(quat);
            }
            else if(!sceneEntity.getIsTransformationLocal())
            {
                //rotationMatrix = glm::mat4x4(1.f);
                glm::vec3 eulerAngles(sceneEntity.getRotation());
                glm::quat quat = glm::quat(eulerAngles);
                rotationMatrix = glm::mat4_cast(quat);
                //rotate at center
                modelMatrix =  translationMatrix * rotationMatrix * scalingMatrix;
            }
        }
        else if(sceneEntity.getIsPivotSet() == true)
        {
            setRotationAroundPivot(rot, sceneEntity.getPivot());
        }
        keepSceneEntityUpdated();
    }
}
/*
 * sets the rotation to be around an defined point
 * Used by: _glwidgetClass on Mousemovement
*/
void _Renderer::setRotationAroundPivot(glm::vec3 rot, glm::vec3 pivot)
{
    if(sceneEntity.getIsTransformationAllowed())
    {
        sceneEntity.setRotation(rot);
        if(sceneEntity.getIsTransformationLocal()){
            //still buggy
            setPosition(pivot);
            glm::vec3 EulerAngles(sceneEntity.getRotation());
            glm::quat quat = glm::quat(EulerAngles);
            modelMatrix *= glm::mat4_cast(quat);
        }
        if(!sceneEntity.getIsTransformationLocal()){
            //this works like an ofset pivot rather than rotae around a point
            //(Alterante implementation involves multiplying parent rotation matrix with childrens model matrix)#Not implemented
            pivotTmat = glm::mat4x4(1.0f);

            pivotTmat[3][0] = pivot.x;
            pivotTmat[3][1] = pivot.y;
            pivotTmat[3][2] = pivot.z;
            glm::vec3 EulerAngles(sceneEntity.getRotation());
            glm::quat quat = glm::quat(EulerAngles);
            rotationMatrix = glm::mat4_cast(quat);
            modelMatrix = translationMatrix * rotationMatrix * pivotTmat * scalingMatrix;
        }
        keepSceneEntityUpdated();
    }
}
/*
 * updates the specific trasformations that affect the model matrix
 * of the matrices of the individual object.In this case the scale
 * Used by: _render class in draw()
*/
void _Renderer::setscale(float scale)
{
    if(sceneEntity.getIsTransformationAllowed())
    {
        sceneEntity.setScale(scale);//reimplemnt
        scalingMatrix = glm::mat4(1.f);
        //scale eqally on all axis(dont need respective sclaing)
        scalingMatrix = glm::scale(scalingMatrix, glm::vec3(sceneEntity.getScale(),
                                                            sceneEntity.getScale(),
                                                            sceneEntity.getScale()));
        modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
    }
    keepSceneEntityUpdated();
}
/*
*/
void _Renderer::lookAt(QVector3D ptl) //Not Implemented properly yet needs to be fixed after Mesh on Mesh Collision
{
    //    glm::vec3 obPos = glm::vec3(sceneEntity.getPostion().x(),sceneEntity.getPostion().y(),sceneEntity.getPostion().z());
    //    glm::vec3 tarPo = glm::vec3(ptl.x(),ptl.y(),ptl.z());

    //    glm::vec3 delta = tarPo - obPos;//targetPosition-objectPosition
    //    glm::vec3 up;
    //    glm::vec3 dir(glm::normalize(delta));

    //    if(abs(dir.x) < 0.00001 && abs(dir.z) < 0.00001){
    //        if(dir.y > 0)
    //            up = glm::vec3(0.0, 0.0, -1.0); //if direction points in +y
    //        else
    //            up = glm::vec3(0.0, 0.0, 1.0); //if direction points in -y
    //    } else {
    //        up = glm::vec3(0.0, 1.0, 0.0); //y-axis is the general up
    //    }
    //    up = glm::normalize(up);
    //    glm::vec3 right = glm::normalize(glm::cross(up,dir));
    //    up = glm::normalize(glm::cross(dir, right));

    //    rotationMatrix *= glm::mat4(right.x, right.y, right.z, 0.0f,
    //                                up.x, up.y, up.z, 0.0f,
    //                                dir.x, dir.y, dir.z, 0.0f,
    //                                obPos.x, obPos.y, obPos.z, 1.0f);

    //    keepSceneEntityUpdated();
    //    _Tools::Debugmatrix4x4(this->sceneEntity.getModelMatrix());

    RotationBetweenVectors(glm::vec3(ptl.x(),ptl.y(),ptl.z()));
}

void _Renderer::RotationBetweenVectors(glm::vec3 dest)
{
    glm::vec3 start = glm::vec3(sceneEntity.getPostion());
    start = glm::normalize(start);//this object location
    dest = glm::normalize(dest);

    float cosTheta = glm::dot(start, dest);
    glm::vec3 rotationAxis;

    glm::quat axisangle;
    if (cosTheta < -1 + 0.001f){
        rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
        if (glm::length(rotationAxis) < 0.01 )
            rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
        rotationAxis = glm::normalize(rotationAxis);
        axisangle = glm::angleAxis(180.0f, rotationAxis);
    }

    rotationAxis = glm::cross(start, dest);

    float s = sqrt((1+cosTheta)*2);
    float invs = 1 / s;

    modelMatrix *= glm::mat4_cast(glm::quat(
                                      s * 0.5f,
                                      rotationAxis.x * invs,
                                      rotationAxis.y * invs,
                                      rotationAxis.z * invs
                                      ));
    this->sceneEntity.setModelMatrix(modelMatrix);
    keepSceneEntityUpdated();
}

/*
  ·▄▄▄▄  ▄▄▄   ▄▄▄· ▄▄▌ ▐ ▄▌
  ██▪ ██ ▀▄ █·▐█ ▀█ ██· █▌▐█
  ▐█· ▐█▌▐▀▀▄ ▄█▀▀█ ██▪▐█▐▐▌
  ██. ██ ▐█•█▌▐█ ▪▐▌▐█▌██▐█▌
  ▀▀▀▀▀• .▀  ▀ ▀  ▀  ▀▀▀▀ ▀▪
*/
/*
 * This is your proprietory draw function
 * Draws frames on a avg of 60frames per second(is subjective and changes with hardware)
 * Used by: the _glWidget class paintGl().
*/
void _Renderer::_Renderer::draw()
{
    if(sceneEntity.getIsLineMode())
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    else if(sceneEntity.getIsLineNoCullMode())
        glPolygonMode(GL_FRONT,GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    if(sceneEntity.getIsActive())
    {
        //Using the shader program in the current context
        shdr->useShaderProgram();
        //Bind Textures
        for(uint t=0;t<textures.size();t++){
            textures[t].bind();
        }
        //Bind the Buffers data of the respective buffer object(only needed if mesh need chenging on runtime)
        if(sceneEntity.getIsMeshEditable())
        {
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        }
        //Bind the VAO of the respective buffer object (needs to be bound everytime)
        glBindVertexArray(VAO);
        //
        //Sets the values for the MVP matrix in the vertex shader
        glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(sceneEntity.getViewMatrix()));
        glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(sceneEntity.getProjectionMatrix()));
        //glUniformMatrix4fv(modelUnifrom, 1, GL_FALSE, glm::value_ptr(sceneEntity.getTranslationMatrix()*sceneEntity.getRotationmatrix()*pivotTmat *sceneEntity.getScaleingMatrix()));
        glUniformMatrix4fv(modelUnifrom,1,GL_FALSE,glm::value_ptr(sceneEntity.getModelMatrix()));
        //
        setColors();//Setting the uniform for color if shader allows
        setLights();//setting the lighting uniforms if shader allows
        //
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);//The Final draw call for each frame
        glBindVertexArray(0);//Clear the buffer
    }
}
/*
 *  Used in the Draw functon
 * Updates the color and relative uniforms
 */
void _Renderer::setColors()
{
    glUniform4f(colorUniform,sceneEntity.getColor().x(),sceneEntity.getColor().y(),sceneEntity.getColor().z(),sceneEntity.getColor().w());
    //set sfidex color attributes for defined objects
    if(sceneEntity.getId() == 999 || sceneEntity.getId() == 991|| sceneEntity.getId() == 992 || sceneEntity.getId() == 993)//PointHelpers
    {
        QVector4D col = sceneEntity.getColor();
        col.setX(col.x() + abs(cos(timer.elapsed() * 0.002)));
        col.setY(col.y() + abs(cos(timer.elapsed() * 0.003)));
        col.setZ(col.z() + abs(cos(timer.elapsed() * 0.005)));
        glUniform4f(colorUniform, col.x(),col.y(), col.z(), col.w());
    }
    if(sceneEntity.getId() == 888)//pivot
    {
        QVector4D col = sceneEntity.getColor();
        col.setX(col.x() + abs(cos(timer.elapsed() * 0.04)));
        col.setY(col.y() + abs(cos(timer.elapsed() * 0.03)));
        col.setZ(col.z() + abs(cos(timer.elapsed() * 0.05)));
        glUniform4f(colorUniform, col.x(),col.y(), col.z(), col.w());
    }
    if(sceneEntity.getisHitByRay())
        sceneEntity.setColor(actualColor * 0.5);
    else if(sceneEntity.getIsHitByTri()){
        QVector4D qc = actualColor;
         qc.setX(actualColor.x() + 1.0f);
        sceneEntity.setColor(qc);}
    else
        sceneEntity.setColor(actualColor);
}

/*
 *  Used in the Draw functon
 * Updates the light uniforms on the model
 */
void _Renderer::setLights()
{
    lightPos = glm::vec3(2.2f,2.0f, 4.0f);//could be an array of lights
//    glUniform3f(shdr->getUniformLocation("objectColor"),0.5f, 0.5f, 0.5f );
    glUniform3f(shdr->getUniformLocation("lightColor"),1.0f, 1.0f, 1.0f);
    glUniform3f(shdr->getUniformLocation("lightPos"),lightPos.x,lightPos.y,lightPos.z);
    glUniform3f(shdr->getUniformLocation("viewPos"),camposForLight.x,camposForLight.y,camposForLight.z);//cam pos is ceneter harcoded , passreal cam value later
}
