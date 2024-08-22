#include "mainwindow.h"
#include "img.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QString>
#include <QColor>
#include <QThread>
#include <QImage>
#include <QPixmap>

/// объект класса Img, хранящий изображения QImage и применяемые к изображениям методы
Img *image;

/// исходное изображение и изображение-копия, которое редактируется в программе
QImage originalImage, Image;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString fileName = "/home";

    /// устанавливаем цвет фона объекта класса QMainWindow, задаём название и фиксированный размер окна, устанавливаем стили
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    this->setPalette(palette);
    this->setFixedSize(1315, 690);
    this->setWindowTitle("Image Editor");
    QString style = "QMenuBar::item:selected { background: #333333; } "
                    "QMenuBar::item:pressed { background: #333333; } "
                    "QMenu::item:disabled { color: #aaaaaa; }"
                    "QSlider::handle:horizontal {background-color: firebrick;}"
            ;
    this->setStyleSheet(style);

    /// скрываем окна-объекты класса QGroupBox с ручной настройкой фильтров,
    /// а также сообщение о загрузке цветовой палитры (объект QLabel)
    ui->groupBoxContrast->hide();
    ui->groupBoxSaturation->hide();
    ui->groupBoxRgb->hide();
    ui->groupBoxBrightness->hide();
    ui->groupBoxCrop->hide();
    ui->label->hide();

    /// создаём изображение-копию для последующего редактирования
    image = new Img(fileName);   
    Image = (*image).getImage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
  * Функция, вычисляющая палитру (набор значений объектов QLabel) главных цветовых оттенков
  */
void MainWindow::paletteTable() {

    /// создаём вектор значений цветов и заполняем его текстовыми значениями цветов фона MainWindow
    QVector<QString> colors (7, QColor(53, 53, 53).name());

    /// устанавливаем объектам QLabel значение цвета фона MainWindow до окончания вычисления значений цветов оттенков
    QPalette palette = ui->label_1->palette();
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    ui->label_1->setPalette(palette);
    ui->label_2->setPalette(palette);
    ui->label_3->setPalette(palette);
    ui->label_4->setPalette(palette);
    ui->label_5->setPalette(palette);
    ui->label_6->setPalette(palette);
    ui->label_7->setPalette(palette);

    /// блокируем панель управления и отображаем сообщение об ожидании до окончания вычисления значений цветов оттенков
    ui->label->show();
    this->setEnabled(false);
    QCoreApplication::processEvents();

    QString color; /// текстовое значение цвета пикселя
    int countColor = 0; /// счётчик для перебора элементов вектора значений цветов colors
    QMap<QString, int>table; /// таблица с текстовыми значениями цветов пикселей

    /// создаём объект-итератор по элементам объекта QMap и вектор из этих объектов
    typedef QMap<QString, int>::const_iterator iter;
    QVector<iter> byVal;

    /// заполняем объект QMap ключами-текстовыми значениями цветов пикселей,
    /// увеличивая по ключу количество найденных пикселей определённого цвета, если тот уже встречался
    for(int x = 0; x < (*image).getWidth(); x++) {
        for(int y = 0; y < (*image).getHeight(); y++) {
            color = (*image).getImage().pixelColor(x,y).name();
            if(table.contains(color)) table[color]++;
            else table.insert(color, 1);
        }
    }

    /// заполняем вектор итераторов
    iter i = table.cbegin();
    while (i != table.cend()) byVal.append(i++);

    /// сортируем значения вектора итераторов по убыванию значений объекта table класса QMap
    sort(byVal.begin(), byVal.end(), [] (iter i0, iter i1) {
            return i0.value() > i1.value();
        });

    /// заполняем 7 элементов вектора colors значениями объекта table класса QMap по первым 7 итераторам из вектора итераторов
    for (auto i : byVal) {
        if (countColor < 7) {
            colors[countColor] = i.key();
            countColor++;
        }
        else break;
    }

    ui->label->hide(); /// скрываем сообщение об ожидании

    /// устанавливаем объектам QLabel (цветовой палитре) значения из вектора цветов colors
    palette.setColor(QPalette::Window, QColor(colors[0]));
    ui->label_1->setAutoFillBackground(true);
    ui->label_1->setPalette(palette);
    palette = ui->label_2->palette();
    palette.setColor(QPalette::Window, QColor(colors[1]));
    ui->label_2->setAutoFillBackground(true);
    ui->label_2->setPalette(palette);
    palette = ui->label_3->palette();
    palette.setColor(QPalette::Window, QColor(colors[2]));
    ui->label_3->setAutoFillBackground(true);
    ui->label_3->setPalette(palette);
    palette = ui->label_4->palette();
    palette.setColor(QPalette::Window, QColor(colors[3]));
    ui->label_4->setAutoFillBackground(true);
    ui->label_4->setPalette(palette);
    palette = ui->label_5->palette();
    palette.setColor(QPalette::Window, QColor(colors[4]));
    ui->label_5->setAutoFillBackground(true);
    ui->label_5->setPalette(palette);
    palette = ui->label_6->palette();
    palette.setColor(QPalette::Window, QColor(colors[5]));
    ui->label_6->setAutoFillBackground(true);
    ui->label_6->setPalette(palette);
    palette = ui->label_7->palette();
    palette.setColor(QPalette::Window, QColor(colors[6]));
    ui->label_7->setAutoFillBackground(true);
    ui->label_7->setPalette(palette);

    this->setEnabled(true); /// восстанавливаем доступ к панели управления
}

