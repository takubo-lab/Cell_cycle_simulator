
# Cell_cycle_simulator


今回、個々の細胞に個別に運命決定を行わせる確率モデルを用いて、96 well plateでのsingle cell cultureを模倣した高速なシミュレーターを作成しました。
それぞれの細胞は確率的に分裂、分化、死亡、そのまま、の挙動を示し、分化後には分裂確率が減衰していきます。分化後に細胞分裂の確率が減衰する以外は、それぞれのパラメーターは独立です（つまり分裂と分化は同期しない）。


環境構築
Pythonのインストール https://www.python.org/

インストール時に、PythonのexeファイルへのPATH追加にチェック
リポジトリのクローン作成(コマンドライン上で)
```
cd C:\クローンを作成するディレクトリのパス
git clone https://github.com/takubo-lab/Cell_cycle_simulato
```

Pythonの実行環境とライブラリー
```
pip install matplotlib 
```

実行
```
python Cell_simulator.py
```
指定可能パラメーター
最初の細胞数、タイムフレーム（初期値30）、タイムフレーム内での分裂確率、死亡確率、分化確率、および分化したあとの増殖の減衰率d（分裂確率×1/(1+ timeframe x d）)
初期細胞数（初期値1）、シミュレーション回数（初期値100）



