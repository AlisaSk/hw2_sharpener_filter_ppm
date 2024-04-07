#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

int main(int argc, char const *argv[])
{
    // open source image
    FILE *f = fopen(argv[1], "rb");

    int width;
    int heigh;

    fscanf(f, "P6 %d %d 255\n", &width, &heigh);

    // parse source image
    int numPixels = width*heigh;
    // printf("w %d h %d\n", width, heigh);
    Pixel* image = (Pixel*)malloc(numPixels*sizeof(Pixel));

    fread(image, sizeof(Pixel), numPixels, f);
    fclose(f);

    // create new image
    Pixel* newImage = (Pixel*)malloc(numPixels*sizeof(Pixel));

    // init grayscale histogram
    int histogram[] = {0, 0, 0, 0, 0, 0};

    // add pix to newImage
    int y = 0;
    int k = 1;
    short newRed = 0, newGreen = 0, newBlue = 0;

    for (int pix = 0; pix < numPixels; pix++) {
      
        // copy first and last row
        if (pix < width || pix >= numPixels - width) {
            newImage[pix] = image[pix];
            y = (0.2126*newImage[pix].red + 0.7152*newImage[pix].green + 0.0722*newImage[pix].blue + 0.5)/51;
            histogram[y]++;
            continue;
        }
        // copy first...
        if (pix == width * k) {
            newImage[pix] = image[pix];
            y = (0.2126*newImage[pix].red + 0.7152*newImage[pix].green + 0.0722*newImage[pix].blue + 0.5)/51;
            histogram[y]++;
            continue;
        }
        // ....and last column
        if (pix == width * k + width - 1) {
            k++;
            newImage[pix] = image[pix];
            y = (0.2126*newImage[pix].red + 0.7152*newImage[pix].green + 0.0722*newImage[pix].blue + 0.5)/51;
            histogram[y]++;
            continue;
        }
        // create new RGB
        int upper = pix-width;
        int lower = pix+width;
        int right = pix + 1;
        int left = pix - 1;

        Pixel pixUpper = image[upper];
        Pixel pixLower = image[lower];
        Pixel pixRight = image[right];
        Pixel pixLeft = image[left];
        Pixel pixCenter = image[pix];


        newRed = -pixUpper.red - pixLeft.red + 5*pixCenter.red - pixRight.red - pixLower.red;
        newGreen = -pixUpper.green - pixLeft.green + 5*pixCenter.green - pixRight.green - pixLower.green;
        newBlue = -pixUpper.blue - pixLeft.blue + 5*pixCenter.blue - pixRight.blue - pixLower.blue; 
        
        newImage[pix].red = newRed > 0 ? (newRed < 255 ? newRed: 255): 0;
        newImage[pix].green = newGreen > 0 ? (newGreen < 255 ? newGreen: 255): 0;
        newImage[pix].blue = newBlue > 0 ? (newBlue < 255 ? newBlue: 255): 0;
        y = (0.2126*newImage[pix].red + 0.7152*newImage[pix].green + 0.0722*newImage[pix].blue + 0.5)/51;
        histogram[y]++;
    }

    free(image);

    // write newImage into new ppm
    f = fopen("output.ppm", "wb");
    fprintf(f, "P6\n%d\n%d\n255\n",width,heigh);
    fwrite(newImage, sizeof(Pixel), numPixels, f);

    fclose(f);

    free(newImage);

    // write histogram to txt
    f = fopen("output.txt", "wb");
    fprintf(f, "%d %d %d %d %d", histogram[0], histogram[1], histogram[2], histogram[3], histogram[4] + histogram[5]);
    fclose(f);

    return 0;
}
