/*
    main_video.h - video driver demo program

    Copyright (C) 2011 Hugo Perquin - http://blog.perquin.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "video.h"

void video_cg(img_struct* img) 
{
	int h=img->h;
	int w=img->w;
	int n=w*h;
	
	int p=0;
	int px=0;
	int py=0;
	int x=0;
	int y=0;
	unsigned char* buf = img->buf;
	for(int i=0;i<n;i++) {
		p+=buf[i];
		px+=x*buf[i];
		py+=y*buf[i];
		x++;
		if(x==w) {
			x=0;
			y++;
		}
	}
	float cg_x = (float)px/p;
	float cg_y = (float)py/p;
	printf("x=%10.6f y=%10.6f\n",cg_x,cg_y);
}

void video_blocksum(img_struct* img1, img_struct* img2, int* dx_out, int* dy_out) 
{
	int h=img1->h;
	int w=img1->w;
	int n=w*h;
	unsigned char* buf1 = img1->buf;
	unsigned char* buf2 = img2->buf;
	
	int dmax = 3;
	int min_sum = 2000000000;
	int min_dx = -99;
	int min_dy = -99;
	for(int dy=-dmax;dy<=dmax;dy++) {
		for(int dx=-dmax;dx<=dmax;dx++) {
			int sum=0;
			for(int y=dmax;y<h-dmax;y++) {
				int i1 = y*w + dmax;
				int i2 = (y+dy)*w + dmax+dx;
				for(int x=dmax;x<w-dmax;x++) {
					//printf("x=%d y=%d i1=%d i2=%d\n",x,y,i1,i2);
					sum += abs(buf1[i1] - buf2[i2]);
					i1++;
					i2++;
				}
			}
			if(min_sum>sum) {
				min_sum = sum;
				min_dx = dx;
				min_dy = dy;
			}
		}
	}
			
	*dx_out=min_dx;
	*dy_out=min_dy;
}

int main(int argc,char ** argv)
{
	vid_struct vid;
	vid.device = (char*)"/dev/video1";
	vid.w=176;
	vid.h=144;
	vid.n_buffers = 4;
	video_Init(&vid);

	img_struct* img_old = video_CreateImage(&vid);
	img_struct* img_new = video_CreateImage(&vid);
	
	int dx,dy;
	int x=0,y=0;

	video_GrabImage(&vid, img_old);
    for (;;) {
		video_GrabImage(&vid, img_new);

		//process
		video_blocksum(img_old, img_new, &dx, &dy);
		x+=dx;
		y+=dy;
		printf("diff between img %5d and %5d -> dx=%2d dy=%2d x=%4d y=%4d\n",img_old->seq,img_new->seq,dx,dy,x,y);
		
		if(dx!=0 || dy!=0) {
			//swap buffers
			img_struct* tmp = img_new;
			img_new=img_old;
			img_old=tmp;
		}
    }

	video_Close(&vid);
	
    return 0;
}