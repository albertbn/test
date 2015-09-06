
// this folk is in c++, yep!

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

int main()
{

  const char* inputfile = "./pics/tj.jpg";
  char *outText = NULL;
  // const char* inputfile = "./pics/tj2.jpg";
  tesseract::Orientation orientation;
  tesseract::WritingDirection direction;
  tesseract::TextlineOrder order;
  float deskew_angle;

  PIX *image = pixRead(inputfile);
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

  api->Init( NULL, "eng" );
  api->SetPageSegMode(tesseract::PSM_AUTO_OSD);
  api->SetImage(image);
  api->Recognize(0);

  tesseract::PageIterator* it =  api->AnalyseLayout();
  it->Orientation(&orientation, &direction, &order, &deskew_angle);

  printf("Orientation: %d;\nWritingDirection: %d\nTextlineOrder: %d\n" \
         "Deskew angle: %.4f\n",
         orientation, direction, order, deskew_angle);

  if( orientation == 3 ){

    image = pixRotate90( image, 1 );
    printf("ok, orientations is 3\n=======\n");

    api->SetImage(image);
  }

  outText = api->GetUTF8Text();
  printf("OCR output:\n%s", outText);

  // Destroy used object and release memory
  api->End();
  delete [] outText;
  pixDestroy(&image);

  return 0;
}


// int main()
// {
//     char *outText;

//     tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
//     // Initialize tesseract-ocr with English, without specifying tessdata path
//     if ( api->Init(NULL, "eng") )  {
//         fprintf(stderr, "Could not initialize tesseract.\n");
//         exit(1);
//     }

//     // Open input image with leptonica library
//     // Pix *image = pixRead("./pics/tj2.jpg");
//     Pix *image = pixRead("./pics/tj.jpg");
//     api->SetImage(image);
//     // Get OCR result
//     outText = api->GetUTF8Text();
//     printf("OCR output:\n%s", outText);

//     // Destroy used object and release memory
//     api->End();
//     delete [] outText;
//     pixDestroy(&image);

//     return 0;
// }
