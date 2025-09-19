diff --git a/README.md b/README.md
index 6898177..10e67e3 100644
--- a/README.md
+++ b/README.md
@@ -1,5 +1,7 @@
 # MiniJV880 ![Github Build Status](https://github.com/giulioz/mini-jv880/actions/workflows/build.yml/badge.svg)
 
+![JV880](https://github.com/user-attachments/assets/04f92b10-9d01-4172-8356-1d199547d564)
+
 Mini-JV880pi is a rompler-style synthesizer closely modeled on the famous JV-880 by a well-known Japanese manufacturer running on a bare metal Raspberry Pi (without a Linux kernel or operating system).
 
 ## Acknowledgements
diff --git a/circle-stdlib b/circle-stdlib
--- a/circle-stdlib
+++ b/circle-stdlib
@@ -1 +1 @@
-Subproject commit 1111eeedb2e2184d234c864947c8a95d68e32ee5
+Subproject commit 1111eeedb2e2184d234c864947c8a95d68e32ee5-dirty
diff --git a/src/Makefile b/src/Makefile
index 83a776f..9a49575 100644
--- a/src/Makefile
+++ b/src/Makefile
@@ -8,7 +8,7 @@ CIRCLE_STDLIB_DIR = ../circle-stdlib
 CMSIS_DIR = ../CMSIS_5/CMSIS
 
 OBJS = main.o kernel.o minijv880.o config.o userinterface.o uibuttons.o \
-       emulator/lcd.o emulator/mcu.o emulator/mcu_opcodes.o emulator/pcm.o
+       emulator/lcd.o emulator/mcu.o emulator/mcu_opcodes.o emulator/pcm.o drivers/ssd1306device24.o
 
 OPTIMIZE = -O3
 
diff --git a/src/drivers/font5x8.h b/src/drivers/font5x8.h
index 53997e7..8ac3ff3 100644
--- a/src/drivers/font5x8.h
+++ b/src/drivers/font5x8.h
@@ -1,17 +1,719 @@
+//
+// font5x8.h
+//
+// mt32-pi - A baremetal MIDI synthesizer for Raspberry Pi
+// Copyright (C) 2020-2022 Dale Whinham <daleyo@gmail.com>
+//
+// This file is part of mt32-pi.
+//
+// mt32-pi is free software: you can redistribute it and/or modify it under the
+// terms of the GNU General Public License as published by the Free Software
+// Foundation, either version 3 of the License, or (at your option) any later
+// version.
+//
+// mt32-pi is distributed in the hope that it will be useful, but WITHOUT ANY
+// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
+// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
+// details.
+//
+// You should have received a copy of the GNU General Public License along with
+// mt32-pi. If not, see <http://www.gnu.org/licenses/>.
+//
+
 #ifndef _font5x8_h
 #define _font5x8_h
 
 #include <circle/types.h>
 
-#define FONT_WIDTH  5
-#define FONT_HEIGHT 8
-#define FONT_SIZE   97
+#define FONT5X8_WIDTH  5
+#define FONT5X8_HEIGHT 8
+#define FONT5X8_SIZE   97
 
-constexpr u8 Font5x8[FONT_SIZE][8] =
+constexpr u8 Font5x8[FONT5X8_SIZE][8] =
 {
 	{
 		0b00000,
 		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00000,
+		0b00000,
+		0b00100,
+		0b00000
+	},
+	{
+		0b01010,
+		0b01010,
+		0b01010,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b01010,
+		0b01010,
+		0b11111,
+		0b01010,
+		0b11111,
+		0b01010,
+		0b01010,
+		0b00000
+	},
+	{
+		0b00100,
+		0b01111,
+		0b10100,
+		0b01110,
+		0b00101,
+		0b11110,
+		0b00100,
+		0b00000
+	},
+	{
+		0b11000,
+		0b11001,
+		0b00010,
+		0b00100,
+		0b01000,
+		0b10011,
+		0b00011,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01010,
+		0b00100,
+		0b01010,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00110,
+		0b00010,
+		0b00100,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00001,
+		0b00010,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00010,
+		0b00001,
+		0b00000
+	},
+	{
+		0b00100,
+		0b00010,
+		0b00001,
+		0b00001,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00100,
+		0b10101,
+		0b01110,
+		0b10101,
+		0b00100,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00100,
+		0b00100,
+		0b11111,
+		0b00100,
+		0b00100,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00110,
+		0b00010,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00000,
+		0b01111,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00110,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b01000,
+		0b10000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01011,
+		0b01101,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00100,
+		0b01100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b01110,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b01000,
+		0b11111,
+		0b00000
+	},
+	{
+		0b11111,
+		0b00001,
+		0b00110,
+		0b00001,
+		0b00001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00010,
+		0b00110,
+		0b01010,
+		0b10010,
+		0b11111,
+		0b00010,
+		0b00010,
+		0b00000
+	},
+	{
+		0b01111,
+		0b01000,
+		0b01110,
+		0b00001,
+		0b00001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01000,
+		0b01000,
+		0b01110,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01111,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01001,
+		0b00111,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00110,
+		0b00110,
+		0b00000,
+		0b00110,
+		0b00110,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00110,
+		0b00110,
+		0b00000,
+		0b00110,
+		0b00010,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00001,
+		0b00010,
+		0b00100,
+		0b01000,
+		0b00100,
+		0b00010,
+		0b00001,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b01111,
+		0b00000,
+		0b01111,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b01000,
+		0b00100,
+		0b00010,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b01000,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b00000,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01011,
+		0b01001,
+		0b00111,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01111,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b01110,
+		0b01001,
+		0b01001,
+		0b01110,
+		0b01001,
+		0b01001,
+		0b01110,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01110,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01110,
+		0b00000
+	},
+	{
+		0b01111,
+		0b01000,
+		0b01000,
+		0b01110,
+		0b01000,
+		0b01000,
+		0b01111,
+		0b00000
+	},
+	{
+		0b01111,
+		0b01000,
+		0b01000,
+		0b01110,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01000,
+		0b01011,
+		0b01001,
+		0b01001,
+		0b00111,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01111,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b00111,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00111,
+		0b00000
+	},
+	{
+		0b00011,
+		0b00001,
+		0b00001,
+		0b00001,
+		0b00001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01010,
+		0b01100,
+		0b01100,
+		0b01100,
+		0b01010,
+		0b01001,
+		0b00000
+	},
+	{
+		0b01000,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b01111,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01111,
+		0b01111,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01101,
+		0b01011,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01110,
+		0b01001,
+		0b01001,
+		0b01110,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01011,
+		0b01010,
+		0b00101,
+		0b00000
+	},
+	{
+		0b01110,
+		0b01001,
+		0b01001,
+		0b01110,
+		0b01010,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b00110,
+		0b01001,
+		0b01000,
+		0b00110,
+		0b00001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01111,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00101,
+		0b00010,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01111,
+		0b01111,
+		0b00101,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00110,
+		0b00110,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00101,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00000
+	},
+	{
+		0b01111,
+		0b00001,
+		0b00010,
+		0b00100,
+		0b00100,
+		0b01000,
+		0b01111,
+		0b00000
+	},
+	{
+		0b00110,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01001,
+		0b01001,
+		0b01111,
+		0b00010,
+		0b01111,
+		0b00100,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00110,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00010,
+		0b00100,
+		0b01000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b11111,
+		0b00000
+	},
+	{
+		0b00011,
+		0b00010,
+		0b00001,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00110,
+		0b00001,
+		0b00111,
+		0b01001,
+		0b00111,
+		0b00000
+	},
+	{
+		0b01000,
+		0b01000,
+		0b01110,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b01110,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00111,
+		0b01000,
+		0b01000,
+		0b01000,
+		0b00111,
+		0b00000
+	},
+	{
+		0b00001,
+		0b00001,
 		0b00111,
 		0b01001,
 		0b01001,
@@ -22,10 +724,120 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 	{
 		0b00000,
 		0b00000,
-		0b01111,
+		0b00110,
 		0b01001,
 		0b01111,
 		0b01000,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00011,
+		0b00100,
+		0b01111,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00111,
+		0b01001,
+		0b00111,
+		0b00001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b01000,
+		0b01000,
+		0b01010,
+		0b01101,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b00100,
+		0b00000,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00000
+	},
+	{
+		0b00010,
+		0b00000,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b01010,
+		0b00100,
+		0b00000
+	},
+	{
+		0b01000,
+		0b01000,
+		0b01010,
+		0b01100,
+		0b01010,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00010,
+		0b00011,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b01101,
+		0b01011,
+		0b01011,
+		0b01011,
+		0b01011,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b01010,
+		0b01101,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b00110,
+		0b01001,
+		0b01001,
+		0b01001,
+		0b00110,
+		0b00000
+	},
+	{
+		0b00000,
+		0b00000,
+		0b01110,
+		0b01001,
+		0b01110,
+		0b01000,
 		0b01000,
 		0b00000
 	},
@@ -42,8 +854,8 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 	{
 		0b00000,
 		0b00000,
-		0b01011,
-		0b01100,
+		0b01010,
+		0b01101,
 		0b01000,
 		0b01000,
 		0b01000,
@@ -56,16 +868,16 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b01000,
 		0b00110,
 		0b00001,
-		0b01111,
+		0b01110,
 		0b00000
 	},
 	{
-		0b00010,
-		0b00010,
+		0b00100,
+		0b00100,
 		0b01111,
-		0b00010,
-		0b00010,
-		0b00010,
+		0b00100,
+		0b00100,
+		0b00100,
 		0b00011,
 		0b00000
 	},
@@ -75,7 +887,7 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b01001,
 		0b01001,
 		0b01001,
-		0b01011,
+		0b01001,
 		0b00110,
 		0b00000
 	},
@@ -85,8 +897,8 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b01001,
 		0b01001,
 		0b01001,
-		0b00101,
-		0b00010,
+		0b00110,
+		0b00110,
 		0b00000
 	},
 	{
@@ -95,7 +907,7 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b01001,
 		0b01001,
 		0b01011,
-		0b01011,
+		0b01111,
 		0b00101,
 		0b00000
 	},
@@ -103,9 +915,9 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b00000,
 		0b00000,
 		0b01001,
-		0b00101,
-		0b00010,
-		0b00101,
+		0b01001,
+		0b00110,
+		0b01001,
 		0b01001,
 		0b00000
 	},
