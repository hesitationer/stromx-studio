#include "Image.h"

#include "Config.h"

const std::string Image::TYPE = "Image";
const std::string Image::PACKAGE = STROMX_STUDIO_PACKAGE_NAME;
const stromx::core::Version Image::VERSION = stromx::core::Version(STROMX_STUDIO_VERSION_MAJOR,
                                                                   STROMX_STUDIO_VERSION_MINOR,
                                                                   STROMX_STUDIO_VERSION_PATCH);
               
Image::Image(const QString& fileName)
{
    QImage image = QImage(fileName);
    
    constructFromImage(image);
}

Image::Image(const QImage& image)
{
    constructFromImage(image);
}

Image::Image(const Image& image)
{
    m_image = image.m_image;
    initializeParent();
}

void Image::constructFromImage(const QImage& image)
{
    switch(image.format())
    {
    case QImage::Format_Indexed8:
        m_image = image;
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        m_image = image.convertToFormat(QImage::Format_RGB888);
        break;
    default:
        return;
    }
    
    initializeParent();
}

stromx::core::Data*const Image::clone() const
{
    return new Image(*this);
}

void Image::initializeParent()
{
    stromx::core::Image::PixelType pixelType = stromx::core::Image::MONO_8;
    
    switch(m_image.format())
    {
    case QImage::Format_Indexed8:
        pixelType = stromx::core::Image::MONO_8;
        break;
    case QImage::Format_RGB888:
    case QImage::Format_ARGB32:
        pixelType = stromx::core::Image::RGB_24;
        break;
    default:
        m_image = QImage();
    }
    
    setBuffer(reinterpret_cast<uint8_t*>(m_image.bits()));
    setBufferSize(m_image.byteCount());
    
    initialize(m_image.width(), m_image.height(), m_image.bytesPerLine(),
               m_image.bits(), pixelType);
}
