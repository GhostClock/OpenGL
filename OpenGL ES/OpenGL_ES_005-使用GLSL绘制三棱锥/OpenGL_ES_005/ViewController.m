//
//  ViewController.m
//  OpenGL_ES_005
//
//  Created by GhostClock on 2020/8/2.
//  Copyright © 2020 GhostClock. All rights reserved.
//

#import "ViewController.h"
#import "GCView.h"

@interface ViewController ()

@property(nonatomic, strong) GCView *cView;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
//    self.view = GCView.new;
    self.cView = (GCView *)self.view;
    NSLog(@"222");
}


@end