@@ -116,17 +928,17 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b01001,
 		0b00111,
 		0b00001,
-		0b00111,
+		0b00110,
 		0b00000
 	},
 	{
 		0b00000,
 		0b00000,
-		0b01111,
+		0b11111,
 		0b00010,
 		0b00100,
 		0b01000,
-		0b01111,
+		0b11111,
 		0b00000
 	},
 	{
@@ -140,13 +952,13 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 		0b00000
 	},
 	{
-		0b00010,
-		0b00010,
-		0b00010,
-		0b00000,
-		0b00010,
-		0b00010,
-		0b00010,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
+		0b00100,
 		0b00000
 	},
 	{
@@ -161,11 +973,11 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 	},
 	{
 		0b00000,
+		0b00100,
 		0b00010,
-		0b00001,
 		0b01111,
-		0b00001,
 		0b00010,
+		0b00100,
 		0b00000,
 		0b00000
 	},
@@ -192,4 +1004,4 @@ constexpr u8 Font5x8[FONT_SIZE][8] =
 	}
 };
 
-#endif
+#endif
\ No newline at end of file
diff --git a/src/drivers/ssd1306device24.cpp b/src/drivers/ssd1306device24.cpp
index cce369f..cd945eb 100644
--- a/src/drivers/ssd1306device24.cpp
+++ b/src/drivers/ssd1306device24.cpp
@@ -1,26 +1,6 @@
 //
 // ssd1306device24.cpp
 //
-// Much of this driver is based on code from:
-//    mt32-pi - A baremetal MIDI synthesizer for Raspberry Pi
-//    Copyright (C) 2020-2022 Dale Whinham <daleyo@gmail.com>
-//
-// Circle - A C++ bare metal environment for Raspberry Pi
-// Copyright (C) 2018-2022  R. Stange <rsta2@o2online.de>
-//
-// This program is free software: you can redistribute it and/or modify
-// it under the terms of the GNU General Public License as published by
-// the Free Software Foundation, either version 3 of the License, or
-// (at your option) any later version.
-//
-// This program is distributed in the hope that it will be useful,
-// but WITHOUT ANY WARRANTY; without even the implied warranty of
-// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-// GNU General Public License for more details.
-//
-// You should have received a copy of the GNU General Public License
-// along with this program.  If not, see <http://www.gnu.org/licenses/>.
-//
 #include "ssd1306device24.h"
 #include <circle/timer.h>
 #include <circle/types.h>
