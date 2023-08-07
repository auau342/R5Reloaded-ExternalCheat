# R5Reloaded-ExternalCheat
友人と共同開発したR5Reloadedの外部チートです。
やりたいことはまだまだありますが、友人も自分も時間が取れないので公開という流れになりました。

## 主な機能
- AimBot
  * Smooth
  * Aim予測
  * 可視チェック
- ESP
  * Box
  * Line
  * Distance
  * HealthBar
- Misc
  * SpeedHack

#### バグ/不具合/仕様と解決策
* 取得できない値
  1. ダミーの"m_vecAbsVelocity"
  2. LocalPlayerの"lastPrimaryWeapon"（取得できる場合もあるが、基本的にはできない）

* ESPの色がVis/Norma Colorで点滅する  
  -> ESPのループが早すぎて、lastvisibletimeの値が更新される前にESPが更新されているのが原因。
  
* 敵が超至近距離にいるとAimBotが反応しない  
  -> Aim Boneのチェックを増やしてみてね
  
* プレイヤーにはAim予測が適応されるが、ダミーにはAim予測が適応されない  
  -> ダミーの m_vecAbsVelocity からはVector3(0.f, 0.f, 0.f)しか取得できなかったのでR5Reloadedの仕様

* Y軸の予測が正確ではない  
  -> lastprimaryweaponがほぼ取得できないのでGravityも取得できません。それっぽい値で無理やり動かしています。

* DummyESP: ON時のパフォーマンスが悪い  
  -> レンダリング中に突如16000回のforループとReadProcessMemoryが出現したらそりゃーパフォーマンスだって落ちます

## 免責事項：
このプロジェクトは学習や教育用です。  
それ以外の用途に使用して発生したいかなる損害についても、開発者一同は一切の責任を負いません。

## メディア
![image](https://github.com/FlankGir1/R5Reloaded-ExternalCheat/assets/124275926/c99fdde2-db25-42e6-81a0-869db74cc153)
![image](https://github.com/FlankGir1/R5Reloaded-ExternalCheat/assets/124275926/eab0f461-bf44-4398-88d7-712d4c6c509d)
