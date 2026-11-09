# LoRa_Puck
Building a custom LoRa device 
The SX1262 receives on two pins (RFI_P and RFI_N) while the antenna side is a single wire, so FL1 (0900FM15K0039) sits between them as a balun. The Radio frequency switch (PE4259) connects the one antenna to either the transmit or receive path, and I learned the radio flips it by itself through its DIO2 pin, which is what radio.setDio2AsRfSwitch(true) turns on in my firmware. 

The USB-C connects to the MCU via the USB_DM and USB_DP (yeah, the RP2040 doesn't come with it's own connector) with a 27 ohms resistor on each pin as stated by the datasheet. 

I added a mini feature using an LED to indicate when a message or signal successful transmitted, connected one end to a 330 resistor before connecting to the GPIO pin (so I don't blow up the MCU) and the other to GND. 
I used Ohm's law to calculate the resistance. We take the GPIO voltage of 3.3V and subtract the LED's forward voltage (assume 2V for a typical LED). That leaves 1.3V across the resistor. If we want approximately 4mA of current through the LED, we get 325 ohms, since 330Ω is a standard resistor value, I chose the closest available value.

When routing the PCB I moved the SX_DIO1 from GPIO17 TO GPIO24, SPI_NSS from GPIO21 to GPIO17, SX_NRST from GPIO23 to GPIO22 and SX_BUSY from GPIO22 to GPIO23. And yes, I did check the datasheet to make sure the new pins match the needed features. This made it easier to track from the SX1262 to the MCU. I added GND copper zones to the F.Cu, In1.Cu, and B.Cu layers to provide ground connections and then created a via fence around the RF module to reduice electrical noise. 


![Schematic](lora_sch.png)
![3D View](LoRa_3D_View.png)
