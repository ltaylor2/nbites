//
//  nbfuncs.cpp
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//

#include "nbfuncs.h"
#include <assert.h>
#include <vector>

#include "Images.h"
#include "image/ImageConverterModule.cpp"
#include "vision/Hough/HoughLineModule.cpp"
#include "vision/FieldBoundary/FieldBoundaryModule.cpp"
#include "RoboGrams.h"

std::vector<nbfunc_t> FUNCS;

std::vector<logio::log_t> args;
std::vector<logio::log_t> rets;

//Common arg types -- used to check arg types and for human readability.
const char sYUVImage[] = "YUVImage";
const char sParticleSwarm_pbuf[] = "ParticleSwarm";
const char sParticle_pbuf[] = "Particle";
const char sTest[] = "test";

const char stext[] = "text";//No current sources for this data type.

int test_func() {
    assert(args.size() == 2);
    for (int i = 0; i < args.size(); ++i) {
        printf("test_func(): %s\n", args[i].desc);
    }
    
    return 0;
}

int arg_test_func() {
    printf("arg_test_func()\n");
    assert(args.size() == 2);
    for (int i = 0; i < 2; ++i) {
        printf("\t%s\n", args[i].desc);
        rets.push_back(logio::copyLog(&args[i]));
    }
    
    return 0;
}

int CrossBright_func() {
    assert(args.size() == 1);
    printf("CrossBright_func()\n");
    //work on a copy of the arg so we can safely push to rets.
    logio::log_t log = logio::copyLog(&args[0]);
    for (int i = 0; i < log.dlen; i += 2) {
        *(log.data + i) = 240;
    }
    
    printf("[%s] modified.\n", log.desc);
    rets.push_back(log);
    
    return 0;
}

int ImageConverter_func() {
    assert(args.size() == 1);

    printf("ImageConverter_func()\n");

    logio::log_t arg1 = args[0];
    std::vector<std::string> kvp = logio::pairs(arg1.desc);
    int width = 640;
    int height = 480;

    messages::YUVImage image(arg1.data, width, height, width);
    portals::Message<messages::YUVImage> message(&image);
    man::image::ImageConverterModule module;

    module.imageIn.setMessage(message);
    module.run();

    // Y image name and data
    const messages::PackedImage<short unsigned int>* yImage = module.yImage.getMessage(true).get();
    logio::log_t yRet;

    std::string yName = "type=YUVImage encoding=[Y16] width=";
    yName += std::to_string(yImage->width());
    yName += " height=";
    yName += std::to_string(yImage->height());

    yRet.desc = (char*)malloc(yName.size()+1);
    memcpy(yRet.desc, yName.c_str(), yName.size() + 1);

    yRet.dlen = yImage->width() * yImage->height() * 2;
    yRet.data = (uint8_t*)malloc(yRet.dlen);
    memcpy(yRet.data, yImage->pixelAddress(0, 0), yRet.dlen);

    rets.push_back(yRet);

    // White image retreval, description, and data
    const messages::PackedImage<unsigned char>* whiteImage = module.whiteImage.getMessage(true).get();
    logio::log_t whiteRet;

    std::string whiteName = "type=YUVImage encoding=[Y8] width=";
    whiteName += std::to_string(whiteImage->width());
    whiteName += " height=";
    whiteName += std::to_string(whiteImage->height());

    whiteRet.desc = (char*)malloc(whiteName.size()+1);
    memcpy(whiteRet.desc, whiteName.c_str(), whiteName.size()+1);

    whiteRet.dlen = whiteImage->width() * whiteImage->height();
    whiteRet.data = (uint8_t*)malloc(whiteRet.dlen);
    memcpy(whiteRet.data, whiteImage->pixelAddress(0, 0), whiteRet.dlen);

    rets.push_back(whiteRet);

    // Orange image retreval, description, and data
    const messages::PackedImage<unsigned char>* orangeImage = module.orangeImage.getMessage(true).get();
    logio::log_t orangeRet;

    std::string orangeName = "type=YUVImage encoding=[Y8] width=";
    orangeName += std::to_string(orangeImage->width());
    orangeName += " height=";
    orangeName += std::to_string(orangeImage->height());

    orangeRet.desc = (char*)malloc(orangeName.size()+1);
    memcpy(orangeRet.desc, orangeName.c_str(), orangeName.size()+1);

    orangeRet.dlen = orangeImage->width() * orangeImage->height();
    orangeRet.data = (uint8_t*)malloc(orangeRet.dlen);
    memcpy(orangeRet.data, orangeImage->pixelAddress(0, 0), orangeRet.dlen);

    rets.push_back(orangeRet);

    // Green image retreval, description, and data
    const messages::PackedImage<unsigned char>* greenImage = module.greenImage.getMessage(true).get();
    logio::log_t greenRet;

    std::cout << "Green width: " << greenImage->width() <<  std::endl;

    std::string greenName = "type=YUVImage encoding=[Y8] width=";
    greenName += std::to_string(greenImage->width());
    greenName += " height=";
    greenName += std::to_string(greenImage->height());

    greenRet.desc = (char*)malloc(greenName.size()+1);
    memcpy(greenRet.desc, greenName.c_str(), greenName.size()+1);

    greenRet.dlen = greenImage->width() * greenImage->height();
    greenRet.data = (uint8_t*)malloc(greenRet.dlen);
    memcpy(greenRet.data, greenImage->pixelAddress(0, 0), greenRet.dlen);

    rets.push_back(greenRet);
    
    // Done
    printf("ImageConverter module ran! W: %d, H: %d\n", yImage->width(), yImage->height());
    return 0;
}

