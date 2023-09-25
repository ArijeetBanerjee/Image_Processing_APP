#include <QApplication>
#include <QMainWindow>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QVBoxLayout>
#include <QSlider>
#include <QPixmap>
#include <QImage>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>

class ImageProcessingApp : public QMainWindow {
    Q_OBJECT

public:
    ImageProcessingApp(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
    }

private slots:
    void openImage() {
        QString filePath = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
        if (!filePath.isEmpty()) {
            loadImage(filePath);
        }
    }

    void applyFilter() {
        if (!image.isNull()) {
            QString selectedFilter = filterComboBox->currentText();

            if (selectedFilter == "Original") {
                // Reset to the original image
                displayImage(originalImage);
            } else if (selectedFilter == "Grayscale") {
                processedImage = image.convertToFormat(QImage::Format_Grayscale8);
                displayImage(processedImage);
            } else if (selectedFilter == "Blur") {
                // Apply a basic Gaussian blur filter
                processedImage = applyGaussianBlur(image);
                displayImage(processedImage);
            } else if (selectedFilter == "Sharpen") {
                // Implement sharpening filter
                processedImage = sharpenImage(image);
                displayImage(processedImage);
            }
        }
    }

    void saveImage() {
        if (!processedImage.isNull()) {
            QString filePath = QFileDialog::getSaveFileName(this, "Save Image", "", "Images (*.png *.jpg *.jpeg *.bmp)");
            if (!filePath.isEmpty()) {
                processedImage.save(filePath);
            }
        }
    }

    void adjustColor() {
        if (!image.isNull()) {
            int brightnessValue = brightnessSlider->value();
            int contrastValue = contrastSlider->value();

            processedImage = adjustColorBrightnessContrast(image, brightnessValue, contrastValue);
            displayImage(processedImage);
        }
    }

    void cropAndResize() {
        if (!image.isNull()) {
            int width = cropWidthSlider->value();
            int height = cropHeightSlider->value();

            processedImage = cropAndResizeImage(image, width, height);
            displayImage(processedImage);
        }
    }

private:
    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        setCentralWidget(centralWidget);

        imageLabel = new QLabel(this);
        layout->addWidget(imageLabel);

        QMenuBar *menuBar = new QMenuBar(this);
        setMenuBar(menuBar);

        QMenu *fileMenu = menuBar->addMenu("File");
        QAction *openAction = fileMenu->addAction("Open Image");
        connect(openAction, &QAction::triggered, this, &ImageProcessingApp::openImage);
        QAction *saveAction = fileMenu->addAction("Save Processed Image");
        connect(saveAction, &QAction::triggered, this, &ImageProcessingApp::saveImage);

        filterComboBox = new QComboBox(this);
        filterComboBox->addItem("Original");
        filterComboBox->addItem("Grayscale");
        filterComboBox->addItem("Blur");
        filterComboBox->addItem("Sharpen");
        layout->addWidget(filterComboBox);

        applyFilterButton = new QPushButton("Apply Filter", this);
        layout->addWidget(applyFilterButton);
        connect(applyFilterButton, &QPushButton::clicked, this, &ImageProcessingApp::applyFilter);

        brightnessSlider = new QSlider(Qt::Horizontal, this);
        brightnessSlider->setMinimum(-100);
        brightnessSlider->setMaximum(100);
        brightnessSlider->setValue(0);
        layout->addWidget(brightnessSlider);

        contrastSlider = new QSlider(Qt::Horizontal, this);
        contrastSlider->setMinimum(-100);
        contrastSlider->setMaximum(100);
        contrastSlider->setValue(0);
        layout->addWidget(contrastSlider);

        adjustColorButton = new QPushButton("Adjust Color", this);
        layout->addWidget(adjustColorButton);
        connect(adjustColorButton, &QPushButton::clicked, this, &ImageProcessingApp::adjustColor);

        cropWidthSlider = new QSlider(Qt::Horizontal, this);
        cropWidthSlider->setMinimum(1);
        cropWidthSlider->setMaximum(999);
        cropWidthSlider->setValue(100);
        layout->addWidget(cropWidthSlider);

        cropHeightSlider = new QSlider(Qt::Horizontal, this);
        cropHeightSlider->setMinimum(1);
        cropHeightSlider->setMaximum(999);
        cropHeightSlider->setValue(100);
        layout->addWidget(cropHeightSlider);

        cropResizeButton = new QPushButton("Crop and Resize", this);
        layout->addWidget(cropResizeButton);
        connect(cropResizeButton, &QPushButton::clicked, this, &ImageProcessingApp::cropAndResize);

