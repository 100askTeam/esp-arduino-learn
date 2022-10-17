
// ensure this library description is only included once
#ifndef EPD_100ASK_h
#define EPD_100ASK_h


/***************************************************************************************
**                         Load required header files
***************************************************************************************/
#include <Arduino.h>
#include <SPI.h>


#define EPD_COLOR_WHITE     0xFF
#define EPD_COLOR_BLACK     0x00

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270


#define tft_Write_8(C)   spi.transfer(C)


/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} EPD_MIRROR_IMAGE;
#define EPD_MIRROR_IMAGE EPD_MIRROR_NONE

/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1,		// 1 x 1
    DOT_PIXEL_2X2  , 		// 2 X 2
    DOT_PIXEL_3X3  ,		// 3 X 3
    DOT_PIXEL_4X4  ,		// 4 X 4
    DOT_PIXEL_5X5  , 		// 5 X 5
    DOT_PIXEL_6X6  , 		// 6 X 6
    DOT_PIXEL_7X7  , 		// 7 X 7
    DOT_PIXEL_8X8  , 		// 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,		// dot pixel 1 x 1
    DOT_FILL_RIGHTUP  , 		// dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Refresh type
**/
typedef enum {
    EPD_100ASK_LUT_GC,         /* 全刷 */
    EPD_100ASK_LUT_DU,         /* 局刷 */
    EPD_100ASK_LUT_5S,         /* ... */
    _EPD_100ASK_LUT_LAST,      /** Number of default events*/
} EPD_100ASK_LUT_TYPE;



typedef struct {
    unsigned char * image;
    unsigned char color;
    unsigned int width;
    unsigned int height;
    unsigned int widthMemory;
    unsigned int heightMemory;
    unsigned int rotate;
    unsigned int mirror;
    unsigned int widthByte;
    unsigned int heightByte;
} EPD_PAINT;


// library interface description
class EPD_100ASK {
  public:
    // constructors:
    EPD_100ASK(int8_t busy, int8_t reset, int8_t dc, int8_t cs, int8_t sclk, int8_t sdi, uint32_t freq, uint8_t mode);

    // initialization
    void begin(uint16_t _W, uint16_t _H);
    void reset(void);

    void startWrite(void);
    void endWrite(void);

    void writecommand(uint8_t c);
    void writedata(uint8_t d);

    void initPaint(unsigned char * image, unsigned char color);
    void clearColor(uint8_t color);
    void refresh(EPD_100ASK_LUT_TYPE lut);
    void paintSetPixel(uint16_t Xpoint, uint16_t Ypoint, uint8_t Color);
    void paintClear(uint8_t color);
    void paintDrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint8_t Color, DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE);
    void paintDrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint8_t Color, DOT_PIXEL Dot_Pixel, LINE_STYLE Line_Style);
    void paintDrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint8_t Color, DOT_PIXEL Dot_Pixel, DRAW_FILL Filled);
    void paintDrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius, uint8_t Color, DOT_PIXEL Dot_Pixel, DRAW_FILL  Draw_Fill);

    void displayImage(const uint8_t * picData, uint16_t w, uint16_t h);
    void displayPartial(unsigned int x0, unsigned int y0, unsigned int w, unsigned int h, const uint8_t * data); //partial display 

  private:
    void SPI_Init(void);
    void EPD_Init(void);

    void chkstatus(void);

    void lut5S(void);
    void lutGC(void);
    void lutDU(void);

    // New begin and end prototypes
    // begin/end a TFT write transaction
    // For SPI bus the transmit clock rate is set
    inline void beginEDPWrite() __attribute__((always_inline));
    inline void endEDPWrite()   __attribute__((always_inline));

    SPIClass * spi;

    // SPI frequency
    uint32_t epd_spi_freq;

    // SPI MODE
    uint8_t epd_spi_mode;

    // motor pin numbers:
    int epd_busy_pin;
    int epd_reset_pin;
    int epd_dc_pin;
    int epd_cs_pin;
    int epd_sclk_pin;
    int epd_sdi_pin;

    uint16_t epd_width;
    uint16_t epd_height;

    int lut_flag;

    EPD_PAINT paint;

};

#endif

