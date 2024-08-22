#include "img.h"
#include <typeinfo>
#include <QDebug>
#include <QMultiMap>

/**
  * Конструктор объекта класса Img через путь до файла
  *
  * @param filePath путь до изображения
  */
Img::Img(QString filePath) {
    this->FilePath = filePath;

    originalImage.load(filePath);
    this->Image = this->originalImage.copy();
    this->width = originalImage.width();
    this->height = originalImage.height();
}

/**
  * Конструктор объекта класса Img через объект класса QImage (конструктор копирования)
  *
  * @param Image изображение
  * @param width ширина в пикселях
  * @param height высота в пикселях
  */
Img::Img(QImage Image, int width, int height) {
    this->Image = Image.copy(); /// копируем изображение встроенным методом класса QImage
    this->width = width;
    this->height = height;
}

/**
  * Функция восстановления изображения до исходного (сброса настроек)
  */
void Img::resetImage() {
    this->Image = this->originalImage.copy();  /// копируем изображение встроенным методом класса QImage
    this->width = this->originalImage.width();
    this->height = this->originalImage.height();
}

/**
  * Функция обесцвечивания изображения
  */
void Img::convertToGreyScale() {
    Image = Image.convertToFormat(QImage::Format_Grayscale8);
}

/**
  * Функция смены яркости изображения
  *
  * @param value значение яркости, положительное или отрицательное
  */
void Img::changeBrightness(int value) {

    QColor oldColor; /// значение исходного цвета пикселя
    QRgb newColor;
    int red, green, blue;

    for(int x = 0; x < this->width; x++) {
        for(int y = 0; y < this->height; y++) {
            oldColor = QColor(this->Image.pixel(x,y)); /// получаем значение исходного цвета пикселя

            /// считаем значения оттенков красного, зелёного и синего соответственно
            red = adjustValue(oldColor.red() + value);
            green = adjustValue(oldColor.green() + value);
            blue = adjustValue(oldColor.blue() + value);

            newColor = qRgb(red, green, blue); /// формируем новый цвет
            Image.setPixel(x, y, newColor); /// устанавливаем новый пиксель на месте старого
        }
    }
}

/**
  * Функция смены цветового баланса изображения
  *
  * @param valueRed значение красного оттенка, положительное или отрицательное
  * @param valueGreen значение красного оттенка, положительное или отрицательное
  * @param valueBlue значение красного оттенка, положительное или отрицательное
  */
void Img::changeRgb(int valueRed, int valueGreen, int valueBlue) {

    QColor oldColor; /// значение исходного цвета пикселя
    QRgb newColor;
    int red, green, blue;

    for(int x = 0; x < this->width; x++) {
        for(int y = 0; y < this->height; y++) {
            oldColor = QColor(this->Image.pixel(x,y));  /// получаем значение исходного цвета пикселя

            /// считаем значения оттенков красного, зелёного и синего соответственно
            red = adjustValue(oldColor.red() + valueRed);
            green = adjustValue(oldColor.green() + valueGreen);
            blue = adjustValue(oldColor.blue() + valueBlue);

            newColor = qRgb(red, green, blue); /// формируем новый цвет
            Image.setPixel(x, y, newColor); /// устанавливаем новый пиксель на месте старого
        }
    }
}

/**
  * Функция смены контрастности изображения
  *
  * @param value значение контрастности, положительное или отрицательное
  */
