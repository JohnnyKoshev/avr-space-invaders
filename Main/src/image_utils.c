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

    // Check if the image will fit within the screen resolution (64x128)
    if ((rotation == ROTATE_0 && xStart + width > 64) ||
        (rotation == ROTATE_90 && xStart + height > 64) ||
        (rotation == ROTATE_180 && xStart + width > 64) ||
        (rotation == ROTATE_270 && xStart + height > 64) ||
        yStart + width > 128)
    {
        // Ensure the yStart + width is within 128 (max height)
        return; // Image does not fit on the screen
    }

    // Loop through the image pixel data
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            byte = image[y * (width / 8) + (x / 8)]; // Determine the byte for the current column
            bit = 1 << (7 - (x % 8));                 // Calculate the bit position

            if (byte & bit)
            {
                // Handle each rotation case and adjust coordinates accordingly
                if (rotation == ROTATE_0)
                {
                    // No rotation (original image)
                    rotatedX = xStart + x;
                    rotatedY = yStart + y;
                }
                else if (rotation == ROTATE_90)
                {
                    // 90-degree clockwise rotation: (x, y) -> (y, width - 1 - x)
                    rotatedX = xStart + y;
                    rotatedY = yStart + (width - 1 - x);
                }
                else if (rotation == ROTATE_180)
                {
                    // 180-degree clockwise rotation: (x, y) -> (width - 1 - x, height - 1 - y)
                    rotatedX = xStart + (width - 1 - x);
                    rotatedY = yStart + (height - 1 - y);
                }
                else if (rotation == ROTATE_270)
                {
                    // 270-degree clockwise rotation: (x, y) -> (height - 1 - y, x)
                    rotatedX = xStart + (height - 1 - y);
                    rotatedY = yStart + x;
                }

                // Draw the pixel at the rotated coordinates
                GLCD_Dot(rotatedX, rotatedY);
            }
        }
    }
};