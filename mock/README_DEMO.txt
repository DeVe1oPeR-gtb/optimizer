デモ用ソースです。後で削除する前提で配置しています。

■ 実行
  make demo
  ./build/Demo
  → result/summary.csv が出力される。trace=on のときは log/ にトレースログも出力。

■ トレース ON/OFF
  config/developer.cfg で trace=on または trace=off。trace=on のときは log/ にトレース CSV を出力。

■ 内容
  - mock/Demo.h,cpp: 3種の物理モデル（2次多項式/線形/有理式+指数）と対応データローダ
  - mock/demo_main.cpp: 各モデルを PSO, DE, LM で最適化（設定の optimizer= で切り替え可能）
  - mock/Mock.h,cpp: テスト用モック（IPhysicalModel / IProductDataLoader）

■ 削除時
  - mock/ 内の Demo.* と demo_main.cpp を削除（Mock はテストで使用するため残す）
  - Makefile の DEMO 関連ルールと demo ターゲットを削除