void Img::changeContrast(float value) {

    QColor oldColor;  /// значение исходного цвета пикселя
    QRgb newColor;
    int red, green, blue;
    float contrast, pixel; /// коэффициент контраста и переменная для расчёта оттенков RGB

    for(int x = 0; x < this->width; x++) {
        for(int y = 0; y < this->height; y++) {
            oldColor = QColor(this->Image.pixel(x,y));  /// получаем значение исходного цвета пикселя

            /// считаем значения оттенков красного, зелёного и синего соответственно
            contrast = (255.0 + value) / 255.0;
            contrast *= contrast;
            pixel = oldColor.red() / 255.0;
            pixel -= 0.5;
            pixel *= contrast;
            pixel += 0.5;
            pixel *= 255;
            red = adjustValue(pixel);

            pixel = oldColor.green() / 255.0;
            pixel -= 0.5;
            pixel *= contrast;
            pixel += 0.5;
            pixel *= 255;
            green = adjustValue(pixel);

            pixel = oldColor.blue() / 255.0;
            pixel -= 0.5;
            pixel *= contrast;
            pixel += 0.5;
            pixel *= 255;
            blue = adjustValue(pixel);

            newColor = qRgb(red, green, blue); /// формируем новый цвет
            Image.setPixel(x, y, newColor); /// устанавливаем новый пиксель на месте старого
        }
    }
}

/**
  * Функция смены яркости изображения
  *
  * @param value значение насыщенности, положительное или отрицательное
  */
void Img::changeSaturation(float value) {
    QColor oldColor, newColor;
    value /= 100.0; /// делим значение для корректной отработки функции hslHueF()
    for (int x = 0; x < this->width; x++) {
        for (int y = 0; y < this->height; y++) {
            oldColor = QColor(this->Image.pixel(x,y));  /// получаем значение исходного цвета пикселя

            /// считаем значение насыщенности и балансируем его при выходе за границы
            float temp = oldColor.hslSaturationF()+value;
            if (temp > 1) temp = 1.0;
            else if (temp < 0) temp = 0.0;
            newColor.setHslF(oldColor.hslHueF(), temp, oldColor.lightnessF()); /// формируем новый цвет переводом изображения в формат HSL
            Image.setPixel(x, y, newColor.rgb()); /// устанавливаем новый пиксель на месте старого
        }
    }
}

/**
  * Функция инвертирования изображения
  */
void Img::negative() {
    QColor oldColor; /// значение исходного цвета пикселя
    QRgb newColor;
    int red, green, blue;

    for(int x = 0; x < this->width; x++) {
        for(int y = 0; y < this->height; y++) {
            oldColor = QColor(this->Image.pixel(x,y)); /// получаем значение исходного цвета пикселя

            /// считаем значения оттенков красного, зелёного и синего соответственно
            red = 255 - oldColor.red();
            green = 255 - oldColor.green();
            blue = 255 - oldColor.blue();
            newColor = qRgb(red, green, blue); /// формируем новый цвет
            Image.setPixel(x, y, newColor); /// устанавливаем новый пиксель на месте старого
        }
    }
}

/**
  * Функция балансировки значений RGB
  *
  * @param value целое число
  * @return балансированное значение
  */
int Img::adjustValue(int value) {
    if (value < 0) value = 0;
    else if (value > 255) value = 255;
    return value;
}

/**
  * Функция поворота изображения
  *
  * @param right индекс поворота изображения (по часовой стрелке или против)
  * @return повёрнутое изображение
  */
QImage Img::rotate(int right) {

    /// создаем новое изображение с шириной, равной высоте исходного изображения и наоборот
    int newWidth = height, newHeight = this->width;
    QImage rotatedImage (QSize(newWidth, newHeight), this->Image.format());

    QRgb pixel;
    for (int x = 0; x < this->width; x++){
        for(int y = 0; y < this->height; y++) {
            pixel = this->Image.pixel(x,y); /// получаем пиксель
            if (right) rotatedImage.setPixel(this->height - y - 1, x, pixel); /// поворот по часовой стрелке
            else rotatedImage.setPixel(y, this->width - x - 1, pixel); /// поворот против часовой стрелки
        }
    }

    /// копируем новое изображение в старое (изображение-копию)
    this->Image = rotatedImage.copy();
    this->width = this->Image.width();
    this->height = this->Image.height();
    return this->Image;
}

/**
  * Функция обрезки изображения
  *
  * @param rect прямоугольная область обрезки
  * @return обрезанное изображение
  */
QImage Img::copy(QRect rect) {

    this->Image = this->Image.copy(rect); /// обрезаем изображение встроенным методом объекта QImage
    this->width = this->Image.width();
    this->height = this->Image.height();
    return this->Image;
}
