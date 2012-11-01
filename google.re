= OpenFlow でデータセンターをつなげる

//lead{
データセンターを一台の巨大なコンピュータと見立てると、この巨大コンピュータ同士をつなぐネットワークが必要になります。たくさんのデータセンターを持つグーグルは、OpenFlow を使って低コストでこのネットワークを作り上げてしまいました。
//}

== グーグルは OpenFlow を導入ずみ!

「グーグルはデータセンター間のトラフィックの 100% を、すでに SDN でまかなっている」。2012 年 4 月、SDN や OpenFlow をテーマにした一大カンファレンスである Open Networking Summit において、グーグルの上級副社長兼フェローであるウルス・ヘルツル氏が衝撃の発表を行いました。グーグルはすでに OpenFlow を実戦投入しており、データセンター間のネットワークを OpenFlow で構築したというのです。インターネット界の巨人グーグルは、OpenFlow をどのように活用しているのか？ヘルツル氏の講演内容をわかりやすく解説します。

グーグルはいくつもの最新鋭のデータセンターを使って、インターネット上のさまざまな情報を大量に収集しています。例えばグーグルが公開している資料からは、以下のすさまじいデータを発見できます。

 * 毎分 YouTube にアップロードされる動画は 60 時間分
 * 一日に処理するデータ量は数十ペタバイト以上
 * 一月あたり収集する Web ページは数十億ページ

//noindent
このような大量のデータを効率的に処理するために、グーグルは処理能力が高く電力効率も良いデータセンターを低いコストで実現しています。

データセンターは効率化できたが、ほかに改善できる部分は？グーグルはデータセンター同士をつなぐネットワーク (広域通信網、WAN) に目をつけました。OpenFlow を使えば、WAN のコストを下げられそうだ。グーグルの次の狙いはネットワーク、とくに WAN の改善だったのです。

=== WAN のコストを下げるには？

世界各地にデータセンターを持つグーグルでは、WAN 回線のコスト削減が急務です。

 * WAN 回線は容量が大きいほど高価になります。グーグルは世界各国のデータセンターをつなぐために専用の WAN  回線を引いています。データセンター間のトラフィックは帯域を使いますが、帯域の大きな WAN 回線は高くなります。そこで、必要最小限の回線を最大限まで使いきる必要があります。
 * WAN 回線は距離が長いほど高価になります。グーグルが他社と共同で引いた日米間約 1 万km の海底ケーブルは、総建設費が約 320 億円でした。このケーブルの回線容量は 4.8 Tbps なので、1 Gbps 当たり約 670 万円もかかったことになります。コストを下げるためには、あまり長いケーブルは引けません。

WAN 回線に追加投資をしなくとも、効率的にデータを転送するにはどうすればいいでしょうか？従来のインターネットのルーティングでは、宛先に対して必ず最短となるパスを選んで転送を行います。どれくらい帯域を使うかに関わらず最短経路を選択してしまうため、トラフィックが集中する回線や、逆にまったく使われない回線が出てきます。これは大きな無駄で、確かにコスト削減の余地があります。

=== グーグルの WAN 構成

続く分析のために、グーグルの WAN 構成を詳しく見ておきましょう。その役割に応じて、グーグルの WAN は次の二つのネットワークに分かれています。

 1. I-Scale：インターネットに直接つながるネットワーク。グーグルを利用するユーザのトラフィックを運ぶ。
 2. G-Scale：データセンター間をつなぐバックボーンネットワーク。グーグル内部のトラフィックを運ぶ。

グーグルは、このうち G-Scale に OpenFlow を適用しました。次の G-Scale の特徴により、効率化の余地が大きいと判断したためです。

 * インターネットにつながらない閉じたネットワークなので、どんなトラフィックが流れるかを予測しやすい。
 * グーグル専用のネットワークなので、トラフィックごとの優先度を自由に決定できる。

//noindent
これらの特徴を踏まえると、WAN 回線を効率的に使うための課題がはっきりします。

== WAN 回線を効率的に使うには？

@<img>{wan} のデータセンター A からデータセンター B へ、どれだけトラフィックを流せるかを考えてみましょう。普通のルーティングでは、最短経路となるリンク 1 のみを使うため、このリンクの帯域が上限となります。もしこの上限を超えるトラフィックを運びたい場合、通常はリンク帯域の増設が必要になり、コストが増えてしまいます。

