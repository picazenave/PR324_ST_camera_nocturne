# ESPCAM Serial

## Commands

| Hex  | Nargs | Description        | Description Args                        |
|------|-------|--------------------|-----------------------------------------|
| 0xAA | 0     | ACK                | Test if camera is ready (0 if OK)       |
| 0x55 | 0     | Get jpg            |                                         |
| 0x44 | 0     | Get jpg size       |                                         |
| 0x66 | 0     | Take picture       |                                         |
| 0xA1 | 0     | Start init config  |                                         |
| 0xA2 | 0     | End init config    |                                         |
| 0x34 | 1     | Set brightness     | -2 to 2                                 |
| 0x33 | 1     | set_special_effect |                                         |
| 0x22 | 1     | Set jpg quality    | 10-63 lower number means higher quality |
| 0x11 | 1     | Set frame_size     | ex :FRAMESIZE_VGA                       |

## How to use ?

Default is `serial_camera_config_t camera_config = {.brightness = 0, .special_effect = 0, .jpg_quality = 30, .frame_size = FRAMESIZE_UXGA};`

`Init`

- 0xA1 : Start config, will restart ESP if config was already done
- 0xAA : While no answer ESP isn't ready for Serial
- 0xA1 : Start config
- 0X34 : set brightness (default 0)
- 0x33 : set special effect (default 0)
- 0x22 : set jpg quality (default 30)
- 0x11 : set frame size (default FRAMESIZE_VGA)
- 0xA2 : end config and apply parameters to camera

`Usage`

- 0x55 : Get jpg size
- 0x55 : Get jpg data

`Change parameters`

- Same as init because 0xA1 will restart ESP
## Frame size

- FRAMESIZE_UXGA (1600 x 1200) = 0
- FRAMESIZE_QVGA (320 x 240) = 1
- FRAMESIZE_CIF (352 x 288) = 2
- FRAMESIZE_VGA (640 x 480) = 3
- FRAMESIZE_SVGA (800 x 600) = 4
- FRAMESIZE_XGA (1024 x 768) = 5
- FRAMESIZE_SXGA (1280 x 1024) = 6


## Special effects

- 0 – No Effect
- 1 – Negative
- 2 – Grayscale
- 3 – Red Tint
- 4 – Green Tint
- 5 – Blue Tint
- 6 – Sepia