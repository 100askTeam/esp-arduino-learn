#include <Arduino.h>
#include <lvgl.h>
#include <lv_lib_100ask.h>
#include <TFT_eSPI.h>
#include <FS.h>
#include "SD.h"
#include "SPI.h"
#include <FcJoypad_100ask.h>

#define SD_CARD_CS_PIN 15
#define SD_CARD_SCK_PIN 16
#define SD_CARD_MOSI_PIN 17
#define SD_CARD_MISO_PIN 18

#define FC_JOYPAD_CLOCK_PIN 40 /*ID*/
#define FC_JOYPAD_LATCH_PIN 41 /*D+*/
#define FC_JOYPAD_DATA_PIN 42  /*D-*/

#define LVGL_TICK_PERIOD 60
#define LVGL_100ASK_USE_SD_CARD 1

static SPIClass sdCardSpi;

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 480;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

lv_indev_t *lv_joypad_device_object;

// initialize the FcJoypad_100ask library
FcJoypad_100ask myFcJoypad(FC_JOYPAD_CLOCK_PIN, FC_JOYPAD_LATCH_PIN, FC_JOYPAD_DATA_PIN);

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  Serial.printf(buf);
  Serial.flush();
}
#endif

#if LVGL_100ASK_USE_SD_CARD != 0
static void fs_init(void) {
  sdCardSpi.begin(SD_CARD_SCK_PIN, SD_CARD_MISO_PIN, SD_CARD_MOSI_PIN, SD_CARD_CS_PIN);  //SCLK, MISO, MOSI, SS

  if (!SD.begin(SD_CARD_CS_PIN, sdCardSpi)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

static void *sd_fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
  LV_UNUSED(drv);

  const char *flags = "";

  if (mode == LV_FS_MODE_WR)
    flags = FILE_WRITE;
  else if (mode == LV_FS_MODE_RD)
    flags = FILE_READ;
  else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD))
    flags = FILE_WRITE;

  File f = SD.open(path, flags);
  if (!f) {
    Serial.println("Failed to open file!");
    return NULL;
  }

  File *lf = new File{ f };

  //make sure at the beginning
  //fp->seek(0);

  return (void *)lf;
}

static lv_fs_res_t sd_fs_close(lv_fs_drv_t *drv, void *file_p) {
  LV_UNUSED(drv);

  File *fp = (File *)file_p;

  fp->close();

  delete (fp);  // when close
  return LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_read(lv_fs_drv_t *drv, void *file_p, void *fileBuf, uint32_t btr, uint32_t *br) {
  LV_UNUSED(drv);

  File *fp = (File *)file_p;

  *br = fp->read((uint8_t *)fileBuf, btr);

  return (int32_t)(*br) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw) {
  LV_UNUSED(drv);

  File *fp = (File *)file_p;

  *bw = fp->write((const uint8_t *)buf, btw);

  return (int32_t)(*bw) < 0 ? LV_FS_RES_UNKNOWN : LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence) {
  LV_UNUSED(drv);

  File *fp = (File *)file_p;

  SeekMode mode;
  if (whence == LV_FS_SEEK_SET)
    mode = SeekSet;
  else if (whence == LV_FS_SEEK_CUR)
    mode = SeekCur;
  else if (whence == LV_FS_SEEK_END)
    mode = SeekEnd;

  fp->seek(pos, mode);

  return LV_FS_RES_OK;
}

static lv_fs_res_t sd_fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
  LV_UNUSED(drv);

  File *fp = (File *)file_p;

  *pos_p = fp->position();

  return LV_FS_RES_OK;
}


static void *sd_dir_open(lv_fs_drv_t *drv, const char *dirpath) {
  LV_UNUSED(drv);

  File root = SD.open(dirpath);
  if (!root) {
    Serial.println("Failed to open directory!");
    return NULL;
  }

  if (!root.isDirectory()) {
    Serial.println("Not a directory!");
    return NULL;
  }

  File *lroot = new File{ root };

  return (void *)lroot;
}


static lv_fs_res_t sd_dir_read(lv_fs_drv_t *drv, void *dir_p, char *fn) {
  LV_UNUSED(drv);

  File *root = (File *)dir_p;
  fn[0] = '\0';

  File file = root->openNextFile();
  while (file) {
    if (strcmp(file.name(), ".") == 0 || strcmp(file.name(), "..") == 0) {
      continue;
    } else {
      if (file.isDirectory()) {
        Serial.print("  DIR : ");
        Serial.println(file.name());
        fn[0] = '/';
        strcpy(&fn[1], file.name());
      } else {
        Serial.print("  FILE: ");
        Serial.print(file.name());
        Serial.print("  SIZE: ");
        Serial.println(file.size());

        strcpy(fn, file.name());
      }
      break;
    }
    file = root->openNextFile();
  }

  return LV_FS_RES_OK;
}

static lv_fs_res_t sd_dir_close(lv_fs_drv_t *drv, void *dir_p) {
  LV_UNUSED(drv);

  File *root = (File *)dir_p;

  root->close();

  delete (root);  // when close

  return LV_FS_RES_OK;
}


static void lv_port_sd_fs_init(void) {
  /*----------------------------------------------------
    * Initialize your storage device and File System
    * -------------------------------------------------*/
  fs_init();

  /*---------------------------------------------------
    * Register the file system interface in LVGL
    *--------------------------------------------------*/

  /*Add a simple drive to open images*/
  static lv_fs_drv_t fs_drv;
  lv_fs_drv_init(&fs_drv);

  /*Set up fields...*/
  fs_drv.letter = 'S';
  fs_drv.cache_size = 0;

  fs_drv.open_cb = sd_fs_open;
  fs_drv.close_cb = sd_fs_close;
  fs_drv.read_cb = sd_fs_read;
  fs_drv.write_cb = sd_fs_write;
  fs_drv.seek_cb = sd_fs_seek;
  fs_drv.tell_cb = sd_fs_tell;

  fs_drv.dir_close_cb = sd_dir_close;
  fs_drv.dir_open_cb = sd_dir_open;
  fs_drv.dir_read_cb = sd_dir_read;

  lv_fs_drv_register(&fs_drv);
}

