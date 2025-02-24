#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <iomanip> // 出力フォーマット用

using namespace std;

const int NI = 21;

/*********************************************************************************/
/*  【ｻﾝﾌﾟﾙﾃﾞｰﾀ】                                                                */
/*    y = 0.1458 x^5 -2.7604 x^4 + 13.542 x^3 + 6.0417 x^2 - 106.50 x + 180.00   */
/*********************************************************************************/
double x[NI] = {
    0.00, 0.50, 1.00, 1.50, 2.00, 2.50, 3.00, 3.50,
    4.00, 4.50, 5.00, 5.50, 6.00, 6.50, 7.00, 7.50,
    8.00, 8.50, 9.00, 9.50, 10.00
};

double y_actual[NI] = {
    180.00, 129.79, 90.47, 66.68, 60.00, 69.51, 92.35, 124.22,
    159.99, 194.21, 221.67, 237.91, 239.84, 226.20, 198.19, 159.95,
    119.15, 87.51, 81.36, 122.18, 237.17
};
// double x[21] = {
//     0.000, 0.500, 1.000, 1.500, 2.000, 2.500, 3.000, 3.500,
//     4.000, 4.500, 5.000, 5.500, 6.000, 6.500, 7.000, 7.500,
//     8.000, 8.500, 9.000, 9.500, 10.000
// };

// double y_actual[21] = {
//     50.000, 56.560, 63.262, 59.170, 41.109, 12.983, -16.560, -37.667,
//     -42.997, -30.425, -3.847, 28.158, 55.321, 69.285, 66.541, 49.675,
//     26.487, 7.296, 1.354, 13.647, 43.172
// };


// PSOパラメータ
const int NUM_PARTICLES = 500;
const int MAX_ITER = 10000;
const double W_MAX = 0.9;
const double W_MIN = 0.7;
const double C1 = 2.0;
const double C2 = 0.8;
const double MIN_POS = -300;
const double MAX_POS = 300;
const double MIN_VEL = -30;
const double MAX_VEL = 30;
const double INIT_POS_RANGE = 0.1;  // 初期位置の範囲（±1）

// シードの固定
const int FIXED_SEED = 12345;
std::mt19937 gen(FIXED_SEED);

// `rand()` の特性を模倣する乱数生成
double pseudo_rand() {
    std::uniform_int_distribution<int> dist(0, RAND_MAX);
    return dist(gen) / (double)RAND_MAX;
}

// Adaptive PSO 用の乱数生成
double adaptive_random(int iter, int max_iter) {
    if (iter < max_iter / 2) {
        return pseudo_rand();
    } else {
        std::normal_distribution<double> dist(0.5, 0.1);
        return max(0.0, min(1.0, dist(gen)));
    }
}

// 新しい関数による y の計算
double calculate_y(double x, double p0, double p1, double p2, double p3, double p4, double p5) {
    return p5 * sin(p4 * x) + p3 * pow(x, 3) + p2 * pow(x, 2) + p1 * x + p0 + p5 * exp(-p4 * x);
}

// RMSE の計算（新しい関数を使用）
// double rmse(const vector<double>& coeffs) {
//     double error = 0.0;
//     for (int i = 0; i < NI; i++) {
//         double y_pred = calculate_y(x[i], coeffs[0], coeffs[1], coeffs[2], coeffs[3], coeffs[4], coeffs[5]);
//         error += pow(y_actual[i] - y_pred, 2);
//     }
//     return sqrt(error / NI);
// }
// RMSE の計算
double rmse(const vector<double>& coeffs) {
    double error = 0.0;
    for (int i = 0; i < NI; i++) {
        double y_pred = coeffs[0] + coeffs[1] * x[i] + coeffs[2] * pow(x[i], 2) +
                        coeffs[3] * pow(x[i], 3) + coeffs[4] * pow(x[i], 4) + coeffs[5] * pow(x[i], 5);
        error += pow(y_actual[i] - y_pred, 2);
    }
    return sqrt(error / NI);
}

