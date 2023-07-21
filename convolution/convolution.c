#include<stdio.h>
typedef unsigned char byte;

int main(){
    int width =256;
    int height = 256;
    int mwidth = 3;
    int mheight = 3;
    float rate =0.0625 ;
    byte headbuf[1078];
    byte buf[256][256];
    byte outbuf[256][256];
    FILE *fp = fopen("czp256.bmp","rb");
    fseek(fp,1078,SEEK_SET);
    fread(buf,sizeof(byte),256*256,fp);
    fclose(fp);
    int convmask[3][3] = {{1,2,1},
                         {2,4,2},
                         {1,2,1}};
    int convsum = 0;

    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++)
        outbuf[i][j]=0;
    }  
    
    for(int i=0;i<256;i++){
        for(int j=0;j<256;j++){
            if(i == 0 && j==0){
                 for(int k=1;k<3;k++){
                    for(int p=1;p<3;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
                
            }
            else if(i == 255 && j == 255){
                for(int k=0;k<2;k++){
                    for(int p=0;p<2;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else if(i == 255 && j == 0){
                for(int k=0;k<2;k++){
                    for(int p=1;p<3;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else if(i == 0 && j == 255){
                for(int k=1;k<3;k++){
                    for(int p=0;p<2;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else if(i == 0  && j != 255 && j != 0 ){
                for(int k=1;k<3;k++){
                    for(int p=0;p<3;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else if(i == 255 && j != 255 && j != 0 ){
                for(int k=0;k<2;k++){
                    for(int p=0;p<3;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else if(j == 0 && i != 255 && i != 0){
                for(int k=0;k<3;k++){
                    for(int p=1;p<3;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else if(j == 255 && i != 255 && i != 0){
                for(int k=0;k<3;k++){
                    for(int p=0;p<2;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
            }
            else{
                for(int k=0;k<3;k++){
                    for(int p=0;p<3;p++){
                        convsum+=(int)(buf[i-1+k][j-1+p]*convmask[k][p]);
                    }
                }
                convsum*=rate;
                  outbuf[i][j] = convsum;
            
                if(convsum < 0 && convsum > -256 ){
                    outbuf[i][j] = convsum;
                    outbuf[i][j] = 256 - outbuf[i][j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[i][j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[i][j] = convsum;
                }
                
            }
            convsum=0;
        }
    }
    /*
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++)
        printf("[%d]",outbuf[i][j]);
    }
    */
    
    
    
    fp = fopen("czp256.bmp","rb");
    fread(headbuf,sizeof(byte),1078,fp);
    fclose(fp);

    FILE *fp2 = fopen("Gaussian_czp256.bmp","wb");
    fwrite(headbuf,sizeof(byte),1078,fp2);
    fwrite(outbuf,sizeof(byte),256*256,fp);
    fclose(fp2);

    return 0;

}