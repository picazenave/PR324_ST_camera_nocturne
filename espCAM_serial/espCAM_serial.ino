#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"

// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

struct serial_camera_config_t {
  byte brightness;
  byte special_effect;
  byte jpg_quality;
  framesize_t frame_size;
};
serial_camera_config_t camera_config = {.brightness = 0, .special_effect = 2, .jpg_quality = 20, .frame_size = FRAMESIZE_SVGA};

uint16_t fb_size = 0; 

bool serial_init_done = false;
bool setup_done = false;
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(2000000);                     // Define and start serial monitor

  while (serial_init_done == false)
  {
    dispatch_serial_command();
  }

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

  config.frame_size = camera_config.frame_size;
  config.jpeg_quality = camera_config.jpg_quality;
  config.fb_count = 3;
  config.grab_mode = CAMERA_GRAB_LATEST;
  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    while (1)
      Serial.write(0xAA);
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, camera_config.brightness);
  s->set_special_effect(s, camera_config.special_effect);
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)

  setup_done = true;
}

void loop() {
  dispatch_serial_command();
}

char wait_for_serial()
{
  char r = 0;
  while (!Serial.available());
  r = Serial.read();

  return r;
}


void dispatch_serial_command()
{
  camera_fb_t * fb = NULL;
  char r = wait_for_serial();
  char temp = 0;
  switch (r)
  {
    case 0xAA : //ACK
      temp = (setup_done + serial_init_done << 1);
      Serial.write(temp);
      break;
    case 0xA1 : // start config
      //other chip reset but not esp so restart fresh
      if (serial_init_done || setup_done) {
        pinMode(PWDN_GPIO_NUM, OUTPUT);
        digitalWrite(PWDN_GPIO_NUM, HIGH);
        delay(10);
        digitalWrite(PWDN_GPIO_NUM, LOW);
        delay(10);

        pinMode(RESET_GPIO_NUM, OUTPUT);
        digitalWrite(RESET_GPIO_NUM, LOW);
        delay(10);
        digitalWrite(RESET_GPIO_NUM, HIGH);
        delay(10);
        esp_restart();
      }

      break;
    case 0xA2 : //end config
      serial_init_done = true;
      break;
    case 0x55 : //get jpg
      fb = esp_camera_fb_get();
      
      fb_size = fb->len;
      Serial.write(uint8_t(fb_size >> 8));
      Serial.write(uint8_t(fb_size & 0xFF));
      while (wait_for_serial() != 0x55);
      for (unsigned int i = 0; i < fb->len; i++)
      {
        Serial.write(fb->buf[i]);
      }
      esp_camera_fb_return(fb);
      break;
    case 0x34 : //set brightness
      camera_config.brightness = wait_for_serial();
      break;
    case 0x33 : //set special effect
      camera_config.special_effect = wait_for_serial();
      break;
    case 0x22 : //set jpg quality
      camera_config.jpg_quality = wait_for_serial();
      break;
    case 0x11 : // set frame size
      temp = wait_for_serial();
      switch (temp)
      {
        case 0:
          camera_config.frame_size = FRAMESIZE_UXGA;
          break;
        case 1:
          camera_config.frame_size = FRAMESIZE_QVGA;
          break;
        case 2:
          camera_config.frame_size = FRAMESIZE_CIF;
          break;
        case 3:
          camera_config.frame_size = FRAMESIZE_VGA;
          break;
        case 4:
          camera_config.frame_size = FRAMESIZE_SVGA;
          break;
        case 5:
          camera_config.frame_size = FRAMESIZE_XGA;
          break;
        case 6:
          camera_config.frame_size = FRAMESIZE_SXGA;
          break;
        default:
          camera_config.frame_size = FRAMESIZE_QVGA;
          break;
      }
      break;

    default :
      break;
  }

}
