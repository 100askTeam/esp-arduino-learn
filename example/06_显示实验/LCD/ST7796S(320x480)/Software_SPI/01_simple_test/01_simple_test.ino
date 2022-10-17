#define TFT_PWM_PIN 7
#define TFT_RESET_PIN 6
#define TFT_MOSI_PIN 11
#define TFT_SCK_PIN 12
#define TFT_RS_PIN 5
#define TFT_CS_PIN 10


void Lcd_Writ_Bus(unsigned char d) {
  uint8_t val = 0x80;
  while (val) {
    if (d & val) {
      digitalWrite(TFT_MOSI_PIN, HIGH);
    } else {
      digitalWrite(TFT_MOSI_PIN, LOW);
    }
    digitalWrite(TFT_SCK_PIN, LOW);
    digitalWrite(TFT_SCK_PIN, HIGH);
    val >>= 1;
  }
}

void Lcd_Write_Com(unsigned char VH) {
  *(portOutputRegister(digitalPinToPort(TFT_RS_PIN))) &= ~digitalPinToBitMask(TFT_RS_PIN);  //LCD_RS=0;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Data(unsigned char VH) {
  *(portOutputRegister(digitalPinToPort(TFT_RS_PIN))) |= digitalPinToBitMask(TFT_RS_PIN);  //LCD_RS=1;
  Lcd_Writ_Bus(VH);
}

void Lcd_Write_Com_Data(unsigned char com, unsigned char dat) {
  Lcd_Write_Com(com);
  Lcd_Write_Data(dat);
}

void Address_set(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  Lcd_Write_Com(0x2a);
  Lcd_Write_Data(x1 >> 8);
  Lcd_Write_Data(x1);
  Lcd_Write_Data(x2 >> 8);
  Lcd_Write_Data(x2);
  Lcd_Write_Com(0x2b);
  Lcd_Write_Data(y1 >> 8);
  Lcd_Write_Data(y1);
  Lcd_Write_Data(y2 >> 8);
  Lcd_Write_Data(y2);
  Lcd_Write_Com(0x2c);
}