int FieldBoundary_func()
{
    assert(args.size() == 1);

    printf("FieldBoundary_func()\n");

    logio::log_t arg1 = args[0];
    std::vector<std::string> kvp = logio::pairs(arg1.desc);
    int width = 640;
    int height = 480;

    messages::YUVImage image(arg1.data, width, height, width);
    portals::Message<messages::YUVImage> message(&image);

    man::vision::FieldBoundaryModule module;
    module.imageIn.setMessage(message);
    module.run();

    const messages::PackedImage<unsigned char>* gOut = module.gImage.getMessage(true).get();
    logio::log_t gRet;

    std::string gName = "type=YUVimage encoding=[Y8] width=";
    gName += std::to_string(gOut->width());
    gName += " height=";
    gName += std::to_string(gOut->height());

    gRet.desc = (char*)malloc(gName.size() + 1);
    memcpy(gRet.desc, gName.c_str(), gName.size() +1);

    gRet.dlen = gOut->width() * gOut->height();
    gRet.data = (uint8_t*)malloc(gRet.dlen);
    memcpy(gRet.data, gOut->pixelAddress(0,0), gRet.dlen);

    rets.push_back(gRet);

    const messages::PackedImage<unsigned char>* fOut = module.fImage.getMessage(true).get();
    logio::log_t fRet;

    std::string fName = "type=YUVimage encoding=[Y8] width=";
    fName += std::to_string(fOut->width());
    fName += " height=";
    fName += std::to_string(fOut->height());

    fRet.desc = (char*)malloc(gName.size() + 1);
    memcpy(fRet.desc, fName.c_str(), fName.size() +1);

    fRet.dlen = fOut->width() * fOut->height();
    fRet.data = (uint8_t*)malloc(fRet.dlen);
    memcpy(fRet.data, fOut->pixelAddress(0,0), fRet.dlen);

    rets.push_back(fRet);

    const messages::BoundaryLines* lines = module.bLineList.getMessage(true).get();

    std::string lineName = "type=proto-VisionField";

    logio::log_t lineRet;

    lineRet.desc = (char*)malloc(lineName.size() + 1);
    memcpy(lineRet.desc, lineName.c_str(), lineName.size() + 1);

    std::string data;
    lines->SerializeToString(&data);

    lineRet.data = (uint8_t*)malloc(data.size());
    lineRet.dlen = data.size();
    memcpy(lineRet.data, (uint8_t*)data.c_str(), data.size());

    rets.push_back(lineRet);

    return 0;
}

