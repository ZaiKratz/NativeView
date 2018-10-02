// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARBlueprintLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FQRCodeCaptured, FString, _decodedQRCode);

/**
 *
 */

#if PLATFORM_IOS
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

@interface QRReader : NSObject <AVCaptureMetadataOutputObjectsDelegate>
{
    NSString* _capturedCode;
    bool bIsQRCodeCaptured;
}

@property (nonatomic, strong) UIView* _viewContainer;
@property (nonatomic, strong) AVCaptureSession* _captureSession;
@property (nonatomic,strong) AVCaptureVideoPreviewLayer* _videoPreviewLayer;

-(void)StartReading;
-(void)StopReading;
-(bool)IsQRCodeCaptured;

@end

#endif

UCLASS()
class NATIVETEST_API UQRReader : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
private:
#if PLATFORM_IOS
    static QRReader* _reader;
#endif
public:
    UFUNCTION(BlueprintCallable)
    static void StartQRScanner(FQRCodeCaptured QRCodeCapturedEvent);
    
    UFUNCTION(BlueprintCallable)
    static void StopQRScanner();
    
};
