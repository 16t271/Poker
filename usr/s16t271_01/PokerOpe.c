//====================================================================
//  工学部「情報環境実験２」(富永)  C言語プログラミング  例題
//  ポーカーゲームの戦略
//--------------------------------------------------------------------
//  Poker  usr/s16t271_01/  PokerOpe.c
//  Linux CentOS 6.7  GCC 4.4.7
//--------------------------------------------------------------------
//  安藤研究室  matsubara 松原香太
//  2019.12.03
//====================================================================


//====================================================================
//  仕様
//====================================================================

/*--------------------------------------------------------------------

手札、場札、チェンジ数、テイク数、捨札を引数とし、捨札を決める。
返却値は、捨札の位置である。-1のときは、交換しないで、手札を確定させる。
関数 strategy() は、戦略のインタフェースであり、この関数内で、
実際の戦略となる関数を呼び出す。手札と捨札は、不正防止のため、
変更不可なので、局所配列にコピーして、整列などの処理を行う。
複数の戦略を比較したり、パラメタを変化させて、より強い戦略を目指す。

---------------------------------------------------------------------*/


//====================================================================
//  前処理
//====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Poker.h"

//--------------------------------------------------------------------
//  関数宣言
//--------------------------------------------------------------------

int make_deck(int myhd[], int ud[], int us, int deck[]);
void cal_exval(int myhd[], int deck[], int exval[]);
int pick_card(int exval[], int maxex);
void expect_next(int myhd[], int deck[], int exval[]);
void cal_exval_2(int myhd[], int deck[], int exval[], int the, int num);
int conbination(int n, int k);

//====================================================================
//  戦略
//====================================================================

/*--------------------------------------------------------------------
//  ユーザ指定
//--------------------------------------------------------------------

最初の手札のまま交換しない。

hd : 手札配列
fd : 場札配列(テイク内の捨札)
cg : チェンジ数
tk : テイク数
ud : 捨札配列(過去のテイクも含めた全ての捨札)
us : 捨札数

--------------------------------------------------------------------*/

int strategy(int hd[], int fd[], int cg, int tk, int ud[], int us)
{
	int myhd[HNUM];
	int exval[HNUM] = {};
	int the, decknum;
	int i, j, k, l, max;
	int point[HNUM] = {};
	int point_myhd;

	arr_copy(myhd, hd, HNUM);
	if(poker_point(myhd) >= P3){
		return -1;
	}
	// decknum = make_deck(myhd, ud, us, deck);
	//deck作成
	decknum = CNUM;
	int deck[CNUM] = {};
	for (k = 0; k < HNUM; k++) { deck[myhd[k]] = -1; decknum--; }
	for (k = 0; k < us; k++) { deck[ud[k]] = -1; decknum--;}

	max = poker_point(myhd) * decknum * (decknum - 1);

	for(i = 0; i < HNUM; i++){
		for(j  = 0; j < CNUM; j++){
			//1手交換
			if(deck[j] == 0) {
				myhd[i] = j;
				for(k = 0; k < HNUM; k++){
					if(k == i) { continue; }
					for(l = 0; l < CNUM; l++){
						if(l == j) { continue; };
						if(deck[l] == 0){
							myhd[k] = l;
							exval[i] += poker_point(myhd);
						}
					}
					//2手先の手札の初期化
					myhd[k] = hd[k];
				}
			}
		}
		//2手先の手札の初期化
		myhd[i] = hd[i];
		printf("%d ", point[i]);
	}

	for(i = 0; i < HNUM; i++){
		if(max < point[i]){
			max = point[i];
			the = i;
		}
	}

	return the;
}

//====================================================================
//  補助関数
//====================================================================

//---n個の中からk個選ぶ組み合わせ---
int conbination(int n, int k){
	int i;
	int val = n;
	int dev = k;

	for(i = 1; i < k; i++){
		val *= --n;
	}
	while(k > 1) {
		dev *= --k;
	}
	return val /= dev;
}

//---手札と捨て札から山札をつくる---
int make_deck(int myhd[], int ud[], int us, int deck[]) {
	int k;
	int decknum = CNUM;
	for (k = 0; k < HNUM; k++) { deck[myhd[k]] = -1; decknum--; }
	for (k = 0; k < us; k++) { deck[ud[k]] = -1; decknum--;}
	return decknum;
}

//---期待値の計算---
void cal_exval(int myhd[], int deck[], int exval[]) {
	int changehd[HNUM];
	int i, j, k;

	for (k = 0; k < HNUM; k++) {
		//初期化
		i = 0;
		arr_copy(changehd, myhd, HNUM);

		//山札の1枚と手札を交換し、得点を算出
		while (i < CNUM) {
			if (deck[i] == -1) {
				i++;
				continue;
			}
			changehd[k] = i++;
			exval[k] += poker_point(changehd);
		}
	}
}

//---最大値の位置を決める---
int pick_card(int exval[], int maxex) {
	int k;
	int pick = -1;

	//exvalが同点のときどうするか？
	for (k = 0; k < HNUM; k++) {
		if (exval[k] >= maxex) {
			maxex = exval[k];
			pick = k;
		}
	}
	return pick;
}

//---2手先期待値の計算---
//　　　　　　　　　　　手札　　　　山札　　　　　期待値　　手札番号　交換する山札番号
void cal_exval_2(int myhd[], int deck[], int exval[], int the, int num) {
	int changehd[HNUM];
	int k, i;

	//2枚目の交換
	for (k = 0; k < HNUM; k++) {
		if(k == the){
			k++;
			continue;
		}
		// 初期化
		i = 0;
		arr_copy(changehd, myhd, HNUM);

		//山札の1枚と手札を交換し、得点を算出
		while (i < CNUM) {
			//deck内に含まれているカードを探す
			if (deck[i] == -1 || i == num) {
				i++;
				continue;
			}
			//手札入れ替え
			changehd[k] = i++;
			//期待値更新
			exval[the] += poker_point(changehd);
		}
	}
}

//---n手先の期待値  未完成---
void expect_next(int myhd[], int deck[], int exval[]) {
	int k;  //手札交換
	int i;  //山札番号
	int changehd[HNUM];

	//期待値の合計値が高いもの
	for(k = 0; k < HNUM; k++){
		// 初期化
		i = 0;
		arr_copy(changehd, myhd, HNUM);

		//山札の1枚と手札を交換し、得点を算出
		while (i < CNUM) {
			//山札内のカードを探す
			if (deck[i] == -1) {
				i++;
				continue;
			}
			//手札交換
			changehd[k] = i++;
			//2手先の期待値計算
			cal_exval_2(changehd, deck, exval, k, i);
		}
	}
}