void Lcd_Init(void) {
  digitalWrite(TFT_RESET_PIN, HIGH);
  delay(5);
  digitalWrite(TFT_RESET_PIN, LOW);
  delay(15);
  digitalWrite(TFT_RESET_PIN, HIGH);
  delay(15);

  digitalWrite(TFT_CS_PIN, LOW);  //TFT_CS_PIN
                                  //ST7796S
  Lcd_Write_Com(0xF0);
  Lcd_Write_Data(0xC3);
  Lcd_Write_Com(0xF0);
  Lcd_Write_Data(0x96);
  Lcd_Write_Com(0x36);
  Lcd_Write_Data(0x68);
  Lcd_Write_Com(0x3A);
  Lcd_Write_Data(0x05);
  Lcd_Write_Com(0xB0);
  Lcd_Write_Data(0x80);
  Lcd_Write_Com(0xB6);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x02);
  Lcd_Write_Com(0xB5);
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x03);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x04);
  Lcd_Write_Com(0xB1);
  Lcd_Write_Data(0x80);
  Lcd_Write_Data(0x10);
  Lcd_Write_Com(0xB4);
  Lcd_Write_Data(0x00);
  Lcd_Write_Com(0xB7);
  Lcd_Write_Data(0xC6);
  Lcd_Write_Com(0xC5);
  Lcd_Write_Data(0x24);
  Lcd_Write_Com(0xE4);
  Lcd_Write_Data(0x31);
  Lcd_Write_Com(0xE8);
  Lcd_Write_Data(0x40);
  Lcd_Write_Data(0x8A);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x29);
  Lcd_Write_Data(0x19);
  Lcd_Write_Data(0xA5);
  Lcd_Write_Data(0x33);
  Lcd_Write_Com(0xC2);
  Lcd_Write_Com(0xA7);

  Lcd_Write_Com(0xE0);
  Lcd_Write_Data(0xF0);
  Lcd_Write_Data(0x09);
  Lcd_Write_Data(0x13);
  Lcd_Write_Data(0x12);
  Lcd_Write_Data(0x12);
  Lcd_Write_Data(0x2B);
  Lcd_Write_Data(0x3C);
  Lcd_Write_Data(0x44);
  Lcd_Write_Data(0x4B);
  Lcd_Write_Data(0x1B);
  Lcd_Write_Data(0x18);
  Lcd_Write_Data(0x17);
  Lcd_Write_Data(0x1D);
  Lcd_Write_Data(0x21);

  Lcd_Write_Com(0XE1);
  Lcd_Write_Data(0xF0);
  Lcd_Write_Data(0x09);
  Lcd_Write_Data(0x13);
  Lcd_Write_Data(0x0C);
  Lcd_Write_Data(0x0D);
  Lcd_Write_Data(0x27);
  Lcd_Write_Data(0x3B);
  Lcd_Write_Data(0x44);
  Lcd_Write_Data(0x4D);
  Lcd_Write_Data(0x0B);
  Lcd_Write_Data(0x17);
  Lcd_Write_Data(0x17);
  Lcd_Write_Data(0x1D);
  Lcd_Write_Data(0x21);

  Lcd_Write_Com(0X36);
  Lcd_Write_Data(0x48);
  Lcd_Write_Com(0xF0);
  Lcd_Write_Data(0xC3);
  Lcd_Write_Com(0xF0);
  Lcd_Write_Data(0x69);
  Lcd_Write_Com(0X13);
  Lcd_Write_Com(0X11);
  Lcd_Write_Com(0X29);

  digitalWrite(TFT_CS_PIN, HIGH);
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  digitalWrite(TFT_CS_PIN, LOW);
  Lcd_Write_Com(0x02c);  //write_memory_start

  l = l + x;
  Address_set(x, y, l, y);
  j = l * 2;
  for (i = 1; i <= j; i++) {
    Lcd_Write_Data(c >> 8);
    Lcd_Write_Data(c);
  }

  digitalWrite(TFT_CS_PIN, HIGH);
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  digitalWrite(TFT_CS_PIN, LOW);
  Lcd_Write_Com(0x02c);  //write_memory_start

  l = l + y;
  Address_set(x, y, x, l);
  j = l * 2;
  for (i = 1; i <= j; i++) {
    Lcd_Write_Data(c >> 8);
    Lcd_Write_Data(c);
  }

  digitalWrite(TFT_CS_PIN, HIGH);
}

void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x, y, w, c);
  H_line(x, y + h, w, c);
  V_line(x, y, h, c);
  V_line(x + w, y, h, c);
}

void Rectf(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x, y, w, c);
    H_line(x, y + i, w, c);
  }
}

int RGB(int r, int g, int b) {
  return r << 16 | g << 8 | b;
}

void LCD_Clear(unsigned int j) {
  unsigned int i, m;
  digitalWrite(TFT_CS_PIN, LOW);

  Address_set(0, 0, 320, 480);
  for (i = 0; i < 320; i++)
    for (m = 0; m < 480; m++) {
      Lcd_Write_Data(j >> 8);
      Lcd_Write_Data(j);
    }

  digitalWrite(TFT_CS_PIN, HIGH);
}

void setup() {
  pinMode(TFT_PWM_PIN, OUTPUT);
  pinMode(TFT_RESET_PIN, OUTPUT);
  pinMode(TFT_MOSI_PIN, OUTPUT);
  pinMode(TFT_SCK_PIN, OUTPUT);
  pinMode(TFT_RS_PIN, OUTPUT);
  pinMode(TFT_CS_PIN, OUTPUT);

  digitalWrite(TFT_PWM_PIN, HIGH);
  digitalWrite(TFT_RESET_PIN, HIGH);
  digitalWrite(TFT_MOSI_PIN, HIGH);
  digitalWrite(TFT_SCK_PIN, HIGH);
  digitalWrite(TFT_RS_PIN, HIGH);
  digitalWrite(TFT_CS_PIN, HIGH);

  Lcd_Init();
}

void loop() {
  LCD_Clear(0xf800);
  LCD_Clear(0x07E0);
  LCD_Clear(0x001F);
  LCD_Clear(0x0);
  for (int i = 0; i < 500; i++) {
    Rect(random(300), random(300), random(300), random(300), random(65535));  // rectangle at x, y, with, hight, color
  }

  //  LCD_Clear(0xf800);
}