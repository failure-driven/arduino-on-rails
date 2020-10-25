/*
   8x8 LED matrix digits
*/

const static byte matrix0[8] = {
  B01111000,
  B11001100,
  B11001100,
  B11001100,
  B11001100,
  B11001100,
  B01111000,
  B00000000
};
const static byte matrix1[8] = {
  B00110000,
  B01110000,
  B00110000,
  B00110000,
  B00110000,
  B00110000,
  B11111100,
  B00000000
};
const static byte matrix2[8] = {
  B01111000,
  B11001100,
  B00001100,
  B00111000,
  B01100000,
  B11001100,
  B11111100,
  B00000000
};
const static byte matrix3[8] = {
  B01111000,
  B11001100,
  B00001100,
  B00111000,
  B00001100,
  B11001100,
  B01111000,
  B00000000
};
const static byte matrix4[8] = {
  B00011100,
  B00111100,
  B01101100,
  B11001100,
  B11111110,
  B00001100,
  B00011110,
  B00000000
};
const static byte matrix5[8] = {
  B11111100,
  B11000000,
  B11111000,
  B00001100,
  B00001100,
  B11001100,
  B01111000,
  B00000000
};
const static byte matrix6[8] = {
  B00111000,
  B01100000,
  B11000000,
  B11111000,
  B11001100,
  B11001100,
  B01111000,
  B00000000
};
const static byte matrix7[8] = {
  B11111100,
  B11001100,
  B00001100,
  B00011000,
  B00110000,
  B00110000,
  B00110000,
  B00000000
};
const static byte matrix8[8] = {
  B01111000,
  B11001100,
  B11001100,
  B01111000,
  B11001100,
  B11001100,
  B01111000,
  B00000000
};
const static byte matrix9[8] = {
  B01111000,
  B11001100,
  B11001100,
  B01111100,
  B00001100,
  B00011000,
  B01110000,
  B00000000
};

const static byte* matrixNumeral[10] = {
  matrix0,
  matrix1,
  matrix2,
  matrix3,
  matrix4,
  matrix5,
  matrix6,
  matrix7,
  matrix8,
  matrix9
};