@@ -28,35 +8,33 @@
 #include <assert.h>
 #include "font5x8.h"
 
-enum TSSD1306Command24 : u8
-{
-	SetMemoryAddressingMode24    = 0x20,
-	SetColumnAddress24           = 0x21,
-	SetPageAddress24             = 0x22,
-	SetStartLine24               = 0x40,
-	SetContrast24                = 0x81,
-	SetChargePump24              = 0x8D,
-	EntireDisplayOnResume24      = 0xA4,
-	SetNormalDisplay24           = 0xA6,
-	SetMultiplexRatio24          = 0xA8,
-	SetDisplayOff24              = 0xAE,
-	SetDisplayOn24               = 0xAF,
-	SetDisplayOffset24           = 0xD3,
-	SetDisplayClockDivideRatio24 = 0xD5,
-	SetPrechargePeriod24         = 0xD9,
-	SetCOMPins24                 = 0xDA,
-	SetVCOMHDeselectLevel24      = 0xDB,
+enum TSSD1306Command : u8
+{
+	SetMemoryAddressingMode    = 0x20,
+	SetColumnAddress           = 0x21,
+	SetPageAddress             = 0x22,
+	SetStartLine               = 0x40,
+	SetContrast                = 0x81,
+	SetChargePump              = 0x8D,
+	EntireDisplayOnResume      = 0xA4,
+	SetNormalDisplay           = 0xA6,
+	SetMultiplexRatio          = 0xA8,
+	SetDisplayOff              = 0xAE,
+	SetDisplayOn               = 0xAF,
+	SetDisplayOffset           = 0xD3,
+	SetDisplayClockDivideRatio = 0xD5,
+	SetPrechargePeriod         = 0xD9,
+	SetCOMPins                 = 0xDA,
+	SetVCOMHDeselectLevel      = 0xDB,
 };
 
 // Compile-time (constexpr) font/graphics conversion functions.
-// The SSD1306 stores pixel data in columns, but our source data is stored as rows.
-// These templated functions generate column-wise versions of our graphics at compile-time.
 namespace
 {
 	using CharData = u8[8];
 
 	// Iterate through each row of the character data and collect bits for the nth column
-	static constexpr u8 SingleColumn24(const CharData& CharData, u8 nColumn)
+	static constexpr u8 SingleColumn(const CharData& CharData, u8 nColumn)
 	{
 		u8 bit = 4 - nColumn;  // 5-1-nColumn for 5-width font
 		u8 column = 0;
@@ -67,34 +45,18 @@ namespace
 		return column;
 	}
 
-	// Double the height of the character by duplicating column bits into a 16-bit value
-	static constexpr u16 DoubleColumn24(const CharData& CharData, u8 nColumn)
-	{
-		u8 singleColumn = SingleColumn24(CharData, nColumn);
-		u16 column = 0;
-
-		for (u8 i = 0; i < 8; ++i)
-		{
-			bool bit = singleColumn >> i & 1;
-			column |= bit << i * 2 | bit << (i * 2 + 1);
-		}
-
-		return column;
-	}
-
 	// Templated array-like structure with precomputed font data
 	template<size_t N, class F>
-	class Font24
+	class Font
 	{
 	public:
-		// Result type of conversion function should determine array type, but fixed here to u16
-		using Column = u16;
+		using Column = u8;
 		using ColumnData = Column[5];  // 5 columns for 5x8 font
 
-		constexpr Font24(const CharData(&CharData)[N], F Function) : mCharData{ {0} }
+		constexpr Font(const CharData(&CharData)[N], F Function) : mCharData{ {0} }
 		{
 			for (size_t i = 0; i < N; ++i)
-				for (u8 j = 0; j < 5; ++j)  // 5 columns
+				for (u8 j = 0; j < 5; ++j)
 					mCharData[i][j] = Function(CharData[i], j);
 		}
 
@@ -105,8 +67,8 @@ namespace
 	};
 }
 
-// Single and double-height versions of the font
-constexpr auto FontDouble24 = Font24<FONT_SIZE, decltype(DoubleColumn24)>(Font5x8, DoubleColumn24);
+// Single height version of the font
+constexpr auto FontSingle = Font<FONT5X8_SIZE, decltype(SingleColumn)>(Font5x8, SingleColumn);
 
 CSSD1306Device24::CSSD1306Device24 (unsigned nWidth, unsigned nHeight,
 				CI2CMaster *pI2CMaster, u8 nAddress,
@@ -131,140 +93,116 @@ boolean CSSD1306Device24::Initialize24 (void)
 {
 	assert(m_pI2CMaster != nullptr);
 
-	// Validate dimensions - only 128x32 and 128x64 supported for now
 	if (!(m_nHeight == 32 || m_nHeight == 64) || !(m_nWidth == 128 || m_nWidth == 132))
 		return false;
 
 	const bool bIsSSD1305 = m_nWidth == 132;
-	const u8 nMultiplexRatio24  = m_nHeight - 1;
-	const u8 nCOMPins24         = (m_nHeight == 32 && !bIsSSD1305) ? 0x02 : 0x12;
-	const u8 nColumnAddrRange24 = m_nWidth - 1;
-	const u8 nPageAddrRange24   = m_nHeight / 8 - 1;
-	// https://www.buydisplay.com/download/ic/SSD1312_Datasheet.pdf   Pg. 51 Section 2.1.19
-	//            normal    inverted
-	// normal     A1 C8       A0 C0
-	// mirrored   A0 C8       A1 C0
-	const u8 nSegRemap24        = (m_bRotated && !m_bMirrored) || (!m_bRotated && m_bMirrored) ? 0xA0 : 0xA1;
-	const u8 nCOMScanDir24      = m_bRotated ? 0xC0 : 0xC8;
-
-	const u8 InitSequence24[] =
+	const u8 nMultiplexRatio  = m_nHeight - 1;
+	const u8 nCOMPins         = (m_nHeight == 32 && !bIsSSD1305) ? 0x02 : 0x12;
+	const u8 nColumnAddrRange = m_nWidth - 1;
+	const u8 nPageAddrRange   = m_nHeight / 8 - 1;
+	const u8 nSegRemap        = (m_bRotated && !m_bMirrored) || (!m_bRotated && m_bMirrored) ? 0xA0 : 0xA1;
+	const u8 nCOMScanDir      = m_bRotated ? 0xC0 : 0xC8;
+
+	const u8 InitSequence[] =
 	{
-		SetDisplayOff24,
-		SetDisplayClockDivideRatio24,	0x80,				// Default value
-		SetMultiplexRatio24,		nMultiplexRatio24,		// Screen height - 1
-		SetDisplayOffset24,		0x00,				// None
-		SetStartLine24 | 0x00,						// Set start line
-		SetChargePump24,			0x14,				// Enable charge pump
-		SetMemoryAddressingMode24,	0x00,				// 00 = horizontal
-		nSegRemap24,
-		nCOMScanDir24,							// COM output scan direction
-		SetCOMPins24,			nCOMPins24,			// Alternate COM config and disable COM left/right
-		SetContrast24,			0x7F,				// 00-FF, default to half
-		SetPrechargePeriod24,		0x22,				// Default value
-		SetVCOMHDeselectLevel24,		0x20,				// Default value
-		EntireDisplayOnResume24,						// Resume to RAM content display
-		SetNormalDisplay24,
-		SetDisplayOn24,
-		SetColumnAddress24,		0x00,	nColumnAddrRange24,
-		SetPageAddress24,			0x00,	nPageAddrRange24,
+		SetDisplayOff,
+		SetDisplayClockDivideRatio,	0x80,
+		SetMultiplexRatio,		nMultiplexRatio,
+		SetDisplayOffset,		0x00,
+		SetStartLine | 0x00,
+		SetChargePump,			0x14,
+		SetMemoryAddressingMode,	0x00,
+		nSegRemap,
+		nCOMScanDir,
+		SetCOMPins,			nCOMPins,
+		SetContrast,			0x7F,
+		SetPrechargePeriod,		0x22,
+		SetVCOMHDeselectLevel,		0x20,
+		EntireDisplayOnResume,
+		SetNormalDisplay,
+		SetDisplayOn,
+		SetColumnAddress,		0x00,	nColumnAddrRange,
+		SetPageAddress,			0x00,	nPageAddrRange,
 	};
 
-	for (u8 nCommand24 : InitSequence24)
-		WriteCommand24(nCommand24);
+	for (u8 nCommand : InitSequence)
+		WriteCommand24(nCommand);
 
 	return CCharDevice::Initialize();
 }
 
-void CSSD1306Device24::DevClearCursor24 (void)
+void CSSD1306Device24::DevClearCursor (void)
 {
-	// Just clear the display
 	Clear24(TRUE);
 }
 
-void CSSD1306Device24::DevSetCursorMode24 (boolean bVisible)
+void CSSD1306Device24::DevSetCursorMode (boolean bVisible)
 {
 }
 
-void CSSD1306Device24::DevSetChar24 (unsigned nPosX, unsigned nPosY, char chChar)
+void CSSD1306Device24::DevSetChar (unsigned nPosX, unsigned nPosY, char chChar)
 {
-	// Draw a non-inverted, non-double width character
 	DrawChar24 (chChar, nPosX, nPosY, FALSE, FALSE);
 }
 
-void CSSD1306Device24::DevSetCursor24 (unsigned nCursorX, unsigned nCursorY)
+void CSSD1306Device24::DevSetCursor (unsigned nCursorX, unsigned nCursorY)
 {
 }
 
-void CSSD1306Device24::DevUpdateDisplay24 (void) {
+void CSSD1306Device24::DevUpdateDisplay (void) {
 	WriteFrameBuffer24(true);
 }
 
-void CSSD1306Device24::WriteCommand24(u8 nCommand24) const
+void CSSD1306Device24::WriteCommand24(u8 nCommand) const
 {
-	const u8 Buffer24[] = { 0x80, nCommand24 };
-	m_pI2CMaster->Write(m_nAddress, Buffer24, sizeof(Buffer24));
+	const u8 Buffer[] = { 0x80, nCommand };
+	m_pI2CMaster->Write(m_nAddress, Buffer, sizeof(Buffer));
 }
 
 void CSSD1306Device24::WriteFrameBuffer24(bool bForceFullUpdate) const
 {
-	// Reset start line
-	WriteCommand24(SetStartLine24 | 0x00);
+	WriteCommand24(SetStartLine | 0x00);
 
-	// Compare two framebuffers
 	const size_t nFrameBufferSize = m_nWidth * m_nHeight / 8;
 	const bool bNeedsUpdate = bForceFullUpdate || memcmp(m_FrameBuffers[0].FrameBuffer, m_FrameBuffers[1].FrameBuffer, nFrameBufferSize) != 0;
 
-	// Copy entire framebuffer
 	if (bNeedsUpdate)
 		m_pI2CMaster->Write(m_nAddress, &m_FrameBuffers[m_nCurrentFrameBuffer], sizeof(TFrameBufferUpdatePacket24::DataControlByte) + nFrameBufferSize);
 }
 
 void CSSD1306Device24::SwapFrameBuffers24()
 {
-	// Make other framebuffer current
 	m_nCurrentFrameBuffer = (m_nCurrentFrameBuffer + 1) % 2;
 }
 
 void CSSD1306Device24::DrawChar24(char chChar, u8 nCursorX, u8 nCursorY, bool bInverted, bool bDoubleWidth)
 {
 	const size_t nRowOffset    = nCursorY * m_nWidth;
-	const size_t nColumnOffset = nCursorX * (bDoubleWidth ? 10 : 5);  // 5 or 10 pixels per character
+	const size_t nColumnOffset = nCursorX * (bDoubleWidth ? 10 : 5);
 	u8* pFrameBuffer           = m_FrameBuffers[m_nCurrentFrameBuffer].FrameBuffer;
 
-	// Handle character range
 	if (chChar < ' ')
 		chChar = ' ';
 
 	size_t charIndex = static_cast<u8>(chChar - ' ');
-	if (charIndex >= FONT_SIZE)
-		charIndex = 0;  // Use first character for out of range
+	if (charIndex >= FONT5X8_SIZE)
+		charIndex = 0;
 
-	for (u8 i = 0; i < 5; ++i)  // 5 columns for 5x8 font
+	for (u8 i = 0; i < 5; ++i)
 	{
-		u16 nFontColumn = FontDouble24[charIndex][i];
+		u8 nFontColumn = FontSingle[charIndex][i];
 
-		// Don't invert the leftmost column or last two rows
 		if (i > 0 && bInverted)
-			nFontColumn ^= 0x3FFF;
+			nFontColumn ^= 0xFF;
 
-		// Shift down by 2 pixels (as in original)
-		nFontColumn <<= 2;
-
-		// Upper half of font
 		const size_t nOffset = nRowOffset + nColumnOffset + (bDoubleWidth ? i * 2 : i);
 
 		if (nOffset < sizeof(m_FrameBuffers[0].FrameBuffer))
-			pFrameBuffer[nOffset] = nFontColumn & 0xFF;
-		if (nOffset + m_nWidth < sizeof(m_FrameBuffers[0].FrameBuffer))
-			pFrameBuffer[nOffset + m_nWidth] = (nFontColumn >> 8) & 0xFF;
+			pFrameBuffer[nOffset] = nFontColumn;
 			
-		if (bDoubleWidth)
-		{
-			if (nOffset + 1 < sizeof(m_FrameBuffers[0].FrameBuffer))
-				pFrameBuffer[nOffset + 1] = pFrameBuffer[nOffset];
-			if (nOffset + m_nWidth + 1 < sizeof(m_FrameBuffers[0].FrameBuffer))
-				pFrameBuffer[nOffset + m_nWidth + 1] = pFrameBuffer[nOffset + m_nWidth];
-		}
+		if (bDoubleWidth && nOffset + 1 < sizeof(m_FrameBuffers[0].FrameBuffer))
+			pFrameBuffer[nOffset + 1] = nFontColumn;
 	}
 }
 
@@ -311,7 +249,5 @@ void CSSD1306Device24::Clear24(bool bImmediate)
 void CSSD1306Device24::SetBacklightState24(bool bEnabled)
 {
 	m_bBacklightEnabled = bEnabled;
-
-	// Power on/off display
-	WriteCommand24(bEnabled ? SetDisplayOn24 : SetDisplayOff24);
-}
\ No newline at end of file
+	WriteCommand24(bEnabled ? SetDisplayOn : SetDisplayOff);
+}
diff --git a/src/drivers/ssd1306device24.h b/src/drivers/ssd1306device24.h
index 232134e..92ecb2c 100644
--- a/src/drivers/ssd1306device24.h
+++ b/src/drivers/ssd1306device24.h
@@ -5,11 +5,11 @@
 #include <circle/i2cmaster.h>
 #include <circle/spinlock.h>
 #include <circle/types.h>
-#include "chardevice.h"
+#include <display/chardevice.h>
 
 // Hard-codes how the text maps to the graphics display
 #define SSD1306_COLUMNS24	25	// 128/5 = 25.6 (25 characters)
-#define SSD1306_ROWS24	8	// 64/8 = 8
+#define SSD1306_ROWS24	4	// 64/8 = 8
 
 class CSSD1306Device24 : public CCharDevice	/// LCD dot-matrix display driver (using SSD1306 controller)
 {
@@ -29,17 +29,18 @@ public:
 	boolean Initialize24 (void);
 
 private:
-	void DevClearCursor24 (void) override;
-	void DevSetCursor24 (unsigned nCursorX, unsigned nCursorY) override;
-	void DevSetCursorMode24 (boolean bVisible) override;
-	void DevSetChar24 (unsigned nPosX, unsigned nPosY, char chChar) override;
-	void DevUpdateDisplay24 (void) override;
+	// Virtual methods that must override base class methods (no suffix to maintain override compatibility)
+	void DevClearCursor (void) override;
+	void DevSetCursor (unsigned nCursorX, unsigned nCursorY) override;
+	void DevSetCursorMode (boolean bVisible) override;
+	void DevSetChar (unsigned nPosX, unsigned nPosY, char chChar) override;
+	void DevUpdateDisplay (void) override;
 
-	// Character functions
+	// Character functions with 24 suffix
 	void Clear24(bool bImmediate = false);
 	void Print24(const char* pText, u8 nCursorX, u8 nCursorY, bool bClearLine = false, bool bImmediate = false);
 
-	// Graphics functions
+	// Graphics functions with 24 suffix
 	void DrawChar24(char chChar, u8 nCursorX, u8 nCursorY, bool bInverted = false, bool bDoubleWidth = false);
 	void Flip24();
 
@@ -54,6 +55,7 @@ private:
     bool m_bRotated;
     bool m_bMirrored;
 
+	// Frame buffer structure with 24 suffix
 	struct TFrameBufferUpdatePacket24
 	{
 		u8 DataControlByte;
@@ -61,6 +63,7 @@ private:
 	}
 	PACKED;
 
+	// Helper methods with 24 suffix
 	void WriteCommand24(u8 nCommand) const;
 	void WriteFrameBuffer24(bool bForceFullUpdate = false) const;
 	void SwapFrameBuffers24();
diff --git a/src/emulator/mcu_opcodes.cpp b/src/emulator/mcu_opcodes.cpp
index f961cd1..ef22d2b 100644
--- a/src/emulator/mcu_opcodes.cpp
+++ b/src/emulator/mcu_opcodes.cpp
@@ -595,7 +595,7 @@ void MCU_Operand_General(MCU *mcu, uint8_t operand)
     uint32_t type = GENERAL_DIRECT;
     uint32_t disp = 0;
     uint32_t increase = INCREASE_NONE;
-    uint32_t absolute = 0;
+    //uint32_t absolute = 0;
     uint32_t reg = 0;
     uint32_t siz = OPERAND_BYTE;
     uint32_t data = 0;
diff --git a/src/minijv880.cpp b/src/minijv880.cpp
index b28935c..32d7700 100644
--- a/src/minijv880.cpp
+++ b/src/minijv880.cpp
@@ -311,7 +311,7 @@ void CMiniJV880::Run(unsigned nCore) {
     } 
     else if (nCore == 2) { // 2nd core - MCU + audio output
         const int MCU_INSTR_BURST = 64;
-        unsigned log_counter = 0;
+        //unsigned log_counter = 0;
         while (true) {
             unsigned nFrames = m_nQueueSizeFrames - m_pSoundDevice->GetQueueFramesAvail();
             if (nFrames < m_nQueueSizeFrames / 2) {
@@ -380,11 +380,11 @@ void CMiniJV880::Run(unsigned nCore) {
         constexpr uint64_t MCU_CLOCK_HZ = 12000000ull; // if your MCU clock differs, set accordingly
         constexpr uint32_t AUDIO_RATE = 32000u;
         constexpr uint64_t CYCLES_PER_SAMPLE = MCU_CLOCK_HZ / AUDIO_RATE; // 375 typical for H8@12MHz
-        constexpr uint64_t CYCLES_PER_SAMPLE_FP = CYCLES_PER_SAMPLE << 32; // fixed-point
+        //constexpr uint64_t CYCLES_PER_SAMPLE_FP = CYCLES_PER_SAMPLE << 32; // fixed-point
         const uint32_t MAX_SAMPLES_PER_ITER = 128; // bound to avoid huge bursts
 
         uint64_t last_generated_cycles = __atomic_load_n(&mcu.mcu.cycles, __ATOMIC_RELAXED);
-        uint64_t cycles_acc_fp = 0; // optional accumulator if needed by PCM internals
+        //uint64_t cycles_acc_fp = 0; // optional accumulator if needed by PCM internals
 
         while (true) {
             uint64_t cycles_target = __atomic_load_n(&mcu.mcu.cycles, __ATOMIC_ACQUIRE);
diff --git a/src/userinterface.cpp b/src/userinterface.cpp
index 2192276..2dc5d4b 100644
--- a/src/userinterface.cpp
+++ b/src/userinterface.cpp
@@ -152,160 +152,19 @@ void CUserInterface::Process (void)
 	{
 		m_pUIButtons->Update();
 	}
-	// for (size_t y = 0; y < 32; y++) {
-  //     for (size_t x = 0; x < 128; x++) {
-  //       int destX = (int)(((float)x / 128) * 820);
-  //       int destY = (int)(((float)y / 32) * 100);
-  //       int sum = 0;
-  //       for (int py = -1; py <= 1; py++) {
-  //         for (int px = -1; px <= 1; px++) {
-  //           if ((destY + py) >= 0 && (destX + px) >= 0) {
-  //             bool pixel = m_pMiniJV880->mcu.lcd.lcd_buffer[destY + py][destX + px] == lcd_col1;
-  //             sum += pixel;
-  //           }
-  //         }
-  //       }
-
-  //       bool pixel = sum > 0;
-  //       // bool pixel = mcu.lcd.lcd_buffer[destY][destX] == lcd_col1;
-  //       set_pixel(screen_buffer, x, y, pixel);
-	//
-  //       // m_ScreenUnbuffered->SetPixel(x + 800, y + 300, pixel ? 0xFFFF : 0x0000);
-  //     }
-  //   }
-
-  // Test code
-/*int displayCols = m_pConfig->GetLCDColumns();
-CString Msg("\x1B[H\x1B[?25l");
-
-for (int i = 0; i < 2; i++)
-{
-    // Read full 40-character line from buffer
-    std::string line;
-    for (int j = 0; j < 40; j++) {
-        uint8_t ch = m_pMiniJV880->mcu.lcd.LCD_Data[i * 40 + j];
-        line.push_back(ch);
-    }
-
-    // Gently remove spaces one by one, checking length after each removal
-    while (static_cast<int>(line.size()) > displayCols) {
-        bool spaceRemoved = false;
-        
-        // 1. Try to remove one space from double spaces
-        size_t pos = line.find("  ");
-        if (pos != std::string::npos) {
-            line.erase(pos, 1); // remove only one space
-            spaceRemoved = true;
-            continue; // IMMEDIATELY check length again
-        }
-        
-        // 2. If no double spaces, try removing single spaces
-        if (!spaceRemoved) {
-            pos = line.find(' ');
-            if (pos != std::string::npos) {
-                line.erase(pos, 1); // remove one single space
-                spaceRemoved = true;
-                continue; // IMMEDIATELY check length again
-            }
-        }
-        
-        // 3. If no spaces found at all - truncate
-        if (!spaceRemoved) {
-            line.resize(displayCols);
-            break;
-        }
-    }
-
-    // Draw line
-    for (int j = 0; j < displayCols; j++)
-    {
-        char ch = (j < static_cast<int>(line.size())) ? line[static_cast<size_t>(j)] : ' ';
-        
-        // Calculate cursor position in original buffer
-        int cursor_pos = m_pMiniJV880->mcu.lcd.LCD_DD_RAM;
-        int cursor_line = cursor_pos / 40;
-        int cursor_col = cursor_pos % 40;
-
-        // Check if cursor should be displayed at this position
-        bool show_cursor = (i == cursor_line && j == cursor_col && 
-                           cursor_col < displayCols && 
-                           m_pMiniJV880->mcu.lcd.LCD_C);
-
-        if (show_cursor) {
-            Msg.Append("_"); // cursor
-        } else {
-            Msg.Append(std::string(1, ch).c_str());
-        }
-    }
-    
-    // Add newline between lines
-    if (i == 0) {
-        Msg.Append("\n");
-    }
-}
-
-LCDWrite(Msg);*/
-
-//MY ROUTINE
-  /*
-	int displayCols = m_pConfig->GetLCDColumns();
-	CString Msg ("\x1B[H\E[?25l");
-	for (int i = 0; i < 2; i++)
-	{
-		// Standard line 24
-		std::string line;
-		line.reserve(ACTUAL_COLS);
-		for (int j = 0; j < ACTUAL_COLS; j++) {
-			uint8_t ch = m_pMiniJV880->mcu.lcd.LCD_Data[i * 40 + j];
-			line.push_back(ch);
-		}
-
-		// Stretch > 20
-		while ((int)line.size() > displayCols) {
-			// 1. double spaces
-			size_t pos = line.find("  ");
-			if (pos != std::string::npos) {
-				line.erase(pos, 1); // one space
-				continue;
-			}
-			// 2. one spaces
-			pos = line.find(' ');
-			if (pos != std::string::npos) {
-				line.erase(pos, 1);
-				continue;
-			}
-			// 3. If no spaces - cut
-			line.resize(displayCols);
-		}
-
-		// Draw
-		for (int j = 0; j < displayCols; j++)
-		{
-			char ch = (j < (int)line.size()) ? line[j] : ' ';
-			int jj = m_pMiniJV880->mcu.lcd.LCD_DD_RAM % 0x40;
-			int ii = m_pMiniJV880->mcu.lcd.LCD_DD_RAM / 0x40;
-
-			if (i == ii && j == jj && ii < 2 && jj < ACTUAL_COLS && m_pMiniJV880->mcu.lcd.LCD_C) {
-				// cursor
-				Msg.Append("_");
-			} else {
-				Msg.Append(std::string(1, ch).c_str());
-			}
-		}
-	}
 
-	LCDWrite(Msg);*/
 
-
-// SCROLL ROUTINE
-int displayCols = m_pConfig->GetLCDColumns(); // 24
-CString Msg("\x1B[H\E[?25l"); // clear screen + hide cursor
+// Universal display function - sync scrolling for all rows
+int displayCols = m_pConfig->GetLCDColumns();
+int displayRows = m_pConfig->GetLCDRows();
+CString Msg("\x1B[H\x1B[?25l"); // clear screen + hide cursor
 
 unsigned long currentTime = CTimer::GetClockTicks();
 
-// Compute actual length of each row
-int rowLen[2];
-for (int r = 0; r < 2; r++) {
+// Compute actual length of each row and find maximum
+int rowLen[8] = {0};
+int maxRowLen = 0;
+for (int r = 0; r < displayRows && r < 8; r++) {
     rowLen[r] = 0;
     for (int c = ACTUAL_COLS - 1; c >= 0; c--) {
         if (m_pMiniJV880->mcu.lcd.LCD_Data[r * 40 + c] != ' ') {
@@ -313,45 +172,55 @@ for (int r = 0; r < 2; r++) {
             break;
         }
     }
+    if (rowLen[r] > maxRowLen) {
+        maxRowLen = rowLen[r];
+    }
 }
 
-// Scroll logic
-for (int r = 0; r < 2; r++) {
-    if (rowLen[r] > displayCols) {
-        if (currentTime - m_lastScrollTime >= SCROLL_INTERVAL) {
-            m_scrollPosition[r] += m_scrollDir[r];
-            if (m_scrollPosition[r] <= 0) {
-                m_scrollPosition[r] = 0;
-                m_scrollDir[r] = +1;
-            } else if (m_scrollPosition[r] >= rowLen[r] - displayCols) {
-                m_scrollPosition[r] = rowLen[r] - displayCols;
-                m_scrollDir[r] = -1;
-            }
+// Unified scroll logic - only scroll if any row needs it
+static int unifiedScrollPos = 0;
+static int unifiedScrollDir = 1;
+
+if (maxRowLen > displayCols) {  // Only scroll if content is wider than display
+    if (currentTime - m_lastScrollTime >= SCROLL_INTERVAL) {
+        unifiedScrollPos += unifiedScrollDir;
+        if (unifiedScrollPos <= 0) {
+            unifiedScrollPos = 0;
+            unifiedScrollDir = +1;
+        } else if (unifiedScrollPos >= maxRowLen - displayCols) {
+            unifiedScrollPos = maxRowLen - displayCols;
+            unifiedScrollDir = -1;
         }
-    } else {
-        m_scrollPosition[r] = 0; // no scroll if text fits
     }
+} else {
+    unifiedScrollPos = 0;  // No scrolling needed
 }
 if (currentTime - m_lastScrollTime >= SCROLL_INTERVAL) m_lastScrollTime = currentTime;
 
-// Render display
-for (int row = 0; row < 2; row++) {
-    int startPos = m_scrollPosition[row];
+// Render display for all rows with unified scrolling
+for (int row = 0; row < displayRows && row < 8; row++) {
+    int startPos = unifiedScrollPos; // Use unified scroll position
+
+	if (rowLen[row] == 0) continue;
+    
+    // Get cursor information
     int cursorRow = m_pMiniJV880->mcu.lcd.LCD_DD_RAM / 0x40;
     int cursorCol = m_pMiniJV880->mcu.lcd.LCD_DD_RAM % 0x40;
     bool cursorEnabled = m_pMiniJV880->mcu.lcd.LCD_C != 0;
 
+    if (cursorRow >= displayRows) {
+        cursorRow = 0;
+    }
+
     for (int col = 0; col < displayCols; col++) {
         int sourcePos = col + startPos;
         uint8_t ch = (sourcePos < ACTUAL_COLS) ? m_pMiniJV880->mcu.lcd.LCD_Data[row * 40 + sourcePos] : ' ';
 
-        // replace   characters
-        if (ch == 0x09) ch = 0x7C; // vertical bar
+        if (ch == 0x09) ch = 0x7C;
         else if (ch < 32 || ch > 126) ch = ' ';
 
-        // cursor as space
-        if (cursorEnabled && row == cursorRow && col == cursorCol - startPos && col >= 0 && col < displayCols) {
-            Msg.Append(" ");
+        if (cursorEnabled && row == cursorRow && sourcePos == cursorCol) {
+            Msg.Append("_");
         } else {
             char buf[2] = { (char)ch, 0 };
             Msg.Append(buf);
@@ -362,99 +231,59 @@ for (int row = 0; row < 2; row++) {
 LCDWrite(Msg);
 
 
-
-/*
-    int displayCols = m_pConfig->GetLCDColumns();
-    CString Msg ("\x1B[H\E[?25l");
-
-    unsigned long currentTime = CTimer::GetClockTicks();
-
-    // Need Scroll?
-    bool needScroll = false;
-    if (ACTUAL_COLS > displayCols) {
-        bool allSpaces = true;
-        for (int r = 0; r < 2; r++) {
-            for (int c = displayCols; c < ACTUAL_COLS; c++) {
-                if (m_pMiniJV880->mcu.lcd.LCD_Data[r * 40 + c] != ' ') {
-                    allSpaces = false;
-                    break;
-                }
-            }
-            if (!allSpaces) break;
-        }
-        needScroll = !allSpaces;
-    }
-
-    // Scroll logic
-    if (needScroll && (currentTime - m_lastScrollTime >= SCROLL_INTERVAL)) {
-        for (int r = 0; r < 2; r++) {
-            m_scrollPosition[r] += m_scrollDir[r];
-            if (m_scrollPosition[r] <= 0) {
-                m_scrollPosition[r] = 0;
-                m_scrollDir[r] = +1;
-            } else if (m_scrollPosition[r] >= ACTUAL_COLS - displayCols) {
-                m_scrollPosition[r] = ACTUAL_COLS - displayCols;
-                m_scrollDir[r] = -1;
-            }
-        }
-        m_lastScrollTime = currentTime;
-    }
-
-    // Strings
-    for (int i = 0; i < 2; i++) {
-        int startPos = needScroll ? m_scrollPosition[i] : 0;
-
-        // cursor
-        int cursorRow = m_pMiniJV880->mcu.lcd.LCD_DD_RAM / 0x40;
-        int cursorCol = m_pMiniJV880->mcu.lcd.LCD_DD_RAM % 0x40;
-        bool cursorEnabled = m_pMiniJV880->mcu.lcd.LCD_C != 0;
-
-        for (int j = 0; j < displayCols; j++) {
-            int sourcePos = (j + startPos) % ACTUAL_COLS;
-            uint8_t ch = m_pMiniJV880->mcu.lcd.LCD_Data[i * 40 + sourcePos];
-
-            // cursor in window?
-            bool cursorHere = false;
-            if (cursorEnabled && cursorRow == i) {
-                int rel = cursorCol - startPos;
-                if (rel < 0) rel += ACTUAL_COLS;
-                if (rel >= 0 && rel < displayCols && j == rel) {
-                    cursorHere = true;
-                }
-            }
-
-            if (cursorHere) {
-                Msg.Append("_");
-            } else {
-                char buf[2] = { (char)ch, 0 };
-                Msg.Append(buf);
-            }
-        }
-    }
-
-    LCDWrite(Msg);
-*/
-
 }
 
 bool CUserInterface::LCDInit()
 {
-if (m_pConfig->GetLCDEnabled ())
+	if (m_pConfig->GetLCDEnabled ())
 	{
 		unsigned i2caddr = m_pConfig->GetLCDI2CAddress ();
 		unsigned ssd1306addr = m_pConfig->GetSSD1306LCDI2CAddress ();
 		bool st7789 = m_pConfig->GetST7789Enabled ();
+		unsigned lcdColumns = m_pConfig->GetLCDColumns(); // Get number of columns
+		
 		if (ssd1306addr != 0) {
-			m_pSSD1306 = new CSSD1306Device (m_pConfig->GetSSD1306LCDWidth (), m_pConfig->GetSSD1306LCDHeight (),
-											 m_pI2CMaster, ssd1306addr,
-											 m_pConfig->GetSSD1306LCDRotate (), m_pConfig->GetSSD1306LCDMirror ());
-			if (!m_pSSD1306->Initialize ())
-			{
-				LOGDBG("LCD: SSD1306 initialization failed");
-				return false;
+			if (lcdColumns >= 24) {
+				// Use 24-driver (5x8 font) for 24 columns
+				CSSD1306Device24* pSSD1306Device24 = new CSSD1306Device24 (
+					m_pConfig->GetSSD1306LCDWidth (), 
+					m_pConfig->GetSSD1306LCDHeight (),
+					m_pI2CMaster, 
+					ssd1306addr,
+					m_pConfig->GetSSD1306LCDRotate (), 
+					m_pConfig->GetSSD1306LCDMirror ()
+				);
+				
+				if (!pSSD1306Device24->Initialize24 ())
+				{
+					LOGDBG("LCD: SSD1306 24 initialization failed");
+					delete pSSD1306Device24;
+					return false;
+				}
+				
+				LOGDBG ("LCD: SSD1306 24 (5x8 font, 24 columns)");
+				m_pLCD = pSSD1306Device24; // Assign directly to m_pLCD
+				// m_pSSD1306 remains NULL
+			} else {
+				// Use original driver (6x8 font) for 20 columns
+				m_pSSD1306 = new CSSD1306Device (
+					m_pConfig->GetSSD1306LCDWidth (), 
+					m_pConfig->GetSSD1306LCDHeight (),
+					m_pI2CMaster, 
+					ssd1306addr,
+					m_pConfig->GetSSD1306LCDRotate (), 
+					m_pConfig->GetSSD1306LCDMirror ()
+				);
+				
+				if (!m_pSSD1306->Initialize ())
+				{
+					LOGDBG("LCD: SSD1306 initialization failed");
+					return false;
+				}
+				
+				LOGDBG ("LCD: SSD1306 (6x8 font, 20 columns)");
+				m_pLCD = m_pSSD1306;
 			}
-			LOGDBG ("LCD: SSD1306");
-			m_pLCD = m_pSSD1306;
 		}
 		else if (st7789)
 		{
@@ -535,13 +364,16 @@ if (m_pConfig->GetLCDEnabled ())
 			LOGDBG ("LCD: HD44780 I2C");
 			m_pLCD = m_pHD44780;
 		}
-		assert (m_pLCD);
+		if (!m_pLCD) {
+			LOGDBG("LCD: No display device initialized");
+			return false;
+		}
 
 		m_pLCDBuffered = new CWriteBufferDevice (m_pLCD);
 		assert (m_pLCDBuffered);
 
 		LCDWrite ("\x1B[?25l\x1B""d+");		// cursor off, autopage mode
-		LCDWrite ("Start MiniJV880\n");
+		LCDWrite ("Start MiniJV880pi\n");
 		LCDWrite ("version ");
 		LCDWrite (VERSION_SHORT);
 		m_pLCDBuffered->Update ();
diff --git a/src/userinterface.h b/src/userinterface.h
index c3bd840..e1ed765 100644
--- a/src/userinterface.h
+++ b/src/userinterface.h
@@ -25,6 +25,7 @@
 #include <sensor/ky040.h>
 #include <display/hd44780device.h>
 #include <display/ssd1306device.h>
+#include "drivers/ssd1306device24.h"
 #include <display/st7789device.h>
 #include <circle/gpiomanager.h>
 #include <circle/writebuffer.h>
@@ -100,8 +101,8 @@ private:
 	bool isPaused[2] = {true, true};  // Start paused
 	bool isAtEnd[2] = {false, false};
 	unsigned long pauseStartTime[2] = {0, 0};
-	static const unsigned long SCROLL_INTERVAL = 500000;
-	static const unsigned long PAUSE_DURATION = 1000000;
+	static const unsigned long SCROLL_INTERVAL = 1000000;
+	static const unsigned long PAUSE_DURATION = 1500000;
 	static const int ACTUAL_COLS = 24;
 
 	
diff --git a/src/version.h b/src/version.h
index 950c7f3..431d3a4 100644
--- a/src/version.h
+++ b/src/version.h
@@ -1,8 +1,8 @@
 #ifndef VERSION_H
 #define VERSION_H
 
-#define VERSION_STRING "v1.0.5-9-g5af12e8-dirty"
-#define VERSION_SHORT "v1.0.5"  
+#define VERSION_STRING "v1.0.6-2-gdd212dc-dirty"
+#define VERSION_SHORT "v1.0.6"  
 #define VERSION_IS_DIRTY "" == "dirty" || echo "clean"
 
 #endif // VERSION_H
