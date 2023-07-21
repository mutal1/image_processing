 #include<stdio.h>
 #include<stdlib.h>
 
 typedef unsigned char BYTE;

 int main(){

    int hsize = 512;
    int wsize = 512;
    char *filename="map.bmp"; 
    int mheight = 3;
    int mwidth =3 ;
    float rate=1.0;
    char *convfile="testconv9999.bmp";
   /* BYTE buf[512][512];
    BYTE outbuf[512][512];
    int convmask[3][3]={{0,-1,0},
                        {-1, 5,-1},
                        {0,-1,0}
                    };*/

    BYTE headbuf[1078]; // header size buf

	//buf �����Ҵ�
    BYTE *buf;
    buf=(BYTE*)malloc(sizeof(BYTE)*(hsize*wsize));
	
	//out buf �����Ҵ�
    BYTE *outbuf;
    outbuf=(BYTE*)malloc(sizeof(BYTE)*(hsize*wsize));
    
	//���� �б�
    FILE *fp = fopen(filename,"rb");
    fseek(fp,1078,SEEK_SET);
    fread(buf,sizeof(BYTE),wsize*hsize,fp);
    fclose(fp);

	//����ũ ���� ���� �Ҵ� 
    int *convmask;
    convmask=(int*)malloc(sizeof(int)*(mheight*mwidth));

	// ���� �� �Է� �ޱ�
    printf("mask filter 값을 입력하시오:\n");
    for(int i=0;i<mheight;i++){
		for(int j=0;j<mwidth;j++){
			scanf("%d",&convmask[mheight*i+j]);
		}
	}

	for(int i=0;i<mheight;i++){
		for(int j=0;j<mwidth;j++){
			printf("[%d]",convmask[mheight*i+j]);
		}
	}


    int convsum = 0; //���� ��� ������ ����

    for(int i=0;i<wsize;i++){
        for(int j=0;j<hsize;j++)
        outbuf[hsize*i+j]=0;
    }  
    
    for(int i=0;i<hsize;i++){
        for(int j=0;j<wsize;j++){
            if(i == 0 && j==0){
                 for(int k=mheight/2;k<mheight;k++){
                    for(int p=mwidth/2;p<mwidth;p++){
                        convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == hsize-1 && j == wsize-1){
                for(int k=0;k<=mheight/2;k++){
                    for(int p=0;p<=mwidth/2;p++){
                        convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == hsize-1 && j == 0){
                for(int k=0;k<=mheight/2;k++){
                    for(int p=0;p<=mwidth/2;p++){
                        convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == 0 && j == mwidth-1){
                for(int k=mheight/2;k<mheight;k++){
                    for(int p=0;p<=mwidth/2;p++){
                          convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == 0  && j != mwidth-1 && j != 0 ){
                for(int k=mheight/2;k<mheight;k++){
                    for(int p=0;p<mwidth;p++){
                          convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(i == mheight-1 && j != mwidth-1 && j != 0 ){
                for(int k=0;k<=mheight/2;k++){
                    for(int p=0;p<mwidth;p++){
                          convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(j == 0 && i != mheight-1 && i != 0){
                for(int k=0;k<mheight;k++){
                    for(int p=mwidth/2;p<mwidth;p++){
                          convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else if(j == mwidth-1 && i != mheight-1 && i != 0){
                for(int k=0;k<mheight;k++){
                    for(int p=0;p<=mwidth;p++){
                          convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
                }
            }
            else{
                for(int k=0;k<mheight;k++){
                    for(int p=0;p<mwidth;p++){
                          convsum+=(int)(buf[hsize*(i-1+k)+(j-1+p)]*convmask[mwidth*k+p]);
                    }
                    convsum= convsum*rate;
                }
                  // outbuf[hsize*i+j] = convsum;
            
                if(convsum < 0 && convsum > -256 ){
                    outbuf[hsize*i+j] = convsum;
                    outbuf[hsize*i+j] = 256 - outbuf[hsize*i+j]; 
                }
                else if(convsum>=256 || convsum <= -256 ){
                    outbuf[hsize*i+j] = 255;
                }
                else if(convsum >= 0 && convsum < 256){
                    outbuf[hsize*i+j] = convsum;
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
    
    
    
    fp = fopen(filename,"rb");
    fread(headbuf,sizeof(BYTE),1078,fp);
    fclose(fp);

    FILE *fp2 = fopen(convfile,"wb");
    fwrite(headbuf,sizeof(BYTE),1078,fp2);
    fwrite(outbuf,sizeof(BYTE),wsize*hsize,fp);
    fclose(fp2);

    // buf 할당 해제
	free(buf);

	//outbuf 할당 해제
	free(outbuf);

	//convmask 할당 해제
	free(convmask);
}