//image[wan][データセンター A からデータセンター B へどのくらいトラフィックを流せる？][scale=0.6]

しかし、スイッチ C を経由する経路を足すことで、さらにトラフィックを流せるようになります (@<img>{multipath})。

//image[multipath][複数の経路を使ってトラフィックを転送する][scale=0.6]

このように複数の経路を使うと、どれだけトラフィックを流せるでしょうか？もしもデータセンター A と B ですべてのリンクを占有できるなら、それぞれのリンク帯域の上限まで流せます。たとえば、リンク 1 にはリンク帯域の 10 Gbps までトラフィックを流せます。スイッチ C を通る経路のように複数のリンクを使う場合、流せるトラフィック量は帯域が最小のリンクによって決まります。つまり、スイッチ C を通る経路は 6Gbps までのトラフィックを流せます。結局、@<img>{multipath} のネットワークでは、データセンター A からデータセンター B へ、最大で 16Gbps のトラフィックを流せることになります。

しかし、実際に複数の経路を使ってトラフィックを流すには、次の 3 つの課題をクリアしなければなりません。

=== 課題 1：空いた帯域を調べるには？

実際に流せるトラフィック量は、各リンクの利用状況によります。@<img>{multipath} では、データセンター A からデータセンター B へは 16Gbps までトラフィックを流せると説明しました。しかし、普通は他のデータセンター間にもトラフィックがあるので、使えるのは 16Gbps のうち空いている帯域だけです。そこで実際にどの程度トラフィックを流せるかを知るためには、各リンクの空き帯域の情報が必要です。

しかし、離れた場所にあるリンクの空き帯域はどうやれば分かるでしょうか？データセンター A は、直接つながるスイッチ A がやり取りしているトラフィック量を調べることで、リンク 1 とリンク 2 の空き帯域が分かります。しかし、データセンター A から離れたリンク 3 の空き帯域を知るには、別の方法が必要です。もちろん、実際のネットワークにはもっとたくさんのリンクがあるので、すべてのリンクの空き帯域情報が必要です。

=== 課題 2：空いた帯域をどう分け合うか？

二つめは、空いた帯域をデータセンター間でどう分け合うかという問題です。例えば、@<img>{conflict} のネットワークにおいて、リンク 1 の空き帯域が 10Gbps であったとします。もしデータセンター X とデータセンター Y がそれぞれが勝手に、データセンター Z へ 10Gbps のトラフィックを流そうとすると、当然このうち半分のトラフィックしかリンク 1 を通過できません。しかも通過できないトラフィックは、スイッチが破棄してしまいます。

これを避けるためにはトラフィック間での調停者が必要です。つまり、どちらか片方のトラフィックがリンク 1 の空き帯域を占有する、もしくはリンク 1 の空き帯域を仲良く分けあう、というように上から指示する仕組みが必要です。

//image[conflict][どのトラフィックが空き帯域を使うかを、うまく調停する必要がある][scale=0.7]

=== 課題 3：複数の経路にどう振り分ける？

三つめは、ひとつのデータセンターから出るトラフィックを複数の経路へどう振り分けるかという問題です。例えば、@<img>{traffic} 中のデータセンターからのトラフィックのために、リンク 1 経由の経路で 10Gbps、リンク 2 経由の経路で 6Gbps の帯域がそれぞれ利用できるとします。スイッチは、これらの帯域を超えないように、これらの経路に各サービスのトラフィックを振り分ける必要があります。

このためには、サービスごとのトラフィック量を知っている必要があります。なぜならば、OpenFlow スイッチが認識できるのはヘッダの情報だけで、サービスレベルの情報までは知らないからです。

//image[traffic][サービスごとのトラフィックを、それぞれどの経路に流すかを決定する必要がある][scale=0.6]

== G-Scale での WAN 回線の効率的な利用法

それでは Google のデータセンター間ネットワークである G-Scale が、これら 3 つの課題をどのように解決し、WAN を効率的に利用しているかを見ていきましょう。

=== トラフィックエンジニアリングサーバの導入