// 粒子クラス
class Particle {
public:
    vector<double> position;
    vector<double> velocity;
    vector<double> best_position;
    double best_rmse;

    Particle() {
        position.resize(6);
        velocity.resize(6);
        std::uniform_real_distribution<double> pos_dist(-INIT_POS_RANGE, INIT_POS_RANGE);
        std::uniform_real_distribution<double> vel_dist(MIN_VEL, MAX_VEL);

        for (int i = 0; i < 6; i++) {
            position[i] = pos_dist(gen);
            velocity[i] = vel_dist(gen);
        }
        best_position = position;
        best_rmse = rmse(position);
    }

    void update_velocity(const vector<double>& global_best, double w, int iter) {
        for (int i = 0; i < 6; i++) {
            double r1 = adaptive_random(iter, MAX_ITER);
            double r2 = adaptive_random(iter, MAX_ITER);
            velocity[i] = w * velocity[i] +
                          C1 * r1 * (best_position[i] - position[i]) +
                          C2 * r2 * (global_best[i] - position[i]);
            velocity[i] = max(MIN_VEL, min(MAX_VEL, velocity[i]));
        }
    }

    void update_position() {
        for (int i = 0; i < 6; i++) {
            position[i] += velocity[i];
            position[i] = max(MIN_POS, min(MAX_POS, position[i]));
        }
        double current_rmse = rmse(position);
        if (current_rmse < best_rmse) {
            best_rmse = current_rmse;
            best_position = position;
        }
    }
};

// PSOの実行
vector<double> pso() {
    vector<Particle> particles(NUM_PARTICLES);
    
    vector<double> global_best = particles[0].best_position;
    double global_best_rmse = particles[0].best_rmse;

    for (const auto& p : particles) {
        if (p.best_rmse < global_best_rmse) {
            global_best = p.best_position;
            global_best_rmse = p.best_rmse;
        }
    }

    cout << setw(8) << "Iter" << setw(8) << "RMSE"
         << setw(8) << "p5" << setw(8) << "p4" << setw(8) << "p3"
         << setw(8) << "p2" << setw(8) << "p1" << setw(8) << "p0" << endl;

    for (int iter = 0; iter < MAX_ITER; iter++) {
        double w = W_MAX - ((W_MAX - W_MIN) * iter / MAX_ITER);
        for (auto& p : particles) {
            p.update_velocity(global_best, w, iter);
            p.update_position();
            if (p.best_rmse < global_best_rmse) {
                global_best = p.best_position;
                global_best_rmse = p.best_rmse;
            }
        }

        if (iter % 10 == 0) {
            cout << setw(8) << iter << fixed << setprecision(3)
                 << setw(8) << global_best_rmse;
            for (int i = 5; i >= 0; i--) {
                cout << setw(8) << global_best[i];
            }
            cout << endl;
        }
    }

    cout << "\nFinal Global Best Parameters:" << endl;
    for (int i = 5; i >= 0; i--) {
        cout << "p" << i << " = " << fixed << setprecision(3) << global_best[i] << endl;
    }

    cout << "\nPredicted y values using the final global best parameters:" << endl;
    cout << setw(8) << "x" << setw(8) << "y_pred" << endl;
    for (int i = 0; i < NI; i++) {
        double y_pred = global_best[0] + global_best[1] * x[i] + global_best[2] * pow(x[i], 2) +
                        global_best[3] * pow(x[i], 3) + global_best[4] * pow(x[i], 4) + global_best[5] * pow(x[i], 5);
        // double y_pred = calculate_y(x[i], global_best[0], global_best[1], global_best[2], global_best[3], global_best[4], global_best[5]);
        cout << setw(8) << x[i] << setw(8) << fixed << setprecision(3) << y_pred << endl;
    }

    return global_best;
}

int main() {
    vector<double> best_coeffs = pso();
    return 0;
}
