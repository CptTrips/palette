// Create a single-colored bitmap
#include <iostream>
#include "palette.h"

// Height and width in pixels
const int h = 1081;
const int w = 1919;

void pixel_color(int* col, int y, int x)
{
  // fill col with rgb pixel color
  col[0] = int(float(x)/float(w)*255.99); //R
  col[1] = int(float(y)/float(h)*255.99); //G
  col[2] = 51; //B
}

void shade_buffer(char* pixel_buffer)
{
  int col[3];

  for (int r=0; r<h; r++)
  {
    for (int c=0; c<w; c++)
    {

      // Calculate the color for pixel (r, c)
      pixel_color(col, r, c);

      // Copy the pixel color to the buffer
      pixel_buffer[r*w*3 + c*3 + 0] = col[0]; // R
      pixel_buffer[r*w*3 + c*3 + 1] = col[1]; // G
      pixel_buffer[r*w*3 + c*3 + 2] = col[2]; // B
    }
  }
}

int main(int argc, char* argv[])
{

  char *pixel_buffer = new char[w*h*3];

  std::cout << "Shading pixel buffer\n";
  shade_buffer(pixel_buffer);

  std::cout << "Writing to file\n";
  bmp_write(pixel_buffer, h, w, L"my_palette.bmp");

  return 0;
}
