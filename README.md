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

#### バグ/不具合/仕様と解決策
* 取得できない値
  1. ダミーの"m_vecAbsVelocity"
  2. LocalPlayerの"lastPrimaryWeapon"（取得できる場合もあるが、基本的にはできない）

* ESPの色がVis/Norma Colorで点滅する  
  -> ESPのループが早すぎて、lastvisibletimeの値が更新される前にESPが更新されているのが原因。

* プレイヤーにはAim予測が適応されるが、ダミーにはAim予測が適応されない  
  -> ダミーの m_vecAbsVelocity からはVector3(0.f, 0.f, 0.f)しか取得できなかったのでR5Reloadedの仕様

* Y軸の予測が正確ではない  
  -> lastprimaryweaponがほぼ取得できないのでGravityも取得できません。それっぽい値で無理やり動かしています。

## 免責事項：
このプロジェクトは学習や教育用です。  
それ以外の用途に使用して発生したいかなる損害についても、開発者一同は一切の責任を負いません。

## メディア
準備中
