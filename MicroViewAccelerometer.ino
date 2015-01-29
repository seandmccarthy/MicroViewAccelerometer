#include <MicroView.h>
#include <Wire.h>

#define SCALE_FACTOR_2G 0.061035f
#define SCALE_FACTOR_4G 0.122070f
#define SCALE_FACTOR_8G 0.244140f

const uint8_t i2c_address = 0x18;
const int gScale = 2;
const int displayScale = 100;
const int scaleMax = 2 * gScale * displayScale;

double x, y, z;
double x_val;
double y_val;
double z_val;

MicroViewWidget *sliderX, *sliderY, *sliderZ;

void setup() {
  Wire.begin();
  Wire.beginTransmission(i2c_address);
  // See the data sheet for configuration values: http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00274221.pdf
  Wire.write(0x20); // CTRL_REG1 20h
  Wire.write(0x7F); // 400hz, low power mode, Z Y X axis on
  //Wire.write(0x77); // 400hz, normal power mode, Z Y X axis on
  Wire.endTransmission();
  Wire.beginTransmission(i2c_address);
  Wire.write(0x23); // CTRL_REG4 23h
  Wire.write(0x08); // +/- 2G, high resolution
  Wire.endTransmission();

  uView.begin();
  uView.clear(PAGE);

  sliderX = new MicroViewSlider( 0, 0, 0, scaleMax, WIDGETSTYLE3 + WIDGETNOVALUE);
  sliderY = new MicroViewSlider(25, 0, 0, scaleMax, WIDGETSTYLE3 + WIDGETNOVALUE);
  sliderZ = new MicroViewSlider(50, 0, 0, scaleMax, WIDGETSTYLE3 + WIDGETNOVALUE);
}

uint16_t rawFrom(byte highByte, byte lowByte, boolean highOnly)
{
  uint8_t val_l, val_h;
 
  Wire.beginTransmission(i2c_address);
  Wire.write(highByte);
  Wire.endTransmission();
  Wire.requestFrom(i2c_address, 1u);
  if(Wire.available())
  {
    val_h = Wire.read();
  }
  
  Wire.beginTransmission(i2c_address);
  Wire.write(lowByte);
  Wire.endTransmission();
  Wire.requestFrom(i2c_address, 1u);
  if(Wire.available())
  {
    val_l = Wire.read();
  }

  return ((val_h) << 8) | val_l;
}

int16_t rawX()
{
  return (int16_t)rawFrom(0x29, 0x28, false);
}

double rawXhigh()
{
  return rawFrom(0x29, 0x28, true);
} 

int16_t rawY()
{
  return (int16_t)rawFrom(0x2B, 0x2A, false);
}

double rawYhigh()
{
  return rawFrom(0x2B, 0x2A, true);
}

int16_t rawZ()
{
  return (int16_t)rawFrom(0x2D, 0x2C, false);
}

double scaled2G(double raw)
{
  return (raw * SCALE_FACTOR_2G) / 1000;
}

double scaled4G(double raw)
{
  return (raw * SCALE_FACTOR_4G)/ 1000;
}

double scaled8G(double raw)
{
  return (raw * SCALE_FACTOR_8G)/ 1000;  
}

int scaleForSlider(double val)
{
  return (int)(val * displayScale) + (gScale * displayScale);
}

void loop()
{
  x_val = rawX();
  y_val = rawY();
  z_val = rawZ();
 
  x = scaled2G(x_val);
  y = scaled2G(y_val);
  z = scaled2G(z_val);

  sliderX->setValue(scaleForSlider(x));
  sliderY->setValue(scaleForSlider(y));
  sliderZ->setValue(scaleForSlider(z));
  uView.display();
  delay(50);
}
