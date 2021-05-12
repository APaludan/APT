# APT
## SÅDAN KØRER DU DET!!!!
### Tx (Transmitter)
 - udpak ffmpeg.exe til Tx-mappen
new: 
 - compile gcc createSoundFromImage.c
 - kør dette: a.exe -webcam 0/1
      -> der skal stå et 0 hvis man ik vil tage billede og 1 hvis man gerne vil 
         (webcam skal ændres afhængigt af computer, det sker inde i createSound.. i capture funktionen)

(old:
 - måske ændre om der er webcam eller ej og webcam navn i main.c
 - compile og kør main.exe)

### Rx (Receiver)
 - åbn hele mappen i visual studio code
 - brug extension "live server" for at åbne det som en server (eller evt. lav vores egen server 🅱aseret på yatzy-server😤)
 - måske giver chrome ikke lov til at tilgå mic. bare tryk en masse gange på opdater så virker det (nogle gange)
 - åbn console for at se om det virker