#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void get_joypad_value(lv_indev_data_t *data, fc_joypad_btn_t btn, bool btn_state) {

  data->state = (lv_indev_state_t)((btn_state) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);

  switch (btn) {
    case FC_JOYPAD_BTN_A:
      data->key = LV_KEY_ENTER;
      //Serial.println("Key FC_JOYPAD_BTN_A is pressed!");
      break;
    case FC_JOYPAD_BTN_B:
      data->key = LV_KEY_BACKSPACE;
      //Serial.println("Key FC_JOYPAD_BTN_B is pressed!");
      break;
    case FC_JOYPAD_BTN_START:
      data->key = LV_KEY_NEXT;
      //Serial.println("Key FC_JOYPAD_BTN_START is pressed!");
      break;
    case FC_JOYPAD_BTN_SELECT:
      data->key = LV_KEY_PREV;
      //Serial.println("Key FC_JOYPAD_BTN_SELECT is pressed!");
      break;
    case FC_JOYPAD_BTN_UP:
      data->key = LV_KEY_UP;
      //Serial.println("Key FC_JOYPAD_BTN_UP is pressed!");
      break;
    case FC_JOYPAD_BTN_DOWN:
      data->key = LV_KEY_DOWN;
      //Serial.println("Key FC_JOYPAD_BTN_DOWN is pressed!");
      break;
    case FC_JOYPAD_BTN_LEFT:
      data->key = LV_KEY_LEFT;
      //Serial.println("Key FC_JOYPAD_BTN_LEFT is pressed!");
      break;
    case FC_JOYPAD_BTN_RIGHT:
      data->key = LV_KEY_RIGHT;
      //Serial.println("Key FC_JOYPAD_BTN_RIGHT is pressed!");
      break;
    default:
      Serial.println("Data error!");
      break;
  }
}

/*Read the touchpad*/
void my_joypad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  static uint8_t count = 0;
  static bool btn_state = false;

  myFcJoypad.read();
  btn_state = myFcJoypad.state();
  for (count = 0; count < 8; count++) {
    if (myFcJoypad.isPressed((fc_joypad_btn_t)count))
      get_joypad_value(data, (fc_joypad_btn_t)count, btn_state);
    else if (myFcJoypad.isRelease((fc_joypad_btn_t)count))
      get_joypad_value(data, (fc_joypad_btn_t)count, btn_state);
  }
}


/*** lv_lib_100ask_file_explorer ***/
static void file_explorer_simple_test_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);

  if (code == LV_EVENT_VALUE_CHANGED) {
    char *cur_path = lv_100ask_file_explorer_get_cur_path(obj);
    char *sel_fn = lv_100ask_file_explorer_get_sel_fn(obj);
    LV_LOG_USER("%s%s", cur_path, sel_fn);
    Serial.println("----------------------");
    Serial.println(cur_path);
    Serial.println(sel_fn);
  }
}

void lv_100ask_file_explorer_simple_test(void) {
  lv_obj_t *file_explorer = lv_100ask_file_explorer_create(lv_scr_act());

  // 打开根目录
  lv_100ask_file_explorer_open_dir(file_explorer, "S:/");

#if LV_100ASK_FILE_EXPLORER_QUICK_ACCESS
  // 设置快速访问路径
  lv_100ask_file_explorer_set_quick_access_path(file_explorer, LV_100ASK_EXPLORER_HOME_DIR, "S:/root");
  lv_100ask_file_explorer_set_quick_access_path(file_explorer, LV_100ASK_EXPLORER_VIDEO_DIR, "S:/root/Videos");
  lv_100ask_file_explorer_set_quick_access_path(file_explorer, LV_100ASK_EXPLORER_PICTURES_DIR, "S:/root/Pictures");
  lv_100ask_file_explorer_set_quick_access_path(file_explorer, LV_100ASK_EXPLORER_MUSIC_DIR, "S:/root/Music");
  lv_100ask_file_explorer_set_quick_access_path(file_explorer, LV_100ASK_EXPLORER_DOCS_DIR, "S:/root/Documents");
#endif

  lv_obj_add_event_cb(file_explorer, file_explorer_simple_test_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
}

void setup() {
  Serial.begin(115200); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  Serial.println(LVGL_Arduino);
  Serial.println("I am LVGL_Arduino");

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */
  tft.invertDisplay(0);

  myFcJoypad.begin();

  lv_port_sd_fs_init();

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

#if 1
  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t joypad_drv;
  lv_indev_drv_init(&joypad_drv);
  joypad_drv.type = LV_INDEV_TYPE_KEYPAD;
  joypad_drv.read_cb = my_joypad_read;
  lv_joypad_device_object = lv_indev_drv_register(&joypad_drv);

  // 创建一个组，稍后将需要使用键盘或编码器或按钮控制的部件(对象)添加进去，并且将输入设备和组关联
  // 如果将这个组设置为默认组，那么对于那些在创建时会添加到默认组的部件(对象)就可以省略 lv_group_add_obj()
  // 视频教程学习：https://www.bilibili.com/video/BV1Ya411r7K2?p=19
  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);
  lv_indev_set_group(lv_joypad_device_object, g);  // joypad
#endif

  lv_100ask_file_explorer_simple_test();
  
  Serial.println("Setup done");
  
}


void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}