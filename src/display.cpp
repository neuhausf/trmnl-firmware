#include <Arduino.h>
#include <PNG-flip.c>
#include <display.h>
#include <ArduinoLog.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <config.h>
#include <ImageData.h>

GxEPD2_BW<GxEPD2_750_T7,
GxEPD2_750_T7::HEIGHT> display(
GxEPD2_750_T7(EPD_CS_PIN,
      PIN_EPD_DC,
      EPD_RST_PIN,
      EPD_BUSY_PIN));

/**
 * @brief Function to init the display
 * @param none
 * @return none
 */
void display_init(void)
{
    Log.info("%s [%d]: dev module start\r\n", __FILE__, __LINE__);
    DEV_Module_Init();
    Log.info("%s [%d]: dev module end\r\n", __FILE__, __LINE__);

    Log.info("%s [%d]: screen hw start\r\n", __FILE__, __LINE__);

    #if FIREBEETLE
    pinMode(PIN_EPD_PWR, OUTPUT);
    digitalWrite(PIN_EPD_PWR, HIGH);
    
    display.init(115200, true, 10, false);

    display.setRotation(0);
    display.setTextSize(1);
    display.setTextColor(GxEPD_BLACK);
    display.setTextWrap(false);
    // display.fillScreen(GxEPD_WHITE);
    display.setFullWindow();
    display.firstPage(); // use paged drawing mode, sets fillScreen(GxEPD_WHITE)
    #else
    EPD_7IN5_V2_Init_New();
    #endif

    Log.info("%s [%d]: screen hw end\r\n", __FILE__, __LINE__);
}

/**
 * @brief Function to reset the display
 * @param none
 * @return none
 */
void display_reset(void)
{
    Log.info("%s [%d]: e-Paper Clear start\r\n", __FILE__, __LINE__);
    #if FIREBEETLE
    display.clearScreen();
    #else
    EPD_7IN5_V2_Clear();
    #endif
    Log.info("%s [%d]:  e-Paper Clear end\r\n", __FILE__, __LINE__);
    // DEV_Delay_ms(500);
}

/**
 * @brief Function to read the display height
 * @return uint16_t - height of display in pixels
 */
uint16_t display_height()
{
    return EPD_7IN5_V2_HEIGHT;
}

/**
 * @brief Function to read the display width
 * @return uint16_t - width of display in pixels
 */
uint16_t display_width()
{
    return EPD_7IN5_V2_WIDTH;
}

#ifdef FIREBEETLE
/**
 * @brief Function to show the image on the display
 * @param image_buffer pointer to the uint8_t image buffer
 * @param reverse shows if the color scheme is reverse
 * @return none
 */
void display_show_image(uint8_t *image_buffer, bool reverse, bool isPNG)
{
    auto width = display_width();
    auto height = display_height();
 
    Log.info("%s [%d]: show image for array\r\n", __FILE__, __LINE__);
    if (reverse)
    {
        Log.info("%s [%d]: inverse the image\r\n", __FILE__, __LINE__);
        for (size_t i = 0; i < DISPLAY_BMP_IMAGE_SIZE; i++)
        {
            image_buffer[i] = ~image_buffer[i];
        }
    }
    if(isPNG == true)
    {
        Log.info("Drawing PNG\n");
        flip_image(image_buffer, width, height);
        horizontal_mirror(image_buffer,width,height);
        display.drawImage(image_buffer, GxEPD_BLACK, 0, 0, width, height, reverse, true, false);
    }
    else{
        display.drawImage(image_buffer + 62, GxEPD_BLACK, 0, 0, width, height, reverse, true, false);
    }
    Log.info("%s [%d]: display\r\n", __FILE__, __LINE__);
}
/**
 * @brief Function to show the image with message on the display
 * @param image_buffer pointer to the uint8_t image buffer
 * @param message_type type of message that will show on the screen
 * @return none
 */
