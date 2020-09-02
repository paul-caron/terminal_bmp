# terminal_bmp
Outputs bmp image on terminal screen. 

Support only for 40 bytes DIB header, RGB color images and uncompressed.

Works also only in Linux. Doesnt work on Windows.

# compile
``` sh
$ git clone https://github.com/paul-caron/terminal_bmp
$ make
```

# command line
``` sh
$ ./termbmp file.bmp
```
or

``` sh
$ ./termbmp
Please enter file name.
file.bmp
```
