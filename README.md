Overview
This project implements Image Steganography using the Least Significant Bit (LSB) technique in the C programming language.
The program allows you to:
  Encode (hide) a secret file inside a 24-bit BMP image
  Decode (extract) the hidden file back from the stego image

How It Works
The LSB steganography method modifies the least significant bit of each image byte to store secret data.

Encoding steps:
  Copy BMP header
  Embed magic string (#*)
  Embed secret file extension size
  Embed secret file extension
  Embed secret file size
  Embed secret file data bit-by-bit
  Copy remaining image bytes
Decoding performs the exact reverse process.

Requirements:
  GCC Compiler
  24-bit BMP image
  Any small secret text file

Encoding:
  ./a.out -e sample/beautiful.bmp sample/secret.txt sample/stego.bmp
Expected Output:
  Info: Files opened successfully
  Secret data can fit in given image file
  Copied BMP header successfully
  Encoded magic string successfully
  Encoded secret file extn size successfully
  Encoded secret file extn successfully
  Encoded secret file size successfully
  Encoded secret file data successfully
  Copied remaining data successfully

Decoding:
  ./a.out -d sample/stego.bmp
Expected Output:
  Decoding started...
  Info: Magic string matched successfully
  Info: Secret file extension size decoded: 4
  Info: Secret file extension decoded: .txt
  Info: Secret file size decoded: XX bytes
  Info: Secret file successfully decoded and saved as decoded.txt

To view message:
  cat decoded.txt


  

  

