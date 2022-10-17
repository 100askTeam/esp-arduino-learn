#include "Arduino.h"
#include "EPD_100ask.h"
#include "EPD_LUTS_100ask.h"

/*
 * two-wire constructor.
 * Sets which wires should control the motor.
 */
EPD_100ASK::EPD_100ASK(int8_t busy, int8_t reset, int8_t dc, int8_t cs, int8_t sclk, int8_t sdi, uint32_t freq, uint8_t mode)
{
    this->epd_busy_pin  = busy;
    this->epd_reset_pin = reset;
    this->epd_dc_pin    = dc;
    this->epd_cs_pin    = cs;
    this->epd_sclk_pin  = sclk;
    this->epd_sdi_pin   = sdi;
    this->epd_spi_freq  = freq;
    this->epd_spi_mode  = mode;
    this->lut_flag      = 0;
}

void EPD_100ASK::begin(uint16_t w, uint16_t h)
{
    this->epd_width  = w;
    this->epd_height = h;

    SPI_Init();

    pinMode(this->epd_busy_pin, INPUT);
    pinMode(this->epd_reset_pin, OUTPUT);
    pinMode(this->epd_dc_pin, OUTPUT);

    reset();
}


void EPD_100ASK::SPI_Init(void)
{
  this->spi = new SPIClass(HSPI);
  this->spi->begin(this->epd_sclk_pin, -1, this->epd_sdi_pin, this->epd_cs_pin); //SCLK-14, MISO-12, MOSI-13, SS-15

  //set up slave select pins as outputs as the Arduino API
  //doesn't handle automatically pulling SS low
  pinMode(this->spi->pinSS(), OUTPUT); //HSPI SS
}


void EPD_100ASK::reset(void)
{
  digitalWrite(this->epd_reset_pin, HIGH);
  delay(20);
  digitalWrite(this->epd_reset_pin, LOW);
  delay(20);
  digitalWrite(this->epd_reset_pin, HIGH);
  delay(20);

  EPD_Init();
}

void EPD_100ASK::EPD_Init(void)
{
    this->lut_flag = 0;

    beginEDPWrite();

    writecommand(0x00);     // panel setting   PSR
    writedata(0xFF);      // RES1 RES0 REG KW/R     UD    SHL   SHD_N  RST_N
    writedata(0x01);      // x x x VCMZ TS_AUTO TIGE NORG VC_LUTZ

    writecommand(0x01);     // POWER SETTING   PWR
    writedata(0x03);      //  x x x x x x VDS_EN VDG_EN
    writedata(0x10);      //  x x x VCOM_SLWE VGH[3:0]   VGH=20V, VGL=-20V
    writedata(0x3F);      //  x x VSH[5:0]  VSH = 15V
    writedata(0x3F);      //  x x VSL[5:0]  VSL=-15V
    writedata(0x03);      //  OPTEN VDHR[6:0]  VHDR=6.4V
   
    // T_VDS_OFF[1:0] 00=1 frame; 01=2 frame; 10=3 frame; 11=4 frame
    writecommand(0x06);     // booster soft start   BTST
    //writedata(0x37);      //  BT_PHA[7:0]
    //writedata(0x3D);      //  BT_PHB[7:0]
    writedata(0x17);      //  BT_PHA[7:0]
    writedata(0x37);      //  BT_PHB[7:0]
    writedata(0x3D);      //  x x BT_PHC[5:0]

    //writecommand(0x60);     // TCON setting     TCON
    //writedata(0x22);      // S2G[3:0] G2S[3:0]   non-overlap = 12

    writecommand(0x82);     // VCOM_DC setting    VDCS
    writedata(0x07);      // x  VDCS[6:0] VCOM_DC value= -1.9v    00~3f,0x12=-1.9v

    writecommand(0x30);
    writedata(0x09);

    //writecommand(0xe3);     // power saving     PWS
    //writedata(0x88);      // VCOM_W[3:0] SD_W[3:0]

    writecommand(0x61);     // resoultion setting
    writedata(0xf0);      //  HRES[7:3] 0 0 0
    writedata(0x01);      //  x x x x x x x VRES[8]
    writedata(0x68);      //  VRES[7:0]

    writecommand(0X50);      //
    //writedata(0xB7);     // Border
    writedata(0xD7);

    endEDPWrite();

    delay(10);
}


