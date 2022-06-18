#include "esp_camera.h" 
#include "esp_timer.h" 
#include "img_converters.h" 
#include "Arduino.h" 
#include "fb_gfx.h" 
#include "fd_forward.h" 
#include "fr_forward.h" 
#include "FS.h" 
#include "SD_MMC.h" 
#include "soc/soc.h" 
#include "soc/rtc_cntl_reg.h" 
//#include "dl_lib.h" 
#include "driver/rtc_io.h" 
#include <EEPROM.h> 
 
 
#define EEPROM_SIZE 512 
 
 
#define PWDN_GPIO_NUM 32 
#define RESET_GPIO_NUM -1 
#define XCLK_GPIO_NUM 0 
#define SIOD_GPIO_NUM 26 
#define SIOC_GPIO_NUM 27 
 
#define Y9_GPIO_NUM 35 
#define Y8_GPIO_NUM 34 
#define Y7_GPIO_NUM 39 
#define Y6_GPIO_NUM 36 
#define Y5_GPIO_NUM 21 
#define Y4_GPIO_NUM 19 
#define Y3_GPIO_NUM 18 
#define Y2_GPIO_NUM 5 
#define VSYNC_GPIO_NUM 25 
#define HREF_GPIO_NUM 23 
#define PCLK_GPIO_NUM 22 
 
int pictureNumber = 0; 
int eeprom_cell_num = 0; 
 
void setup() { 
 WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
 Serial.begin(115200); 
 
 
 camera_config_t config; 
 config.ledc_channel = LEDC_CHANNEL_0; 
 config.ledc_timer = LEDC_TIMER_0; 
 config.pin_d0 = Y2_GPIO_NUM; 
 config.pin_d1 = Y3_GPIO_NUM; 
 config.pin_d2 = Y4_GPIO_NUM; 
 config.pin_d3 = Y5_GPIO_NUM; 
 config.pin_d4 = Y6_GPIO_NUM; 
 config.pin_d5 = Y7_GPIO_NUM; 
 config.pin_d6 = Y8_GPIO_NUM; 
 config.pin_d7 = Y9_GPIO_NUM; 
 config.pin_xclk = XCLK_GPIO_NUM; 
 config.pin_pclk = PCLK_GPIO_NUM; 
 config.pin_vsync = VSYNC_GPIO_NUM; 
 config.pin_href = HREF_GPIO_NUM; 
 config.pin_sscb_sda = SIOD_GPIO_NUM; 
 config.pin_sscb_scl = SIOC_GPIO_NUM; 
 config.pin_pwdn = PWDN_GPIO_NUM; 
 config.pin_reset = RESET_GPIO_NUM; 
 config.xclk_freq_hz = 20000000; 
 config.pixel_format = PIXFORMAT_JPEG; 
 
 if(psramFound()){ 
 config.frame_size = FRAMESIZE_UXGA; 
 config.jpeg_quality = 10; 
 config.fb_count = 2; 
 } else { 
 config.frame_size = FRAMESIZE_SVGA; 
 config.jpeg_quality = 12; 
 config.fb_count = 1; 
 } 
 
 // Init Camera 
 esp_err_t err = esp_camera_init(&config); 
 if (err != ESP_OK) { 
 Serial.printf("Camera init failed with error 0x%x", err); 
 return; 
 } 
 
 //Serial.println("Starting SD Card"); 
 if(!SD_MMC.begin()){ 
 Serial.println("SD Card Mount Failed"); 
 return; 
 } 
 
 uint8_t cardType = SD_MMC.cardType(); 
 if(cardType == CARD_NONE){ 
 Serial.println("No SD Card attached"); 
 return; 
 } 
 
 
 
 
 // Включить светодиод, он находится на 4 ножке в модуле esp32-cam 
 /* 
 pinMode(4, OUTPUT); 
 digitalWrite(4, LOW); 
 rtc_gpio_hold_en(GPIO_NUM_4); 
 
 delay(2000); 
 Serial.println("Going to sleep now"); 
 delay(2000); 
 //esp_deep_sleep_start(); 
 Serial.println("This will never be printed"); 
 */ 
} 
 
void loop() { 
 camera_fb_t * fb = NULL; 
 
 // Сделать снимок с помощью камеры 
 fb = esp_camera_fb_get(); 
 if(!fb) { 
 Serial.println("Camera capture failed"); 
 return; 
 } 
 // Инициализация EEPROM 
 EEPROM.begin(EEPROM_SIZE); 
 pictureNumber = (int)EEPROM.read(eeprom_cell_num) + 1; 
 if (pictureNumber == 256) { 
 ++eeprom_cell_num; 
 pictureNumber = 0; 
 } 
 
 // Путь сохранения на флешке 
 String path = "/picture" + String(eeprom_cell_num) + "_" + String(pictureNumber) +".jpg"; 
 
 fs::FS &fs = SD_MMC; 
 Serial.printf("Picture file name: %s\n", path.c_str()); 
 
 File file = fs.open(path.c_str(), FILE_WRITE); 
 if(!file){ 
 Serial.println("Failed to open file in writing mode"); 
 } 
 else { 
 file.write(fb->buf, fb->len); 
 Serial.printf("Saved file to path: %s\n", path.c_str()); 
 EEPROM.write(eeprom_cell_num, pictureNumber); 
 EEPROM.commit(); 
 } 
 file.close(); 
 esp_camera_fb_return(fb); 
 delay(2000); 
}
