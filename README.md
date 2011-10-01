Operators Supported
-------------------

* Blur
* Gaussian Blur
* Sobel Edge Detection
* Canny Edge Detection
* Hough Transform
* Skin Detection

Dependency
----------
* OpenCV

Build
-----

`g++ calibrate.cc -I/opt/local/include/opencv -L/opt/local/lib -lcv -lhighgui - lcxcore -Wall -o calibrate`

Usage
-----

`filter <filterName> [<filenameFlag> <filename>]`

where:
`filterName: [-blur] | [-gauss] | [-sobel] | [-canny] | [-hough] | [-skin]`
`filenameFlag: [-i] | [-v]`

Documents
---------

* [Report](http://cloud.github.com/downloads/vbajpai/imagefilters/report.pdf)
