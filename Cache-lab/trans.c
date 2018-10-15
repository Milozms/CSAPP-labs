/* 
 Name = Maosen Zhang 
 ID = 1500012898
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"
#define B1 8
#define B2 16
int is_transpose(int M, int N, int A[N][M], int B[M][N]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	//32 bytes per set ==> 8 int per set
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,i1,j1,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
	if(M==32) {
	//0x80 bytes per row
	// 1 set for A
	//at most 8 sets can be used for B
	//may conflict with the set for A, so load to local variables
		for(i=0;i<N;i+=B1)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B1 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	else if(M==64){
	//0x100 bytes per row
	// 1 set for A
	//at most 4 sets can be used for B
	//may conflict with the set for A, so load to local variables
		for(i=0;i<64;i+=8)
			for(j=0;j<64;j+=8){
				for(i1=i;i1<i+4;++i1){
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];
					//try to fully exploit the load for A[i][j]~A[i+3][j]
					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;
					//temporarily use these spaces storing data 
					//(the right-up part of the block)
					B[j][i1+4]=tmp4;
					B[j+1][i1+4]=tmp5;
					B[j+2][i1+4]=tmp6;
					B[j+3][i1+4]=tmp7;
				}
				//after that, A[i][j]~A[i+3][j] will not be used again
				//recover the temporary space
				for(j1=j;j1<j+4;++j1){
					tmp0=B[j1][i+4];
					tmp1=B[j1][i+5];
					tmp2=B[j1][i+6];
					tmp3=B[j1][i+7];

					//try to fully use the load for B[j1][i]
					B[j1][i+4]=A[i+4][j1];
					B[j1][i+5]=A[i+5][j1];
					B[j1][i+6]=A[i+6][j1];
					B[j1][i+7]=A[i+7][j1];
					//after that, B[j1][i] will not be used again
					//then B[j1+4][i] evict B[j1][i]
					B[j1+4][i]=tmp0;
					B[j1+4][i+1]=tmp1;
					B[j1+4][i+2]=tmp2;
					B[j1+4][i+3]=tmp3;
				}
				//deal with the right-down part
				for(i1=i+4;i1<i+8;++i1){
					tmp0=A[i1][j+4];
					tmp1=A[i1][j+5];
					tmp2=A[i1][j+6];
					tmp3=A[i1][j+7];

					B[j+4][i1]=tmp0;
					B[j+5][i1]=tmp1;
					B[j+6][i1]=tmp2;
					B[j+7][i1]=tmp3;
				}
			}
	}
	else{
	//for A: 0x0f4 bytes per row
	//for B: 0x10c bytes per row
	//there will not be many conflicts
	//16*8 block (for 8 ints per set)
		for(i=0;i<N;i+=B2)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B2 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	ENSURES(is_transpose(M, N, A, B));
}
/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
//the following several functions are some previous strategy I tried
void transpose_1393(int M, int N, int A[N][M], int B[M][N])
{
	//32 bytes per set ==> 8 int per set
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,i1,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp;
	if(M==32) {
	//0x80 bytes per row
	// 1 set for A
	//at most 8 sets can be used for B
	//may conflict with the set for A, so load to local variables
		for(i=0;i<N;i+=B1)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B1 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	else if(M==64){
	//0x100 bytes per row
	// 1 set for A
	//at most 4 sets can be used for B
	//may conflict with the set for A, so load to local variables
	//use B[56][0]~B[63][63] as middle variables
	tmp=0;	
		for(i=0;i<64;i+=8)			
			for(j=0;j<56;j+=8){
				while((((&B[56][tmp]-&B[j][i])>>3)&0x7)==0 
					|| (((&B[56][tmp]-&A[i][j])>>3)&0x7)==0){
					if(tmp==56)
						tmp=0;
					else tmp+=8;
				}
				//B[56][tmp] not conflict with B[j][i] and A[i][j]
				for(i1=0;i1<8;++i1){
					B[j][i+i1]=A[i+i1][j];
					B[j+1][i+i1]=A[i+i1][j+1];
					B[j+2][i+i1]=A[i+i1][j+2];
					B[j+3][i+i1]=A[i+i1][j+3];
					
					B[j][i+i1]=A[i+i1][j];
					B[j+1][i+i1]=A[i+i1][j+1];
					B[j+2][i+i1]=A[i+i1][j+2];
					B[j+3][i+i1]=A[i+i1][j+3];
					//temporarily use these space
					B[56][tmp+i1]=A[i+i1][j+4];
					B[57][tmp+i1]=A[i+i1][j+5];
					B[58][tmp+i1]=A[i+i1][j+6];
					B[59][tmp+i1]=A[i+i1][j+7];
				}
				for(i1=0;i1<8;++i1){
					B[j+4][i+i1]=B[56][tmp+i1];
					B[j+5][i+i1]=B[57][tmp+i1];
					B[j+6][i+i1]=B[58][tmp+i1];
					B[j+7][i+i1]=B[59][tmp+i1];
				}

		}
		j=56;tmp=56;
			for(i=0;i<56;i+=8){
				//B[56][tmp] not conflict with B[j][i] and A[i][j]
				for(i1=0;i1<8;++i1){
					B[j][i+i1]=A[i+i1][j];
					B[j+1][i+i1]=A[i+i1][j+1];
					B[j+2][i+i1]=A[i+i1][j+2];
					B[j+3][i+i1]=A[i+i1][j+3];
					//temporarily use these space
					B[56][tmp+i1]=A[i+i1][j+4];
					B[57][tmp+i1]=A[i+i1][j+5];
					B[58][tmp+i1]=A[i+i1][j+6];
					B[59][tmp+i1]=A[i+i1][j+7];
				}
				for(i1=0;i1<8;++i1){
					B[j+4][i+i1]=B[56][tmp+i1];
					B[j+5][i+i1]=B[57][tmp+i1];
					B[j+6][i+i1]=B[58][tmp+i1];
					B[j+7][i+i1]=B[59][tmp+i1];
				}
			}

		i=56;j=56;
			for(i1=i;i1<i+7 && i1<N ; i1++){
					//by the way we can reduce 2 load for A[i+7] and A[i+6]
						tmp0=A[i1][j];
						tmp1=A[i1][j+1];
						tmp2=A[i1][j+2];
						tmp3=A[i1][j+3];
						if(i1==i+6){
							tmp4=A[i1][j+4];
							tmp5=A[i1][j+5];
							tmp6=A[i1][j+6];
							tmp7=A[i1][j+7];
						}
						B[j][i1]=tmp0;
						B[j+1][i1]=tmp1;
						B[j+2][i1]=tmp2;
						B[j+3][i1]=tmp3;
					}

					B[j][i+7]=A[i+7][j];
					B[j+1][i+7]=A[i+7][j+1];
					B[j+2][i+7]=A[i+7][j+2];
					B[j+3][i+7]=A[i+7][j+3];

					B[j+4][i+7]=A[i+7][j+4];
					B[j+5][i+7]=A[i+7][j+5];
					B[j+6][i+7]=A[i+7][j+6];
					B[j+7][i+7]=A[i+7][j+7];

					B[j+4][i+6]=tmp4;
					B[j+5][i+6]=tmp5;
					B[j+6][i+6]=tmp6;
					B[j+7][i+6]=tmp7;
					
					for(i1=i+5;i1>=i;i1--){
						tmp4=A[i1][j+4];
						tmp5=A[i1][j+5];
						tmp6=A[i1][j+6];
						tmp7=A[i1][j+7];

						B[j+4][i1]=tmp4;
						B[j+5][i1]=tmp5;
						B[j+6][i1]=tmp6;
						B[j+7][i1]=tmp7;
					}

	}
	else{
	//for A: 0x0f4 bytes per row
	//for B: 0x10c bytes per row
	//there will not be many conflicts
	//16*8 block (for 8 ints per set)
		for(i=0;i<N;i+=B2)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B2 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	ENSURES(is_transpose(M, N, A, B));
}
//the following several functions are some previous strategy I tried
void transpose_1403(int M, int N, int A[N][M], int B[M][N])
{
	//32 bytes per set ==> 8 int per set
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,i1,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp;
	if(M==32) {
	//0x80 bytes per row
	// 1 set for A
	//at most 8 sets can be used for B
	//may conflict with the set for A, so load to local variables
		for(i=0;i<N;i+=B1)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B1 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	else if(M==64){
	//0x100 bytes per row
	// 1 set for A
	//at most 4 sets can be used for B
	//may conflict with the set for A, so load to local variables
		
			for(i=0;i<N;i+=8)			
				for(j=0;j<M;j+=8){											
					for(i1=i;i1<i+8 && i1<N ; i1++){
						//if B[j][i] conflict with A[i1][j]
						if((&B[j][i]-&A[i1][j]+256)%256==0){
							tmp=A[i1][j];
							B[j+1][i1]=A[i1][j+1];
							B[j+2][i1]=A[i1][j+2];
							B[j+3][i1]=A[i1][j+3];
						}
						//if B[j+1][i] conflict with A[i1][j]
						else if((&B[j][i]-&A[i1][j]+256)%256==192){
							B[j][i1]=A[i1][j];
							tmp=A[i1][j+1];
							B[j+2][i1]=A[i1][j+2];
							B[j+3][i1]=A[i1][j+3];
						}
						//if B[j+2][i] conflict with A[i1][j]
						else if((&B[j][i]-&A[i1][j]+256)%256==128){
							B[j][i1]=A[i1][j];
							B[j+1][i1]=A[i1][j+1];
							tmp=A[i1][j+2];
							B[j+3][i1]=A[i1][j+3];
						}
						//if B[j+3][i] conflict with A[i1][j]
						else {
							B[j][i1]=A[i1][j];
							B[j+1][i1]=A[i1][j+1];
							B[j+2][i1]=A[i1][j+2];
							tmp=A[i1][j+3];
						}						
						//by the way we can reduce 2 load for A[i+7] and A[i+6]
						if(i1==i+6){
							tmp4=A[i1][j+4];
							tmp5=A[i1][j+5];
							tmp6=A[i1][j+6];
							tmp7=A[i1][j+7];
						}
						if(i1==i+7){
							tmp0=A[i1][j+4];
							tmp1=A[i1][j+5];
							tmp2=A[i1][j+6];
							tmp3=A[i1][j+7];
						}
						//if B[j][i] conflict with A[i1][j]
						if((&B[j][i]-&A[i1][j]+256)%256==0){
							B[j][i1]=tmp;
						}
						//if B[j+1][i] conflict with A[i1][j]
						else if((&B[j][i]-&A[i1][j]+256)%256==192){
							B[j+1][i1]=tmp;
						}
						//if B[j+2][i] conflict with A[i1][j]
						else if((&B[j][i]-&A[i1][j]+256)%256==128){
							B[j+2][i1]=tmp;
						}
						//if B[j+3][i] conflict with A[i1][j]
						else {
							B[j+3][i1]=tmp;
						}
					}

						B[j+4][i+6]=tmp4;
						B[j+5][i+6]=tmp5;
						B[j+6][i+6]=tmp6;
						B[j+7][i+6]=tmp7;

						B[j+4][i+7]=tmp0;
						B[j+5][i+7]=tmp1;
						B[j+6][i+7]=tmp2;
						B[j+7][i+7]=tmp3;
						
						for(i1=i+5;i1>=i;i1--){
							tmp4=A[i1][j+4];
							tmp5=A[i1][j+5];
							tmp6=A[i1][j+6];
							tmp7=A[i1][j+7];

							B[j+4][i1]=tmp4;
							B[j+5][i1]=tmp5;
							B[j+6][i1]=tmp6;
							B[j+7][i1]=tmp7;
						}
				
				//if B[j][i] not conflict with A[i][j]
									
			}
	}
	else{
	//for A: 0x0f4 bytes per row
	//for B: 0x10c bytes per row
	//there will not be many conflicts
	//16*8 block (for 8 ints per set)
		for(i=0;i<N;i+=B2)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B2 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	ENSURES(is_transpose(M, N, A, B));
}
//the following several functions are some previous strategy I tried
void transpose_1411_1(int M, int N, int A[N][M], int B[M][N]){

	//32 bytes per set ==> 8 int per set
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,i1,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
	if(M==32) {
	//0x80 bytes per row
	// 1 set for A
	//at most 8 sets can be used for B
	//may conflict with the set for A, so load to local variables
		for(i=0;i<N;i+=B1)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B1 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	else if(M==64){
	//0x100 bytes per row
	// 1 set for A
	//at most 4 sets can be used for B
	//may conflict with the set for A, so load to local variables
			
			//version A: 1411 misses
			for(i=0;i<N;i+=8)
				for(j=0;j<M;j+=8){
					for(i1=i;i1<i+8 && i1<N ; i1++){
					//by the way 
					//we can reduce 2 load for A[i+7] and A[i+6]
						tmp0=A[i1][j];
						tmp1=A[i1][j+1];
						tmp2=A[i1][j+2];
						tmp3=A[i1][j+3];
						if(i1==i+6){
							tmp4=A[i1][j+4];
							tmp5=A[i1][j+5];
							tmp6=A[i1][j+6];
							tmp7=A[i1][j+7];
						}
						B[j][i1]=tmp0;
						B[j+1][i1]=tmp1;
						B[j+2][i1]=tmp2;
						B[j+3][i1]=tmp3;
						if(i1==i+7){
							tmp0=A[i1][j+4];
							tmp1=A[i1][j+5];
							tmp2=A[i1][j+6];
							tmp3=A[i1][j+7];
						}

					}


					B[j+4][i+6]=tmp4;
					B[j+5][i+6]=tmp5;
					B[j+6][i+6]=tmp6;
					B[j+7][i+6]=tmp7;

					B[j+4][i+7]=tmp0;
					B[j+5][i+7]=tmp1;
					B[j+6][i+7]=tmp2;
					B[j+7][i+7]=tmp3;
					
					for(i1=i+5;i1>=i;i1--){
						tmp4=A[i1][j+4];
						tmp5=A[i1][j+5];
						tmp6=A[i1][j+6];
						tmp7=A[i1][j+7];

						B[j+4][i1]=tmp4;
						B[j+5][i1]=tmp5;
						B[j+6][i1]=tmp6;
						B[j+7][i1]=tmp7;
					}
				}
	}
	else{
	//for A: 0x0f4 bytes per row
	//for B: 0x10c bytes per row
	//there will not be many conflicts
	//16*8 block (for 8 ints per set)
		for(i=0;i<N;i+=B2)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B2 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	ENSURES(is_transpose(M, N, A, B));
}
//the following several functions are some previous strategy I tried
void transpose_1411_2(int M, int N, int A[N][M], int B[M][N])
{
	//32 bytes per set ==> 8 int per set
	REQUIRES(M > 0);
	REQUIRES(N > 0);
	int i,j,i1,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
	if(M==32) {
	//0x80 bytes per row
	// 1 set for A
	//at most 8 sets can be used for B
	//may conflict with the set for A, so load to local variables
		for(i=0;i<N;i+=B1)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B1 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	else if(M==64){
	//0x100 bytes per row
	// 1 set for A
	//at most 4 sets can be used for B
	//may conflict with the set for A, so load to local variables
			for(i=0;i<N;i+=8)
				for(j=0;j<M;j+=8){
					for(i1=i;i1<i+7 && i1<N ; i1++){
					//by the way we can reduce 2 load for A[i+7] and A[i+6]
						tmp0=A[i1][j];
						tmp1=A[i1][j+1];
						tmp2=A[i1][j+2];
						tmp3=A[i1][j+3];
						if(i1==i+6){
							tmp4=A[i1][j+4];
							tmp5=A[i1][j+5];
							tmp6=A[i1][j+6];
							tmp7=A[i1][j+7];
						}
						B[j][i1]=tmp0;
						B[j+1][i1]=tmp1;
						B[j+2][i1]=tmp2;
						B[j+3][i1]=tmp3;
					}

					B[j][i+7]=A[i+7][j];
					B[j+1][i+7]=A[i+7][j+1];
					B[j+2][i+7]=A[i+7][j+2];
					B[j+3][i+7]=A[i+7][j+3];

					B[j+4][i+7]=A[i+7][j+4];
					B[j+5][i+7]=A[i+7][j+5];
					B[j+6][i+7]=A[i+7][j+6];
					B[j+7][i+7]=A[i+7][j+7];

					B[j+4][i+6]=tmp4;
					B[j+5][i+6]=tmp5;
					B[j+6][i+6]=tmp6;
					B[j+7][i+6]=tmp7;
					
					for(i1=i+5;i1>=i;i1--){
						tmp4=A[i1][j+4];
						tmp5=A[i1][j+5];
						tmp6=A[i1][j+6];
						tmp7=A[i1][j+7];

						B[j+4][i1]=tmp4;
						B[j+5][i1]=tmp5;
						B[j+6][i1]=tmp6;
						B[j+7][i1]=tmp7;
					}
				}
	}
	else{
	//for A: 0x0f4 bytes per row
	//for B: 0x10c bytes per row
	//there will not be many conflicts
	//16*8 block (for 8 ints per set)
		for(i=0;i<N;i+=B2)
			for(j=0;j<M;j+=B1)
				for(i1=i;i1<i+B2 && i1<N ;i1++){
					
					tmp0=A[i1][j];
					tmp1=A[i1][j+1];
					tmp2=A[i1][j+2];
					tmp3=A[i1][j+3];

					tmp4=A[i1][j+4];
					tmp5=A[i1][j+5];
					tmp6=A[i1][j+6];
					tmp7=A[i1][j+7];

					B[j][i1]=tmp0;
					B[j+1][i1]=tmp1;
					B[j+2][i1]=tmp2;
					B[j+3][i1]=tmp3;

					B[j+4][i1]=tmp4;
					B[j+5][i1]=tmp5;
					B[j+6][i1]=tmp6;
					B[j+7][i1]=tmp7;
					
				}
	}
	ENSURES(is_transpose(M, N, A, B));
}
/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, tmp;

	REQUIRES(M > 0);
	REQUIRES(N > 0);

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			tmp = A[i][j];
			B[j][i] = tmp;
		}
	}    

	ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
	/* Register your solution function */
	//registerTransFunction(transpose_submit, transpose_submit_desc); 

	//registerTransFunction(transpose_test1, transpose_test1_desc); 
	/* Register any additional transpose functions */
	registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
	int i, j;

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; ++j) {
			if (A[i][j] != B[j][i]) {
				return 0;
			}
		}
	}
	return 1;
}