        setWindowTitle("Image Processing App");
        resize(800, 600);
    }

    void loadImage(const QString &filePath) {
        originalImage = QImage(filePath);  // Store the original image
        image = originalImage.copy();
        if (!image.isNull()) {
            processedImage = image;
            displayImage(image);
        }
    }

    void displayImage(const QImage &img) {
        QPixmap pixmap = QPixmap::fromImage(img);
        imageLabel->setPixmap(pixmap);
        imageLabel->adjustSize();
    }

    QImage sharpenImage(const QImage &inputImage) {
        QImage outputImage = inputImage;
        int kernelSize = 3;  // Adjust the kernel size as needed
        int kernel[3][3] = {
            {0, -1, 0},
            {-1, 5, -1},
            {0, -1, 0}
        };

        int width = inputImage.width();
        int height = inputImage.height();

        for (int y = kernelSize / 2; y < height - kernelSize / 2; ++y) {
            for (int x = kernelSize / 2; x < width - kernelSize / 2; ++x) {
                int sumRed = 0, sumGreen = 0, sumBlue = 0;

                for (int ky = 0; ky < kernelSize; ++ky) {
                    for (int kx = 0; kx < kernelSize; ++kx) {
                        QRgb pixel = inputImage.pixel(x + kx - kernelSize / 2, y + ky - kernelSize / 2);
                        sumRed += qRed(pixel) * kernel[ky][kx];
                        sumGreen += qGreen(pixel) * kernel[ky][kx];
                        sumBlue += qBlue(pixel) * kernel[ky][kx];
                    }
                }

                int newRed = qBound(0, sumRed, 255);
                int newGreen = qBound(0, sumGreen, 255);
                int newBlue = qBound(0, sumBlue, 255);

                outputImage.setPixel(x, y, qRgb(newRed, newGreen, newBlue));
            }
        }

        return outputImage;
    }

    QImage adjustColorBrightnessContrast(const QImage &inputImage, int brightnessValue, int contrastValue) {
        QImage outputImage = inputImage;
        int width = inputImage.width();
        int height = inputImage.height();

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                QRgb pixel = inputImage.pixel(x, y);
                int red = qRed(pixel);
                int green = qGreen(pixel);
                int blue = qBlue(pixel);

                // Adjust brightness
                red = qBound(0, red + brightnessValue, 255);
                green = qBound(0, green + brightnessValue, 255);
                blue = qBound(0, blue + brightnessValue, 255);

                // Adjust contrast
                red = qBound(0, (red - 128) * contrastValue / 100 + 128, 255);
                green = qBound(0, (green - 128) * contrastValue / 100 + 128, 255);
                blue = qBound(0, (blue - 128) * contrastValue / 100 + 128, 255);

                outputImage.setPixel(x, y, qRgb(red, green, blue));
            }
        }

        return outputImage;
    }

    QImage applyGaussianBlur(const QImage &inputImage) {
        QImage outputImage = inputImage;
        int kernelSize = 5;  // Adjust the kernel size as needed
        int kernel[5][5] = {
            {1, 4, 6, 4, 1},
            {4, 16, 24, 16, 4},
            {6, 24, 36, 24, 6},
            {4, 16, 24, 16, 4},
            {1, 4, 6, 4, 1}
        };

        int divisor = 256;  // The sum of the kernel values

        int width = inputImage.width();
        int height = inputImage.height();

        for (int y = kernelSize / 2; y < height - kernelSize / 2; ++y) {
            for (int x = kernelSize / 2; x < width - kernelSize / 2; ++x) {
                int sumRed = 0, sumGreen = 0, sumBlue = 0;

                for (int ky = 0; ky < kernelSize; ++ky) {
                    for (int kx = 0; kx < kernelSize; ++kx) {
                        QRgb pixel = inputImage.pixel(x + kx - kernelSize / 2, y + ky - kernelSize / 2);
                        sumRed += qRed(pixel) * kernel[ky][kx];
                        sumGreen += qGreen(pixel) * kernel[ky][kx];
                        sumBlue += qBlue(pixel) * kernel[ky][kx];
                    }
                }

                int newRed = qBound(0, sumRed / divisor, 255);
                int newGreen = qBound(0, sumGreen / divisor, 255);
                int newBlue = qBound(0, sumBlue / divisor, 255);

                outputImage.setPixel(x, y, qRgb(newRed, newGreen, newBlue));
            }
        }

        return outputImage;
    }

    QImage cropAndResizeImage(const QImage &inputImage, int width, int height) {
        QImage outputImage = inputImage.scaled(width, height, Qt::KeepAspectRatio);
        return outputImage;
    }

    void applyGrayscale() {
        if (!image.isNull()) {
            processedImage = image.convertToFormat(QImage::Format_Grayscale8);
            displayImage(processedImage);
        }
    }

    QLabel *imageLabel;
    QComboBox *filterComboBox;
    QPushButton *applyFilterButton;
    QSlider *brightnessSlider;
    QSlider *contrastSlider;
    QPushButton *adjustColorButton;
    QSlider *cropWidthSlider;
    QSlider *cropHeightSlider;
    QPushButton *cropResizeButton;
    QImage image;
    QImage processedImage;
    QImage originalImage;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ImageProcessingApp mainWindow;
    mainWindow.show();
    return app.exec();
}

#include "main.moc"
