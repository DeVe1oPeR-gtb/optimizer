#include "Optimizer/LM/LM.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>

// CppUTest includes should be after your and system includes
#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

// Utility
#include "csvwriter.hpp"

using std::cout;
using std::endl;

// helper function
double y_cal_fz(int num, double x, double p[]);

TEST_GROUP(LM_SOT) {
    LM<double> *levm;
    void setup() {
        int num_dim  = 6;
        int num_data = 20;
        std::vector<double> z(6,0.001);
        levm         = new LM<double>(num_dim, num_data, z);
    }
    void teardown() {
        delete levm;
    }
};

TEST_GROUP(LM_AOT) {
    LM<double> *levm;
    void setup() {
    }
    void teardown() {
        delete levm;
    }
};

TEST(LM_SOT, Create) {

    double x[3] = {1., 2., 3.};
    double p[3] = {1., 2., 3.};
    for (int i = 0; i < 3; i++) {
        (void)y_cal_fz(3, x[i], p);
    }
    // FAIL("Start here..");
}

TEST(LM_AOT, DEMO) {
    constexpr int NI = 21;
    constexpr int NM = 6;
    constexpr int NUMAX = 1000; // 更新回数
    // サンプルデータ
    double x[NI] = {0.00, 0.50, 1.00, 1.50, 2.00, 2.50, 3.00, 3.50, 4.00, 4.50, 5.00,
                    5.50, 6.00, 6.50, 7.00, 7.50, 8.00, 8.50, 9.00, 9.50, 10.00};
    double y[NI] = {180.00, 129.79, 90.47,  66.68,  60.00,  69.51,  92.35, 124.22, 159.99, 194.21, 221.67,
                    237.91, 239.84, 226.20, 198.19, 159.95, 119.15, 87.51, 81.36,  122.18, 237.17};
    double fz_base[NI] = {0.0};
    double fz_pert[NI] = {0.0};
    std::vector<double> z (NM,0.001);
    double zx[NM] = {0.0};
    levm         = new LM<double>(NM, NI, z);
    
    std::vector<double> dz (NM,0.0);
    std::vector<double> r (NM,0.0);
    std::vector<double> dfdz (NI,0.0);
    std::vector<std::pair<double,double>> eval_data(NI);


    // 前処理
    //  1回目はループ前回値を使えないため、初期ステップサイズを取得する
    r = levm->getDelta();

    // ループ
    for (int ik = 0; ik < NUMAX; ++ik) { // 更新回数ループ(ik)
        
        // 評価データをセット
        for (int d = 0; d < NM; ++d) zx[d] = z[d];
        for (int i = 0; i < NI; ++i) {
            fz_base[i] = y_cal_fz(NM, x[i], zx);
            eval_data[i] = std::make_pair(y[i], fz_base[i]);
        }
        levm->setEvalData(eval_data);

        (void)levm->getStats().mean;
        (void)levm->getStats().std_dev;
        (void)levm->getStats().rmse;
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//         if(ik == 0){
//             cout << endl;
//             cout << std::setprecision(1);
//             cout << std::setw(8) << "update";
//             cout << std::setw(8) << "ave";
//             cout << std::setw(8) << "sig";
//             cout << std::setw(8) << "RMSE";
//             cout <<"   ﾊﾟﾗﾒｰﾀ 1～6"<< endl;
//         }
// ////////////////////////////////////////////////////////////////////////
//         cout << std::setw(8) << ik+1;
//         cout << std::right << std::fixed;
//         cout << std::setw(8) << mean;
//         cout << std::setw(8) << std;
//         cout << std::setw(8) << rmse;

//         for(int i = NM-1; i >= 0; --i ) {
//         // cout << std::scientific;
//             cout << std::setprecision(3) << std::setw(10) << z[i];
//         }
//         cout << endl;
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
        

        for (int d = 0; d < NM; ++d) { // パラメータループ(d)
            zx[d] += r[d]; // パラメータの摂動

            for (int i = 0; i < NI; ++i) {  // データ数ループ(i)
                fz_pert[i] = y_cal_fz(NM, x[i], zx);
                dfdz[i] = (fz_pert[i] - fz_base[i]) / r[d]; // 摂動による変化量
            }
            levm->setJacobian(d, dfdz); // ヤコビアンに保存
            zx[d] = z[d]; // パラメータ戻し
        }
        dz = levm->getNextDelta();                  // ステップサイズを更新
        for (int d = 0; d < NM; ++d) z[d] += dz[d]; // zの更新
    }

    for (int i = 0; i < NI; ++i) {
        fz_base[i] = y_cal_fz(NM, x[i], zx); // 更新後zで計算
        eval_data[i] = std::make_pair(y[i], fz_base[i]); // 評価データを更新
    }
    for (int d = 0; d < NM; ++d) zx[d] -= dz[d];    // 更新前のzに戻す
    for (int d = 0; d < NM; ++d) r[d] = z[d]*0.005; // 次回摂動量
}

double y_cal_fz(int num, double x, double p[]) {
    double fz = p[0];
    for (int i = 1; i < num; ++i) {
        fz += p[i] * pow(x,i);
    }
    return fz;
}
