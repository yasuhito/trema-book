= スイッチ監視ツール

#@warn(気の利いたリード文をここに)

== ネットワークを集中管理しよう

OpenFlow の特徴は「たくさんのスイッチをひとつのコントローラで集中制御する」という点にありました。スイッチにはフローテーブルに従ったパケットの転送という単純な仕事だけをやらせ、頭脳であるコントローラが全体のフローを統括するというわけです。これによって第 I 部で見てきたように、自動化やシステム連携のしやすさ、トラフィックエンジニアリングの容易さ、スイッチの低価格化、そしてベンダロックインの防止といったさまざまなメリットが生まれるのでした。

本章ではこの集中制御の一例として、スイッチの監視ツールを作ります。このツールは「今、ネットワーク中にどんなスイッチが動いているか」をリアルタイムに表示します。たくさんあるスイッチ全体が正常に動いているかを確認するのに便利です。

//image[switch_monitor_overview][スイッチ監視ツール]

スイッチ監視ツールは@<img>{switch_monitor_overview} のように動作します。OpenFlow スイッチは、起動すると指定した OpenFlow コントローラへ接続します。コントローラはスイッチの接続を検知すると、起動したスイッチの情報を表示します。逆にスイッチが予期せぬ障害など何らかの原因で接続を切った場合、コントローラはこれを検知して警告を表示します。また現在接続しているスイッチの一覧を一定時間ごとに表示することによって、何らかの障害で落ちてしまったスイッチを発見したり、予期しないスイッチがネットワークに接続されていることをネットワーク管理者が発見できるようにします。

== SwitchMonitor コントローラ

まずはざっとスイッチ監視ツールのソースコード (@<list>{switch-monitor.rb}) を眺めてみましょう。このソースコードは Trema のサンプルアプリに付属する @<tt>{switch_monitor/switch-monitor.rb} でも読むことができます。今回も Ruby の文法は後まわしで先に実行してみますので、細かいところは気する必要はありません。@<chap>{openflow_framework_trema}で学んだ Ruby の品詞を頭の片隅に置きながら、ざっくりと次のコードに目を通してみてください。

