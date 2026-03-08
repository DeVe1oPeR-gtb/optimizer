# 参照: PSO_sample.c

元: `PSO_sample (2).c`（同一関数への5次多項式フィッティング）

## ビルド・実行

```bash
gcc -std=c99 -Wall -O2 -o build/PSO_sample reference/PSO_sample.c -lm
./build/PSO_sample
```

## 単体実行チェック結果

- **データ**: 21点、5次多項式 y = p0*x^5 + ... + p5（test_PSO.cpp DEMO と同じ x[], y[]）
- **設定**: 粒子数 500、初期範囲 [-0.1, 0.1]、反復 1000、srand(0) / srand(k) で再現
- **RMSE**: 約 **785 反復目** で初めて 1 未満（0.89）、**1000 反復で約 0.11**
- **係数**: 真値に近い（例: 0.1465, -2.78, 13.69, 5.51, -105.88, 179.93）

本リポジトリの C++ DEMO は **コンフィグでモード切替**（`config/para.cfg` の `pso_demo_mode=reference|default`）:
- **reference**: 参照 PSO_sample.c 互換（反復ごと w 更新・反復ごとシード・粒子ごと r1,r2 1組）。固定シードで再現、約 783 反復で RMSE < 1。
- **default**: 通常 PSO（固定 w・連続乱数・次元ごと r1,r2）。非決定的。
- ファイル未設定時は reference として動作。
