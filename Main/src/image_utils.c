#include "config.h"

typedef enum
{
    ROTATE_0 = 0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
} ImageRotation;

void GLCD_DrawImageWithRotation(unsigned char xStart, unsigned char yStart, unsigned char *image, unsigned char width, unsigned char height, ImageRotation rotation)
{
    unsigned char x, y;
    unsigned char byte, bit;
    unsigned char rotatedX, rotatedY;

    if ((rotation == ROTATE_0 && xStart + width > 64) ||
        (rotation == ROTATE_90 && xStart + height > 64) ||
        (rotation == ROTATE_180 && xStart + width > 64) ||
        (rotation == ROTATE_270 && xStart + height > 64) ||
        yStart + width > 128)
    {

        return;
    }

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            byte = image[y * (width / 8) + (x / 8)];
            bit = 1 << (7 - (x % 8));

            if (byte & bit)
            {

                if (rotation == ROTATE_0)
                {

                    rotatedX = xStart + x;
                    rotatedY = yStart + y;
                }
                else if (rotation == ROTATE_90)
                {

                    rotatedX = xStart + y;
                    rotatedY = yStart + (width - 1 - x);
                }
                else if (rotation == ROTATE_180)
                {

                    rotatedX = xStart + (width - 1 - x);
                    rotatedY = yStart + (height - 1 - y);
                }
                else if (rotation == ROTATE_270)
                {

                    rotatedX = xStart + (height - 1 - y);
                    rotatedY = yStart + x;
                }

                GLCD_Dot(rotatedX, rotatedY);
            }
        }
    }
};