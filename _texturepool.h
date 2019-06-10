#ifndef _TEXTUREPOOL_H
#define _TEXTUREPOOL_H

#include <_texture.h>
#include <map>

class _TexturePool
{
public:
    _TexturePool();
    _TexturePool(GLenum color_format,GLenum data_type);
    _TexturePool(GLenum color_format,GLenum data_type,GLenum internal_format);

    int getTextureNumber() const;
    void setTextureNumber(int value);

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    void setWidthHeight(unsigned int w,unsigned int h);

    bool doesExists(QString texture_name);

    _Texture* textureFromPool(QString texture_name);
    _Texture* textureFromPool(QString texture_name, unsigned int w, unsigned int h);

    _Texture *operator[](QString key);

protected:
    int texture_numbers = 5;
    unsigned int width=MAX_FRAME_WIDTH,height=MAX_FRAME_HEIGHT;
    std::map<QString,_Texture*> texture_pool_map;
    GLenum _color_format = GL_RGBA,_internal_format=GL_RGBA,_data_type = GL_UNSIGNED_BYTE;

};

#endif // _TEXTUREPOOL_H
