/*
Author: Paul Caron
Date: September2, 2020
*/
#include <iostream>
#include <fstream>
#include <cstdint>
#include <sys/ioctl.h>
#include <unistd.h>
using namespace std;

int main(int argc, char ** argv){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    string filename;
    if(argc>1) filename = argv[1];
    else{
        cout<<"\033[38;2;255;255;0mPlease provide the bmp filename.\033[0m \n";
        cin >> filename;
    }
    ifstream myFile;
    myFile.open(filename.c_str(),ios::in | ios::binary);
    if(!myFile.is_open()){
        cerr<<"\033[31mCould not open file.\033[0m \n";
        exit(1);
    }
    //file info
    uint16_t mn;
    uint32_t fbs;
    uint16_t res1;
    uint16_t res2;
    uint32_t offset;
    //dib header
    uint32_t dib_header_size;
    int32_t image_width;
    int32_t image_height;
    uint16_t color_planes;
    uint16_t bits_per_pixel;
    uint32_t compression_model;
    uint32_t image_size;
    int32_t hor_resolution;
    int32_t vert_resolution;
    uint32_t palette_colors;
    uint32_t important_colors;
    //file info
    myFile.read(reinterpret_cast<char *>(&mn),sizeof(mn));
    if(mn!=19778){
        cerr<<"\033[31mFile Magic Number is not 'BM'.\033[0m \n";
        exit(1);
    }
    myFile.read((char *)&fbs, 4);
    myFile.read((char *)&res1, 2);
    myFile.read((char *)&res2, 2);
    myFile.read((char *)&offset, 4);
    cout<<"\nFile Info:\n"
        <<mn<<endl
        <<fbs<<endl
        <<res1<<endl
        <<res2<<endl
        <<offset<<endl;
    //dib header
    myFile.read((char*)&dib_header_size,4);
    if(dib_header_size!=40){
   //     cerr<<"\033[31mThis version of DIB header is not supported.\033[0m \n";
   //     exit(1);
    }
    myFile.read(reinterpret_cast<char*>(&image_width),sizeof(image_width));
    myFile.read(reinterpret_cast<char*>(&image_height),sizeof(image_height));
    myFile.read(reinterpret_cast<char*>(&color_planes),sizeof(color_planes));
    myFile.read(reinterpret_cast<char*>(&bits_per_pixel),sizeof(bits_per_pixel));
    myFile.read(reinterpret_cast<char*>(&compression_model),sizeof(compression_model));
    if(compression_model!=0){
        cerr<<"\033[31mCompression not supported.\033[0m \n";
        exit(1);
    }
    myFile.read(reinterpret_cast<char*>(&image_size),sizeof(image_size));
    myFile.read(reinterpret_cast<char*>(&hor_resolution),sizeof(hor_resolution));
    myFile.read(reinterpret_cast<char*>(&vert_resolution),sizeof(vert_resolution));
    myFile.read(reinterpret_cast<char*>(&palette_colors),sizeof(palette_colors));
    myFile.read(reinterpret_cast<char*>(&important_colors),sizeof(important_colors));

    cout<<"\nDIB header:\n"
        <<"dib header size: "<<dib_header_size<<endl
        <<"width: "<<image_width<<endl
        <<"height: "<<image_height<<endl
        <<"color planes: "<<color_planes<<endl
        <<"bits_per_pixel: "<<bits_per_pixel<<endl
        <<"compression: "<<compression_model<<endl
        <<"image_size: "<<image_size<<endl
        <<"hor resolution: "<<hor_resolution<<endl
        <<"vert resolution: "<<vert_resolution<<endl
        <<"palette colors: "<<palette_colors<<endl
        <<"important colors: "<<important_colors<<endl;

    int palette_offset = 14 + dib_header_size ;
    if(compression_model == 3) palette_offset += 12;
    
    myFile.seekg(palette_offset,myFile.beg);
    char * palette = (char*) malloc(palette_colors*4);
    myFile.read(palette, palette_colors*4);

    myFile.seekg(offset, myFile.beg);
    cout<< endl;
    char * image_data = (char*)malloc(bits_per_pixel/8*image_width*image_height);
    myFile.read(image_data, bits_per_pixel/8*image_width*image_height);

    int ratio = image_width/w.ws_col+1;

    for(int i=image_height-1;i>=0;i-=ratio){
    for(int j=0;j<image_width*bits_per_pixel/8;j+=ratio*bits_per_pixel/8){
        int index = image_size/image_height*i+j;
        if(image_size==0) index=j+i*image_width*bits_per_pixel/8;
        char red = image_data[index+2];
        char green = image_data[index+1];
        char blue = image_data[index];
        if(bits_per_pixel==24)
        cout<<"\033[48;2;"<<int(red)<<";"<<int(green)<<";"<<int(blue)<<"m ";
        else if(bits_per_pixel==8){
        green = palette[4*blue+1];
        red = palette[4*blue+2];
        blue = palette[4*blue];;
        cout<<"\033[48;2;"<<int(red)<<";"<<int(green)<<";"<<int(blue)<<"m ";

       }
    }
    cout<<"\033[0m"<<endl;
    }
    myFile.close();
    cout<<"\033[0m\n";
    free(palette);
    free(image_data);
    return 0;
}
