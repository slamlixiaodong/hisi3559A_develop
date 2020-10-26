#include<stdio.h>

void testShow()
{
    bool CanRun = true;
    int result = 0;

    cvNamedWindow("recognize", 1);
    //* 注册
    av_register_all();
    AVDictionary * options = NULL;
    av_dict_set(&options,"buffer_size","1024000",0);
    //   av_dict_set(&options,"stimeout","2000000",0);
    av_dict_set(&options,"max_delay","500000",0);
    av_dict_set(&options,"rtsp_transport","tcp",0);
    av_dict_set(&options,"preset","medium",0);
    av_dict_set(&options,"tune","zerolatency",0);
    //* 打开文件
    AVFormatContext* pFormatCtx = NULL;
    pFormatCtx = avformat_alloc_context();
    // step1: 打开媒体文件,最后2个参数是用来指定文件格式，buffer大小和格式参数，设置成NULL的话，libavformat库会自动去探测它们  
    result = avformat_open_input(&pFormatCtx, "rtsp://192.168.1.23:554/user=admin&password=&channel=1&stream=0.sdp?real_stream -s 640*360", NULL, &options);

    if (result != 0)  {  
    printf("open file fail \n");
    exit(1);
    }  
    printf("open file succ \n");
    // step2: 查找信息流的信息  
    result = avformat_find_stream_info(pFormatCtx, NULL);  
    if (result != 0)  {  
        printf("find stream fail \n");
        exit(1); 
    }  
    printf("find stream succ \n"); 

    // step3: 打印信息  
    //av_dump_format(pFormatCtx, 0, filename, 0);  

    // step4：找到video流数据  
    int i = 0;  
    int videoStream = -1;  
    AVCodecContext* pCodecCtx = NULL;  
    for (i = 0; i < pFormatCtx->nb_streams; i++)  
    {  
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)  
        {  
            videoStream = i;  
            break;  
        }
    }  
    // 得到video编码格式  
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;  

    // step5: 得到解码器  
    AVCodec* pCodec = NULL;  
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);  
    if (pCodec == NULL) {  
        printf("find decoder fail \n");
        exit(1); 
    }  
    printf("find decoder succ \n");

    result = avcodec_open2(pCodecCtx, pCodec, NULL);  
    if (result != 0)  {  
        printf("open codec fail \n");
        exit(1);  
    } 
    printf("open codec succ %d \n",pCodecCtx->bit_rate);
    // step6: 申请原始数据帧 和 RGB帧内存  
    AVFrame* pFrame = NULL;  
    AVFrame* pFrameRGB = NULL;  
    pFrame = av_frame_alloc();  
    pFrameRGB = av_frame_alloc();  
    if (pFrame == NULL || pFrameRGB == NULL)  {  
        return -1;  
    }  

    int numBytes = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);  
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));  
    avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);  

    int frameFinishsed = 0;  
    AVPacket packet;  
    i = 0;  
    printf("step6 succ \n");
    // step7: 创建格式转化文本     
    struct SwsContext *pSwxCtx = sws_getContext(  
    pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,  
    pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24,  
    SWS_BILINEAR, 0, 0, 0);
    printf("step7 succ \n"); 
    unsigned char *rgbBuf;
    rgbBuf = (unsigned char *)malloc( pCodecCtx->width * pCodecCtx->height * 3 * sizeof(unsigned char));
    unsigned char *tmp;
    tmp = (unsigned char *)malloc( pCodecCtx->width * pCodecCtx->height * 3 * sizeof(unsigned char));
    IplImage* img = cvCreateImage(cvSize(pCodecCtx->width,pCodecCtx->height),IPL_DEPTH_8U,3);

    int frame_rate = 25;
    if(pFormatCtx->streams[videoStream]->r_frame_rate.den>0){
        frame_rate = pFormatCtx->streams[videoStream]->r_frame_rate.num/pFormatCtx->streams[videoStream]->r_frame_rate.den;
    }else if(pCodecCtx->framerate.den>0){
        frame_rate = pCodecCtx->framerate.num/pCodecCtx->framerate.den;
    }
    printf("width:%d height %d frame rate %d\n",pCodecCtx->width,pCodecCtx->height,frame_rate);
    int skipped_frame = 0;
    av_init_packet(&packet);
    while (av_read_frame(pFormatCtx, &packet)>=0) {  
        ++skipped_frame;
        if(skipped_frame>300){
            break;
        }
        if (packet.stream_index == videoStream)  {  
            // 解码  
            result = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinishsed, &packet);   
            if (frameFinishsed)  {  
                if(av_read_frame_aozhen(pFormatCtx, &packet)==0){
                // 转换  
                sws_scale(pSwxCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height,  
                    (uint8_t**)&rgbBuf, pFrameRGB->linesize);                 
                img->imageData = rgbBuf;
                img->imageDataOrigin = rgbBuf;
                cvShowImage("recognize", img);
                //cvWaitKey(1000/frame_rate);
                cvWaitKey(1);
            }
            //   savejpg(rgbBuf, i, pCodecCtx->width, pCodecCtx->height);//生成图片
            //   if(i>3){
            //     break;
            //   }
            }else{
                //skipped_frame++;
            } 
        }  
        av_free_packet(&packet);  
    } 
        
    avformat_close_input(&pFormatCtx);
    printf("Hello world!! .\n");
}

int main(){

    testShow();    

    return 0;
}
