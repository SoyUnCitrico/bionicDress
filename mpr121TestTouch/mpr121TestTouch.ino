#include <Arduino.h>
#include <MPR121.h>
#include <Streaming.h>

#include "Constants.h"


MPR121 mpr121;

void setup()
{
  Serial.begin(constants::baud);

  mpr121.setupSingleDevice(*constants::wire_ptr,
    constants::device_address,
    constants::fast_mode);

  mpr121.setAllChannelsThresholds(constants::touch_threshold,
    constants::release_threshold);
  mpr121.setDebounce(constants::device_address,
    constants::touch_debounce,  
    constants::release_debounce);
  mpr121.setBaselineTracking(constants::device_address,
    constants::baseline_tracking);
  mpr121.setChargeDischargeCurrent(constants::device_address,
    constants::charge_discharge_current);
  mpr121.setChargeDischargeTime(constants::device_address,
    constants::charge_discharge_time);
  mpr121.setFirstFilterIterations(constants::device_address,
    constants::first_filter_iterations);
  mpr121.setSecondFilterIterations(constants::device_address,
    constants::second_filter_iterations);
  mpr121.setSamplePeriod(constants::device_address,
    constants::sample_period);

  mpr121.startChannels(constants::physical_channel_count,
    constants::proximity_mode);
}

void loop()
{
  delay(constants::loop_delay);
  if (!mpr121.communicating(constants::device_address))
  {
    Serial << "mpr121 device not commmunicating!\n\n";
    return;
  }
  
  uint16_t touch_status = mpr121.getTouchStatus(constants::device_address);
  Serial << "touch_status: " << _BIN(touch_status) << "\n";

//  for(int i = 0; i < constants::physical_channel_count; i++) {
//      bool channel_touched = mpr121.channelTouched(i);
//      Serial << "Sensor" << i << ": " << channel_touched << "\n";
//  }

  Serial << "\n";
}
