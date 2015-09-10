
// ok, yep! go on from finding the coords for boxes and printing...

// g++ $(pkg-config --cflags --libs opencv) opencv_test.cc -o opencv_test && ./opencv_test

#include "opencv2/opencv.hpp"
// credits: http://stackoverflow.com/questions/23506105/extracting-text-opencv
std::vector<cv::Rect> detectLetters(cv::Mat img)
{
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1);
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        {
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height)
                boundRect.push_back(appRect);
        }
    return boundRect;
}

int main ( int argc,char** argv )
{
    //Read
    cv::Mat img1=cv::imread("./pics/tj2.jpg");
    // cv::Mat img2=cv::imread("side_2.jpg");

    //Detect
    std::vector<cv::Rect> letterBBoxes1=detectLetters(img1);

    // std::vector<cv::Rect> letterBBoxes2=detectLetters(img2);

    //Display

    int tl[2]={-1,-1}, tr[2]={-1,-1}, bl[2]={-1,-1}, br[2]={-1,-1};

    for( int i=0; i< letterBBoxes1.size(); i++ ){

      // top left x - smallest
      if(tl[0]<0 || tl[0]>letterBBoxes1[i].x) tl[0]=letterBBoxes1[i].x;
      // top left y - smallest
      if(tl[1]<0 || tl[1]>letterBBoxes1[i].y) tl[1]=letterBBoxes1[i].y;

      // top righ x - largest
      if(tr[0]<0 || tr[0]<(letterBBoxes1[i].x+letterBBoxes1[i].width)) tr[0]=(letterBBoxes1[i].x+letterBBoxes1[i].width);
      // top right y - smallest
      if(tr[1]<0 || tr[1]>letterBBoxes1[i].y) tr[1]=letterBBoxes1[i].y;

      // bottom left x - smallest
      if(bl[0]<0 || bl[0]>letterBBoxes1[i].x) bl[0]=letterBBoxes1[i].x;
      // bottom left y - largest
      if(bl[1]<0 || bl[1]<(letterBBoxes1[i].y+letterBBoxes1[i].height)) bl[1]=(letterBBoxes1[i].y+letterBBoxes1[i].height);

      // bottom righ x - largest
      if(br[0]<0 || br[0]<(letterBBoxes1[i].x+letterBBoxes1[i].width)) br[0]=(letterBBoxes1[i].x+letterBBoxes1[i].width);
      // bottom right y - largest
      if(br[1]<0 || br[1]<(letterBBoxes1[i].y+letterBBoxes1[i].height)) br[1]=(letterBBoxes1[i].y+letterBBoxes1[i].height);

      printf(
             "rect x: %d, y: %d, w: %d, h: %d\n",
              letterBBoxes1[i].x, letterBBoxes1[i].y,
              letterBBoxes1[i].width, letterBBoxes1[i].height
      );

      cv::rectangle(img1,letterBBoxes1[i],cv::Scalar(0,255,0),3,8,0);
    }

    printf("ok man, should have got the rect coords for whole receipt, lets see: \ntl: %d, %d\n  \ntr: %d, %d\n \nbl: %d, %d\n \nbr: %d, %d\n",
           tl[0],tl[1],
           tr[0],tr[1],
           bl[0],bl[1],
           br[0],br[1]
    );

    cv::imwrite( "imgOut1.jpg", img1);
    // cv::namedWindow("Display Image", cv::WINDOW_NORMAL );
    // cv::imshow("Display Image", img1);

    // for(int i=0; i< letterBBoxes2.size(); i++)
    //     cv::rectangle(img2,letterBBoxes2[i],cv::Scalar(0,255,0),3,8,0);
    // cv::imwrite( "imgOut2.jpg", img2);

    // cv::waitKey(0);

    return 0;
}
