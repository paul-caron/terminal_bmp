/*
Author: Paul Caron
Date: September2, 2020
*/
#include <iostream>
#include <fstream>
#include <cstdint>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cmath>
using namespace std;

#pragma pack(push, 1)

struct BMPFileHeader {
     uint16_t magic_number{0};
     uint32_t file_size{0};
     uint16_t reserved1{0};
     uint16_t reserved2{0};
     uint32_t data_offset{0};
};

struct DIBHeader {
     uint32_t dib_header_size{ 0 };
     int32_t image_width{ 0 };
     int32_t image_height{ 0 };
     uint16_t color_planes{ 1 };
     uint16_t bits_per_pixel{0};
     uint32_t compression_type{ 0 };
     uint32_t image_size{ 0 };
     int32_t x_pixels_per_meter{ 0 };
     int32_t y_pixels_per_meter{ 0 };
     uint32_t palette_colors{0};
     uint32_t important_colors{0};
};
#pragma pack(pop)

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

    BMPFileHeader bfh;
    myFile.read(reinterpret_cast<char*>(&bfh),sizeof(bfh));
    cout<<"magic number: "<<bfh.magic_number<<endl;
    cout<<"file size: "<<bfh.file_size<<endl;
    cout<<"reserve 1: "<<bfh.reserved1<<endl;
    cout<<"reserve 2: "<<bfh.reserved2<<endl;
    cout<<"data offset: "<<bfh.data_offset<<endl;
    if(bfh.magic_number!=19778){
        cerr<<"\033[31mThe file type is not BMP.\033[0m \n";
        exit(1);
    }

    DIBHeader dh;
    myFile.read(reinterpret_cast<char*>(&dh),sizeof(dh));
    cout<<"dib header size: "<<dh.dib_header_size<<endl;
    if(dh.dib_header_size<40){
        cerr<<"\033[31mThis DiB header is not supported.\033[0m \n";
        exit(1);
    }
    cout<<"image width: "<<dh.image_width<<endl;
    cout<<"image height: "<<dh.image_height<<endl;
    cout<<"color planes: "<<dh.color_planes<<endl;
    cout<<"bits per pixel: "<<dh.bits_per_pixel<<endl;
    cout<<"compression type: "<<dh.compression_type<<endl;
    cout<<"image size: "<<dh.image_size<<endl;
    cout<<"palette colors: "<<dh.palette_colors<<endl;
    cout<<"important colors: "<<dh.important_colors<<endl;


    int palette_offset=14+dh.dib_header_size;
    if(dh.compression_type==3) palette_offset+=12;

    char * palette = new char[4*dh.palette_colors];
    myFile.seekg(palette_offset, myFile.beg);
    myFile.read(palette, 4*dh.palette_colors);

    myFile.seekg(bfh.data_offset,myFile.beg);
    int abs_height = abs(dh.image_height);
    int abs_width = abs(dh.image_width);
    if(dh.image_size!=0) abs_width = dh.image_size/dh.image_height%2+abs_width;
    char * image_data;
    if(dh.image_size==0){
        image_data= new char [abs_height*abs_width*dh.bits_per_pixel/8];
        myFile.read(image_data,abs_height*abs_width*dh.bits_per_pixel/8);
    }else{
        image_data = new char [dh.image_size];
        myFile.read(image_data,dh.image_size);
    }
    int ratio = abs_width/w.ws_col+1;
    int pixel_bytes = dh.bits_per_pixel/8;
    if(dh.image_height>=0){
        for(int i=abs_height-1;i>=0;i-=ratio){
            for(int j=0;j<pixel_bytes*abs_width;j+=pixel_bytes*ratio){
               int index = dh.image_size/dh.image_height*i+j;
               if(dh.image_size==0) index=j+i*dh.image_width*pixel_bytes;
               if(dh.bits_per_pixel>=24){
                    uint8_t blue = image_data[index];
                    uint8_t green = image_data[index+1];
                    uint8_t red = image_data[index+2];
                    cout<<"\033[48;2;"<<+red<<";"<<+green<<";"<<+blue<<"m ";
                }
                if(dh.bits_per_pixel==8){
                 if(dh.palette_colors!=0){
                    int color_index = image_data[index]*4;
                    uint8_t blue = palette[color_index];
                    uint8_t green = palette[color_index + 1];
                    uint8_t red = palette[color_index + 2];
                    cout<< "\033[48;2;"<<+red<<";"<<+green<<";"<<+blue<<"m ";
                 }
                 else{
                    int color = image_data[index];
                    uint8_t blue = color;
                    uint8_t green = color;
                    uint8_t red = color;
                    cout<< "\033[48;2;"<<+red<<";"<<+green<<";"<<+blue<<"m ";
                 }
                }
            }
            cout<<"\033[0m\n";
        }
    }
    else{
        for(int i=0;i<abs_height;i+=ratio){
            for(int j=0;j<pixel_bytes*abs_width;j+=pixel_bytes*ratio){
               int index = dh.image_size/abs_height*i+j;
               if(dh.image_size==0) index=j+i*dh.image_width*pixel_bytes;
               if(dh.bits_per_pixel==24){
                    uint8_t blue = image_data[index];
                    uint8_t green = image_data[index+1];
                    uint8_t red = image_data[index+2];
                    cout<<"\033[48;2;"<<+red<<";"<<+green<<";"<<+blue<<"m ";
                }
                if(dh.bits_per_pixel==8){
                 if(dh.palette_colors!=0){
                    int color_index = image_data[index]*4;
                    uint8_t blue = palette[color_index];
                    uint8_t green = palette[color_index + 1];
                    uint8_t red = palette[color_index + 2];
                    cout<< "\033[48;2;"<<+red<<";"<<+green<<";"<<+blue<<"m ";
                 }
                 else{
                    int color = image_data[index];
                    uint8_t blue = color;
                    uint8_t green = color;
                    uint8_t red = color;
                    cout<< "\033[48;2;"<<+red<<";"<<+green<<";"<<+blue<<"m ";
                 }
                }
            }
            cout<<"\033[0m\n";
        }
    }
    free(palette);
    free(image_data);
    return 0;
}
