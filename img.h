#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>
#include <QImage>
#include <iostream>
#include <QString>
#include <string>
#include <QMap>
#include <QVector>

using namespace std;

/**
 * @brief Класс для хранения изображений и работы с ними.
 *
 * Класс содержит в себе атрибуты для хранения исходного изображения (хранение необходимо для выполнения сторонней функции сброса настроек)
 * и изображения-копии, к которому будут применяться дальнейшие изменения, реализованные в методах этого класса.
 */
class Img
{
    //Q_OBJECT
    private:
        QPixmap *FileMap; /// объект для отрисовки изображения на экране
        QImage originalImage, Image; /// исходное изображение и изображение-копия для редактирования
        QString FilePath; /// путь к файлу изображения
        int width, height; /// ширина и высота изображения
        int adjustValue(int value);
    public:
        Img(QString filePath);
        Img(QImage Image, int width, int height);
        QImage getOriginalImage() { return originalImage;}
        QString getFileName() { return FilePath; }
        QImage getImage() { return Image; }
        int getWidth() { return width; }
        int getHeight() { return height; }
        void changeBrightness(int value);
        void changeSaturation(float value);
        void changeContrast(float value);
        void changeRgb(int valueRed, int valueGreen, int valueBlue);
        void negative();
        void resetImage();
        QImage copy(QRect rect);
        QImage rotate(int right);
        /// перегрузка оператора = для копирования изображения, его ширины и высоты
        Img& operator=(const Img &img) {
            Image = img.Image.copy();
            width = img.width;
            height = img.width;
        };
        void greyImage();
        void convertToGreyScale();
};

#endif // IMAGE_H
