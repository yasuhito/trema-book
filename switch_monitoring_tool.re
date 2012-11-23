= スイッチ監視ツール

//lead{
OpenFlowの一番の特長は、たくさんのスイッチをひとつのコントローラで集中制御できることです。その雰囲気を簡単なOpenFlowコントローラを書いて体験してみましょう。
//}

== ネットワークを集中管理しよう

OpenFlowの特長は「たくさんのスイッチをひとつのコントローラで集中制御できる」という点にありました。スイッチにはフローテーブルに従ったパケットの転送という単純な仕事だけをやらせ、頭脳であるコントローラが全体のフローテーブルを統括するというわけです。これによって第I部で見てきたように、自動化やシステム連携のしやすさ、ネットワークトラフィック制御の容易さ、そしてソフトウェア開発のテクニックやツールの応用といったさまざまなメリットが生まれるのでした。

本章ではこの集中制御の一例として、スイッチの監視ツールを作ります。このツールは「今、ネットワーク中にどんなスイッチが動いているか」をリアルタイムに表示します。たくさんあるスイッチ全体が正常に動いているかを確認するのに便利です。

//image[switch_monitor_overview][スイッチ監視ツール][width=12cm]

スイッチ監視ツールは@<img>{switch_monitor_overview} のように動作します。OpenFlowスイッチは、起動すると指定したOpenFlowコントローラへ接続します。コントローラはスイッチの接続を検知すると、起動したスイッチの情報を表示します。逆にスイッチが予期せぬ障害など何らかの原因で接続を切った場合、コントローラはこれを検知して警告を表示します。また現在接続しているスイッチの一覧を一定時間ごとに表示することによって、何らかの障害で落ちてしまったスイッチを発見したり、予期しないスイッチがネットワークに接続されていることをネットワーク管理者が発見できるようにします。

== SwitchMonitorコントローラ

まずはざっとスイッチ監視ツールのソースコード(@<list>{switch-monitor.rb})を眺めてみましょう。このソースコードはTremaのサンプルアプリに付属する@<tt>{switch_monitor/switch-monitor.rb}でも読むことができます。今回もRubyの文法は後まわしで先に実行してみますので、細かいところは気する必要はありません。@<chap>{openflow_framework_trema}で学んだRubyの品詞を頭の片隅に置きながら、ざっくりと次のコードに目を通してみてください。

