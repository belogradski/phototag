# phototag
Phototag is an open source software for geotagging pictures using GoogleMaps. Geotags are saved in EXIF format, if a picture is already tagged, the corresponding location will be shown on the map.
Pictures can be tagged or retagged by dragging  and dropping it at a desired location on the map.
The software requires the ExifTool to read/write exif tags (https://github.com/exiftool/exiftool). The executable (e.g. exiftool.exe on Windows) should be available in the same directory as the Phototag executable.

# Compiling
The software is based on Qt libraries. So you need to configure your Qt project environment and then build this project using Qt tools of your choice (e.g. Qt Creator). For more information on Qt visit https://www.qt.io
To interact with Google Maps you'll need your own Google API key. To obtain it please refer to https://developers.google.com/maps/documentation/javascript/get-api-key After you have the key you will have to replace the dummy key in the html code of the project (google_maps.html) and also in the C++ code (geocode_data_manager.cpp).

# Distribution
If you want just to use the software you can download the Windows 32 bit version here https://1drv.ms/u/s!AtrGdLHZReCekQL9tWmiB9RsG71o
Just unzip the archive, no installation is needed

# Author
Send any comments, requests, bug reports to Dmitri Belogradski phototag@outlook.de
