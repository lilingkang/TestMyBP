#include "TestMyBPF.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
 
using namespace cv;

bool UTestMyBPF::ASimpleNodeFunction(UObject* AnyObject)
{
	return AnyObject != nullptr;
}

UTexture2D* UTestMyBPF::GenerateTextureFunction(TArray<uint8> ByteArray)
{
	int32 SrcHeight = 2;
	int32 SrcWidth = 2;
	int32 TextureHeight = 512;
	int32 TextureWidth = 512;
	UTexture2D* TheTexture2D = UTexture2D::CreateTransient(TextureHeight, TextureWidth, PF_B8G8R8A8);
	uint8* Pixels = new uint8[TextureWidth * TextureHeight * 4];
	if (ByteArray.Num() != SrcHeight*SrcWidth*3)
		return TheTexture2D;
	uint8* ImgData = ByteArray.GetData();
	// SrcImage添加通道
	Mat SrcImage(SrcHeight, SrcWidth, CV_8UC3, ImgData);
	Mat BGRA(SrcHeight, SrcWidth, CV_8UC4, ImgData);
	Mat A(SrcHeight, SrcWidth, CV_8UC1, Scalar(255));
	Mat In[] = {SrcImage, A};
	int FromTo[] = {0,0, 1,1, 2,2, 3,3};
	mixChannels(In, 2, &BGRA, 1, FromTo, 4);
	// 图像缩放
	Mat dst(TextureHeight, TextureWidth, CV_8UC3);
	resize(BGRA,dst,dst.size(),0,0, INTER_LINEAR);
	// Mat转为数组
	if (dst.isContinuous())
	{
		Pixels = dst.data;
	}
	// uint8* Pixels = new uint8[TextureWidth * TextureHeight * 4];
	// for (int32 y = 0; y < TextureHeight; y++)
	// {
	// 	for (int32 x = 0; x < TextureWidth; x++)
	// 	{
	// 		int32 curPixelIndex = ((y * TextureWidth) + x);
	// 		int32 color = x;
	// 		while (color > 255)
	// 			color -= 255;
	// 		Pixels[4 * curPixelIndex] = color;
	// 		Pixels[4 * curPixelIndex + 1] = color;
	// 		Pixels[4 * curPixelIndex + 2] = color;
	// 		Pixels[4 * curPixelIndex + 3] = 255;
	// 	}
	// }
	void* TextureData = TheTexture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * TextureHeight * TextureWidth * 4);
	// TheTexture2D->PlatformData = new FTexturePlatformData();
	// Then we initialize the PlatformData
	TheTexture2D->PlatformData->Mips[0].BulkData.Unlock();
	TheTexture2D->UpdateResource();
	return TheTexture2D;
}
