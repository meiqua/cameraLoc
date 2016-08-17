# cameraLoc
## environment  
qt5-mingw32, opencv3.0  
## function  
recognize a QR code locator in an image, then tranport the center into world point, to locate the object the locators attach to  
## feature  
can deal with some perspective distortion and rotation, but too much is not guaranteed.  
## menthod
* the QR code locator is designed cleverly. Even when the locator is rotated, we can still get a horizental line bar and a vertical line bar, and each line in these two bars have the same ratio: 1black-1white-3black-1white-1black. center lines of two bar intersect at the locator center  
* when I test this property in practice, I find it hard to have a continous bar, or there exists other lines with correct ratio outside the bar. I write a filter for one dimention situation to solve this.
* also, I find it hard to have exact 1:1:3:1:1. I believe once the ratio error comparing to all found length(note to get more possible lines I compare all found) exceed tolerance, the proportion is wrong. Above codes can be found in Detector::centerRecognize  
* also, using opencv adaptiveThreshold is another point. See Detector::getImagePoint. I have also tried other ways like gussian blur, erode, but these won't give better results.  
* At last, I find my method works not so well, so I read zxing source code, which is an open source project for QR code recognization. I extract the locator recognization part, and convert them to qt(c++)...Then it works like a magic. See finderPattern and finderPatternFinder class for recognization details. using it is quite simple: FinderPatternFinder finder(src); finder.find(); see Detector::centerRecognize for details about how I use it.  

![click_me](https://github.com/meiqua/cameraLoc/tree/master/images/example.png)
              
