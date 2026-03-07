デモ用ソースです。後で削除する前提で配置しています。

■ 実行
  make demo
  ./build/Demo
  → result/summary.csv が出力される。trace=on のときは log/ にトレースログも出力。

■ トレース ON/OFF
  config/developer.cfg で trace=on または trace=off。trace=on のときは log/ にトレース CSV を出力。

■ 内容
  - 3種の物理モデル: 2次多項式 / 線形 / 有理式+指数
  - それぞれ PSO, DE, LM で最適化（計9実行）
  - トレースは PSO/DE/LM の setTraceStream, setTraceEnabled, writeTraceLine で出力

■ 削除時
  - demo/ ディレクトリごと削除
  - Makefile から以下を削除:
    - INC_DEMO、DEMO_OBJ_LIST、$(BUILD)/Demo のルール
    - demo_DemoPhysicalModel.o / demo_DemoDataLoader.o / demo_demo_main.o のルール
    - demo ターゲットと .PHONY の demo
