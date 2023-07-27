/**
 * @file camera_utils.h
 * @brief Contains all the function and variable definitions for the camera_utils functionalities.
 *
 * @author Ethan Jones
 * @date 2023-04-17
 */

#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    4
#define SIOD_GPIO_NUM    18
#define SIOC_GPIO_NUM    23
#define Y9_GPIO_NUM      36
#define Y8_GPIO_NUM      37
#define Y7_GPIO_NUM      38
#define Y6_GPIO_NUM      39
#define Y5_GPIO_NUM      35
#define Y4_GPIO_NUM      14
#define Y3_GPIO_NUM      13
#define Y2_GPIO_NUM      34
#define VSYNC_GPIO_NUM   5
#define HREF_GPIO_NUM    27
#define PCLK_GPIO_NUM    25

bool check_img( fs::FS &fs );
void cap_and_save();

#endif // CAMERA_UTILS_H