//list[switch-monitor.rb][スイッチ監視ツール (@<tt>{switch-monitor.rb}) のソースコード]{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10


  def start
    @switches = []
  end


  def switch_ready datapath_id
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end


  def switch_disconnected datapath_id
    @switches -= [ datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end


  private


  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

新しい品詞や構文がいくつかありますが、今までに学んだ知識だけでこの Ruby ソースコードの構成はなんとなくわかったはずです。まず、スイッチ監視ツールの本体は @<tt>{SwitchMonitor} という名前のクラスです。そしてこのクラスにはいくつかハンドラメソッドが定義してあるようです。おそらくそれぞれがスイッチの接続や切断イベントを処理しているんだろう、ということが想像できれば上出来です。

=== 実行してみよう

それでは早速実行してみましょう。でも、スイッチ監視ツールを試すには実際に OpenFlow スイッチを何台か接続してみなければなりません。どうすればいいでしょうか？

なんと、Trema を使えば OpenFlow スイッチを持っていなくてもこうしたコントローラを実環境さながらに実行できます。その秘密は Trema の強力な機能の 1 つ、仮想ネットワーク機能にあります。これは仮想的なスイッチやホストからなる仮想ネットワークを開発マシン内に作ってしまう機能です。この仮想ネットワークを使えば、実際の OpenFlow スイッチを用意しなくとも開発マシン内でコントローラをテストできるのです。しかも、こうして開発したコントローラは実際のネットワークでもそのまま動作します！

この仮想ネットワーク機能を使うには、まずはそのネットワーク構成を設定ファイルとして書く必要があります。たとえば、仮想スイッチ 1 台からなる仮想ネットワークを設定ファイルは次のように書きます。

//emlist{
vswitch { datapath_id 0xabc }
//}

@<tt>{vswitch} で始まる行が 1 台の仮想スイッチに対応します。続く括弧 (@<tt>{{...}}) 内で指定されている @<tt>{datapath_id} (@<tt>{0xabc}) は、仮想スイッチを識別するための 16 進数の値です。これはちょうどネットワークカードにおける MAC アドレスのような存在で、スイッチを一意に特定する ID として使われます。OpenFlow の仕様によると、この値には 64 ビットの一意な整数値を割り振ることになっています。仮想スイッチでは好きな値を設定できるので、もし仮想スイッチを複数台作る場合にはお互いがぶつからないように注意してください。

基本がわかったところで、試しに仮想スイッチ 3 台の構成でスイッチ監視ツールを起動してみましょう。次の内容の設定ファイルを @<tt>{switch-monitor.conf} として保存してください。なお、それぞれの @<tt>{datapath_id} がお互いにかぶらないように、@<tt>{0x1}、@<tt>{0x2}、@<tt>{0x3} と連番を振っていることに注意してください。

//emlist{
vswitch { datapath_id 0x1 }
vswitch { datapath_id 0x2 }
vswitch { datapath_id 0x3 }
//}

@<tt>{switch-monitor.rb} コントローラをこの仮想ネットワーク内で実行するには、この設定ファイルを @<tt>{trema run} の "@<tt>{-c}" オプションに渡します。スイッチ監視ツールの出力は次のようになります。

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

最初の 3 行で (@<tt>{"Switch 0x1 is UP"} などの行)、仮想ネットワーク設定ファイルに定義したスイッチ 3 台が検出されていることがわかります 。また、一定時間ごとにスイッチ一覧が出力されています (@<tt>{"All switches = 0x1, 0x2, 0x3"} の行)。どうやらスイッチ監視ツールはうまく動いているようです。このように実際にスイッチを持っていなくても、設定ファイルを書くだけでスイッチを何台も使ったコントローラの動作テストができました! @<tt>{vswitch {...}} の行を増やせば、スイッチをさらに 5 台、10 台…と足していくことも思いのままです。

====[column] 友太郎の質問: Datapath ってどういう意味？

Q. 「スイッチに付いている ID を Datapath ID って呼ぶのはわかったんだけど、そもそもいったい Datapath ってなに？ スイッチのこと？」@<br>{}

A. 実用的には "Datapath = OpenFlow スイッチ" と考えて問題ありません。

"データパス" でググると、「CPU は演算処理を行うデータパスと、指示を出すコントローラから構成されます」というハードウェア教科書の記述がみつかります。つまり、ハードウェアの世界では一般に

 * 筋肉にあたる部分 = データパス
 * 脳にあたる部分 = コントローラ
 
という分類をするようです。

OpenFlow の世界でもこれと同じ用法が踏襲されています。OpenFlow のデータパスはパケット処理を行うスイッチを示し、その制御を行うソフトウェア部分をコントローラと呼びます。

====[/column]

=== 仮想スイッチを停止/再起動してみる

それでは、スイッチの切断がうまく検出されるか確かめてみましょう。仮想スイッチを停止するコマンドは @<tt>{trema kill} です。@<tt>{trema run} を実行したターミナルはそのままで別ターミナルを開き、次のコマンドで仮想スイッチ @<tt>{0x3} を落としてみてください。

//cmd{
% trema kill 0x3
//}

すると、@<tt>{trema run} を動かしたターミナルで新たに @<tt>{"Switch 0x3 is DOWN"} の行が出力されているはずです。

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

うまくいきました！それでは、逆にさきほど落とした仮想スイッチを再び起動してみましょう。仮想スイッチを起動するコマンドは @<tt>{trema up} です。

//cmd{
% trema up 0x3
//}

@<tt>{"Switch 0x3 is UP"} の行が出力されれば成功です。

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

このように、@<tt>{trema kill} と @<tt>{trema up} は仮想ネットワークのスイッチを制御するためのコマンドです。引数にスイッチの Datapath ID を指定することで、スイッチを停止または起動してコントローラの反応を確かめることができます。

 * @<tt>{trema kill [Datapath ID]}: 指定した仮想スイッチを停止する。
 * @<tt>{trema up [Datapath ID]}: 指定した仮想スイッチを起動する。

#@warn(リンクを切ったりつなげたりする例をトポロジの章で紹介 & Hello Trema の章からも引用)

スイッチ監視ツールの動作イメージがわかったところで、そろそろソースコードの解説に移りましょう。まずはスイッチ監視ツールのソースコードで使う Ruby の品詞や構文を頭に入れておきましょう。

== 即席 Ruby 入門

スイッチ監視ツールのソースコード (@<list>{switch-monitor.rb}) を眺めると、いくつか見慣れない品詞や構文が登場していることに気付きます。この節では順にそれぞれを紹介していきますが、最初からすべてを覚える必要はありません。もし後でわからなくなったときには見直すようにしてください。

=== シンボル

#@warn(以下の説明に出てくる、文字列とメソッドの引数をまだ説明していない)

シンボルは文字列の軽量版と言える品詞です。英字、数字、アンダースコアを含めることができますが、@<tt>{:a} や @<tt>{:number} や @<tt>{:show_switches} のように必ずコロンで始めることになっています。シンボルは定数のように一度決めると変更できないので、文字列のようにいつの間にか書き変わっている心配がありません。このため、ハッシュテーブルの検索キーとしてよく使われます。

また、シンボルは "引数にメソッドを取るメソッド" を使うときにも登場します。これだけですと分かりづらいと思うので、具体的な例を見ていきましょう。まず、次の @<tt>{hello} は画面に "Hello" と表示するだけの簡単なメソッドです。

//emlist{
# 画面に Hello と表示する
def hello
  puts "Hello"
end
//}

ここで、指定したメソッドを 3 回実行してくれる @<tt>{repeat_3times} というメソッドがあったとします。もし @<tt>{hello} を 3 回実行したければ、この @<tt>{repeat_3times} に 「@<tt>{hello} メソッドを 3 回実行して」とお願いすればいいわけです。

たとえばもし、次のように @<tt>{repeat_3times} の引数に @<tt>{hello} を直接渡してしまうとどうなるでしょうか。

//emlist{
# まちがい!
@<tt>{repeat_3times} hello
//}

この場合、@<tt>{repeat_3times} の実行に入る前に引数の @<tt>{hello} が先に実行され、その返り値だけが @<tt>{repeat_3times} への引数として渡されてしまいます。このため @<tt>{repeat_3times} は正しく @<tt>{hello} を 3 回実行できません@<fn>{repeat_3times}。

//footnote[repeat_3times][@<tt>{hello} の返り値は@<ruby>{空,から} (Ruby の世界では @<tt>{nil} と表現されます) なので @<tt>{repeat_3times nil} と同じになり、意図した通りには動きません。]

そこでもしメソッドの引数にメソッド名を渡したいときには、引数として渡すメソッド名を @<tt>{:hello} とシンボルにし、誤って先に実行されないようにします。

//emlist{
# 正しい
@<tt>{repeat_3times} :hello
//}

こうすれば、シンボルを渡された @<tt>{repeat_3times} 側ではシンボルから元のメソッドを取り出して呼び出すことができます@<fn>{yield}。このように、メソッドの引数にメソッド名を渡すときはシンボルを使う、と覚えてください。

//footnote[yield][本書では説明しませんが、シンボルで指定されたメソッドを呼び出すには @<tt>{send} メソッドを使います。詳しくは@<chap>{openflow_framework_trema}の参考文献で紹介した Ruby の入門書などを参照してください。]

=== インスタンス変数

アットマーク (@<tt>{@}) で始まる語はインスタンス変数です。@<tt>{@x} や @<tt>{@age} や @<tt>{@switches} がそれに当たります。これらの変数はたとえば人間の歳や身長などといった属性を定義するときによく使われます。アットマークはアトリビュート (属性) を意味すると考えれば覚えやすいでしょう。

インスタンス変数は同じクラスの中のメソッド定義内であればどこからでも使えます。具体的な例として次の Human クラスを見てください。

//emlist{
# 人間を表すクラス
class Human
  # 初期化。生まれたときは歳は 0 歳
  def initialize
    @age = 0
  end


  # 一年に一度歳をとる
  def happy_birthday
    @age += 1
  end
end
//}

@<tt>{Human} クラスで定義される @<tt>{Human} オブジェクトは、初期化したときにはそのインスタンス変数 @<tt>{@age} は 0、つまり 0 歳です。@<tt>{happy_birthday} を呼び出すたびに歳をとり @<tt>{@age} が 1 インクリメントされます。このように @<tt>{@age} は @<tt>{initialize} および @<tt>{happy_birthday} メソッドのどちらからでもその値を変更できます。

=== 配列

配列は角カッコで囲まれたリストで、カンマで区切られています。

 * @<tt>{[]} は空の配列です。
 * @<tt>{[ 1, 2, 3 ]} は数字の配列です。
 * @<tt>{[ "友太郎", "青井さん", "宮坂主任" ]} は文字列の配列です。
 
Ruby の配列はとても直感的に要素を足したり取り除いたりできます。たとえば配列の最後に要素を加えるには @<tt>{<<} を使います。

//emlist{
members = [ "友太郎", "青井さん", "宮坂主任" ]
members << "取間先生"
#=> [ "友太郎", "青井さん", "宮坂主任", "取間先生" ]
//}

//noindent
配列から要素を取り除くには @<tt>{-=} を使います。これは左右の配列どうしを見比べ、共通する要素を取り除いてくれます。

//emlist{
fruits = [ "バナナ", "みかん", "テレビ", "りんご", "たわし" ]
fruits -= [ "テレビ", "たわし" ]
#=> [ "バナナ", "みかん", "りんご" ]
//}

配列は Ruby で多用するデータ構造で、この他にもたくさんの操作があらかじめ定義されています。もし詳しく知りたい人は @<chap>{openflow_framework_trema} の参考文献で紹介した Ruby のサイトや書籍を参照してください。

=== メソッド呼び出し

定数や変数が名詞なら、メソッドは動詞です。メソッドは普通、変数や定数の後にドットでつなげられています。

//emlist{
door.open
//}

上の例では @<tt>{open} がメソッドです。英語の open は動詞なので、当然メソッドであるとも言えます。@<br>{}

//noindent
ふつう、メソッド呼び出しによって何らかの動作をすると新しい結果が得られます。

//emlist{
"redrum".reverse
#=> "murder"
//}

//noindent
この場合、文字列がひっくりかえり新しい文字列が返ってきました。@<br>{}

//noindent
質問すると答を返してくれることもあります。

//emlist{
[ "卵" ].empty?
#=> false
//}

//noindent
要素が一つの配列 (@<tt>{[ "卵" ]}) は空ではないので @<tt>{false} (偽) が返ってきました。@<br>{}

//noindent
メソッドは引数を取るものもあります。次の例は配列の各要素の間に指定した文字をはさんで連結 (@<tt>{join}) しています。

//emlist{
[ "M", "A", "S", "H" ].join( "★" )
#=> "M★A★S★H"
//}

//noindent
Ruby にはこのような組込みメソッドが何百種類もあります。それぞれの動作は名前から大体想像できるものがほとんどです。@<br>{}

さて、今回は少し長くなりましたがこれで必要な Ruby の勉強は終わりです。わからなくなったらいつでも戻って読み返してください。

== ソースコード

それではスイッチ監視ツールのソースコードを読み解いていきましょう。今回の肝となるのは、スイッチの接続と切断イベントをハンドラで検知する部分と、スイッチ一覧を一定時間ごとに表示する部分です。Trema はスイッチの接続と切断を捕捉するための 2 つのハンドラメソッドを提供しています。

 * @<tt>{switch_ready}: OpenFlow スイッチは、起動すると OpenFlow コントローラへ接続しに行きます。コントーラはスイッチとの接続が確立すると @<tt>{switch_ready} ハンドラが呼ばれます。引数にはスイッチの Datapath ID が渡されます。
 * @<tt>{switch_disconnected}: スイッチが障害など何らかの原因でコントローラとの接続を切った場合、コントローラの @<tt>{switch_disconnected} ハンドラが呼ばれます。引数にはスイッチの Datapath ID が渡されます。

===[column] 取間先生いわく: @<tt>{switch_ready} の中身はどうなっている？

実は OpenFlow の仕様には @<tt>{switch_ready} というメッセージは定義されておらん。これは Trema が独自に定義するイベントなのじゃ。@<tt>{switch_ready} の裏では @<img>{switch_ready} に示す一連の複雑な処理が行われていて、Trema が OpenFlow プロトコルの詳細をうまくカーペットの裏に隠してくれているんじゃ。

//image[switch_ready][@<tt>{switch_ready} イベントが起こるまで]

最初に、スイッチとコントローラがしゃべる OpenFlow プロトコルが合っているか確認する。OpenFlow の Hello メッセージを使ってお互いのプロトコルバージョンを確認し、うまく会話できそうか確認するのじゃ。

次は、スイッチを識別するための Datapath ID の取得じゃ。Datapath ID のようなスイッチ固有の情報は、スイッチに対して OpenFlow の Features Request メッセージを送ることで取得できる。成功した場合、Datapath ID やポート数などの情報が Features Reply メッセージに乗ってやってくる。

最後にスイッチを初期化する。スイッチに以前の状態が残っているとコントローラが管理する情報と競合が起こるので、スイッチを初期化することでこれを避けるのじゃ。これら一連の処理が終わると、ようやく @<tt>{switch_ready} がコントローラに通知されるというわけじゃ。

===[/column]

=== スイッチの起動を捕捉する

@<tt>{switch_ready} ハンドラでは、スイッチ一覧リストに新しく接続したスイッチの Datapath ID を追加し、接続したスイッチの情報を画面に表示します。

//emlist{
class SwitchMonitor < Controller
  # ...

  def start
    @switches = []
  end


  def switch_ready datapath_id
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end

  # ...
end
//}

@<tt>{@switches} は現在起動しているスイッチの Datapath ID を管理するインスタンス変数で、@<tt>{start} ハンドラで空の配列に初期化されます。新しくスイッチが起動すると @<tt>{switch_ready} ハンドラが起動し、@<tt>{@switches} が保持する配列の末尾にその Datapath ID を追加します。また、@<tt>{info} メソッドで新しいスイッチの Datapath ID を表示します。

==== Datapath ID を 16 進にする

@<tt>{to_hex} は整数を 16 進の文字列に変換するメソッドです。@<tt>{switch_ready} ハンドラの引数 @<tt>{datapath_id} の値は @<tt>{65531} のような整数ですが、Datapath ID は @<tt>{0xfffb}  のように 16 進で書くことが多いため画面に表示する場合は 16 進変換しておいたほうが良いでしょう。

==== ロギングメソッド

@<tt>{info} メソッドは Trema が提供するロギングメソッドです。他にも、出力するメッセージの重要度に応じたさまざまなロギングレベル用メソッドが用意されてます。

 * @<tt>{critical}: 回復不能なエラー
 * @<tt>{error}: エラー
 * @<tt>{warn}: 警告
 * @<tt>{notice}: 注意が必要な情報
 * @<tt>{info}: 通常レベルの情報
 * @<tt>{debug}: デバッグ出力

メッセージはコントローラのログファイル (@<tt>{[trema]/tmp/log/SwitchMonitor.log}) に出力されます。また @<tt>{trema run} をフォアグラウンドで実行している場合にはターミナルにも出力されます。

==== 文字列の連結

@<tt>{info} に渡される文字列中の @<tt>{#{...\}} は、文字列内に Ruby の式を組込む文法です。

//emlist{
info "Switch #{ datapath_id.to_hex } is UP"
//}

これは次のコードと同じです。

//emlist{
info "Switch " + datapath_id.to_hex + " is UP"
//}

どちらを使ってもかまいませんが、@<tt>{+} による文字列の連結を使いすぎると最終的な出力がわかりにくくなることがあるため、このように @<tt>{#{...\}} で組み込んだ方が良いこともあります。

=== スイッチの切断を捕捉する

@<tt>{switch_disconnected} ハンドラでは、スイッチ一覧リストから切断したスイッチの Datapath ID を削除し、切断したスイッチの情報を画面に表示します。

//emlist{
class SwitchMonitor < Controller
  # ...

  def switch_disconnected datapath_id
    @switches -= [ datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end

  # ...
end
//}

@<tt>{switch_ready} とは逆に、配列の引き算 (@<tt>{-=}) で切断したスイッチの Datapath ID を @<tt>{@switches} から除いていることに注意してください。

=== スイッチ一覧を一定時間ごとに表示する

#@warn(クラスメソッドの説明)

最後に、スイッチの一覧を一定時間ごとに表示する部分です。このようにいわゆるタイマー処理を行いたい場合には、Trema のタイマー機能を使います。次のように @<tt>{periodic_timer_event} に続いて一定間隔ごとに呼び出したいメソッドのシンボル名、間隔を秒数で指定しておくと、指定されたメソッドが指定された間隔ごとに呼ばれます。

//emlist{
class Human < Controller
  # 1 年 (31536000 秒) ごとに happy_birthday メソッドを呼ぶ
  periodic_timer_event :happy_birthday, 31536000

//}

この定義はクラス名定義の直後に来るので、まるでクラス属性としてタイマーがセットされているように読めることに注目してください。このように Trema ではタイマー処理も短く読みやすく書けるのです。

タイマーで呼び出すメソッドは、通常クラスの外からは呼びませんのでよくプライベートメソッドとして定義されます。Ruby では @<tt>{private} と書いた行以降のメソッドはプライベートメソッドとして定義され、クラスの外からは見えなくなります。

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

クラス名定義直後のタイマー定義より、10 秒ごとに @<tt>{show_switches} メソッドを呼んでいることがわかります。@<tt>{show_switches} メソッドでは、見やすい出力を得るためにスイッチの Datapath ID のリスト (@<tt>{@switches}) をアルファベット順にソートし (@<tt>{sort})、カンマでつなげて (@<tt>{join}) 表示するという工夫をしています。

== まとめ

この章ではスイッチの動作状況を監視するスイッチ監視ツールを作りました。学んだことは次の3つです。また、作ったスイッチ監視ツールをテストするため Trema の仮想ネットワーク機能を使ってみました。

 * スイッチの起動と切断を捕捉するには、@<tt>{switch_ready} と @<tt>{switch_disconnected} ハンドラメソッドを定義します。
 * タイマー (@<tt>{periodic_timer_event}) を使うと一定間隔ごとに指定したメソッドを起動できます。
 * 仮想ネットワーク機能を使うと OpenFlow スイッチを持っていなくてもコントローラのテストができます。

今回作ったスイッチ監視ツールのように、比較的短いソースコードでもそこそこ実用的なツールを書けるところが Trema + OpenFlow の魅力です。続く章では、もう 1 つの便利ツールの例として遠隔操作可能なソフトウェアパッチパネルを作ります。ネットワークケーブルを挿し替えるためだけにラックのあるサーバルームまで出向く必要はなくなります。
