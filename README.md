# Labo beeldinterpretatie 2018
## Sessie 0

### labo1-sessie0


| parameter | doel |
| -------- | -------- |
| -h | help |
| -ig | required! locatie van image in grijswaarden |
| -ic | required! locatie van image in kleur |

de applicatie gaat door alle stappen van de gevraagde opgave.
om naar het volgend scherm te gaan, gelieve op eender welke knop (behalve de powerknop) te drukken.

***

## Sessie 1

| Command | Action |
| ----- | ----- |
| (h)elp | Display help message |
| (t)hresholding| 1.1 Applies thresholding|
| (b)imodal | 1.1.2 Applies thresholding, OTSU Histogram equalisation and CHANE |
|(c)lean | 1.2 Use erosion and dilation to clean up picture, and find features (arms)|
| @image | (absolute) Path to image |



## usage:
  * in case of trouble: use absolute paths to images
  * to proceed to next window, press any button (except power)

  
### 1.1
```
./sessie1 -t /imageColor.jpg
```
### 1.1.2
```
./sessie1 -b /imageModal.jpg
```
### 1.2
```
./sessie1 -c /imageColorAdapted.jpg
```

***
