#include <lvgl.h>
#include <EPD_100ASK.h>
#include <Arduino.h>
#include <SPI.h>

#define EPD_WIDTH             240
#define EPD_HEIGHT            360

#define EPD_BUSY_PIN          7
#define EPD_RESET_PIN         6
#define EPD_DC_PIN            5
#define EPD_CS_PIN            10  /* CS */
#define EPD_SCLK_PIN          12  /* CLK */
#define EPD_SDI_PIN           11  /* MOSI */

#define SPI_FREQUENCY         25000000
#define SPI_MODE              SPI_MODE0

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ EPD_WIDTH * 10 ];

static uint16_t ref_count;
unsigned char epd_image[((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8 ) : (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT];
// initialize the edp_100ask library
EPD_100ASK my_epd_100ask(EPD_BUSY_PIN, EPD_RESET_PIN, EPD_DC_PIN, EPD_CS_PIN, EPD_SCLK_PIN, EPD_SDI_PIN, SPI_FREQUENCY, SPI_MODE);


#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
#if 1
  int32_t x;
  int32_t y;

  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      /*Put a pixel to the display. For example:*/
      /*put_px(x, y, *color_p)*/
      my_epd_100ask.paintSetPixel(x, y, *((uint8_t *)color_p));
      color_p++;
    }
  }

  if (((area->y2) >= 359) && (1 == digitalRead(EPD_BUSY_PIN))) {
    //my_epd_100ask.reset();
    my_epd_100ask.clearColor(EPD_COLOR_WHITE);
    my_epd_100ask.refresh(EPD_LUT_GC);
    my_epd_100ask.displayImage(epd_image, EPD_WIDTH, EPD_HEIGHT);
    my_epd_100ask.refresh(EPD_LUT_GC);
#if 0
    if(ref_count >= 10) {
      my_epd_100ask.refresh(EPD_LUT_GC);
      ref_count = 0;
    }
    else {
      ref_count++;
      my_epd_100ask.refresh(EPD_LUT_DU);
    }
    //delay(20);
#endif

  }

#else
  my_epd_100ask.displayPartial(area->x1, area->y1, area->x2, area->y2, (uint8_t*)color_p);
  my_epd_100ask.refresh(EPD_LUT_DU);
#endif


  lv_disp_flush_ready( disp );
}


void setup()
{
  // nothing to do inside the setup
  Serial.begin( 115200 ); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println( LVGL_Arduino );
  Serial.println( "I am LVGL_Arduino" );

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

  ref_count = 0;
  my_epd_100ask.begin(EPD_WIDTH, EPD_HEIGHT);
  my_epd_100ask.initPaint(epd_image, EPD_COLOR_WHITE);
  my_epd_100ask.paintClear(EPD_COLOR_WHITE);
  my_epd_100ask.displayImage(epd_image, EPD_WIDTH, EPD_HEIGHT);
  my_epd_100ask.refresh(EPD_LUT_GC);
  my_epd_100ask.reset();

  lv_disp_draw_buf_init( &draw_buf, buf, NULL, EPD_WIDTH * 10 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = EPD_WIDTH;
  disp_drv.ver_res = EPD_HEIGHT;
  disp_drv.full_refresh = 1;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  /*Finally register the driver*/
  lv_disp_drv_register( &disp_drv );

#if LV_USE_THEME_MONO
    lv_disp_t * disp = lv_disp_get_default();
    if(lv_theme_mono_is_inited() == false) {
        //disp->theme = lv_theme_mono_init(disp, 0, LV_FONT_DEFAULT);
        disp->theme = lv_theme_mono_init(disp, 0, LV_FONT_DEFAULT);
    }
#endif

  //lv_100ask_2048_simple_test();

#if 1
  /* Create simple label */
  lv_obj_t *label = lv_label_create( lv_scr_act() );
  lv_label_set_text( label, LVGL_Arduino.c_str() );
  lv_obj_align( label, LV_ALIGN_TOP_MID, 0, 0 );
  
  lv_obj_t * btnm = lv_btnmatrix_create( lv_scr_act() );
  lv_obj_align( btnm, LV_ALIGN_BOTTOM_MID, 0, 0 );
#endif

#if 0
  /* Try an example from the lv_examples Arduino library
     make sure to include it as written above.
    lv_example_btn_1();
  */

  // uncomment one of these demos
  lv_demo_widgets();            // OK
  // lv_demo_benchmark();          // OK
  // lv_demo_keypad_encoder();     // works, but I haven't an encoder
  // lv_demo_music();              // NOK
  // lv_demo_printer();
  // lv_demo_stress();             // seems to be OK
#endif
  Serial.println( "Setup done" );
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay( 5 );
}