void EPD_100ASK::initPaint(unsigned char * image, unsigned char color)
{
    uint16_t w = this->epd_width;
    uint16_t h = this->epd_height;

    this->paint.image = image;
    this->paint.color = color;
    this->paint.rotate = 0;

    this->paint.widthMemory = w;
    this->paint.heightMemory = h;
    
    this->paint.widthByte = (w % 8 == 0)? (w / 8 ): (w / 8 + 1);
    this->paint.heightByte = h;

    this->paint.mirror = MIRROR_NONE;

    if(this->paint.rotate == ROTATE_0 || this->paint.rotate == ROTATE_180) {
        this->paint.width = w;
        this->paint.height = h;
    } else {
        this->paint.width = h;
        this->paint.height = w;
    }
}


/***************************************************************************************
** Function name:           writecommand
** Description:             Send an 8 bit command to the EDP
***************************************************************************************/
void EPD_100ASK::writecommand(uint8_t c)
{
    digitalWrite(this->epd_dc_pin, LOW);
    this->spi->transfer(c);
}

/***************************************************************************************
** Function name:           writedata
** Description:             Send a 8 bit data value to the EDP
***************************************************************************************/
void EPD_100ASK::writedata(uint8_t d)
{
    digitalWrite(this->epd_dc_pin, HIGH);
    this->spi->transfer(d);
}



/***************************************************************************************
** Function name:           startWrite
** Description:             begin transaction with CS low, MUST later call endWrite
***************************************************************************************/
void EPD_100ASK::startWrite(void)
{
  beginEDPWrite();
}

/***************************************************************************************
** Function name:           endWrite
** Description:             end transaction with CS high
***************************************************************************************/
void EPD_100ASK::endWrite(void)
{
    endEDPWrite();         // Release SPI bus
}


/***************************************************************************************
** Function name:           beginEDPWrite (was called spi_begin)
** Description:             Start SPI transaction for writes and select EDP
***************************************************************************************/
inline void EPD_100ASK::beginEDPWrite(void){
    this->spi->beginTransaction(SPISettings(this->epd_spi_freq, MSBFIRST, this->epd_spi_mode));
    //digitalWrite(this->epd_cs_pin, LOW)
    digitalWrite(this->spi->pinSS(), LOW); //HSPI SS
}

/***************************************************************************************
** Function name:           endEDPWrite (was called spi_end)
** Description:             End transaction for write and deselect EDP
***************************************************************************************/
inline void EPD_100ASK::endEDPWrite(void){
    digitalWrite(this->spi->pinSS(), HIGH); //pull ss high to signify end of data transfer
    //digitalWrite(this->epd_cs_pin, HIGH)
    this->spi->endTransaction();
}


/******************************************************************************
function:	Clear the color of the picture
parameter:
    Color   :   Painted colors
******************************************************************************/
void EPD_100ASK::paintClear(uint8_t color)
{
	uint16_t x, y;
	uint32_t addr;
    for (y = 0; y < this->paint.heightByte; y++) {
        for (x = 0; x < this->paint.widthByte; x++ ) {//8 pixel =  1 byte
            addr = x + y * (this->paint.widthByte);
            this->paint.image[addr] = color;
        }
    }
}



/******************************************************************************
function:	Draw Pixels
parameter:
    Xpoint  :   At point X
    Ypoint  :   At point Y
    Color   :   Painted colors
******************************************************************************/
void EPD_100ASK::paintSetPixel(uint16_t Xpoint, uint16_t Ypoint, uint8_t Color)
{
	uint16_t X, Y;
	uint32_t Addr;
	uint8_t Rdata;

    if(Xpoint > this->paint.width || Ypoint > this->paint.height){
        //Debug("Exceeding display boundaries\r\n");
        return;
    }      
    

    switch(this->paint.rotate) {
    case 0:
        X = Xpoint;
        Y = Ypoint;  
        break;
    case 90:
        X = this->paint.widthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = this->paint.widthMemory - Xpoint - 1;
        Y = this->paint.heightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = this->paint.heightMemory - Xpoint - 1;
        break;
		
    default:
        return;
    }
    
    switch(this->paint.mirror) {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = this->paint.widthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = this->paint.heightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = this->paint.widthMemory - X - 1;
        Y = this->paint.heightMemory - Y - 1;
        break;
    default:
        return;
    }

    if(X > this->paint.widthMemory || Y > this->paint.heightMemory){
        //Debug("Exceeding display boundaries\r\n");
        return;
    }
    
    Addr = X / 8 + Y * this->paint.widthByte;
    Rdata = this->paint.image[Addr];
    if(Color == EPD_COLOR_BLACK)
        this->paint.image[Addr] = Rdata & ~(0x80 >> (X % 8));
    else
        this->paint.image[Addr] = Rdata | (0x80 >> (X % 8));
}

