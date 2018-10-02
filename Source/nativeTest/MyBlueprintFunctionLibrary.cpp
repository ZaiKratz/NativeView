// Fill out your copyright notice in the Description page of Project Settings.

#include "MyBlueprintFunctionLibrary.h"

static FQRCodeCaptured _onQRCodeCaptured;

#if PLATFORM_IOS
#include "IOS/IOSView.h"
#include "IOS/IOSAppDelegate.h"

@implementation QRReader

@synthesize _viewContainer;
@synthesize _captureSession;
@synthesize _videoPreviewLayer;

-(bool)IsQRCodeCaptured
{
    return self->bIsQRCodeCaptured;
}

-(void)StartReading
{
    self->bIsQRCodeCaptured = false;
    [self InitViewer];
    
    _captureSession = [[AVCaptureSession alloc] init];
    [_captureSession setSessionPreset : AVCaptureSessionPresetHigh];
    
    AVCaptureDevice* captureDevice = [AVCaptureDevice defaultDeviceWithMediaType : AVMediaTypeVideo];
    
    NSError *error;
    
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice : captureDevice error : &error];
    if (!input)
    {
        NSLog(@"MLARALOG: %@",[error localizedDescription]);
    }
    
    [_captureSession beginConfiguration];
    AVCaptureMetadataOutput *captureMetadataOutput = [[AVCaptureMetadataOutput alloc] init];

    if ([_captureSession canAddInput : input])
        [_captureSession addInput : input];

    // Creating a capture session and  capturing metadata
    if ([_captureSession canAddOutput : captureMetadataOutput])
        [_captureSession addOutput : captureMetadataOutput];

    dispatch_queue_t metaDataOutputQueue;
    metaDataOutputQueue = dispatch_queue_create("MetaDataOutputQueue", DISPATCH_QUEUE_SERIAL);
    [captureMetadataOutput setMetadataObjectsDelegate : self queue : metaDataOutputQueue];
    [captureMetadataOutput setMetadataObjectTypes : [NSArray arrayWithObject : AVMetadataObjectTypeQRCode]];
    
    _videoPreviewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:_captureSession];
    
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       [_videoPreviewLayer setVideoGravity : AVLayerVideoGravityResizeAspectFill];
                       [_videoPreviewLayer setFrame : _viewContainer.layer.bounds];
                       [_viewContainer.layer addSublayer : _videoPreviewLayer];
                   });
    
    [_captureSession commitConfiguration];
    [_captureSession startRunning];
}

//Capture QR image
-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects : (NSArray *)metadataObjects fromConnection : (AVCaptureConnection *)connection
{
    if (metadataObjects != nil && [metadataObjects count] > 0) {
        AVMetadataMachineReadableCodeObject *metadataObj = [metadataObjects objectAtIndex : 0];
        if ([[metadataObj type] isEqualToString:AVMetadataObjectTypeQRCode])
        {
            _capturedCode = [metadataObj stringValue];
            self->bIsQRCodeCaptured = true;
            [self StopReading];
        }
    }
}

-(void)StopReading
{
    [_captureSession stopRunning];
    [_captureSession release];
    _captureSession = nil;
    
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       [_videoPreviewLayer removeFromSuperlayer];
                       _videoPreviewLayer = nil;
                       [_viewContainer removeFromSuperview];
                       _viewContainer = nil;
                   });
    
    if(self->bIsQRCodeCaptured == true)
        _onQRCodeCaptured.ExecuteIfBound(FString([_capturedCode UTF8String]));
    NSLog(@"StopReading: subviews: %lu", [[IOSAppDelegate GetDelegate].IOSView.subviews count]);
    UQRReader::StopQRScanner();
}

-(void)InitViewer
{
    dispatch_async(dispatch_get_main_queue(), ^
                   {
                       UIView* IOSView = [IOSAppDelegate GetDelegate].IOSView;
                       _viewContainer = [[UIView alloc] init];
                       
                       [[IOSAppDelegate GetDelegate].IOSView addSubview : _viewContainer];
                       
                       NSLog(@"InitViewer: subviews: %lu", [[IOSAppDelegate GetDelegate].IOSView.subviews count]);
                       
                       _viewContainer.translatesAutoresizingMaskIntoConstraints = NO;
                       [[_viewContainer.topAnchor constraintEqualToAnchor:IOSView.safeAreaLayoutGuide.topAnchor] setActive:YES];
                       [[_viewContainer.leadingAnchor constraintEqualToAnchor:IOSView.safeAreaLayoutGuide.leadingAnchor] setActive:YES];
                       [[_viewContainer.trailingAnchor constraintEqualToAnchor:IOSView.safeAreaLayoutGuide.trailingAnchor] setActive:YES];
                       [[_viewContainer.bottomAnchor constraintEqualToAnchor:IOSView.safeAreaLayoutGuide.bottomAnchor] setActive:YES];
                   });
}

@end

QRReader* UQRReader::_reader = nil;
#endif

void UQRReader::StartQRScanner(FQRCodeCaptured QRCodeCapturedEvent)
{
    _onQRCodeCaptured = QRCodeCapturedEvent;
#if PLATFORM_IOS
    if(_reader == nil)
        _reader = [[QRReader alloc] init];
    [_reader StartReading];
#endif
}

void UQRReader::StopQRScanner()
{
#if PLATFORM_IOS
    if(_reader != nil)
    {
        if([_reader IsQRCodeCaptured] == false)
            [_reader StopReading];
        [_reader release];
    }
    _reader = nil;
#endif
}