//list[switch-monitor.rb][スイッチ監視ツール (@<tt>{switch-monitor.rb}) のソースコード]{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10


  def start
    @switches = []
  end


  def switch_ready( datapath_id )
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end


  def switch_disconnected( datapath_id )
    @switches -= [ datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end


  private


  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

新しい品詞や構文がいくつかありますが、今までに学んだ知識だけでこのRubyソースコードの構成はなんとなくわかったはずです。まず、スイッチ監視ツールの本体は@<tt>{SwitchMonitor}という名前のクラスです。そしてこのクラスにはいくつかハンドラメソッドが定義してあるようです。おそらくそれぞれがスイッチの接続や切断イベントを処理しているんだろう、ということが想像できれば上出来です。

=== 実行してみよう

それでは早速実行してみましょう。でも、スイッチ監視ツールを試すには実際にOpenFlowスイッチを何台か接続してみなければなりません。どうすればいいでしょうか？

なんと、Tremaを使えばOpenFlowスイッチを持っていなくてもこうしたコントローラを実環境さながらに実行できます。その秘密はTremaの強力な機能の1つ、仮想ネットワーク機能にあります。これは仮想的なスイッチやホストからなる仮想ネットワークを開発マシン内に作ってしまう機能です。この仮想ネットワークを使えば、実際のOpenFlowスイッチを用意しなくとも開発マシン内でコントローラをテストできるのです。しかも、こうして開発したコントローラは実際のネットワークでもそのまま動作します！

この仮想ネットワーク機能を使うには、まずはそのネットワーク構成を設定ファイルとして書く必要があります。たとえば、仮想スイッチ1台からなる仮想ネットワークを設定ファイルは次のように書きます。

//emlist{
vswitch { datapath_id 0xabc }
//}

@<tt>{vswitch}で始まる行が1台の仮想スイッチに対応します。続く波括弧 (@<tt>{{}}) 内で指定されている@<tt>{datapath_id}(@<tt>{0xabc})は、仮想スイッチを識別するための16進数の値です。これはちょうどネットワークカードにおけるMACアドレスのような存在で、スイッチを一意に特定するIDとして使われます。OpenFlowの仕様によると、この値には64ビットの一意な整数値を割り振ることになっています。仮想スイッチでは好きな値を設定できるので、もし仮想スイッチを複数台作る場合にはお互いがぶつからないように注意してください。

基本がわかったところで、試しに仮想スイッチ3台の構成でスイッチ監視ツールを起動してみましょう。次の内容の設定ファイルを@<tt>{switch-monitor.conf}として保存してください。なお、それぞれの@<tt>{datapath_id}がお互いにかぶらないように@<tt>{0x1}、@<tt>{0x2}、@<tt>{0x3}と連番を振っていることに注意してください。

//emlist{
vswitch { datapath_id 0x1 }
vswitch { datapath_id 0x2 }
vswitch { datapath_id 0x3 }
//}

@<tt>{switch-monitor.rb}コントローラをこの仮想ネットワーク内で実行するには、この設定ファイルを@<tt>{trema run}の@<tt>{-c}オプションに渡します。スイッチ監視ツールの出力は次のようになります。

//cmd{
% trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
//}

最初の3行で (@<tt>{Switch 0x1 is UP}などの行)、仮想ネットワーク設定ファイルに定義したスイッチ3台が検出されていることがわかります 。また、一定時間ごとにスイッチ一覧が出力されています(@<tt>{All switches = 0x1, 0x2, 0x3}の行)。どうやらスイッチ監視ツールはうまく動いているようです。このように実際にスイッチを持っていなくても、設定ファイルを書くだけでスイッチを何台も使ったコントローラの動作テストができました!@<tt>{vswitch { }}の行を増やせば、スイッチをさらに5台、10台…と足していくことも思いのままです。

====[column] 友太郎の質問: Datapath ってどういう意味？

実用的には"Datapath=OpenFlowスイッチ"と考えて問題ありません。

"データパス"で検索すると、「CPUは演算処理を行うデータパスと、指示を出すコントローラから構成されます」というハードウェア教科書の記述がみつかります。つまり、ハードウェアの世界では一般に

 * 筋肉にあたる部分=データパス
 * 脳にあたる部分=コントローラ
 
という分類をするようです。

OpenFlow の世界でもこれと同じ用法が踏襲されています。OpenFlowのデータパスはパケット処理を行うスイッチを示し、その制御を行うソフトウェア部分をコントローラと呼びます。

====[/column]

=== 仮想スイッチを停止／再起動してみる

それでは、スイッチの切断がうまく検出されるか確かめてみましょう。仮想スイッチを停止するコマンドは@<tt>{trema kill}です。@<tt>{trema run}を実行したターミナルはそのままで別ターミナルを開き、次のコマンドで仮想スイッチ@<tt>{0x3}を落としてみてください。

//cmd{
% trema kill 0x3
//}

すると、@<tt>{trema run}を動かしたターミナルで新たに@<tt>{"Switch 0x3 is DOWN"}の行が出力されているはずです。

//cmd{
% trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
Switch 0x3 is DOWN
//}

うまくいきました！それでは、逆にさきほど落とした仮想スイッチを再び起動してみましょう。仮想スイッチを起動するコマンドは@<tt>{trema up}です。

//cmd{
% trema up 0x3
//}

@<tt>{"Switch 0x3 is UP"}の行が出力されれば成功です。

//cmd{
% trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
Switch 0x3 is DOWN
……
Switch 0x3 is UP
//}

このように、@<tt>{trema kill}と@<tt>{trema up}は仮想ネットワークのスイッチを制御するためのコマンドです。引数にスイッチのDatapath IDを指定することで、スイッチを停止または起動してコントローラの反応を確かめることができます。

 * @<tt>{trema kill [Datapath ID]}：指定した仮想スイッチを停止する
 * @<tt>{trema up [Datapath ID]}：指定した仮想スイッチを起動する

#@warn(リンクを切ったりつなげたりする例をトポロジの章で紹介 & Hello Trema の章からも引用)

スイッチ監視ツールの動作イメージがわかったところで、そろそろソースコードの解説に移りましょう。まずはスイッチ監視ツールのソースコードで使うRubyの品詞や構文を頭に入れておきましょう。

== 即席Ruby入門

スイッチ監視ツールのソースコード(@<list>{switch-monitor.rb})を眺めると、いくつか見慣れない品詞や構文が登場していることに気付きます。この節では順にそれぞれを紹介していきますが、最初からすべてを覚える必要はありません。もし後でわからなくなったときには見直すようにしてください。

=== シンボル

#@warn(以下の説明に出てくる、文字列とメソッドの引数をまだ説明していない)

シンボルは文字列の軽量版と言える品詞です。英字、数字、アンダースコアを含めることができますが、@<tt>{:a} や @<tt>{:number} や @<tt>{:show_switches} のように必ずコロンで始めることになっています。シンボルは定数のように一度決めると変更できないので、文字列のようにいつの間にか書き変わっている心配がありません。このため、ハッシュテーブルの検索キーとしてよく使われます。

また、シンボルは誰かにメソッドを名前で渡すときにも登場します。これだけですと分かりづらいと思うので、具体的な例を見ていきましょう。@<list>{switch-monitor.rb}には、次のようにシンボルを使っている箇所がありました。

//emlist{
periodic_timer_event :show_switches, 10
//}

この@<tt>{:show_switches}は@<tt>{SwitchMonitor}クラスのメソッド名です。ここでやっていることを簡単に言うと、@<tt>{periodic_timer_event}の引数として@<tt>{show_switches}メソッドの名前を渡しています。

もしここでシンボルを使わずに、直接次のように指定するとどうなるでしょうか？

//emlist{
# まちがい!
periodic_timer_event show_switches, 10
//}

これではうまく動きません。なぜならば、ソースコードの中に@<tt>{show_switches}とメソッドの名前を書いた時点でそのメソッドが実行されてしまい、その返り値が@<tt>{periodic_timer_event}へと渡されてしまうからです。

そこで、もしソースコード中でメソッドの名前を誰かに渡すときには、かならずシンボルにすることで実行されないようにして渡します。これがシンボルがよく使われるもう1つの用途です。

=== インスタンス変数

アットマーク(@<tt>{@})で始まる語はインスタンス変数です。@<tt>{@x}や@<tt>{@age}や@<tt>{@switches}がそれに当たります。これらの変数はたとえば人間の歳や身長などといった属性を定義するときによく使われます。アットマークはアトリビュート(属性)を意味すると考えれば覚えやすいでしょう。

インスタンス変数は同じクラスの中のメソッド定義内であればどこからでも使えます。具体的な例として次の@<tt>{Human}クラスを見てください。

//emlist{
# 人間を表すクラス
class Human
  # 初期化。生まれたときは歳は0歳
  def initialize
    @age = 0
  end


  # 一年に一度、歳をとる
  def happy_birthday
    @age += 1
  end
end
//}

@<tt>{Human}クラスで定義される@<tt>{Human}オブジェクトは、初期化したときにはそのインスタンス変数@<tt>{@age}は0、つまり0歳です。@<tt>{happy_birthday}を呼び出すたびに歳をとり@<tt>{@age}が1インクリメントします。このように@<tt>{@age}は@<tt>{initialize}および@<tt>{happy_birthday}メソッドのどちらからでもその値を変更できます。

=== 配列

配列は角カッコで囲まれたリストで、カンマで区切られています。

 * @<tt>{[]} は空の配列
 * @<tt>{[ 1, 2, 3 ]} は数字の配列
 * @<tt>{[ "友太郎", "青井", "宮坂主任" ]} は文字列の配列
 
Rubyの配列はとても直感的に要素を足したり取り除いたりできます。たとえば配列の最後に要素を加えるには@<tt>{<<}を使います。

//emlist{
members = [ "友太郎", "青井", "宮坂主任" ]
members << "取間先生"
#=> [ "友太郎", "青井", "宮坂主任", "取間先生" ]
//}

配列から要素を取り除くには@<tt>{-=}を使います。これは左右の配列どうしを見比べ、共通する要素を取り除いてくれます。

//emlist{
fruits = [ "バナナ", "みかん", "テレビ", "りんご", "たわし" ]
fruits -= [ "テレビ", "たわし" ]
#=> [ "バナナ", "みかん", "りんご" ]
//}

配列はRubyで多用するデータ構造で、この他にもたくさんの操作があらかじめ定義されています。もし詳しく知りたい人は@<chap>{openflow_framework_trema}の参考文献で紹介したRubyのサイトや書籍を参照してください。

=== メソッド呼び出し

定数や変数が名詞なら、メソッドは動詞です。メソッドは普通、変数や定数の後にドットでつなげます。

//emlist{
door.open
//}

//noindent
上の例では @<tt>{open} がメソッドです。英語の open は動詞なので、当然メソッドであるとも言えます。@<br>{}

ふつう、メソッド呼び出しによって何らかの動作をすると新しい結果が得られます。

//emlist{
"redrum".reverse
#=> "murder"
//}

//noindent
この場合、文字列がひっくりかえり新しい文字列が返ってきました。@<br>{}

質問すると答を返してくれることもあります。

//emlist{
[ "卵" ].empty?
#=> false
//}

//noindent
要素が一つの配列(@<tt>{[ "卵" ]})は空ではないので@<tt>{false}(偽)が返ってきました。@<br>{}

メソッドは引数を取るものもあります。次の例は配列の各要素の間に指定した文字をはさんで連結(@<tt>{join})します。

//emlist{
[ "M", "A", "S", "H" ].join( "★" )
#=> "M★A★S★H"
//}

Rubyにはこのような組込みメソッドが何百種類もあります。それぞれの動作は名前から大体想像できるものがほとんどです。@<br>{}

さて、今回は少し長くなりましたがこれで必要なRubyの勉強は終わりです。わからなくなったらいつでも戻って読み返してください。

== SwitchMonitorのソースコード

それではスイッチ監視ツールのソースコードを読み解いていきましょう。今回の肝となるのは、スイッチの接続と切断イベントをハンドラで検知する部分と、スイッチ一覧を一定時間ごとに表示する部分です。Tremaはスイッチの接続と切断を捕捉するための2つのハンドラメソッドを提供しています。

 * @<tt>{switch_ready}：OpenFlowスイッチは、起動するとOpenFlowコントローラへ接続しにいく。コントーラはスイッチとの接続が確立すると@<tt>{switch_ready}ハンドラが呼ばれ、引数にはスイッチのDatapath IDが渡される
 * @<tt>{switch_disconnected}：スイッチが障害など何らかの原因でコントローラとの接続を切った場合、コントローラの@<tt>{switch_disconnected}ハンドラが呼ばれ、引数にはスイッチのDatapath IDが渡される

===[column] 取間先生曰く：@<tt>{switch_ready}の中身

実はOpenFlowの仕様には@<tt>{switch_ready}というメッセージは定義されていません。実は、これはTremaが独自に定義するイベントなのです。@<tt>{switch_ready}の裏では@<img>{switch_ready}に示す一連の複雑な処理が行われていて、Tremaがこの詳細をうまくカーペットの裏に隠してくれているというわけです。

//image[switch_ready][@<tt>{switch_ready} イベントが起こるまで][width=12cm]

最初に、スイッチとコントローラがしゃべるOpenFlowプロトコルが合っているか確認します。OpenFlowのHelloメッセージを使ってお互いにプロトコルバージョンを知らせる、うまく会話できそうか判断します。

次は、スイッチを識別するためのDatapath IDの取得です。Datapath IDのようなスイッチ固有の情報は、スイッチに対してOpenFlowのFeatures Requestメッセージを送ることで取得できます。成功した場合、Datapath IDやポート数などの情報がFeatures Replyメッセージに乗ってやってきます。

最後にスイッチを初期化します。スイッチに以前の状態が残っているとコントローラが管理する情報と競合が起こるので、スイッチを初期化することでこれを避けます。これら一連の処理が終わると、ようやく@<tt>{switch_ready}がコントローラに通知されるというわけです。

===[/column]

=== スイッチの起動を捕捉する

@<tt>{switch_ready}ハンドラでは、スイッチ一覧リストに新しく接続したスイッチのDatapath IDを追加し、接続したスイッチの情報を画面に表示します。

//emlist{
class SwitchMonitor < Controller
  # ...

  def start
    @switches = []
  end


  def switch_ready( datapath_id )
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end

  # ...
end
//}

@<tt>{@switches}は現在起動しているスイッチのDatapath IDを管理するインスタンス変数で、@<tt>{start}ハンドラで空の配列に初期化されます。新しくスイッチが起動すると@<tt>{switch_ready}ハンドラが起動し、@<tt>{@switches}が保持する配列の最後にそのDatapath IDを追加します。また、@<tt>{info}メソッドで新しいスイッチのDatapath IDを表示します。

==== Datapath IDを16進にする

@<tt>{to_hex}は整数を16進の文字列に変換するメソッドです。@<tt>{switch_ready}ハンドラの引数@<tt>{datapath_id}の値は@<tt>{65531}のような整数ですが、Datapath ID は@<tt>{0xfffb}のように16進で書くことが多いため画面に表示する場合は16進変換しておいたほうが良いでしょう。

==== ログを出力する

@<tt>{info}メソッドはTremaが提供するロギングメソッドです。他にも、出力するメッセージの重要度に応じたさまざまなロギングメソッドが用意されています。

 * @<tt>{critical}：回復不能なエラー
 * @<tt>{error}：エラー
 * @<tt>{warn}：警告
 * @<tt>{notice}：注意が必要な情報
 * @<tt>{info}：通常レベルの情報
 * @<tt>{debug}：デバッグ出力

メッセージはコントローラのログファイル(@<tt>{[trema]/tmp/log/SwitchMonitor.log})に出力されます。@<tt>{trema run}をフォアグラウンドで実行している場合にはターミナルにも出力されます。

==== 文字列を連結する

@<tt>{info}に渡される文字列中の@<tt>{#{...\}}は、文字列内にRubyの式を組込む文法です。

//emlist{
info "Switch #{ datapath_id.to_hex } is UP"
//}

これは次のコードと同じです。

//emlist{
info "Switch " + datapath_id.to_hex + " is UP"
//}

どちらを使ってもかまいませんが、@<tt>{+}による文字列の連結を使いすぎると最終的な出力がわかりにくくなることがあるため、このように@<tt>{#{...\}}で組み込んだ方が良いこともあります。

=== スイッチの切断を捕捉する

@<tt>{switch_disconnected}ハンドラでは、スイッチ一覧リストから切断したスイッチのDatapath IDを削除し、切断したスイッチの情報を画面に表示します。

//emlist{
class SwitchMonitor < Controller
  # ...

  def switch_disconnected( datapath_id )
    @switches -= [ datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end

  # ...
end
//}

ここでは@<tt>{switch_ready}とは逆に、配列の引き算(@<tt>{-=})で切断したスイッチのDatapath IDを@<tt>{@switches}から除いていることに注意してください。

=== スイッチ一覧を一定時間ごとに表示する

#@warn(クラスメソッドの説明)

最後に、スイッチの一覧を一定時間ごとに表示する部分です。このようにいわゆるタイマー処理を行いたい場合には、Tremaのタイマー機能を使います。次のように@<tt>{periodic_timer_event}に続いて一定間隔ごとに呼び出したいメソッドのシンボル名、間隔を秒数で指定しておくと、指定したメソッドが指定された間隔ごとに呼ばれます。

//emlist{
class Human < Controller
  # 1年(31536000 秒)ごとにhappy_birthdayメソッドを呼ぶ
  periodic_timer_event :happy_birthday, 31536000
//}

この定義はクラス名定義の直後に来るので、まるでクラス属性としてタイマーがセットされているように読めることに注目してください。このようにTremaではタイマー処理も短く読みやすく書けるのです。

タイマーで呼び出すメソッドは、通常クラスの外からは呼びませんのでよくプライベートメソッドとして定義されます。Rubyでは@<tt>{private}と書いた行以降のメソッドはプライベートメソッドとして定義され、クラスの外からは見えなくなります。

//emlist{
class Human < Controller
  periodic_timer_event :happy_birthday, 31536000

  # ...

  private

  def happy_birthday
    @age += 1
  end
//}

これを踏まえてスイッチ監視ツールのソースコードのタイマー部分を見てみましょう。

//emlist{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10

  # ...

  private

  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

クラス名定義直後のタイマー定義より、10秒ごとに@<tt>{show_switches}メソッドを呼んでいることがわかります。@<tt>{show_switches}メソッドでは、見やすい出力を得るためにスイッチのDatapath IDのリスト(@<tt>{@switches})をアルファベット順にソートし(@<tt>{sort})、カンマでつなげて(@<tt>{join})表示するという工夫をしています。なおこの@<tt>{sort}と@<tt>{join}は、ともにRubyが提供する配列のメソッドです。

== まとめ

この章ではスイッチの動作状況を監視するスイッチ監視ツールを作りました。また、作ったスイッチ監視ツールをテストするためTremaの仮想ネットワーク機能を使いました。

 * スイッチの起動と切断を捕捉するには、@<tt>{switch_ready}と@<tt>{switch_disconnected}ハンドラメソッドを定義する
 * タイマー(@<tt>{periodic_timer_event})を使うと一定間隔ごとに指定したメソッドを起動できる
 * 仮想ネットワーク機能を使うとOpenFlowスイッチを持っていなくてもコントローラのテストができる

今回作ったスイッチ監視ツールのように、比較的短いソースコードでもそこそこ実用的なツールを書けるところがTrema+OpenFlowの魅力です。続く章では、もう1つの便利ツールの例として遠隔操作可能なソフトウェアパッチパネルを作ります。ネットワークケーブルを挿し替えるためだけにラックのあるサーバルームまで出向く必要はなくなります。