/******************************************************************************
function:	Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		:   The Xpoint coordinate of the point
    Ypoint		:   The Ypoint coordinate of the point
    Color		:   Set color
    Dot_Pixel	:	point size
******************************************************************************/
void EPD_100ASK::paintDrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint8_t Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE)
{
	int16_t XDir_Num , YDir_Num;
    if (Xpoint > this->paint.width || Ypoint > this->paint.height) {
        //Debug("Paint_DrawPoint Input exceeds the normal display range\r\n");
        return;
    }

    if (DOT_STYLE == DOT_FILL_AROUND) {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                if(Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                paintSetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
                paintSetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
            }
        }
    }
}


/******************************************************************************
function:	Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
******************************************************************************/
void EPD_100ASK::paintDrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
                    uint8_t Color, DOT_PIXEL Dot_Pixel, LINE_STYLE Line_Style)
{    
    uint16_t Xpoint, Ypoint;
    int dx, dy;
    int XAddway,YAddway;
    int Esp;
    char Dotted_Len;

    if (Xstart > this->paint.width || Ystart > this->paint.height ||
        Xend > this->paint.width || Yend > this->paint.height) {
        //Debug("Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    Xpoint = Xstart;
    Ypoint = Ystart;
    dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    XAddway = Xstart < Xend ? 1 : -1;
    YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    Esp = dx + dy;
    Dotted_Len = 0;

    for (;;) {
        Dotted_Len++;
        //Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
            //Debug("LINE_DOTTED\r\n");
            paintDrawPoint(Xpoint, Ypoint, EPD_COLOR_WHITE, Dot_Pixel, DOT_FILL_AROUND);
            Dotted_Len = 0;
        } else {
            paintDrawPoint(Xpoint, Ypoint, Color, Dot_Pixel, DOT_FILL_AROUND);
        }
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}



/******************************************************************************
function:	Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Filled : Whether it is filled--- 1 solid 0：empty
******************************************************************************/
void EPD_100ASK::paintDrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
                         uint8_t Color, DOT_PIXEL Dot_Pixel, DRAW_FILL Filled)
{
	uint16_t Ypoint;
    if (Xstart > this->paint.width || Ystart > this->paint.height ||
        Xend > this->paint.width || Yend > this->paint.height) {
        //Debug("Input exceeds the normal display range\r\n");
        return;
    }

    if (Filled ) {
        for(Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            paintDrawLine(Xstart, Ypoint, Xend, Ypoint, Color , Dot_Pixel, LINE_STYLE_SOLID);
        }
    } else {
        paintDrawLine(Xstart, Ystart, Xend, Ystart, Color, Dot_Pixel, LINE_STYLE_SOLID);
        paintDrawLine(Xstart, Ystart, Xstart, Yend, Color, Dot_Pixel, LINE_STYLE_SOLID);
        paintDrawLine(Xend, Yend, Xend, Ystart, Color, Dot_Pixel, LINE_STYLE_SOLID);
        paintDrawLine(Xend, Yend, Xstart, Yend, Color, Dot_Pixel, LINE_STYLE_SOLID);
    }
}


