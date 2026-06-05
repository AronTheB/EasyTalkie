// Todo - Add oled screen and also the connect to wifi


#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <driver/i2s.h>

#define SAMPLE_RATE 16000
#define SAMPLE_PER_CHUNK 128
#define ESP_NOW_CHANNEL 1

#define MIC_SCK GPIO_NUM_2
#define MIC_WS GPIO_NUM_3
#define MIC_SD GPIO_NUM_4

#define SPK_DIN GPIO_NUM_43
#define SPK_LR GPIO_NUM_3
#define SPK_BCLK GPIO_NUM_2

#define BTN_PIN GPIO_NUM_9


// Buffer for audio samples, basicly the rx_head and rx_tail show where the write and read is happening.
#define BUF_SAMPLES (300 * 16)

static int16_t rx_buf[BUF_SAMPLES];
static volatile int rx_head = 0, rx_tail = 0;

static void buf_push(const int16_t * src, int n) {
  for (int i = 0; i < n; i++) {
    rx_buf[rx_head] = src[i];
    rx_head = (rx_head + 1) % BUF_SAMPLES;
  }
}
static void buf_pop(int16_t * dst, int n) {
  for (int i = 0; i < n; i++) {
    dst[i] = (rx_head != rx_tail) ? rx_buf[rx_tail] : 0;
    if (rx_head != rx_tail) rx_tail = (rx_tail + 1) % BUF_SAMPLES;
  }
}
static void buf_flush() {
  rx_head = rx_tail = 0;
}

// basic ESPNOW Setup.

static uint8_t BROADCAST[] = {
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF
};

static void on_recv(const uint8_t * ,
  const uint8_t * data, int len) {
  buf_push((const int16_t * ) data, len / 2);
}

#define PKT_SAMPLES 125
static int16_t pkt[PKT_SAMPLES];
static int pkt_pos = 0;

static void send_sample(int16_t s) {
  pkt[pkt_pos++] = s;
  if (pkt_pos == PKT_SAMPLES) {
    esp_now_send(BROADCAST, (uint8_t * ) pkt, sizeof(pkt));
    pkt_pos = 0;
  }
}
static void send_flush() {
  if (pkt_pos) {
    esp_now_send(BROADCAST, (uint8_t * ) pkt, pkt_pos * 2);
    pkt_pos = 0;
  }
}

static void espnow_setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(ESP_NOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(on_recv);
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, BROADCAST, 6);
  peerInfo.channel = ESP_NOW_CHANNEL;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

// Mic

enum Mode { MODE_NONE, MODE_MIC, MODE_SPK };
static Mode current_mode = MODE_NONE;
 
static void i2s_teardown() {
  if (current_mode != MODE_NONE) {
    i2s_driver_uninstall(I2S_NUM_0);
    current_mode = MODE_NONE;
  }
}

static void mic_mode() {
  if (current_mode == MODE_MIC) return;
  i2s_teardown();
 
  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,   // L/R tied GND = left
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
  };
  i2s_pin_config_t pins = {
    .bck_io_num = MIC_SCK,
    .ws_io_num = MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_SD,
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  current_mode = MODE_MIC;
}

// Speaker

static void spk_mode() {
  if (current_mode == MODE_SPK) return;
  i2s_teardown();

  i2s_config_t cfg = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
  };

  i2s_pin_config_t pins = {
    .bck_io_num = SPK_BCLK,
    .ws_io_num = SPK_LR,
    .data_out_num = SPK_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE,
  };
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  current_mode = MODE_SPK;
}

static void mic_task() {
  int32_t raw[SAMPLE_PER_CHUNK];
  size_t br = 0;
  i2s_read(I2S_NUM_0, raw, sizeof(raw), &br, portMAX_DELAY);
  int n = br /4;
  for (int i = 0; i < n; i++) {
    int32_t s = raw[i] >> 16;
    send_sample((int16_t) s);
  }
}

static void spk_task() {
  int16_t chunk[SAMPLE_PER_CHUNK];
  int16_t stereo[SAMPLE_PER_CHUNK * 2];
  buf_pop(chunk, SAMPLE_PER_CHUNK);
  for (int i = 0; i < SAMPLE_PER_CHUNK; i++) {
    stereo[i * 2] = chunk[i];
    stereo[i * 2 + 1] = chunk[i];
  }
  size_t bw = 0;
  i2s_write(I2S_NUM_0, stereo, sizeof(stereo), &bw, portMAX_DELAY);
}

// Main

void setup() {
  Serial.begin(115200);

  pinMode(BTN_PIN, INPUT_PULLUP);
  espnow_setup();
}

void loop() {
  static bool was_talking = false;
  bool talking = (digitalRead(BTN_PIN) == LOW);
  if (talking && !was_talking) {
    mic_mode();
  } else if (!talking && was_talking) {
    send_flush();
    buf_flush();
    spk_mode();
  }    
  was_talking = talking;

  if (talking) {
    mic_task();
  } else {
    spk_task();
  }
}




