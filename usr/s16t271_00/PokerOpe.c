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

#define TAKE 5
#define CHANGE 7
#define MIN_POINT 8
//--------------------------------------------------------------------
//  関数宣言
//--------------------------------------------------------------------

int make_deck(int myhd[], int ud[], int us, int deck[]);
void cal_exval(int myhd[], int deck[], int exval[]);
int pick_card(int exval[], int maxex);
void expect_next(int myhd[], int deck[], int exval[]);
void cal_exval_2(int myhd[], int deck[], int exval[], int the, int num);
int conbination(int n, int k);
int one_future(int hd[], int deck[], int decknum);
int two_future(int hd[], int deck[], int decknum);

//====================================================================
//  戦略
//====================================================================

/*--------------------------------------------------------------------
//  ユーザ指定
//--------------------------------------------------------------------

hd : 手札配列
fd : 場札配列(テイク内の捨札)
cg : チェンジ数
tk : テイク数
ud : 捨札配列(過去のテイクも含めた全ての捨札)
us : 捨札数
GNUM : ゲーム最大回数
CNUM : カードの枚数
HNUM : 手札の枚数
FNUM : 場札の最大枚数(5×2)
GAME_NAME_LEN : ゲームのインスタンス名の最大長
FILE_NAME_LEN : ファイル名の最大長
DIR_NAME_LEN  : ディレクトリ名の最大長
COMM_LEN  : コマンド行の最大長
POINT_MAX : 役番号の最大点
POINT_MIN : 役番号の最小点
POINT_NUM : 役番号の最大長

//----  役の配点
P0 : ノーペア
P1 : ワンペア
P2 : ツーペア
P3 : スリーカインズ
P4 : ストレート
P5 : フラッシュ
P6 : フルハウス
P7 : フォーカード
P8 : ストレートフラッシュ
P9 : ロイヤルストレートフラッシュ

--------------------------------------------------------------------*/
int strategy(int hd[], int fd[], int cg, int tk, int ud[], int us)
{
	float rate[TAKE] = {1.0, 1.5, 2.0, 1.5, 1.0};
	int myhd[HNUM];
	int exval[HNUM] = {};
	int hand, decknum;
	int i, j, k, l, max;
	int point[HNUM] = {};
	int point_myhd;

	// ポイント切り上げ
	if (poker_point(myhd) >= MIN_POINT*rate[tk]) {
		return -1;
	}

	// デッキ作成
	decknum = CNUM;
	int deck[CNUM] = {};
	for (k = 0; k < HNUM; k++) { deck[myhd[k]] = -1; decknum--; }
	for (k = 0; k < us; k++) { deck[ud[k]] = -1; decknum--;}
	
	// hand = one_future(hd, deck, decknum);
	hand = two_future(hd, deck, decknum);
	return hand;
}

// int strategy(int hd[], int fd[], int cg, int tk, int ud[], int us)
// {
// 	int myhd[HNUM];
// 	int exval[HNUM] = {};
// 	int the, decknum;
// 	int i, j, k, l, max;
// 	int point[HNUM] = {};
// 	int point_myhd;

// 	arr_copy(myhd, hd, HNUM);
// 	if(poker_point(myhd) >= P3){
// 		return -1;
// 	}
// 	// decknum = make_deck(myhd, ud, us, deck);
// 	//deck作成
// 	decknum = CNUM;
// 	int deck[CNUM] = {};
// 	for (k = 0; k < HNUM; k++) { deck[myhd[k]] = -1; decknum--; }
// 	for (k = 0; k < us; k++) { deck[ud[k]] = -1; decknum--;}

// 	max = poker_point(myhd) * decknum * (decknum - 1);

// 	for(i = 0; i < HNUM; i++){
// 		for(j  = 0; j < CNUM; j++){
// 			//1手交換
// 			if(deck[j] == 0) {
// 				myhd[i] = j;
// 				for(k = 0; k < HNUM; k++){
// 					if(k == i) { continue; }
// 					for(l = 0; l < CNUM; l++){
// 						if(l == j) { continue; };
// 						if(deck[l] == 0){
// 							myhd[k] = l;
// 							exval[i] += poker_point(myhd);
// 						}
// 					}
// 					//2手先の手札の初期化
// 					myhd[k] = hd[k];
// 				}
// 			}
// 		}
// 		//2手先の手札の初期化
// 		myhd[i] = hd[i];
// 		printf("%d ", point[i]);
// 	}

// 	for(i = 0; i < HNUM; i++){
// 		if(max < point[i]){
// 			max = point[i];
// 			the = i;
// 		}
// 	}

// 	return the;
// }

//====================================================================
//  補助関数
//====================================================================

//---1手先読み---
int one_future(int hd[], int deck[], int decknum){
	int myhd[HNUM];
	int exval[HNUM] = {};
	int i, j, max;
	int hand = -1;

	for(i = 0; i < HNUM; i++){
		arr_copy(myhd, hd, HNUM);
		for(j = 0; j < CNUM; j++){
			if(deck[j] == 0) {
				myhd[i] = j;
				exval[i] += poker_point(myhd);
			}
		}
	}
	max = poker_point(hd) * decknum;
	for(i = 0; i < HNUM; i++){
		if(max < exval[i]){
			max = exval[i];
			hand = i;
		}
	}
	return hand;
}

//---2手先読み---
int two_future(int hd[], int deck[], int decknum){
	int myhd[HNUM];    //手札
	int ofhd[HNUM];    //一手先の手札
	int exval[HNUM] = {};   //期待値
	int hand = -1;     //変更手札位置
	int h_1, h_2, d_1, d_2, max;

	for(h_1 = 0; h_1 < HNUM; h_1++){
		arr_copy(myhd, hd, HNUM);
		for(d_1 = 0; d_1 < CNUM; d_1++){
			if(deck[d_1] == 0) {
				myhd[h_1] = d_1;
			}
			for(h_2 = 0; h_2 < HNUM; h_2++){
				arr_copy(ofhd, myhd, HNUM);
				// 手札の同じ場所は変えない?変えてもいい？
				if(h_1 == h_2){
					continue;
				}
				for(d_2 = 0; d_2 < CNUM; d_2++){
					if(d_2 == d_1){
						continue;
					}
					else if(deck[d_2] == 0){
						ofhd[h_2] = d_2;
						exval[h_1] += poker_point(ofhd);
					}
				}
			}
		}
	}
	max = poker_point(hd) * decknum * (decknum - 1);
	for(h_1 = 0; h_1 < HNUM; h_1++){
		if(max < exval[h_1]){
			max = exval[h_1];
			hand = h_1;
		}
	}
	return hand;
}

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
