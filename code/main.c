#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <time.h>

#include "ssd1306.h"

#define I2C_SDA 4
#define I2C_CLK 5

#define LIGHTSENSE 2
#define PAD1 10
#define PAD2 11
#define PAD3 12
#define BTN1 21
#define BTN2 20

#define SCREENUS 500000
#define TOGGLEUS 60000
#define MAXSCREENDELAYUS SCREENUS / 2

// Number of readings per batch
#define BATCHSIZE 20

void setupGPIO() {
  stdio_init_all();
  
  // Display pins
  i2c_init(i2c0, 400000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_CLK, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_CLK);
  
  // Light pin
  gpio_init(LIGHTSENSE);
  gpio_set_dir(LIGHTSENSE, GPIO_IN);
  //gpio_pull_up( LIGHTSENSE );
  
  // Buttons.
  gpio_init(BTN1);
  gpio_set_dir(BTN1, GPIO_IN);
  gpio_init(BTN2);
  gpio_set_dir(BTN2, GPIO_IN);
  
  // Pad, for the first set to input.
  gpio_init(PAD1);
  gpio_set_dir(PAD1, GPIO_IN);
  gpio_init(PAD2);
  gpio_set_dir(PAD2, GPIO_IN);
  gpio_init(PAD3);
  gpio_set_dir(PAD3, GPIO_IN);
  
}

int main() {
  stdio_init_all();
  setupGPIO();
  
  printf( "Starting\n" );
  
  // Set up display.
  ssd1306_t disp;
  disp.external_vcc = false;
  ssd1306_init( &disp, 128, 64, 0x3C, i2c0 );
  ssd1306_clear( &disp );
  
  // For GBA, set the output register value to low.
  gpio_put( PAD1, 0 );
  //gpio_pull_up( PAD1 );
  
  // Doing the actual measurement
  unsigned curMeasurement = 0;
  unsigned totalMeasurements = 0;
  unsigned lastSensorVal = 0;
  
  // Average val for measurements
  unsigned lagVals[ BATCHSIZE ];
  for ( int i = 0; i < BATCHSIZE; ++i ) {
    lagVals[ i ] = 0;
  }
  
  unsigned registeredChange = 1;
  
  uint64_t buttonPressTime = time_us_64();
  unsigned toggling = 0;
  
  unsigned init = 1;
  // We do not know initially, what color is showing on the screen.
  // We just assume it's black (curScreen = 0). If it's wrong, we change
  // the internal state.
  unsigned curScreen = 0; // black
    
  while ( 1 ) {
    // Get the current light value.
    // Light val == 1 is black, val == 0 is white
    unsigned lightVal = gpio_get( LIGHTSENSE );
    uint64_t curTime = time_us_64();
    
    // Check if a change was detected.
    if ( lightVal != lastSensorVal ) {      
      if ( !registeredChange ) {
        // Only interested in black to white.
        if (lastSensorVal == 1 && lightVal == 0 ) {
          uint32_t lag = (uint32_t) ( curTime - buttonPressTime );
          printf( "Got change!\n" );
          
          lagVals[ curMeasurement ] = lag;
          ++curMeasurement;
          ++totalMeasurements;
          
          if ( curMeasurement >= BATCHSIZE ) {
            curMeasurement = 0;
          }
          
          unsigned minLag = -1;
          unsigned maxLag = 0;
          unsigned avgLag = 0;
          unsigned cnt = 0;
          
          for ( int i = 0; i < BATCHSIZE; ++i ) {
            if ( i >= totalMeasurements ) {
              break;
            }
            
            unsigned curVal = lagVals[ i ];            
            avgLag += curVal;
            
            if ( curVal > maxLag ) {
              maxLag = curVal;
            }
            
            if ( curVal < minLag ) {
              minLag = curVal;
            }
            
            ++cnt;
          }
          
          avgLag = avgLag / cnt;
          
          float avgLag_f = (float) avgLag / 1000.0f;
          float minLag_f = (float) minLag / 1000.0f;
          float maxLag_f = (float) maxLag / 1000.0f;
          
          char textout[100];
          ssd1306_clear( &disp );
          sprintf( textout, "Avg Lag: %f ms", avgLag_f );
          ssd1306_draw_string( &disp, 0, 0, 1, textout );
          sprintf( textout, "Max Lag: %f ms", maxLag_f );
          ssd1306_draw_string( &disp, 0, 16, 1, textout );
          sprintf( textout, "Min Lag: %f ms", minLag_f );
          ssd1306_draw_string( &disp, 0, 32, 1, textout );
          ssd1306_show(&disp);
        }
        
      } else {
        // During white screen, the PWM of many screens cause it to
        // register also black here.
        // Yet, make sure that we did not just switch to a black screen
        // and are seeing a delay.
        if ( curTime - buttonPressTime > MAXSCREENDELAYUS ) {
          curScreen = 1;
        }
      }

      registeredChange = 1;
    }
    
    // Toggle screen?
    if ( curTime - buttonPressTime > SCREENUS ) {
      gpio_set_dir( PAD1, GPIO_OUT );
      buttonPressTime = time_us_64();
      toggling = 1;
      curScreen = !curScreen;
      
      // New screen is white?
      if ( curScreen ) {
        registeredChange = 0;
      }
      
    } else if ( curTime - buttonPressTime > TOGGLEUS && toggling ) {
      // Release button?
      gpio_set_dir( PAD1, GPIO_IN );
      toggling = 0;
    }
    
    lastSensorVal = lightVal;
  }
  
}
