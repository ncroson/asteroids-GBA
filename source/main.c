
#include <stdio.h>
#include <tonc.h>


#include "ship.h"
#include "bullet.h"
#include "bA1.h"
#include "bA2.h"
#include "bA3.h"

#include "part1.h"
#include "part2.h"
#include "part3.h"


OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer= (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *ship = &obj_buffer[0];
int x = 112;
int y = 72;

OBJ_ATTR *part1 = &obj_buffer[21];
OBJ_ATTR *part2 = &obj_buffer[22];
OBJ_ATTR *part3 = &obj_buffer[23];

int px1, px2, px3 = 0;
int py1, py2, py3 = 0;
bool shipDestroyed;

OBJ_ATTR *bullet[11];
int bx[11];
int by[11];

OBJ_ATTR *bA[11];
int bAx[11];
int bAy[11];
int bAtid = 0;
bool asteroidShot[11];


int aff_value = 0;
int set_aff_value = 0;
int shot_aff_value[11];


bool shotFired[11];
int shotCount = 0;

int frames = 0;

void Setup()
{

	
	
	obj_set_attr(ship,
		ATTR0_SQUARE | ATTR0_AFF,			//square affine sprite
		ATTR1_SIZE_8 | ATTR1_AFF_ID(0),		//8x8, using obj_aff[0]
		0 | 0);								//palbank 0, tile 0
	
	obj_set_pos(ship, x, y);
	obj_aff_identity(&obj_aff_buffer[0]);
	
	ship->attr0 ^= ATTR0_AFF_DBL_BIT;
	
	//obj_unhide(ship, 0);
	
	obj_set_attr(part1,
		ATTR0_SQUARE,			//square affine sprite
		ATTR1_SIZE_8,		//8x8, using obj_aff[0]
		0 | 1);			
	
	obj_set_attr(part2,
		ATTR0_SQUARE,			//square affine sprite
		ATTR1_SIZE_8,		//8x8, using obj_aff[0]
		0 | 2);	
		
	obj_set_attr(part3,
		ATTR0_SQUARE,			//square affine sprite
		ATTR1_SIZE_8,		//8x8, using obj_aff[0]
		0 | 3);	
		
	obj_hide(part1);
	obj_hide(part2);
	obj_hide(part3);
	
	shipDestroyed = false;
	
	//initalize big Asteroids
	for(int i = 1; i <= 10; i++)
	{
		bAtid = qran_range(1, 4);
		
		obj_set_attr(bA[i],
			ATTR0_SQUARE,			//square affine sprite
			ATTR1_SIZE_16,			//16x16, using obj_aff[0]
			0 | (bAtid*4+11));			//palbank 0, tile 0
		
		
		
		
		bAx[i]= qran_range(0, 240);
		
		bAy[i] = qran_range(0, 160);
		
		if(bAx[i] > x - 24 && bAx[i] < x + 32)
			bAx[i] += 64;
		if(bAx[i] > y - 24 && bAy[i] < y + 32)
			bAy[i] += 64;
		
		obj_set_pos(bA[i], bAx[i], bAy[i]);
		
		//obj_aff_identity(&obj_aff_buffer[i]);
		
		//bA[i]->attr0 ^= ATTR0_AFF_DBL_BIT;
	}
	
	
	
	for(int i = 1; i <= 10; i++)
	obj_set_attr(bullet[i],
		ATTR0_SQUARE,			//square 
		ATTR1_SIZE_8,			//8x8
		0 | (4+i));					//palbank 0, tile 4
	
	
	for(int i = 1; i <= 10; i++)
	{	
		obj_hide(bullet[i]);
		
		bx[i] = 250;
		by[i] = 180;
		
		shotFired[i] = false;
	
		asteroidShot[i] = false;
	}
	
}

void Draw()
{
	VBlankIntrWait();
	
	obj_set_pos(ship, x, y);
	
	for(int i = 1; i <= 10; i++)
		obj_set_pos(bA[i], bAx[i], bAy[i]);
	
	//if(shotFired[shotCount] == false);
	//	obj_hide(bullet[shotCount]);
		
	//if(shotFired[shotCount] == true);
	//	obj_unhide(bullet[shotCount], 0);
	
	obj_copy(obj_mem, obj_buffer, 128);
	obj_aff_copy(obj_aff_mem, obj_aff_buffer, 15);

}

void Input()
{
	
	OBJ_AFFINE *oaff_curr= &obj_aff_buffer[0];
	OBJ_AFFINE *oaff_base= &obj_aff_buffer[1];
	OBJ_AFFINE *oaff_new= &obj_aff_buffer[2];
	
	key_poll();
	
	int diff = 512;
	
	
	if(key_held(KEY_L) || key_held(KEY_LEFT))
		aff_value += diff;
	if(key_held(KEY_R) || key_held(KEY_RIGHT))
		aff_value -= diff;
	
	if(aff_value == 65536)
		aff_value = 0;
		
	if(aff_value == -65536)
		aff_value = 0;
	
	if(key_held(KEY_A))
	{
		y -=  lu_sin(aff_value)/4096;
		x +=  lu_cos(aff_value)/4096;
		set_aff_value = aff_value;
		
	}	
	
	if(set_aff_value != 0)
	{
		y -= lu_sin(set_aff_value)/2048;
		x += lu_cos(set_aff_value)/2048;
		
	}
	
	if(key_hit(KEY_B))
	{
		shotCount++;
		by[shotCount] = y + 4;
		bx[shotCount] = x + 4;
		shotFired[shotCount] = true;
		shot_aff_value[shotCount] = aff_value;
		if(shotCount >= 10)
			shotCount = 0;
	}	
	
	
	
	//aff_value += (key_is_down(KEY_L) ? diff : -diff);
	
	obj_aff_rotate(oaff_new, aff_value);
	obj_aff_copy(oaff_curr, oaff_base, 1);
	obj_aff_postmul(oaff_curr, oaff_new);

}

void Logic()
{
	
	//boundries
	if(x < 0)
		x = 224;
	if(x > 224 && x < 240)
		x = 0;
	if(y < 0)
		y = 144;
	if(y > 144)
		y = 0;
	
	
	
	//bullets
	for(int i = 1; i <= 10; i++)
	{
		if(shotFired[i] == true)
		{
			obj_unhide(bullet[i], 0);
			by[i] -= lu_sin(shot_aff_value[i])/1024;
			bx[i] += lu_cos(shot_aff_value[i])/1024;
				
			if(bx[i] < 0)
			{	
				shotFired[i] = false;
				obj_hide(bullet[i]);
				bx[i] = 250;
				
			}
			if(bx[i] > 232)
			{
				shotFired[i] = false;
				obj_hide(bullet[i]);
				bx[i] = 250;
			}
			if(by[i] < 0)
			{
				shotFired[i] = false;
				obj_hide(bullet[i]);
				by[i] = 180;
			}
			if(by[i] > 152)
			{
				shotFired[i] = false;
				obj_hide(bullet[i]);
				by[i] = 180;
			}	
			obj_set_pos(bullet[i], bx[i], by[i]);
			
		}
		
		
		
		
	}
	
	//asteroid movement
	if(frames == 2)
	{
		bAy[1] -=  2;
		bAx[1] -=  3;
			
		bAy[2] +=  3;
		bAx[2] +=  2;
			
		bAy[3] -=  3;
		bAx[3] +=  2;
			
		bAy[4] -=  1;
		bAx[4] -=  2;
		
		bAy[5] +=  3;
		bAx[5] +=  1;
			
		bAy[6] +=  1;
		bAx[6] -=  3;
			
		bAy[7] -=  1;
		bAx[7] +=  2;
			
		bAy[8] -=  3;
		bAx[8] +=  2;
		
		bAy[9] +=  1;
		bAx[9] -=  2;
			
		bAy[10] -=  3;
		bAx[10] +=  2;
	}
		
	/*
	if(asteroidShot[1] == false)
	{
		bAy[1] -=  .3;
		bAx[1] -=  .6;
	}
	if(asteroidShot[2] == false)
	{
		bAy[2] +=  .4;
		bAx[2] +=  .3;
	}
	if(asteroidShot[3] == false)
	{
		bAy[3] -=  .4;
		bAx[3] +=  .2;
	}
	if(asteroidShot[4] == false)
	{
		bAy[4] -=  .6;
		bAx[4] -=  .4;
	}
	if(asteroidShot[5] == false)
	{
		bAy[5] +=  .6;
		bAx[5] +=  .5;
	}
	if(asteroidShot[6] == false)
	{
		bAy[6] +=  .6;
		bAx[6] -=  .3;
	}
	if(asteroidShot[7] == false)
	{
		bAy[7] -=  .4;
		bAx[7] +=  .2;
	}
	if(asteroidShot[8] == false)
	{
		bAy[8] -=  .4;
		bAx[8] +=  .6;
	}
	if(asteroidShot[9] == false)
	{
		bAy[9] +=  .6;
		bAx[9] -=  .5;
	}
	if(asteroidShot[10] == false)
	{
		bAy[10] -=  .3;
		bAx[10] +=  .5;
	}
	*/
	
	/*
	if(asteroidShot[1] == true)
	{
		bAy[1] = 18;
		bAx[1] = 250;
	}
	if(asteroidShot[2] == true)
	{
		bAy[2] =  18;
		bAx[2] =  250;
	}
	
	if(asteroidShot[3] == true)
	{
		bAy[3] =  18;
		bAx[3] =  250;
	}
	if(asteroidShot[4] == true)
	{
		bAy[4] =  18;
		bAx[4] =  250;
	}
	if(asteroidShot[5] == true)
	{
		bAy[5] =  18;
		bAx[5] =  250;
	}
	if(asteroidShot[6] == true)
	{
		bAy[6] =  18;
		bAx[6] =  250;
	}
	if(asteroidShot[7] == true)
	{
		bAy[7] =  18;
		bAx[7] =  250;
	}
	if(asteroidShot[8] == true)
	{
		bAy[8] =  18;
		bAx[8] =  250;
	}
	if(asteroidShot[9] == true)
	{
		bAy[9] =  18;
		bAx[9] =  250;
	}
	if(asteroidShot[10] == true)
	{
		bAy[10] =  18;
		bAx[10] =  250;
	}
	*/
	
	
	//all asteroids shot reset
	if(asteroidShot[1] == true && asteroidShot[2] == true && asteroidShot[3] == true && asteroidShot[4] == true && asteroidShot[5] == true
		&& asteroidShot[6] == true && asteroidShot[7] == true && asteroidShot[8] == true && asteroidShot[9] == true && asteroidShot[10] == true)
		Setup();
	
	
	
	//asteroid boundries
	for(int i = 1; i <=10; i++)
	{
		
		//asteroid boundries
		if(bAx[i] < 0)
			bAx[i] = 224;
		if(bAx[i] > 224 && bAx[i] < 240)
			bAx[i] = 0;
		if(bAy[i] < 0)
			bAy[i] = 144;
		if(bAy[i] > 144)
			bAy[i] = 0;
	}
	
	
	
	//asteroid bullet collision
	for(int i = 1; i <= 10; i++)
	{
		
			if(bx[1] >= bAx[i] - 4 && bx[1] + 8 <= bAx[i] + 20 && by[1] >= bAy[i]-4 && by[1] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}	
			if(bx[2] >= bAx[i] -4 && bx[2] + 8 <= bAx[i] + 20 && by[2] >= bAy[i]-4 && by[2] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[3] >= bAx[i] -4 && bx[3] + 8 <= bAx[i] + 20 && by[3] >= bAy[i]-4 && by[3] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[4] >= bAx[i] -4&& bx[4] + 8 <= bAx[i] + 20 && by[4] >= bAy[i]-4 && by[4] + 8 <= bAy[i] + 20)
			{	
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[5] >= bAx[i] -4&& bx[5] + 8 <= bAx[i] + 20 && by[5] >= bAy[i]-4 && by[5] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[6] >= bAx[i] -4&& bx[6] + 8 <= bAx[i] + 20 && by[6] >= bAy[i]-4 && by[6] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[7] >= bAx[i] -4&& bx[7] + 8 <= bAx[i] + 20 && by[7] >= bAy[i]-4 && by[7] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[8] >= bAx[i] -4&& bx[8] + 8 <= bAx[i] + 20 && by[8] >= bAy[i]-4 && by[8] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[9] >= bAx[i] -4&& bx[9] + 8 <= bAx[i] + 20 && by[9] >= bAy[i]-4 && by[9] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			if(bx[10] >= bAx[i] -4&& bx[10] + 8 <= bAx[i] + 20 && by[10] >= bAy[i]-4 && by[10] + 8 <= bAy[i] + 20)
			{
				obj_hide(bA[i]);
				asteroidShot[i] = true;
			}
			
		
			if(asteroidShot[i] == true)
			{
				bAy[i] = 18;
				bAx[i] = 250;
			}
			
			
			//asteroid ship collision
			if(x - 2 > bAx[i] - 8 && x + 10 < bAx[i] + 22 && y - 2 > bAy[i] - 8 && y +  10 <= bAy[i] + 22)
			{	//destroy ship
			
			
			obj_unhide(part1, 0);
			obj_unhide(part2, 0);
			obj_unhide(part3, 0);
			
			px1 = x + 4;
			px2 = x + 4;
			px3 = x + 4;
			
			py1 = y + 4;
			py2 = y + 4;
			py3 = y + 4;
			
			shipDestroyed = true;
			
			obj_hide(ship);
			
			}
	
	}
	
	
	
	/*
	//asteroid ship collision
	for(int i = 1; i <= 10; i++)
	{
		if(x -2 > bAx[i] - 6 && x + 10 < bAx[i] + 22 && y -2 > bAy[i] - 6 && y +  10 <= bAy[i] + 22)
		{	//destroy ship
			
			
			obj_unhide(part1, 0);
			obj_unhide(part2, 0);
			obj_unhide(part3, 0);
			
			px1 = x + 4;
			px2 = x + 4;
			px3 = x + 4;
			
			py1 = y + 4;
			py2 = y + 4;
			py3 = y + 4;
			
			shipDestroyed = true;
			
			obj_hide(ship);
			
			
			
			
		}
	}
	*/
	
	if(shipDestroyed == true)
	{
		py1--;
		py2++;
		px3--;
		
		obj_set_pos(part1, px1, py1);
		obj_set_pos(part2, px2, py2);
		obj_set_pos(part3, px3, py3);
		
		x = 250;
		y = 50;
		
		if(py1 == 0 || py2 == 160 || px3 == 0)
		{
		
			//obj_hide(part1);
			//obj_hide(part2);
			//obj_hide(part3);
			
			
			
			x = 112;
			y = 72;
			set_aff_value = 0;
			Setup();
			
		
		}
	}
	
}

int main()
{
	// Init interrupts and VBlank irq.
	irq_init(NULL);
	irq_add(II_VBLANK, NULL);
	
	// Video mode 0, enable bg 0.
	REG_DISPCNT= DCNT_BG0 |DCNT_OBJ | DCNT_OBJ_1D;
	
	// Init 4bpp vwf text on bg 0.
	tte_init_chr4c(0, 			// BG 0
		BG_CBB(0)|BG_SBB(31),	// Charblock 0; screenblock 31
		0xF000,					// Screen-entry offset
		bytes2word(1,2,0,0),	// Color attributes.
		CLR_YELLOW, 			// Yellow text
		&verdana9Font,			// Verdana 9 font
		NULL					// Use default chr4 renderer
		);
	
	// Initialize use of stdio.
	tte_init_con();
	
	// Printf something at 96,72
	//tte_printf("#{P:96,72}Hello World!");
	
	memcpy32(tile_mem[4], shipTiles, shipTilesLen/4);
	memcpy32(pal_obj_mem, shipPal, shipPalLen/4);
	
	for(int i = 1; i <= 10; i++)
	{
		memcpy(&tile_mem[4][4+i], bulletTiles, bulletTilesLen);
		memcpy(pal_obj_mem, bulletPal, bulletPalLen);
		
		bullet[i] = &obj_buffer[i];
	}
	
	//big asteroids
	memcpy(&tile_mem[4][15], bA1Tiles, bA1TilesLen);
	memcpy(pal_obj_mem, bA1Pal, bA1PalLen);
	memcpy(&tile_mem[4][19], bA2Tiles, bA2TilesLen);
	memcpy(pal_obj_mem, bA1Pal, bA1PalLen);
	memcpy(&tile_mem[4][23], bA3Tiles, bA3TilesLen);
	memcpy(pal_obj_mem, bA3Pal, bA3PalLen);
	
	for(int i = 1; i <= 10; i++)
	{
		bA[i] = &obj_buffer[10 + i];
	
	}
	
	//ship parts
	memcpy(&tile_mem[4][1], part1Tiles, part1TilesLen);
	memcpy(pal_obj_mem, part1Pal, part1PalLen);
	memcpy(&tile_mem[4][2], part2Tiles, part2TilesLen);
	memcpy(pal_obj_mem, part2Pal, part2PalLen);
	memcpy(&tile_mem[4][3], part3Tiles, part3TilesLen);
	memcpy(pal_obj_mem, part3Pal, part3PalLen);
	
	oam_init(obj_buffer, 128);

	
	Setup();
	
	
	
	while(1)
	{
		
		
		Draw();
		Input();
		Logic();
		
		frames++;
		if(frames == 4)
			frames = 0;
		
		if(key_hit(KEY_START))
		{
			x = 112;
			y = 72;
			set_aff_value = 0;
			Setup();
		}
	}
	
	return 0;
}
