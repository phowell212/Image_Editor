#include <opencv4/opencv2/highgui/highgui.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat originalImage, currentImage;
int zoomLevel = 100;



void applyZoom() {
    Mat zoomedImage;
    double scale = zoomLevel / 100.0;
    resize(currentImage, zoomedImage, Size(), scale, scale);
}

void applySharpener() {
    int intensity = getTrackbarPos("Sharpen/Blur", "Image Editor") - 10; // Scale intensity to [-10, 10]

    if (intensity > 0) {
        // Sharpening
        Mat kernel = (Mat_<float>(3, 3) << -intensity, -intensity, -intensity,
                -intensity, intensity * 9, -intensity,
                -intensity, -intensity, -intensity);
        filter2D(originalImage, currentImage, -1, kernel);
    } else {
        // Blurring
        int blurSize = 2 * (-intensity) + 1; // Ensure blurSize is odd
        GaussianBlur(originalImage, currentImage, Size(blurSize, blurSize), 0, 0);
    }
}

void applyHighlighter() {
    int edgeHighlightSliderValue = getTrackbarPos("Edge Highlight", "Image Editor");

    if (edgeHighlightSliderValue > 0) {
        Mat gray, grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y, edge_highlighted;

        // Convert to grayscale
        cvtColor(currentImage, gray, COLOR_BGR2GRAY);

        // Apply Sobel operator in the X and Y direction
        Sobel(gray, grad_x, CV_16S, 1, 0, 3);
        Sobel(gray, grad_y, CV_16S, 0, 1, 3);
        convertScaleAbs(grad_x, abs_grad_x);
        convertScaleAbs(grad_y, abs_grad_y);

        // Combine the two gradients with weight depending on the slider value
        double alpha = 0.5 * edgeHighlightSliderValue / 10.0;
        addWeighted(abs_grad_x, alpha, abs_grad_y, alpha, 0, edge_highlighted);

        // Blend the edge-highlighted image with the original color image
        Mat edge_highlighted_color;
        cvtColor(edge_highlighted, edge_highlighted_color, COLOR_GRAY2BGR);
        double beta = 1.0 - alpha; // Weight for the original image
        addWeighted(currentImage, beta, edge_highlighted_color, alpha, 0, currentImage);
    }
}

void applyContrastAdjuster() {
    int contrastValue = getTrackbarPos("Contrast", "Image Editor");

    // Adjust the mapping to provide higher contrast.
    double contrastFactor = 3.0 * (contrastValue / 100.0);

    currentImage.forEach<Vec3b>([&contrastFactor](Vec3b &pixel, const int * position) -> void {
        for (int i = 0; i < 3; i++) {
            // Apply contrast effect: Formula -> pixelValue = (pixelValue - 128) * factor + 128
            pixel[i] = saturate_cast<uchar>((pixel[i] - 128) * contrastFactor + 128);
        }
    });
}

void applyHueAdjuster() {
    int hueValue = getTrackbarPos("Hue", "Image Editor") - 90; // Adjust based on the range of the slider

    Mat hsvImage;
    cvtColor(currentImage, hsvImage, COLOR_BGR2HSV);

    vector<Mat> hsvChannels;
    split(hsvImage, hsvChannels);

    // Adjust Hue
    hsvChannels[0] += hueValue;
    merge(hsvChannels, hsvImage);

    cvtColor(hsvImage, currentImage, COLOR_HSV2BGR);
}

void onTrackbarChange(int, void*) {
    applySharpener();
    applyHighlighter();
    applyContrastAdjuster();
    applyHueAdjuster();
    imshow("Image Editor", currentImage);
}


int main() {

    // Update the path to the image file
    string imagePath = "/mnt/c/Users/h/CLionProjects/Image_Editor_Test/rye.jpg";
    originalImage = imread(imagePath);
    currentImage = originalImage.clone();

    if (originalImage.empty()) {
        cout << "Could not open or find the image at " << imagePath << endl;
        return -1;
    }

    if (originalImage.cols > 1200 || originalImage.rows > 900) {
        // Calculate the scaling factor to maintain aspect ratio
        double scalingFactor = min(1200.0 / originalImage.cols, 900.0 / originalImage.rows);
        resize(originalImage, originalImage, Size(), scalingFactor, scalingFactor, INTER_AREA);
    }

    currentImage = originalImage.clone();

    namedWindow("Image Editor", WINDOW_AUTOSIZE);

    // Initialize UI elements
    createTrackbar("Sharpen/Blur", "Image Editor", nullptr, 20, onTrackbarChange);
    createTrackbar("Edge Highlight", "Image Editor", nullptr, 10, onTrackbarChange);
    createTrackbar("Contrast", "Image Editor", nullptr, 100, onTrackbarChange);
    createTrackbar("Hue", "Image Editor", nullptr, 100, onTrackbarChange);


    // Set initial trackbar positions
    setTrackbarPos("Sharpen/Blur", "Image Editor", 10);
    setTrackbarPos("Edge Highlight", "Image Editor", 0);
    setTrackbarPos("Contrast", "Image Editor", 33);
    setTrackbarPos("Hue", "Image Editor", 90);


    // Display initial image
    imshow("Image Editor", currentImage);

    int key;
    while (true) {
        key = waitKey(1);

        if (key == 27) { // Escape key pressed
            break;
        } else if (key == 122) { // Z key pressed
            zoomLevel += 10;
            applyZoom();
        } else if (key == 120) { // X key pressed
            zoomLevel -= 10;
            if (zoomLevel < 10) zoomLevel = 10;
            applyZoom();
        }
    }

    return 0;
}
