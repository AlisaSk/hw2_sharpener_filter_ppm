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
    int depth;

    fscanf(f, "P6 %d %d %d\n", &width, &heigh, &depth);

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
    int y;
    int pix = 0;
    int k = 1;
    short newRed, newGreen, newBlue;

    for (; pix < numPixels; pix++) {
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
            // printf("pix %d  y %d\n", pix, y);
            // printf("his in y %d is %d\n", y, histogram[y]);
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
        newRed = -image[pix-width].red - image[pix-1].red + 5*image[pix].red - image[pix+1].red - image[pix+width].red;
        newGreen = -image[pix-width].green - image[pix-1].green + 5*image[pix].green - image[pix+1].green - image[pix+width].green;
        newBlue = -image[pix-width].blue - image[pix-1].blue + 5*image[pix].blue - image[pix+1].blue - image[pix+width].blue; 

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
