#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "esp/uart.h"
#include "espressif/esp_common.h"
#include "task.h"

#include "ws2812_i2s/ws2812_i2s.h"

#define LEDCOUNT 12

static ws2812_pixel_t pixels[LEDCOUNT];

/*
static void fade_pixel(ws2812_pixel_t *pixel, uint32_t factor)
{
  pixel->red = pixel->red / factor;
  pixel->green = pixel->green / factor;
  pixel->blue = pixel->blue / factor;
}*/

static void init() { memset(pixels, 0, sizeof(ws2812_pixel_t) * LEDCOUNT); }

static void set_color(uint32_t red, uint32_t green, uint32_t blue)
{
  for (int i = 0; i < LEDCOUNT; i++) {
    pixels[i].red = red;
    pixels[i].green = green;
    pixels[i].blue = blue;
  }
}

// FreeRTOS task
static void update(void *pvParameters)
{
  ws2812_i2s_init(LEDCOUNT, PIXEL_RGB);

  while (true) {
    ws2812_i2s_update(pixels, PIXEL_RGB);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void user_init(void)
{
  init();

  uart_set_baud(0, 115200);
  struct sdk_station_config config = {.ssid = "PORTER-BGN",
                                      .password = "uphit34!"};

  sdk_wifi_set_opmode(STATION_MODE);
  sdk_wifi_station_set_config(&config);

  // let's pick a color

  set_color(255, 0, 0);  // red!

  xTaskCreate(&update, "ws2812_i2s", 256, NULL, 10, NULL);
}