/**
  * Функция, обновляющая изображение-копию
  */
void MainWindow::updateImage(){

    Image = (*image).getImage(); /// обновляем значение QImage

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(Image.scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);

    /// обновляем значения объекта QLabel с информацией о размере изображения
    int width = (*image).getWidth(), height = (*image).getHeight();
    ui->labelSize->setText(QString(" Размер изображения: %1 px x %2 px").arg(width).arg(height));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Открыть..."
  */
void MainWindow::on_actionOpen_triggered()
{
    /// окно открытия файла и сохранение пути в переменную-объект класса QString
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"),
                                                    QString(),
                                                    tr("Images (*.png *.jpg *.jpeg)"));
    if (fileName != nullptr) {

        /// создаём изображение-копию для редактирования, определяем исходное изображение
        image = new Img (fileName);
        originalImage = image->getOriginalImage();
        Image = image->getImage();

        /// обновляем объект формы Qt Designer с изображением
        QPixmap original;
        original = original.fromImage(originalImage.scaled(880,600,Qt::KeepAspectRatio));
        ui->proImage->setPixmap(original.scaled(880, 600, Qt::KeepAspectRatio));

        /// открываем доступ к функциям редактирования изображений
        ui->actionRgb->setEnabled(true);
        ui->actionMonochrome->setEnabled(true);
        ui->actionInversion->setEnabled(true);
        ui->actionInversion->setEnabled(true);
        ui->actionSave_as->setEnabled(true);
        ui->actionClose->setEnabled(true);
        ui->action90->setEnabled(true);
        ui->resetButton->setEnabled(true);
        ui->cropButton->setEnabled(true);
        ui->action180->setEnabled(true);
        ui->actionBrightness->setEnabled(true);
        ui->actionContrast->setEnabled(true);
        ui->actionSaturation->setEnabled(true);
        ui->action_90->setEnabled(true);
    }

    /// устанавливаем объектам класса QSpinBox "Ширина" и "Высота" исходные значения
    ui->widthBox->setMaximum(image->getWidth());
    ui->heightBox->setMaximum(image->getHeight());
    ui->widthBox->setMinimum(1);
    ui->heightBox->setMinimum(1);
    ui->widthBox->setValue(image->getWidth());
    ui->heightBox->setValue(image->getHeight());

    /// обновляем значения объекта QLabel с информацией о размере изображения
    int width = (*image).getWidth(), height = (*image).getHeight();
    ui->labelSize->setText(QString(" Размер изображения: %1 px x %2 px").arg(width).arg(height));

    paletteTable(); /// обновляем палитру главных цветовых оттенков
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Закрыть"
  */
void MainWindow::on_actionClose_triggered()
{
    /// очищаем объект формы Qt Designer с изображением
    ui->proImage->clear();
    image = nullptr;

    /// блокируем доступ к функциям редактирования изображений
    ui->actionRgb->setEnabled(false);
    ui->actionMonochrome->setEnabled(false);
    ui->actionInversion->setEnabled(false);
    ui->actionSave_as->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cropButton->setEnabled(false);
    ui->actionClose->setEnabled(false);
    ui->action90->setEnabled(false);
    ui->action180->setEnabled(false);
    ui->actionBrightness->setEnabled(false);
    ui->actionContrast->setEnabled(false);
    ui->actionSaturation->setEnabled(false);
    ui->action_90->setEnabled(false);

    /// "обнуляем" палитру цветовых оттенков, устанавливая объектам класса QLabel цвет фона окна MainWindow
    QPalette palette = ui->label_1->palette();
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    ui->label_1->setPalette(palette);
    ui->label_2->setPalette(palette);
    ui->label_3->setPalette(palette);
    ui->label_4->setPalette(palette);
    ui->label_5->setPalette(palette);
    ui->label_6->setPalette(palette);
    ui->label_7->setPalette(palette);
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Сохранить как..."
  */
void MainWindow::on_actionSave_as_triggered()
{
    /// окно сохранения файла и сохранение пути в переменную-объект класса QString
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"),
                                                    (*image).getFileName(),
                                                   "PNG (*.png);;JPG (*.jpg);;JPEG (*.jpeg);;");
    if (!fileName.isEmpty())
    {
      ((*image).getImage()).save(fileName); /// сохранение изображения
    }
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Яркость"
  */
void MainWindow::on_actionBrightness_triggered()
{
    ui->groupBoxBrightness->show(); /// отображаем объект класса QGroupBox с настройкой яркости изображения

    /// блокируем доступ к остальным функциям редактирования изображения
    ui->menubar->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cropButton->setEnabled(false);

    /// устанавливаем значение яркости по умолчанию объектам классов QSlider и QLabel
    ui->brightnessSlider->setValue(0);
    ui->valueBrightness->setText(QString::number(ui->brightnessSlider->value()));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменение значения объекта класса QSlider "Яркость"
  */
void MainWindow::on_brightnessSlider_valueChanged(int value)
{
    ui->valueContrast->setText(QString::number(value)); /// обновляем значение QLabel

    /// создаём изображение-предосмотр и меняем ему яркость методом объекта класса Img
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());
    temp_image.changeBrightness(value);

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(temp_image.getImage().scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);

}

/**
  * Функция изменения яркости, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Ok" c предопределённой
  * "AcceptRole"
  */
void MainWindow::on_buttonBrightnessBox_accepted()
{
    /// меняем изображению-копии яркость методом объекта класса Img
    (*image).changeBrightness(ui->brightnessSlider->value());

    /// обновляем изображение на экране и перезаписываем таблицу основных цветовых оттенков изображения
    updateImage();
    paletteTable();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования яркости
    ui->menubar->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->groupBoxBrightness->hide();
}

/**
  * Функция закрытия окна яркости, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Cancel" c предопределённой
  * "RejectRole"
  */
void MainWindow::on_buttonBrightnessBox_rejected()
{
    /// убираем предосмотр, обновляя объект формы Qt Designer с изображением
    updateImage();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования яркости
    ui->menubar->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->groupBoxBrightness->hide();
    ui->brightnessSlider->setValue(0);
}

/**
  *  Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Цветовой баланс"
  */
void MainWindow::on_actionRgb_triggered()
{
    ui->groupBoxRgb->show(); /// отображаем объект класса QGroupBox с настройкой цветового баланса изображения

    /// блокируем доступ к остальным функциям редактирования изображения
    ui->menubar->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cropButton->setEnabled(false);

    /// устанавливаем значение цветового баланса по умолчанию объектам классов QSlider и QLabel
    ui->redSlider->setValue(0);
    ui->greenSlider->setValue(0);
    ui->blueSlider->setValue(0);
    ui->labelValueRed->setText(QString::number(ui->redSlider->value()));
    ui->labelValueGreen->setText(QString::number(ui->greenSlider->value()));
    ui->labelValueBlue->setText(QString::number(ui->blueSlider->value()));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменение значения объекта класса QSlider "Красный"
  */
void MainWindow::on_redSlider_valueChanged(int value)
{
    ui->labelValueRed->setText(QString::number(value)); /// обновляем значение QLabel

    /// создаём изображение-предосмотр и меняем ему значение красного оттенка методом объекта класса Img
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());
    temp_image.changeRgb(value, ui->greenSlider->value(), ui->blueSlider->value());

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(temp_image.getImage().scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменение значения объекта класса QSlider "Зелёный"
  */
void MainWindow::on_greenSlider_valueChanged(int value)
{
    ui->labelValueGreen->setText(QString::number(value)); /// обновляем значение QLabel

    /// создаём изображение-предосмотр и меняем ему значение зелёного методом объекта класса Img
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());
    temp_image.changeRgb(ui->redSlider->value(), value, ui->blueSlider->value());

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(temp_image.getImage().scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменение значения объекта класса QSlider "Синий"
  */
void MainWindow::on_blueSlider_valueChanged(int value)
{
    ui->labelValueBlue->setText(QString::number(value)); /// обновляем значение QLabel

    /// создаём изображение-предосмотр и меняем ему значение синего оттенка методом объекта класса Img
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());
    temp_image.changeRgb(ui->redSlider->value(), ui->greenSlider->value(), value);

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(temp_image.getImage().scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);
}

/**
  * Функция изменения цветового баланса, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Ok" c предопределённой
  * "AcceptRole"
  */
void MainWindow::on_buttonRgbBox_accepted()
{
    /// меняем изображению-копии цветовой баланс методом объекта класса Img
    (*image).changeRgb(ui->redSlider->value(), ui->greenSlider->value(), ui->blueSlider->value());

    /// обновляем изображение на экране и перезаписываем таблицу основных цветовых оттенков изображения
    updateImage();
    paletteTable();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования цветового баланса
    ui->menubar->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->groupBoxRgb->hide();
}

/**
  * Функция закрытия окна цветового баланса, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Cancel" c предопределённой
  * "RejectRole"
  */
void MainWindow::on_buttonRgbBox_rejected()
{
    /// убираем предосмотр, обновляя объект формы Qt Designer с изображением
    updateImage();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования цветового баланса
    ui->menubar->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->groupBoxRgb->hide();
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Насыщенность"
  */
void MainWindow::on_actionSaturation_triggered()
{
    ui->groupBoxSaturation->show(); /// отображаем объект класса QGroupBox с настройкой контрастности изображения

    /// блокируем доступ к остальным функциям редактирования изображения
    ui->menubar->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cropButton->setEnabled(false);

    /// устанавливаем значение насыщенности по умолчанию объектам классов QSlider и QLabel
    ui->saturationSlider->setValue(0);
    ui->valueContrast->setText(QString::number(ui->contrastSlider->value()));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменение значения объекта класса QSlider "Насыщенность"
  */
void MainWindow::on_saturationSlider_valueChanged(int value)
{
    ui->valueSaturation->setText(QString::number(value)); /// обновляем значение QLabel

    /// создаём изображение-предосмотр и меняем ему значение насыщения методом объекта класса Img
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());
    temp_image.changeSaturation(float(value));

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(temp_image.getImage().scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);
}

/**
  * Функция изменения насыщенности, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Ok" c предопределённой
  * "AcceptRole"
  */
void MainWindow::on_buttonSaturationBox_accepted()
{
    /// меняем изображению-копии насыщенность методом объекта класса Img
    (*image).changeSaturation(ui->saturationSlider->value());

    /// обновляем изображение на экране и перезаписываем таблицу основных цветовых оттенков изображения
    updateImage();
    paletteTable();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования насыщенности
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->menubar->setEnabled(true);
    ui->groupBoxSaturation->hide();
}

/**
  * Функция закрытия окна насыщенности, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Cancel" c предопределённой
  * "RejectRole"
  */
void MainWindow::on_buttonSaturationBox_rejected()
{
    /// убираем предосмотр, обновляя объект формы Qt Designer с изображением
    updateImage();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования насыщенности
    ui->resetButton->setEnabled(true);
    ui->menubar->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->groupBoxSaturation->hide();
    ui->saturationSlider->setValue(0);
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Контраст"
  */
void MainWindow::on_actionContrast_triggered()
{
    ui->groupBoxContrast->show(); /// отображаем объект класса QGroupBox с настройкой контрастности изображения

    /// блокируем доступ к остальным функциям редактирования изображения
    ui->menubar->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cropButton->setEnabled(false);

    /// устанавливаем значение контраста по умолчанию объектам классов QSlider и QLabel
    ui->contrastSlider->setValue(0);
    ui->valueContrast->setText(QString::number(ui->contrastSlider->value()));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменение значения объекта класса QSlider "Контраст"
  */
void MainWindow::on_contrastSlider_valueChanged(int value)
{
    ui->valueContrast->setText(QString::number(value)); /// обновляем значение QLabel

    /// создаём изображение-предосмотр и меняем ему значение контраста методом объекта класса Img
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());
    temp_image.changeContrast(value);

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newImg;
    newImg = newImg.fromImage(temp_image.getImage().scaled(880, 600, Qt::KeepAspectRatio));
    ui->proImage->setPixmap(newImg);

}

/**
  * Функция изменения контраста, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Ok" c предопределённой
  * "AcceptRole"
  */
void MainWindow::on_buttonContrastBox_accepted()
{
    /// меняем изображению-копии контраст методом объекта класса Img
    (*image).changeContrast(ui->valueContrast->text().toInt());

    /// обновляем изображение на экране и перезаписываем таблицу основных цветовых оттенков изображения
    updateImage();
    paletteTable();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования контраста
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->menubar->setEnabled(true);
    ui->groupBoxContrast->hide();
}

/**
  * Функция закрытия окна контраста, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Cancel" c предопределённой
  * "RejectRole"
  */
void MainWindow::on_buttonContrastBox_rejected()
{
    /// убираем предосмотр, обновляя объект формы Qt Designer с изображением
    updateImage();

    /// возвращаем доступ к остальным функциям программы и убираем окно редактирования контраста
    ui->menubar->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->groupBoxContrast->hide();
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Выход"
  */
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из боковой
  * панели кнопки-объекта класса QPushButton с символом сброса
  */
void MainWindow::on_resetButton_clicked()
{
    /// применяем функцию восстановления изображения
    (*image).resetImage();

    /// обновляем объект формы Qt Designer с изображением и обновляем палитру главных цветовых оттенков
    updateImage();
    paletteTable();

    /// обновляем значения объекта QLabel с информацией о размере изображения
    int width = Image.width(), height = Image.height();
    ui->labelSize->setText(QString(" Размер изображения: %1 px x %2 px").arg(width).arg(height));

    /// обновляем значения объектов QSpinBox, устанавливая исходную ширину и высоту изображения
    ui->widthBox->setMaximum(width);
    ui->heightBox->setMaximum(height);
    ui->widthBox->setValue(width);
    ui->heightBox->setValue(height);
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из боковой
  * панели кнопки-объекта класса QPushButton с символом обрезки
  */
void MainWindow::on_cropButton_clicked()
{
    /// включаем отображение панели с параметрами обрезки
    ui->groupBoxCrop->show();

    /// отключаем доступ к остальным функциям программы
    ui->menubar->setEnabled(false);
    ui->cropButton->setEnabled(false);
    ui->resetButton->setEnabled(false);

    /// устанавливаем объектам класса QSpinBox "Ширина" и "Высота" максимальные и изначальные значения
    ui->widthBox->setMaximum((*image).getWidth());
    ui->heightBox->setMaximum((*image).getHeight());
    ui->widthBox->setValue((*image).getWidth());
    ui->heightBox->setValue((*image).getHeight());
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменения значения объекта класса QSpinBox "Ширина"
  */
void MainWindow::on_widthBox_valueChanged(int width)
{
    /// создаём копию объекта Img для обрезки
    Img temp_image(image->getImage(), image->getWidth(), image->getHeight());

    /// формируем область для обрезки, переопределяем параметры объекта QRect в случае,
    /// если выбрана обрезка по центру, а не из верхнего левого края
    QRect rect(0, 0, width, ui->heightBox->value());
    if (ui->radioButtonCenter->isChecked()) {
        rect.setX(0 + (image->getWidth() - (qreal)width)/2);
        rect.setY(0 + (image->getHeight() - (qreal)ui->heightBox->value())/2);
        rect.setWidth((qreal)width);
        rect.setHeight((qreal)ui->heightBox->value());
    }

    /// формируем новое изображение-предосмотр, применив к новому объекту класса Img функцию обрезки
    QImage img = temp_image.copy(rect);

    /// обновляем объект формы Qt Designer с изображением, заменяя его на изображение-предосмотр
    QPixmap newImg;
    newImg = newImg.fromImage(img);
    ui->proImage->setPixmap(newImg.scaled(880, 600, Qt::KeepAspectRatio));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на изменения значения объекта класса QSpinBox "Высота",
  *
  */
void MainWindow::on_heightBox_valueChanged(int height)
{
    /// создаём копию объекта Img для обрезки
    Img temp_image(image->getImage(), image->getWidth(), height);

    /// формируем область для обрезки, переопределяем параметры объекта QRect в случае,
    /// если выбрана обрезка по центру, а не из верхнего левого края
    QRect rect(0, 0, ui->widthBox->value(), height);
    if (ui->radioButtonCenter->isChecked()) {
        rect.setX(0 + (image->getWidth() - (qreal)ui->widthBox->value())/2);
        rect.setY(0 + (image->getHeight() - (qreal)height)/2);
        rect.setWidth((qreal)ui->widthBox->value());
        rect.setHeight((qreal)height);
    }

    /// формируем новое изображение-предосмотр, применив к новому объекту класса Img функцию обрезки
    QImage img = temp_image.copy(rect);

    /// обновляем объект формы Qt Designer с изображением, заменяя его на изображение-предосмотр
    QPixmap newImg;
    newImg = newImg.fromImage(img);
    ui->proImage->setPixmap(newImg.scaled(880, 600, Qt::KeepAspectRatio));
}

/**
  * Функция обрезки, предопределённая в Qt Designer, вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Ok" c предопределённой
  * "AcceptRole"
  */
void MainWindow::on_buttonCropBox_accepted()
{
    /// формируем область для обрезки, переопределяем параметры объекта QRect в случае,
    /// если выбрана обрезка по центру, а не из верхнего левого края
    QRect rect(0, 0, ui->widthBox->value(), ui->heightBox->value());
    if (ui->radioButtonCenter->isChecked()) {
        rect.setX(0 + (image->getWidth() - (qreal)ui->widthBox->value())/2);
        rect.setY(0 + (image->getHeight() - (qreal)ui->heightBox->value())/2);
        rect.setWidth((qreal)ui->widthBox->value());
        rect.setHeight((qreal)ui->heightBox->value());
    }

    /// создаём новое обрезанное изображение из старого
    QImage img = (*image).copy(rect);

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newPic;
    newPic = newPic.fromImage(img);
    ui->proImage->setPixmap(newPic.scaled(880, 600, Qt::KeepAspectRatio));

    /// обновляем значения объектов QSpinBox, устанавливая новую ширину и высоту изображения
    ui->widthBox->setMaximum(ui->widthBox->value());
    ui->heightBox->setMaximum(ui->heightBox->value());
    ui->widthBox->setValue(ui->widthBox->value());
    ui->heightBox->setValue(ui->heightBox->value());

    /// возвращаем доступ к остальным функциям программы и убираем окно обрезки
    ui->menubar->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->groupBoxCrop->hide();

    /// обновляем значения объекта QLabel с информацией о размере изображения
    int width = (*image).getWidth(), height = (*image).getHeight();
    ui->labelSize->setText(QString(" Размер изображения: %1 px x %2 px").arg(width).arg(height));
    paletteTable(); /// обновляем цветовую палитру
}

/**
  * Функция закрытия окна обрезки, предопределённая в Qt Designer,
  * вызываемая в ответ на нажатие кнопки-объекта класса QDialogButtonBox "Cancel" c предопределённой
  * "RejectRole"
  */
void MainWindow::on_buttonCropBox_rejected()
{
    /// убираем предосмотр, обновляя объект формы Qt Designer с изображением
    updateImage();

    /// возвращаем доступ к остальным функциям программы и убираем окно обрезки
    ui->menubar->setEnabled(true);
    ui->cropButton->setEnabled(true);
    ui->resetButton->setEnabled(true);
    ui->widthBox->setValue((*image).getWidth());
    ui->heightBox->setValue((*image).getHeight());
    ui->groupBoxCrop->hide();
}

/**
  * Функция-слот, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Обесцветить"
  */
void MainWindow::on_actionMonochrome_triggered()
{
    /// обесцвечиваем изображение методом класса Img
    (*image).convertToGreyScale();

    /// обновляем объект формы Qt Designer с изображением и обновляем палитру главных цветовых оттенков
    updateImage();
    paletteTable();
}

/**
  * Функция-слот, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "90°"
  */
void MainWindow::on_action90_triggered()
{
    /// методом объекта класса Img создаем новое изображение, повернутое на 90°
    QImage img = (*image).rotate(1);
    /// обновляем объект формы Qt Designer с изображением
    QPixmap newPic;
    newPic = newPic.fromImage(img);
    ui->proImage->setPixmap(newPic.scaled(880, 600, Qt::KeepAspectRatio));
}

/**
  * Функция-слот, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "-90°"
  */
void MainWindow::on_action_90_triggered()
{
    /// методом объекта класса Img создаем новое изображение, повернутое на -90°
    QImage img = (*image).rotate(0);

    /// обновляем объект формы Qt Designer с изображением
    QPixmap newPic;
    newPic = newPic.fromImage(img);
    ui->proImage->setPixmap(newPic.scaled(880, 600, Qt::KeepAspectRatio));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "180°"
  */
void MainWindow::on_action180_triggered()
{
    /// применяя 2 раза метод объекта класса Img создаем новое изображение, повернутое на 180°
    QImage img = (*image).rotate(1);
    img = (*image).rotate(1);
    /// обновляем объект формы Qt Designer с изображением
    QPixmap newPic;
    newPic = newPic.fromImage(img);
    ui->proImage->setPixmap(newPic.scaled(880, 600, Qt::KeepAspectRatio));
}

/**
  * Функция, предопределённая в Qt Designer, вызываемая в ответ на нажатие из меню кнопки-объекта класса QAction "Инверсия"
  */
void MainWindow::on_actionInversion_triggered()
{
    /// инвертируем изображение
    (*image).negative();
    /// обновляем изображение на экране и перезаписываем таблицу основных цветовых оттенков изображения
    updateImage();
    paletteTable();
}
