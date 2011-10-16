Image Filters using OpenCV
--------------------------


Operators Supported
-------------------

* Blur
* Gaussian Blur
* Sobel Edge Detection

![Imgur](http://i.imgur.com/suaoC.png)

* Canny Edge Detection

![Imgur](http://i.imgur.com/kRe6x.png)

* Hough Transform

![Imgur](http://i.imgur.com/s8XlY.png)

* Skin Detection

![Imgur](http://i.imgur.com/7T0rs.png)

Dependency
----------
* OpenCV

Build
-----

    g++ calibrate.cc -I/opt/local/include/opencv -L/opt/local/lib -lcv -lhighgui - lcxcore -Wall -o calibrate

Usage
-----

    filter <filterName> [<filenameFlag> <filename>]

where:

    filterName: [-blur] | [-gauss] | [-sobel] | [-canny] | [-hough] | [-skin]
    filenameFlag: [-i] | [-v]

Documents
---------

* [Report](http://cloud.github.com/downloads/vbajpai/imagefilters/report.pdf)