G-Scale では、ネットワーク全体の空き帯域を一括して管理するためのトラフィックエンジニアリングサーバ (TE サーバ) を導入しています (@<img>{te_server})。TE サーバは、次のようにして各リンクの空き帯域の情報を集めます。そして、各データセンター間のトラフィックに対して、空き帯域を割り当てます。

//image[te_server][TE サーバは空き帯域を把握し、各データセンターのトラフィックへ割り当てる][scale=0.6]

 * リンクの空き帯域は、リンクの帯域とそのリンクに流れているトラフィック量を調べれば、これらの差分から計算できます。一般的なスイッチは、これらの情報を常に収集しています。そのため、すべてのリンクのトラフィック情報を集めることは難しくありません。
 * 空き帯域の割り当ての詳細については、詳しいアルゴリズムが公表されていないので分かりません。しかし、ビジネスポリシーや各データセンターの稼働状況などなど、さまざまなパラメータによって決めていることが予想できます。

どちらにせよ、各リンクの空き帯域を把握し、その空き帯域をトラフィックに割り当てるという役目を、「神様」である TE サーバが一括して行うことで、前節に上げた課題 1, 2 を解決しています。

=== G-Scale の構成

G-Scale は、詳しく見ると、以下の役割を果たしています。

 1. トラフィックがどのデータセンター宛なのかを決定する
 2. 空き帯域の割り当てを受け、トラフィックを流す経路を決定する
 3. 前記で決定した経路にトラフィックを流す

//noindent
これらを実現するために、G-Scale は、OpenFlow スイッチとコントローラおよび BGP 処理部を持っています。これらの三つは、収容する各データセンター毎に配置されており、@<img>{wan_equip} のようにデータセンターと接続しています。

//image[wan_equip][G-Scale には、OpenFlow スイッチ、コントローラと BGP 処理部がある][scale=0.5]

動作 1 を行うのが、BGP 処理部です。G-Scale がトラフィックを宛先のデータセンターに届けるためには、データセンター内にあるホストの IP アドレスを知る必要があります。そのために、BGP 処理部が、データセンターからドレスに関する情報 (経路情報) を受け取り、他の BGP 処理部と経路情報を交換します(詳細はコラムを参照)。このように経路情報を収集することで、パケット中の宛先アドレスをみれば、どのデータセンターに送るべきかを判断できます。

動作 2 は、OpenFlow コントローラが行います。OpenFlow コントローラは、TE サーバに対して、宛先のデータセンターまでの空き帯域の割り当てを要求します。そして、割り当てられた空き帯域にトラフィックが流れるように、OpenFlow スイッチにフローエントリを設定します。

TE サーバから空き帯域を割り当ててもらうためには、前節の課題 3 で上げたように、トラフィックの使用帯域を知る必要がありますが、その方法は公表されていません。実際にトラフィックが流れれば、スイッチの統計情報からその使用帯域はわかります。しかし、空き帯域の割り当てはトラフィックが流れる前に行う必要があります。そのため、G-Scale では、トラフィックの発生元のアプリケーションから教えてもらうか、過去の統計情報から推定するか、どちらかの方法を採用していると推測できます。

OpenFlow コントローラからの指示に従い、実際に動作 3 のトラフィック転送を行うのが、OpenFlow スイッチです。大量のトラフィックを扱う必要があるため、10Gbps Ethernet のインターフェイスを 100 ポート以上持つ大規模な OpenFlow スイッチが複数台配置されています。このような仕様を持つスイッチは、市場には存在しないため、Google 自らが開発しました。自前での装置開発にはそれなりのコストが掛かりますが、WAN の効率化により削減できるコストの方が大きいと、Google は判断したのでしょう。

=== OpenFlow コントローラの協調動作

G-Scale は複数の OpenFlow コントローラが協調してネットワークを制御しています(@<img>{cooperate})。@<chap>{routing_switch} で紹介したルーティングスイッチも、G-Scale と同様に、複数の OpenFlow スイッチを制御しますが、これらを一台のコントローラで行います。G-Scale では フローエントリの作成を BGP や TE サーバからの情報を元に行うことで、コントローラ同士の協調動作を実現しています。