void display_show_msg(uint8_t *image_buffer, MSG message_type)
{
    auto width = display_width();
    auto height = display_height();

    display.drawImage(image_buffer + 62, GxEPD_BLACK, 0, 0, width, height, false, true, false);
    switch (message_type)
    {
    case WIFI_CONNECT:
    {
        char string1[] = "Connect to TRMNL WiFi";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
        drawString((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, CENTER, WHITE);
        char string2[] = "on your phone or computer";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 430, string2, &Font24, WHITE, BLACK);
    }
    break;
    case WIFI_FAILED:
    {
        char string1[] = "Can't establish WiFi";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 340, string1, &Font24, WHITE, BLACK);
        char string2[] = "connection. Hold button on";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 370, string2, &Font24, WHITE, BLACK);
        char string3[] = "the back to reset WiFi";
        Paint_DrawString_EN((800 - sizeof(string3) * 17 > 9) ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 400, string3, &Font24, WHITE, BLACK);
        char string4[] = "or scan QR Code for help.";
        Paint_DrawString_EN((800 - sizeof(string4) * 17 > 9) ? (800 - sizeof(string4) * 17) / 2 + 9 : 0, 430, string4, &Font24, WHITE, BLACK);

        Paint_DrawImage(wifi_failed_qr, 640, 337, 130, 130);
    }
    break;
    case WIFI_INTERNAL_ERROR:
    {
        char string1[] = "WiFi connected, but";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 340, string1, &Font24, WHITE, BLACK);
        char string2[] = "API connection cannot be";
        Paint_DrawString_EN((800 - sizeof(string2) > 9) * 17 ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 370, string2, &Font24, WHITE, BLACK);
        char string3[] = "established. Try to refresh,";
        Paint_DrawString_EN((800 - sizeof(string3) > 9) * 17 ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 400, string3, &Font24, WHITE, BLACK);
        char string4[] = "or scan QR Code for help.";
        Paint_DrawString_EN((800 - sizeof(string4) > 9) * 17 ? (800 - sizeof(string4) * 17) / 2 + 9 : 0, 430, string4, &Font24, WHITE, BLACK);

        Paint_DrawImage(wifi_failed_qr, 640, 337, 130, 130);
    }
    break;
    case WIFI_WEAK:
    {
        char string1[] = "WiFi connected but signal is weak";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case API_ERROR:
    {
        char string1[] = "WiFi connected, TRMNL not responding.";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 340, string1, &Font24, WHITE, BLACK);
        char string2[] = "Short click the button on back,";
        Paint_DrawString_EN((800 - sizeof(string2) > 9) * 17 ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 400, string2, &Font24, WHITE, BLACK);
        char string3[] = "otherwise check your internet.";
        Paint_DrawString_EN((800 - sizeof(string3) > 9) * 17 ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 430, string3, &Font24, WHITE, BLACK);
    }
    break;
    case API_SIZE_ERROR:
    {
        char string1[] = "WiFi connected, TRMNL content malformed.";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
        char string2[] = "Wait or reset by holding button on back.";
        Paint_DrawString_EN((800 - sizeof(string2) > 9) * 17 ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 430, string2, &Font24, WHITE, BLACK);
    }
    break;
    case FW_UPDATE:
    {
        char string1[] = "Firmware update available! Starting now...";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case FW_UPDATE_FAILED:
    {
        char string1[] = "Firmware update failed. Device will restart...";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case FW_UPDATE_SUCCESS:
    {
        char string1[] = "Firmware update success. Device will restart..";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case BMP_FORMAT_ERROR:
    {
        char string1[] = "The image format is incorrect";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case MAC_NOT_REGISTERED:
    {
        char string1[] = "MAC Address is not registered,";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 370, string1, &Font24, WHITE, BLACK);
        char string2[] = "so API is not available.";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 400, string2, &Font24, WHITE, BLACK);
        char string3[] = "Contact support for details.";
        Paint_DrawString_EN((800 - sizeof(string3) * 17 > 9) ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 430, string3, &Font24, WHITE, BLACK);
        break;
    }
    case TEST:
    {
        Paint_DrawString_EN(0, 0, "ABCDEFGHIYABCDEFGHIYABCDEFGHIYABCDEFGHIYABCDEFGHIY", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(0, 40, "abcdefghiyabcdefghiyabcdefghiyabcdefghiyabcdefghiy", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(0, 80, "A B C D E F G H I Y A B C D E F G H I Y A B C D E", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(0, 120, "a b c d e f g h i y a b c d e f g h i y a b c d e", &Font24, WHITE, BLACK);
    }
    break;
    default:
        break;
    }
}
#else
/**
 * @brief Function to show the image on the display
 * @param image_buffer pointer to the uint8_t image buffer
 * @param reverse shows if the color scheme is reverse
 * @return none
 */
void display_show_image(uint8_t *image_buffer, bool reverse, bool isPNG)
{
    auto width = display_width();
    auto height = display_height();
    //  Create a new image cache
    UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((width % 8 == 0) ? (width / 8) : (width / 8 + 1)) * height;
    
    Log.error("%s [%d]: free heap - %d\r\n", __FILE__, __LINE__, ESP.getFreeHeap());
    Log.error("%s [%d]: free alloc heap - %d\r\n", __FILE__, __LINE__, ESP.getMaxAllocHeap());
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        Log.fatal("%s [%d]: Failed to apply for black memory...\r\n", __FILE__, __LINE__);
        ESP.restart();
    }
    Log.info("%s [%d]: Paint_NewImage %d\r\n", __FILE__, __LINE__, reverse);
    
    Paint_NewImage(BlackImage, width, height, 0, WHITE);

    Log.info("%s [%d]: show image for array\r\n", __FILE__, __LINE__);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    if (reverse)
    {
        Log.info("%s [%d]: inverse the image\r\n", __FILE__, __LINE__);
        for (size_t i = 0; i < DISPLAY_BMP_IMAGE_SIZE; i++)
        {
            image_buffer[i] = ~image_buffer[i];
        }
    }
    if(isPNG == true)
    {
        Log.info("Drawing PNG\n");
        flip_image(image_buffer, width, height);
        horizontal_mirror(image_buffer,width,height);
        Paint_DrawBitMap(image_buffer);
    }
    else{
        Paint_DrawBitMap(image_buffer + 62);
    }
    EPD_7IN5_V2_Display(BlackImage);
    Log.info("%s [%d]: display\r\n", __FILE__, __LINE__);

    free(BlackImage);
    BlackImage = NULL;
}
/**
 * @brief Function to show the image with message on the display
 * @param image_buffer pointer to the uint8_t image buffer
 * @param message_type type of message that will show on the screen
 * @return none
 */
void display_show_msg(uint8_t *image_buffer, MSG message_type)
{
    auto width = display_width();
    auto height = display_height();
    UBYTE *BlackImage;
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((width % 8 == 0) ? (width / 8) : (width / 8 + 1)) * height;
    Log.error("%s [%d]: free heap - %d\r\n", __FILE__, __LINE__, ESP.getFreeHeap());
    Log.error("%s [%d]: free alloc heap - %d\r\n", __FILE__, __LINE__, ESP.getMaxAllocHeap());
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        Log.fatal("%s [%d]: Failed to apply for black memory...\r\n", __FILE__, __LINE__);
        ESP.restart();
    }

    Log.info("%s [%d]: Paint_NewImage\r\n", __FILE__, __LINE__);
    Paint_NewImage(BlackImage, width, height, 0, WHITE);

    Log.info("%s [%d]: show image for array\r\n", __FILE__, __LINE__);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawBitMap(image_buffer + 62);
    switch (message_type)
    {
    case WIFI_CONNECT:
    {
        char string1[] = "Connect to TRMNL WiFi";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
        char string2[] = "on your phone or computer";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 430, string2, &Font24, WHITE, BLACK);
    }
    break;
    case WIFI_FAILED:
    {
        char string1[] = "Can't establish WiFi";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 340, string1, &Font24, WHITE, BLACK);
        char string2[] = "connection. Hold button on";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 370, string2, &Font24, WHITE, BLACK);
        char string3[] = "the back to reset WiFi";
        Paint_DrawString_EN((800 - sizeof(string3) * 17 > 9) ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 400, string3, &Font24, WHITE, BLACK);
        char string4[] = "or scan QR Code for help.";
        Paint_DrawString_EN((800 - sizeof(string4) * 17 > 9) ? (800 - sizeof(string4) * 17) / 2 + 9 : 0, 430, string4, &Font24, WHITE, BLACK);

        Paint_DrawImage(wifi_failed_qr, 640, 337, 130, 130);
    }
    break;
    case WIFI_INTERNAL_ERROR:
    {
        char string1[] = "WiFi connected, but";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 340, string1, &Font24, WHITE, BLACK);
        char string2[] = "API connection cannot be";
        Paint_DrawString_EN((800 - sizeof(string2) > 9) * 17 ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 370, string2, &Font24, WHITE, BLACK);
        char string3[] = "established. Try to refresh,";
        Paint_DrawString_EN((800 - sizeof(string3) > 9) * 17 ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 400, string3, &Font24, WHITE, BLACK);
        char string4[] = "or scan QR Code for help.";
        Paint_DrawString_EN((800 - sizeof(string4) > 9) * 17 ? (800 - sizeof(string4) * 17) / 2 + 9 : 0, 430, string4, &Font24, WHITE, BLACK);

        Paint_DrawImage(wifi_failed_qr, 640, 337, 130, 130);
    }
    break;
    case WIFI_WEAK:
    {
        char string1[] = "WiFi connected but signal is weak";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case API_ERROR:
    {
        char string1[] = "WiFi connected, TRMNL not responding.";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 340, string1, &Font24, WHITE, BLACK);
        char string2[] = "Short click the button on back,";
        Paint_DrawString_EN((800 - sizeof(string2) > 9) * 17 ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 400, string2, &Font24, WHITE, BLACK);
        char string3[] = "otherwise check your internet.";
        Paint_DrawString_EN((800 - sizeof(string3) > 9) * 17 ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 430, string3, &Font24, WHITE, BLACK);
    }
    break;
    case API_SIZE_ERROR:
    {
        char string1[] = "WiFi connected, TRMNL content malformed.";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
        char string2[] = "Wait or reset by holding button on back.";
        Paint_DrawString_EN((800 - sizeof(string2) > 9) * 17 ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 430, string2, &Font24, WHITE, BLACK);
    }
    break;
    case FW_UPDATE:
    {
        char string1[] = "Firmware update available! Starting now...";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case FW_UPDATE_FAILED:
    {
        char string1[] = "Firmware update failed. Device will restart...";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case FW_UPDATE_SUCCESS:
    {
        char string1[] = "Firmware update success. Device will restart..";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case BMP_FORMAT_ERROR:
    {
        char string1[] = "The image format is incorrect";
        Paint_DrawString_EN((800 - sizeof(string1) > 9) * 17 ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);
    }
    break;
    case MAC_NOT_REGISTERED:
    {
        char string1[] = "MAC Address is not registered,";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 370, string1, &Font24, WHITE, BLACK);
        char string2[] = "so API is not available.";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 400, string2, &Font24, WHITE, BLACK);
        char string3[] = "Contact support for details.";
        Paint_DrawString_EN((800 - sizeof(string3) * 17 > 9) ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 430, string3, &Font24, WHITE, BLACK);
        break;
    }
    case TEST:
    {
        Paint_DrawString_EN(0, 0, "ABCDEFGHIYABCDEFGHIYABCDEFGHIYABCDEFGHIYABCDEFGHIY", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(0, 40, "abcdefghiyabcdefghiyabcdefghiyabcdefghiyabcdefghiy", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(0, 80, "A B C D E F G H I Y A B C D E F G H I Y A B C D E", &Font24, WHITE, BLACK);
        Paint_DrawString_EN(0, 120, "a b c d e f g h i y a b c d e f g h i y a b c d e", &Font24, WHITE, BLACK);
    }
    break;
    default:
        break;
    }

    EPD_7IN5_V2_Display(BlackImage);
    Log.info("%s [%d]: display\r\n", __FILE__, __LINE__);
    free(BlackImage);
    BlackImage = NULL;
}
#endif

/**
 * @brief Function to show the image with message on the display
 * @param image_buffer pointer to the uint8_t image buffer
 * @param message_type type of message that will show on the screen
 * @param friendly_id device friendly ID
 * @param id shows if ID exists
 * @param fw_version version of the firmware
 * @param message additional message
 * @return none
 */
void display_show_msg(uint8_t *image_buffer, MSG message_type, String friendly_id, bool id, const char *fw_version, String message)
{
    if (message_type == WIFI_CONNECT)
    {
        Log.info("%s [%d]: Display set to white\r\n", __FILE__, __LINE__);
        display.clearScreen(GxEPD_WHITE);
        delay(1000);
    }

    auto width = display_width();
    auto height = display_height();
    display.drawImage(image_buffer + 62, GxEPD_BLACK, 0, 0, width, height, false, false, false);
    switch (message_type)
    {
    case FRIENDLY_ID:
    {
        Log.info("%s [%d]: friendly id case\r\n", __FILE__, __LINE__);
        char string1[] = "Please sign up at usetrmnl.com/signup";
        Paint_DrawString_EN((800 - sizeof(string1) * 17 > 9) ? (800 - sizeof(string1) * 17) / 2 + 9 : 0, 400, string1, &Font24, WHITE, BLACK);

        String string2 = "with Friendly ID ";
        if (id)
        {
            string2 += friendly_id;
        }
        string2 += " to finish setup";
        Paint_DrawString_EN((800 - string2.length() * 17 > 9) ? (800 - string2.length() * 17) / 2 + 9 : 0, 430, string2.c_str(), &Font24, WHITE, BLACK);
    }
    break;
    case WIFI_CONNECT:
    {
        Log.info("%s [%d]: wifi connect case\r\n", __FILE__, __LINE__);

        String string1 = "FW: ";
        string1 += fw_version;
        Paint_DrawString_EN((800 - string1.length() * 17 > 9) ? (800 - string1.length() * 17) / 2 + 9 : 0, 340, string1.c_str(), &Font24, WHITE, BLACK);
        char string2[] = "Connect phone or computer";
        Paint_DrawString_EN((800 - sizeof(string2) * 17 > 9) ? (800 - sizeof(string2) * 17) / 2 + 9 : 0, 370, string2, &Font24, WHITE, BLACK);
        char string3[] = "to \"TRMNL\" WiFi network";
        Paint_DrawString_EN((800 - sizeof(string3) * 17 > 9) ? (800 - sizeof(string3) * 17) / 2 + 9 : 0, 400, string3, &Font24, WHITE, BLACK);
        char string4[] = "or scan QR code for help.";
        Paint_DrawString_EN((800 - sizeof(string4) * 17 > 9) ? (800 - sizeof(string4) * 17) / 2 + 9 : 0, 430, string4, &Font24, WHITE, BLACK);

        Paint_DrawImage(wifi_connect_qr, 640, 337, 130, 130);
    }
    break;
    default:
        break;
    }
}

/**
 * @brief Function to got the display to the sleep
 * @param none
 * @return none
 */
void display_sleep(void)
{
    Log.info("%s [%d]: Goto Sleep...\r\n", __FILE__, __LINE__);
    EPD_7IN5B_V2_Sleep();
}

#if FIREBEETLE
/* Draws a string with alignment
 */
void drawString(int16_t x, int16_t y, const String &text, alignment_t alignment,
                uint16_t color)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.setTextColor(color);
    display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
    if (alignment == RIGHT)
    {
        x = x - w;
    }
    if (alignment == CENTER)
    {
        x = x - w / 2;
    }
    display.setCursor(x, y);
    display.print(text);
    return;
} // end drawString
#endif