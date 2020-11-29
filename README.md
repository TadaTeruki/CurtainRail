# CurtainRail構造 : 空間データ索引構造

**注:作成途中。**

多田 瑛貴 <newertlll@gmail.com>

## 概要

空間インデックス構造(Spacial Index Structure)は、空間データを効率よく索引するための索引構造である。 <br>
<br>
機械学習(なかでもクラスタリング)やゲームプログラミングといった、様々な場面ですでに広く応用されており、 <br>
今や情報科学分野全体にとっても重要なアルゴリズム技術の一つでもある。 <br>
<br>
本研究では、その空間インデックス構造にあたるものとして、一部のパターンにおいて従来の手法よりもより高速な索引を実現することのできる新しい索引構造を提案し、
「**CurtainRail構造**」として取りまとめた。

なお、この索引構造のアイデアは、自分の過去の研究である「[Mulp-si](https://github.com/TadaTeruki/mulpsi)」の考え方がベースとなっている。<br>

## 引用

[1].[Antomn Guttman, R-TREES. A DYNAMIC INDEX STRUCTURE FOR SPATIAL SEARCHING 1984 ACM](http://www-db.deis.unibo.it/courses/SI-LS/papers/Gut84.pdf)
