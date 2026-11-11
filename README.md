# AVR++ / AVRPP
An AVR emulator in C++

The goal of this project is to learn more about the AVR architecture and the different families (excluding AVR32), and write an emulator of it in modern-ish C++. My final-final goal is writing a soft core AVR CPU, perhaps even rebuild some of the peripherals, I'll see. Using this C++ emulator project, I want to understand the architecture and it's quirks in a more debug-friendly way, brushing up on my dusty C++ skills and hopefully extend it with the more recent language additions (C++20, C++23). 

# Datasheets

The emulator follows Microchip's official documentation. The PDFs are © Microchip Technology Inc. and
are not part of this repository; download them from Microchip into `datasheets/` (ignored by git):

| Family | Document | Local file |
|---|---|---|
| All cores | [AVR Instruction Set Manual, DS40002198](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/ReferenceManuals/AVR-InstructionSet-Manual-DS40002198.pdf) | `datasheets/AVR-InstructionSet-Manual-DS40002198.pdf` |
| Classic (avr1) | [AT90S1200, doc0838](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0838.pdf) | `datasheets/classic/AT90S1200-doc0838.pdf` |
| Classic (avr2) | [AT90S8515, doc0841](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0841.pdf) | `datasheets/classic/AT90S8515-doc0841.pdf` |
| Classic (avr31) | [ATmega103, doc0945](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0945.pdf) | `datasheets/classic/ATmega103-doc0945.pdf` |
| Classic (avr25) | [ATtiny25/45/85, doc2586](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf) | `datasheets/classic/ATtiny25-45-85-doc2586.pdf` |
| Classic (avr4, avr5) | [ATmega48A/88A/168A/328P, DS40002061B](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf) | `datasheets/classic/ATmega48A-88A-168A-328P-DS40002061B.pdf` |
| Classic (avr51, avr6) | [ATmega640/1280/1281/2560/2561, DS40002211A](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega640-1280-1281-2560-2561-Datasheet-DS40002211A.pdf) | `datasheets/classic/ATmega640-1280-2560-DS40002211A.pdf` |
| XMEGA | [XMEGA AU Manual, doc8331](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8331-8-and-16-bit-AVR-Microcontroller-XMEGA-AU_Manual.pdf) | `datasheets/xmega/XMEGA-AU-Manual-doc8331.pdf` |
| XMEGA | [XMEGA C Manual, doc8465](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8465-8-and-16-bit-AVR-Microcontrollers-XMEGA-C_Manual.pdf) | `datasheets/xmega/XMEGA-C-Manual-doc8465.pdf` |
| XMEGA | [XMEGA D Manual, doc8210](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8210-8-and-16-bit-AVR-Microcontrollers-XMEGA-D_Manual.pdf) | `datasheets/xmega/XMEGA-D-Manual-doc8210.pdf` |
| XMEGA | [ATxmega16D4/32D4/64D4/128D4, doc8135](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8135-8-and-16-bit-AVR-microcontroller-ATxmega16D4-32D4-64D4-128D4_datasheet.pdf) | `datasheets/xmega/ATxmega16D4-128D4-doc8135.pdf` |
| XMEGA | [ATxmega64A3U/128A3U/192A3U/256A3U, doc8386](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8386-8-and-16-bit-AVR-Microcontroller-ATxmega64A3U-128A3U-192A3U-256A3U_datasheet.pdf) | `datasheets/xmega/ATxmega64A3U-256A3U-doc8386.pdf` |
| XMEGA | [ATxmega384C3, doc8361](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8361-8-and-16-bit-AVR-XMEGA-Microcontrollers-ATxmega384C3_Datasheet.pdf) | `datasheets/xmega/ATxmega384C3-doc8361.pdf` |
| XMEGA | [ATxmega64A1U/128A1U, doc8385](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8385-8-and-16-bit-AVR-Microcontroller-ATxmega64A1U-ATxmega128A1U_datasheet.pdf) | `datasheets/xmega/ATxmega64A1U-128A1U-doc8385.pdf` |
| AVRxt (megaAVR 0) | [ATmega4808/4809, DS40002173C](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega4808-09-DataSheet-DS40002173C.pdf) | `datasheets/avrxt/ATmega4808-4809-DS40002173C.pdf` |
| AVRxt (AVR DA) | [AVR128DA28/32/48/64, DS40002183](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/AVR128DA28-32-48-64-Data-Sheet-DS40002183.pdf) | `datasheets/avrxt/AVR128DA28-32-48-64-DS40002183.pdf` |
| AVRrc | [ATtiny4/5/9/10, doc8127](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8127-AVR-8-bit-Microcontroller-ATtiny4-ATtiny5-ATtiny9-ATtiny10_Datasheet.pdf) | `datasheets/avrrc/ATtiny4-5-9-10-doc8127.pdf` |

The links were checked on 2026-11-11. Microchip moves its documents from time to time; if a link is
dead, search for the document number (e.g. "DS40002198") on microchip.com.
