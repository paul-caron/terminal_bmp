# terminal_bmp
Outputs bmp image on terminal screen. 

No compression.

Works also only in Linux. Doesnt work on Windows, it needs a terminal that supports ANSI eacape sequences.

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

# cool thing to do

```
$ wget https://picsum.photos/200/300 && mv 300 300.jpg && mogrify -format bmp -compress None 300.jpg && ./termbmp 300.bmp
```
