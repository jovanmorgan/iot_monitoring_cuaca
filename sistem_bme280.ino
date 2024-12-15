#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

// Ganti dengan SSID dan password WiFi Anda
const char* ssid = "Syahid";
const char* password = "01020305";

// Ganti dengan alamat IP atau URL yang sesuai dengan server Anda
const char* serverName = "http://192.168.218.247/sistem_bme280/pengguna/admin/post_data.php";

void setup() {
  Serial.begin(115200);

  // Menghubungkan ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Inisialisasi sensor BME280
  if (!bme.begin(0x76)) {
    Serial.println("Sensor BME280 tidak ditemukan!");
    while (1);
  }
}

void loop() {
  // Membaca data dari BME280
  float suhu = bme.readTemperature();
  float tekanan = bme.readPressure() / 100.0F; // Konversi ke hPa
  float ketinggian = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float kelembaban = bme.readHumidity();

  // Mengirim data ke server jika terhubung ke WiFi
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Inisialisasi HTTP POST request
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Membuat data untuk dikirim
    String httpRequestData = "suhu=" + String(suhu) 
                           + "&tekanan=" + String(tekanan) 
                           + "&ketinggian=" + String(ketinggian) 
                           + "&kelembaban=" + String(kelembaban);

    // Mengirim data ke server
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString(); // Dapatkan respon dari server
      Serial.println("HTTP Response Code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Tutup koneksi
  }

  delay(2000); // Tunggu 2 detik sebelum mengirim data lagi
}
