# PR324_ST_camera_nocturne

## Liens utiles

- https://github.com/STMicroelectronics/STM32CubeF4
- https://github.com/STMicroelectronics
- https://www.st.com/en/evaluation-tools/nucleo-f401re.html#documentation
- https://www.st.com/resource/en/datasheet/stm32f401re.pdf
- https://www.st.com/resource/en/reference_manual/rm0368-stm32f401xbc-and-stm32f401xde-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
- https://www.st.com/resource/en/user_manual/um1725-description-of-stm32f4-hal-and-lowlayer-drivers-stmicroelectronics.pdf

## Réu
- estimer la trajectoir de la bestiole
- tof traquer la bestiole pour capturer au centre/ le plus cadré possible
- capteur particulier de ST pour PIR
- 

## Utiliser stm32pio

Comment convertir cubeMX en platformio :
- faire les modifs dans le .ioc (projet cubeMX)
- enregistrer
- ouvrir une terminal dans le dossier du projet cubeMX
- faire `stm32pio generate`
