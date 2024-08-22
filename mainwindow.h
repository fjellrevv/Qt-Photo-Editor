#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QImage>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Класс главного окна приложения.
 *
 * Класс содержит в себе методы-слоты к компонентам формы Qt Designer (mainwindow.ui), а также методы, необходимые для обновления изображения
 * и соответствующей ему цветовой палитры, также состоящей из компонентов формы Qt Designer. Методы описаны в cpp-файле.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_actionOpen_triggered();
    void on_actionSave_as_triggered();
    void on_actionExit_triggered();
    void on_actionClose_triggered();

    void on_action90_triggered();
    void on_action_90_triggered();
    void on_action180_triggered();

    void on_actionInversion_triggered();
    void on_actionMonochrome_triggered();
    void on_actionContrast_triggered();
    void on_actionBrightness_triggered();
    void on_actionSaturation_triggered();
    void on_actionRgb_triggered();

private slots:

    void updateImage();

    void on_resetButton_clicked();
    void on_cropButton_clicked();

    void on_buttonContrastBox_accepted();
    void on_buttonBrightnessBox_accepted();
    void on_buttonSaturationBox_accepted();
    void on_buttonRgbBox_accepted();
    void on_buttonCropBox_accepted();

    void on_buttonContrastBox_rejected();
    void on_buttonBrightnessBox_rejected();
    void on_buttonSaturationBox_rejected();
    void on_buttonRgbBox_rejected();
    void on_buttonCropBox_rejected();

    void on_contrastSlider_valueChanged(int value);
    void on_brightnessSlider_valueChanged(int value);
    void on_saturationSlider_valueChanged(int value);

    void on_redSlider_valueChanged(int value);
    void on_greenSlider_valueChanged(int value);
    void on_blueSlider_valueChanged(int value);

    void on_widthBox_valueChanged(int width);
    void on_heightBox_valueChanged(int height);

    void paletteTable();

private:

    Ui::MainWindow *ui; /// объект формы Qt Designer
};
#endif // MAINWINDOW_H