//image[cooperate][複数の OpenFlow コントローラが協調して動作する]

複数コントローラの協調動作については、さまざまな実現方法が考えられるため、OpenFlow の仕様では規定されていません。ここで紹介した G-Scale の例も、あくまで複数 OpenFlow コントローラの協調動作の一例でしかありません。しかし、実際のネットワークで運用されており、OpenFlow の協調動作における代表例と言ってもいいでしょう。

===[column] どのように BGP は動作する？

G-Scale では、経路情報の交換に BGP を用いています。BGP は、異なる組織間で、アドレスに関する情報 (経路情報と呼びます) を交換するためのプロコトルです。インターネットのように、様々な組織から構成されるネットワークを作るためには欠かすことのできないプロトコルです。BGP では、それぞれの組織を AS と呼び、AS 単位での経路情報を交換します。

通常異なる組織同士の接続に用いられる BGP を、Google はデータセンター間の接続に用いています。各データセンターと G-Scale それぞれを一つの組織に見立て、それぞれが一つの AS として動作しています。G-Scale は、BGP により各データセンターから経路情報をもらい、その情報を元にしてパケットの届け先を決定しています。

//image[bgp][BGP を用いてデータセンター A 内のアドレスを G-Scale に通知する][scale=0.5]

例えば、データセンター A 内のホストには 192.168.1.0/24 のアドレスを持っていたとします(@<img>{bgp})。データセンター A 内の BGP ルータはこのアドレスに対する経路情報を BGP を用いて、G-Scale の BGP 処理部に通知します。経路情報を受け取った BGP 処理部は、やはり BGP を用いて、G-Scale 内の他の BGP 処理部へと経路情報を転送します。このようにすることで、例えば宛先が 192.168.1.1 であるパケットを受け取った時に、そのパケットをデータセンター A へと送り届ければよいということを、G-Scale は知ります。

BGP には External BGP (E-BGP) と Internal BGP (I-BGP) の二種類の接続形態があります。E-BGP は異なる組織間の、I-BGP は組織内の情報交換に、それぞれ用います。@<img>{bgp} では、データセンター A の BGP ルータと G-Scale 内の BGP 処理部との間の接続が、E-BGP になります。また G-Scale 内の BGP 処理部同士は、I-BGP を用いて経路情報を交換しています。

===[/column]

== まとめ

Google が、WAN 回線の有効活用を行うために、データセンター間トラフィックの転送に OpenFlow をどのように活用しているかを紹介しました。

 * Google が OpenFlow を使う理由について説明しました。Google は WAN 回線を効率的に利用することで、低コストに多くのトラフィックを扱うために、OpenFlow を活用しています。
 * WAN 回線の効率化を実現する上での三つの課題を取り上げました。また、Google は、これらの課題を OpenFlow を用いて、どのように解決しているか紹介しました。

Google のような大規模事業者で実際に OpenFlow が動いているという事実は、これから OpenFlow に取り組もうとしている技術者にとって大きな励みとなるでしょう。Google とそのまま同じ事を実現するのはなかなか難しいですが、彼らのチャレンジからは多くのことが学べます。みなさんの課題解決に、この章の内容が少しでも役立てばと思います。

== 参考文献

: ウルス・ヘルツル氏 Open Networking Summit 2012 での講演 ( @<tt>{http://youtu.be/VLHJUfgxEO4} )
  今回の取り上げたウルス・ヘルツル氏の講演内容が、Youtube に投稿されています。Google がどのようなチャレンジを行ったか、彼らの生の声を聞きたい人にとっては必見の動画です。

: Google を支える技術 (西田圭介著、技術評論社)
  この章ではネットワーク面でのコスト削減について取り上げましたが、この本ではデータセンター自体の運営コストなどについての分析が行われています。この本を読むことで、Google が如何にして何万台ものコンピュータを使ってサービスを実現しているかを学べます。

: インターネットルーティングアーキテクチャ - BGPリソースの詳解 (サム・ハラビ、ダニー・マクファーソン著、ソフトバンククリエイティブ)
  この章でも簡単に紹介しましたが、より深く BGP について学びたい人は、この本を読むとよいでしょう。具体的な例が多く紹介されており、BGP の動作をイメージしながら理解できます。

