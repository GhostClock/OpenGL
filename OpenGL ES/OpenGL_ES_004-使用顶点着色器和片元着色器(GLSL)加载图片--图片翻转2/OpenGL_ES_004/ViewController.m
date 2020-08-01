//
//  ViewController.m
//  OpenGL_ES_004
//
//  Created by GhostClock on 2020/8/1.
//  Copyright © 2020 GhostClock. All rights reserved.
//

#import "ViewController.h"
#import "GCView.h"

@interface ViewController ()

@property (nonatomic, strong) GCView *gcView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.view addSubview:GCView.new];
}


@end
