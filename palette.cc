#include <fstream>
#include <iostream>
#include <windows.h>
#include "palette.h"

const int HEADER_SIZE = 54;

void win_write(char* msg, DWORD msg_len, HANDLE file_handle)
{

  BOOL error_flag = FALSE;

  DWORD bytes_written = 0;
  error_flag = WriteFile(
		  file_handle,
		  msg,
		  msg_len,
		  &bytes_written,
		  NULL);

  if (error_flag == FALSE)
  {
	  std::cout << "Unable to write to file.";
	  CloseHandle(file_handle);
	  exit(1);
  }
  else
  {
	  if (bytes_written != msg_len)
	  {
		  std::cout << "Write failed to complete: " << bytes_written << " bytes written.";
		  CloseHandle(file_handle);
		  exit(1);
	  }
  }
}

char* generate_header(int filesize, int w, int h)
{
  static char header[HEADER_SIZE];

  // 'BM' to identify file as BMP
  // could use memcpy
  header[0] = 'B';
  header[1] = 'M';

  // Little-endian filesize in bytes
  write_int_to_header(header, HEADER_SIZE, 2);

  // 4 bytes of padding
  write_int_to_header(header, 0, 6);

  // Data start address
  write_int_to_header(header, HEADER_SIZE, 10);

  // Size of BITMAPINFOHEADER (40 bytes)
  write_int_to_header(header, 40, 14);

  // Bitmap width in pixels
  write_int_to_header(header, w, 18);

  // Bitmap height in pixels
  write_int_to_header(header, h, 22);

  // Necessary stuff which is independent of data
  header[26] = 0x1;
  header[27] = 0x0;

  // Bits per pixel (2 bytes)
  header[28] = 0x18;
  header[29] = 0x0;

  // Compression method (0 = no compression)
  write_int_to_header(header, 0, 30);

  // Padding (assuming no compression)
  write_int_to_header(header, 0, 34);

  // Paddingggg
  write_int_to_header(header, 0, 38);
  write_int_to_header(header, 0, 42);
  write_int_to_header(header, 0, 46);
  write_int_to_header(header, 0, 50);

  return header;
}

// Write a 4-byte word to the header. Little-endian
void write_int_to_header(char* header, int number, int start)
{

  int mask = 0x000000ff;

  for (int i=0; i < 4; i++)
  {
    header[i+start] = (char)((number & mask) >> (8*i));
    mask = mask << 8;
  }

}

void buffer_to_data(char* pixel_buffer, int h, int w, char* data, int pad_len, int byte_w)
{

  for (int row=0; row<h; row++)
  {

    for (int col=0; col<w; col++)
    {
      data[row*byte_w + col*3 + 0] =  pixel_buffer[row*w*3 + col*3 + 2]; // B
      data[row*byte_w + col*3 + 1] =  pixel_buffer[row*w*3 + col*3 + 1]; // G
      data[row*byte_w + col*3 + 2] =  pixel_buffer[row*w*3 + col*3 + 0]; // R
    }

    // Pad the number of bytes in the row to a multiple of 4
    for (int pad=0; pad<pad_len; pad++)
    {
      data[row*byte_w + w*3 + pad] = char(0x01); // Padding character is arbitrary
    }

  }
}

// Writes a length 3*h*w byte array to a bitmap of dimensions (h,w)
void bmp_write(char* pixel_buffer, int h, int w, const wchar_t* filename)
{

  std::ofstream fout;

  // Pixel buffer needs padding for 4 byte alignment
  int pad_len = (4 - (3*w) % 4) % 4;
  int byte_w = 3*w + pad_len;

  std::cout << "Padding length: " << pad_len << "\n";

  const int filesize = HEADER_SIZE + h * byte_w;

  std::cout << "Generating header\n";
  char* header = generate_header(filesize, w, h);

  std::cout << "Allocating memory for bitmap data (" << h*byte_w << " bytes)\n";
  // This will be too big (I think if h*byte_w is larger than the largest integer)
  char *data = new char[h*byte_w];

  //std::cout << "Copying pixel buffer to bitmap data\n";
  buffer_to_data(pixel_buffer, h, w, data, pad_len, byte_w);

  // Open file
  std::cout << "Opening bitmap file" << filename << "\n";

  HANDLE file_handle;

  file_handle = CreateFile(filename, // Filename
		  GENERIC_WRITE, // Open mode
		  0, // Share mode. 0 to block access for other processes
		  NULL, // Security attribute
		  CREATE_ALWAYS, // Create disposition. CREATE_ALWAYS for create/overwrite
		  FILE_ATTRIBUTE_NORMAL, // Windows file system attributes (hidden, archive etc.)
		  NULL); // Can inherit file attributes from another file handle or NULL.

  // Deal with failure

  if (file_handle == INVALID_HANDLE_VALUE)
  {
    std::cout << "Unable to open file.";
    exit(1);
  }

  std::cout << "Writing bitmap header\n";
  win_write(header, HEADER_SIZE, file_handle);

  std::cout << "Writing bitmap data\n";
  win_write(data, filesize - HEADER_SIZE, file_handle);

  std::cout << "Write finished. Closing "<< filename << "\n";
  CloseHandle(file_handle);

  delete[] data;

}