/******************************************************************************
function:	Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Filled    : Whether it is filled: 1 filling 0：Do not
******************************************************************************/
void EPD_100ASK::paintDrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius,
                      uint8_t Color, DOT_PIXEL Dot_Pixel, DRAW_FILL  Draw_Fill)
{
	int16_t Esp, sCountY;
	int16_t XCurrent, YCurrent;
    if (X_Center > this->paint.width || Y_Center >= this->paint.height) {
        //Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    //Draw a circle from(0, R) as a starting point
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
   Esp = 3 - (Radius << 1 );
    if (Draw_Fill == DRAW_FILL_FULL) {
        while (XCurrent <= YCurrent ) { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                paintDrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//1
                paintDrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//2
                paintDrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//3
                paintDrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//4
                paintDrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//5
                paintDrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//6
                paintDrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//7
                paintDrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while (XCurrent <= YCurrent ) {
            paintDrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//1
            paintDrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//2
            paintDrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//3
            paintDrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//4
            paintDrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//5
            paintDrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//6
            paintDrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//7
            paintDrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT);//0

            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}

void EPD_100ASK::clearColor(uint8_t color)
{
    unsigned int i;
    unsigned int row, column;

    beginEDPWrite();

    writecommand(0x10);
    for (i = 0; i < ((this->epd_height) * (this->epd_width) / 8); i++)
    {
        writedata(color);
    }

    writecommand(0x13);
    for (i = 0; i < ((this->epd_height) * (this->epd_width) / 8); i++)
    {
        writedata(color);
    }


    endEDPWrite();
}


void EPD_100ASK::displayImage(const uint8_t * picData, uint16_t w, uint16_t h)
{
    unsigned int i;
    uint16_t count;

    if(w > this->epd_width)     w = this->epd_width;
    if(h > this->epd_height)    h = this->epd_height;

    count = (w * h / 8);

    beginEDPWrite();

    writecommand(0x13);
    for (i = 0; i < (((this->epd_height) * (this->epd_width)) / 8); i++)
    {
        writedata(*picData);
        picData++;
        
    }

    endEDPWrite();
}


void EPD_100ASK::displayPartial(unsigned int x0, unsigned int y0, unsigned int w, unsigned int h, const uint8_t * data) //partial display 
{
    unsigned int i,count;  
    unsigned int x_start1, x_end1,y_start1,y_start2,y_end1,y_end2;

    x_start1 = x0;//转换为字节
    x_end1 = x0 + h-1; 

    y_start1=0;
    y_start2=y0;
    if(y0>=256)
    {
        y_start1=y_start2/256;
        y_start2=y_start2%256;
    }

    y_end1=0;
    y_end2=y0+w-1;
    if(y_end2>=256)
    {
        y_end1=y_end2/256;
        y_end2=y_end2%256;		
    }		

    count=w*h/8;

    /*********************************************************/	
    //需要重新复位和初始化设置!!!!
    /*********************************************************/	
    reset();

    beginEDPWrite();

    writecommand(0X50);      // 
    writedata(0xF7);     // Border
    /*********************************************************/		

    writecommand(0x00);			// panel setting   PSR
    writedata(0xFF);			// RES1 RES0 REG KW/R     UD    SHL   SHD_N  RST_N	
    writedata(0x01);			// x x x VCMZ TS_AUTO TIGE NORG VC_LUTZ	

    writecommand(0x91);		//This command makes the display enter partial mode
    writecommand(0x90);		//resolution setting
    writedata (x_start1);   //x-start     
    writedata (x_end1);	 //x-end	

    writedata (y_start1);
    writedata (y_start2);   //y-start    

    writedata (y_end1);		
    writedata (y_end2);  //y-end
    writedata (0x01);	


    writecommand(0x13);				 //writes New data to SRAM.
    for(i=0;i<count;i++)	     
    {
        writedata(data[i]);         //图片  
    }

    endEDPWrite();
}


void EPD_100ASK::refresh(EPD_100ASK_LUT_TYPE lut)
{
    switch (lut)
    {
    case EPD_100ASK_LUT_GC:
        lutGC();

        beginEDPWrite();
        writecommand(0x17);			//DISPLAY REFRESH
        writedata(0xA5);
        endEDPWrite();

        chkstatus();
        delay(10);
        break;
    case EPD_100ASK_LUT_DU:
        lutDU();
        beginEDPWrite();
        writecommand(0x17);			//DISPLAY REFRESH
        writedata(0xA5);
        endEDPWrite();
        chkstatus();
        delay(100);

        reset();	
        beginEDPWrite();
        writecommand(0X50);      // 
        writedata(0xD7);     // Border
        endEDPWrite();
        break;
    case EPD_100ASK_LUT_5S:
        lut5S();
        break;
    default:
        return;
        break;
    }
}


void EPD_100ASK::chkstatus(void)
{
  while (0 == digitalRead(this->epd_busy_pin))
    delay(200);
}

/*****************************LUT download*************************/
void EPD_100ASK::lut5S(void)
{
  unsigned int count;

  beginEDPWrite();
  
  writecommand(0x20);							//vcom
  for (count = 0; count < 56; count++)
  {
    writedata(lut_vcom[count]);
  }

  writecommand(0x21);							//red not use
  for (count = 0; count < 42; count++)
  {
    writedata(lut_ww[count]);
  }

  writecommand(0x24);							//wb w
  for (count = 0; count < 56; count++)
  {
    writedata(lut_bb[count]);
  }

  if (this->lut_flag == 0)
  {

    writecommand(56);							//bb b
    for (count = 0; count < 80; count++)
    {
      writedata(lut_bw[count]);
    }


    writecommand(0x23);							//bw r
    for (count = 0; count < 56; count++)
    {
      writedata(lut_wb[count]);
    }

    this->lut_flag = 1;
  }
  else
  {

    writecommand(0x23);							//bb b
    for (count = 0; count < 56; count++)
    {
      writedata(lut_bw[count]);
    }


    writecommand(0x22);							//bw r
    for (count = 0; count < 56; count++)
    {
      writedata(lut_wb[count]);
    }

    this->lut_flag = 0;
  }

  endEDPWrite();
}


//LUT download
void EPD_100ASK::lutGC(void)
{
	unsigned int count = 0;
    unsigned int cnt = 0;	 

    beginEDPWrite();

    writecommand(0x20);							//vcom
    cnt = sizeof(lut_R20_GC); 
    for(count=0; count < cnt; count++)
        {writedata(lut_R20_GC[count]);}

    writecommand(0x21);							//red not use
    cnt = sizeof(lut_R21_GC); 
    for(count=0;count<cnt;count++)
        {writedata(lut_R21_GC[count]);}

    writecommand(0x24);							//bb b
    cnt = sizeof(lut_R24_GC); 
    for(count=0;count<cnt;count++)
        {writedata(lut_R24_GC[count]);}			
        
    if (this->lut_flag==0)	
    {

    writecommand(0x22);							//bw r
    cnt = sizeof(lut_R22_GC); 
    for(count=0;count<cnt;count++)
        {writedata(lut_R22_GC[count]);}

    writecommand(0x23);							//wb w
    cnt = sizeof(lut_R23_GC); 
    for(count=0;count<cnt;count++)
        {writedata(lut_R23_GC[count]);}
    this->lut_flag=1;
    }
    else
    {
    cnt = sizeof(lut_R23_GC); 
    writecommand(0x22);							//bw r
    for(count=0;count<cnt;count++)
        {writedata(lut_R23_GC[count]);}

    cnt = sizeof(lut_R22_GC); 
    writecommand(0x23);							//wb w
    for(count=0;count<cnt;count++)
        {writedata(lut_R22_GC[count]);}
    this->lut_flag=0;
    }

    endEDPWrite();

}


//LUT download
void EPD_100ASK::lutDU(void)
{
    unsigned int count;
    unsigned int cnt = 0;

    beginEDPWrite();

    cnt = sizeof(lut_R20_DU); 
    writecommand(0x20);							//vcom
    for(count=0;count<cnt;count++)
        {writedata(lut_R20_DU[count]);}

    cnt = sizeof(lut_R21_DU); 
    writecommand(0x21);							//red not use
    for(count=0;count<cnt;count++)
        {writedata(lut_R21_DU[count]);}

    cnt = sizeof(lut_R24_DU); 
    writecommand(0x24);							//bb b
    for(count=0;count<cnt;count++)
        {writedata(lut_R24_DU[count]);}			
        
    if (this->lut_flag==0)	
    {
    cnt = sizeof(lut_R22_DU); 
    writecommand(0x22);							//bw r
    for(count=0;count<cnt;count++)
        {writedata(lut_R22_DU[count]);}

    cnt = sizeof(lut_R23_DU); 
    writecommand(0x23);							//wb w
    for(count=0;count<cnt;count++)
        {writedata(lut_R23_DU[count]);}
    this->lut_flag=1;
    }
    else
    {
    cnt = sizeof(lut_R23_DU); 
    writecommand(0x22);							//bw r
    for(count=0;count<cnt;count++)
        {writedata(lut_R23_DU[count]);}

    cnt = sizeof(lut_R22_DU); 
    writecommand(0x23);							//wb w
    for(count=0;count<cnt;count++)
        {writedata(lut_R22_DU[count]);}
    this->lut_flag=0;
    }
    
    endEDPWrite();
}