int HoughLine_func()
{
    assert(args.size() == 1);

    printf("HoughLine_func()\n");

    logio::log_t arg1 = args[0];
    std::vector<std::string> kvp = logio::pairs(arg1.desc);
    int width = 640;
    int height = 480;

    messages::YUVImage image(arg1.data, width, height, width);

    portals::Message<messages::YUVImage> message(&image);
    man::vision::HoughLineModule module;

    module.imageIn.setMessage(message);
    module.run();

    const messages::PackedImage<short unsigned int>* houghImage = module.houghImage.getMessage(true).get();

    logio::log_t houghRet;

    std::string houghName = "type=YUVImage encoding=[Y16] width=";
    houghName += std::to_string(houghImage->width());
    houghName += " height=";
    houghName += std::to_string(houghImage->height());

    houghRet.desc = (char*)malloc(houghName.size() + 1);
    memcpy(houghRet.desc, houghName.c_str(), houghName.size() + 1);

    houghRet.dlen = houghImage->width() * houghImage->height() * 2;
    houghRet.data = (uint8_t*)malloc(houghRet.dlen);
    memcpy(houghRet.data, houghImage->pixelAddress(0, 0), houghRet.dlen);

    rets.push_back(houghRet);

    const messages::Lines* lines = module.houghLineList.getMessage(true).get();

    std::string lineName = "type=proto-VisionField";

    logio::log_t lineRet;

    lineRet.desc = (char*)malloc(lineName.size() + 1);
    memcpy(lineRet.desc, lineName.c_str(), lineName.size() + 1);

    std::string data;
    lines->SerializeToString(&data);

    lineRet.data = (uint8_t*)malloc(data.size());
    lineRet.dlen = data.size();
    memcpy(lineRet.data, (uint8_t*)data.c_str(), data.size());
    rets.push_back(lineRet);

    const messages::PackedImage<short unsigned int>* edgeImage = module.edgeImage.getMessage(true).get();

    logio::log_t edgeRet;
    std::string edgeName = "type=YUVImage encoding=[Y16] width=";
    edgeName += std::to_string(edgeImage->width());
    edgeName += " height=";
    edgeName += std::to_string(edgeImage->height());

    edgeRet.desc = (char*)malloc(edgeName.size() + 1);
    memcpy(edgeRet.desc, edgeName.c_str(), edgeName.size() + 1);

    edgeRet.dlen = edgeImage->width() * edgeImage->height() * 2;
    edgeRet.data = (uint8_t*)malloc(edgeRet.dlen);
    memcpy(edgeRet.data, edgeImage->pixelAddress(0,0), edgeRet.dlen);

    rets.push_back(edgeRet);

    const messages::PackedImage<short unsigned int>* fEdgeImage = module.fieldEdgeImage.getMessage(true).get();

    logio::log_t fEdgeRet;
    std::string fEdgeName = "type=YUVImage encoding=[Y16] width=";
    fEdgeName += std::to_string(fEdgeImage->width());
    fEdgeName += " height=";
    fEdgeName += std::to_string(fEdgeImage->height());

    fEdgeRet.desc = (char*)malloc(fEdgeName.size() + 1);
    memcpy(fEdgeRet.desc, fEdgeName.c_str(), fEdgeName.size() + 1);

    fEdgeRet.dlen = fEdgeImage->width() * fEdgeImage->height() * 2;
    fEdgeRet.data = (uint8_t*)malloc(fEdgeRet.dlen);
    memcpy(fEdgeRet.data, fEdgeImage->pixelAddress(0,0), fEdgeRet.dlen);

    rets.push_back(fEdgeRet);

    const messages::PackedImage<unsigned char>* gOut = module.greenImage.getMessage(true).get();
    logio::log_t gRet;

    std::string gName = "type=YUVimage encoding=[Y8] width=";
    gName += std::to_string(gOut->width());
    gName += " height=";
    gName += std::to_string(gOut->height());

    gRet.desc = (char*)malloc(gName.size() + 1);
    memcpy(gRet.desc, gName.c_str(), gName.size() +1);

    gRet.dlen = gOut->width() * gOut->height();
    gRet.data = (uint8_t*)malloc(gRet.dlen);
    memcpy(gRet.data, gOut->pixelAddress(0,0), gRet.dlen);

    rets.push_back(gRet);

    const messages::PackedImage<unsigned char>* fOut = module.fieldPointImage.getMessage(true).get();
    logio::log_t fRet;

    std::string fName = "type=YUVimage encoding=[Y8] width=";
    fName += std::to_string(fOut->width());
    fName += " height=";
    fName += std::to_string(fOut->height());

    fRet.desc = (char*)malloc(gName.size() + 1);
    memcpy(fRet.desc, fName.c_str(), fName.size() +1);

    fRet.dlen = fOut->width() * fOut->height();
    fRet.data = (uint8_t*)malloc(fRet.dlen);
    memcpy(fRet.data, fOut->pixelAddress(0,0), fRet.dlen);

    rets.push_back(fRet);

    const messages::BoundaryLines* bLines = module.bLineList.getMessage(true).get();

    std::string bLineName = "type=proto-VisionField";

    logio::log_t bLineRet;

    bLineRet.desc = (char*)malloc(bLineName.size() + 1);
    memcpy(bLineRet.desc, bLineName.c_str(), bLineName.size() + 1);

    std::string bData;
    bLines->SerializeToString(&bData);

    bLineRet.data = (uint8_t*)malloc(bData.size());
    bLineRet.dlen = bData.size();
    memcpy(bLineRet.data, (uint8_t*)bData.c_str(), bData.size());

    rets.push_back(bLineRet);

    return 0;
}

void register_funcs() {
    
    /*test func 1*/
    nbfunc_t test;
    test.name = (const char *) "simple test";
    test.args = {sTest, sTest};
    test.func = test_func;
    FUNCS.push_back(test);
    
    /*test func 2*/
    nbfunc_t arg_test;
    arg_test.name = (char *) "arg test";
    arg_test.args = {sYUVImage, sYUVImage};
    arg_test.func = arg_test_func;
    FUNCS.push_back(arg_test);
    
    //CrossBright
    nbfunc_t CrossBright;
    CrossBright.name = "CrossBright";
    CrossBright.args = {sYUVImage};
    CrossBright.func = CrossBright_func;
    FUNCS.push_back(CrossBright);

    //ImageConverter
    nbfunc_t ImageConverter;
    ImageConverter.name = "ImageConverter";
    ImageConverter.args = {sYUVImage};
    ImageConverter.func = ImageConverter_func;
    FUNCS.push_back(ImageConverter);

    // Encapsulated Hough Stuff
    nbfunc_t HoughLine;
    HoughLine.name = "HoughLine";
    HoughLine.args = {sYUVImage};
    HoughLine.func = HoughLine_func;
    FUNCS.push_back(HoughLine);

    // Field Boundary Lines
    nbfunc_t FieldBoundary;
    FieldBoundary.name = "FieldBoundary";
    FieldBoundary.args = {sYUVImage};
    FieldBoundary.func = FieldBoundary_func;
    FUNCS.push_back(FieldBoundary);
}


