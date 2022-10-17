#include <Arduino.h>
#include <SPI.h>

#include <lvgl.h>
#include <EPD_100ask.h>

#define EPD_100ASK_TEST_WIDTH 240
#define EPD_100ASK_TEST_HEIGHT 360

#define EPD_100ASK_TEST_BUSY_PIN        7
#define EPD_100ASK_TEST_RESET_PIN       6
#define EPD_100ASK_TEST_DC_PIN          5
#define EPD_100ASK_TEST_CS_PIN          10
#define EPD_100ASK_TEST_SCLK_PIN        12
#define EPD_100ASK_TEST_SDI_PIN         11

#define EPD_100ASK_TEST_SPI_FREQUENCY 25000000
#define EPD_100ASK_TEST_SPI_MODE SPI_MODE0

uint8_t * epdPaintImageBuffer = NULL;
// initialize the edp_100ask library
EPD_100ASK myEPD(EPD_100ASK_TEST_BUSY_PIN, EPD_100ASK_TEST_RESET_PIN, EPD_100ASK_TEST_DC_PIN, EPD_100ASK_TEST_CS_PIN, EPD_100ASK_TEST_SCLK_PIN, EPD_100ASK_TEST_SDI_PIN, EPD_100ASK_TEST_SPI_FREQUENCY, EPD_100ASK_TEST_SPI_MODE);


#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  int32_t x;
  int32_t y;

  for (y = area->y1; y <= area->y2; y++) {
    for (x = area->x1; x <= area->x2; x++) {
      /*Put a pixel to the display. For example:*/
      /*put_px(x, y, *color_p)*/
      myEPD.paintSetPixel(x, y, *((uint8_t *)color_p));
      color_p++;
    }
  }

  myEPD.displayImage(epdPaintImageBuffer, EPD_100ASK_TEST_WIDTH, EPD_100ASK_TEST_HEIGHT);
  myEPD.refresh(EPD_100ASK_LUT_GC);

  lv_disp_flush_ready(disp);
}


void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */

  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t *buf = (lv_color_t *)heap_caps_malloc(((EPD_100ASK_TEST_WIDTH * EPD_100ASK_TEST_HEIGHT) * sizeof(lv_color_t)), MALLOC_CAP_DMA);
  epdPaintImageBuffer = (uint8_t *)heap_caps_malloc(((((EPD_100ASK_TEST_WIDTH % 8 == 0) ? (EPD_100ASK_TEST_WIDTH / 8) : (EPD_100ASK_TEST_WIDTH / 8 + 1)) * EPD_100ASK_TEST_HEIGHT) * sizeof(uint8_t)), MALLOC_CAP_DMA);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  myEPD.begin(EPD_100ASK_TEST_WIDTH, EPD_100ASK_TEST_HEIGHT);
  myEPD.initPaint(epdPaintImageBuffer, EPD_COLOR_WHITE);
  myEPD.paintClear(EPD_COLOR_WHITE);
  myEPD.displayImage(epdPaintImageBuffer, EPD_100ASK_TEST_WIDTH, EPD_100ASK_TEST_HEIGHT);
  myEPD.refresh(EPD_100ASK_LUT_GC);
  myEPD.reset();

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, EPD_100ASK_TEST_WIDTH * EPD_100ASK_TEST_HEIGHT);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = EPD_100ASK_TEST_WIDTH;
  disp_drv.ver_res = EPD_100ASK_TEST_HEIGHT;
  disp_drv.full_refresh = 1;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  /*Finally register the driver*/
  lv_disp_drv_register(&disp_drv);

#if LV_USE_THEME_MONO
  lv_disp_t *disp = lv_disp_get_default();
  if (lv_theme_mono_is_inited() == false) {
    //disp->theme = lv_theme_mono_init(disp, 1, LV_FONT_DEFAULT);
    disp->theme = lv_theme_mono_init(disp, 0, LV_FONT_DEFAULT);
  }
#endif

  /* Create simple label */
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, LVGL_Arduino.c_str());
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  Serial.println("Setup done");
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}