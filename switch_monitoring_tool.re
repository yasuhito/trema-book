= スイッチ監視ツール

#@warn(「運用コストの削減」を説明した章へのリンクを張る)

OpenFlow のメリットのひとつに、運用コストの削減がありました。OpenFlow に対応したスイッチであれば、どんなに数が増えてもリモートで集中的に管理できるのでした。本章ではこの集中管理ツールの一例として、スイッチの監視ツールを作ります。このツールは「今、ネットワーク中にどんなスイッチが動いているか」をリアルタイムに表示します。何らかの障害で落ちてしまったスイッチを発見したり、予期しないスイッチがネットワークに接続されていることを検出するのに便利です。

//indepimage[switch_monitor_overview]

スイッチ監視ツールの動きは次のようになります。OpenFlow スイッチは、起動すると指定した OpenFlow コントローラへ接続します。コントローラはスイッチが接続してきたことを検知すると、接続したスイッチを "起動中のスイッチ" リストに追加します。逆にスイッチが何らかの原因で接続を切った場合、コントローラはこれを検知していなくなったスイッチをリストから削除します。つまりスイッチの接続イベントと切断イベントを捕捉してリストを更新する、たったこれだけです。

== SwitchMonitor コントローラ

スイッチ監視ツールを動かすために、さっそくコントローラのソースコードを書いて実行してみます。適当なディレクトリに switch-monitor.rb というファイルを作り、エディタで次のコードを入力してください。今回も Ruby の文法は後まわしで先に実行してみますので、細かいところは気する必要はありません。@<chap>{openflow_framework_trema} で学んだ Ruby の品詞を頭の片隅に置きながら、そのまま写してください。

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
    @switches -= [datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end


  private


  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

新しい品詞がいくつかありますが、この Ruby ソースコードの構成はなんとなくわかったはずです。スイッチ監視ツールの本体は @<tt>{SwitchMonitor} という名前のクラスです。ここにいくつかハンドラメソッドが定義してあり、おそらくそれぞれがスイッチの接続や切断イベントを処理しているんだろう、ということが想像できれば上出来です。

=== 実行してみよう

それでは早速実行してみましょう。でも、スイッチ監視ツールは実際に OpenFlow スイッチを接続してみなければおもしろくありません。どうすればいいでしょうか？

なんと、Trema を使えば OpenFlow スイッチを持っていなくてもこうしたコードを実行できます。いったいどういうことでしょうか？その答は、Trema の強力な機能の 1 つ、仮想ネットワーク機能にあります。これは仮想 OpenFlow スイッチや仮想ホストどうしを接続した仮想ネットワークを開発マシン内に作ってしまう機能です。この仮想ネットワークを使えば、実際の OpenFlow スイッチを用意しなくとも開発マシン内で OpenFlow コントローラをテストできるのです。しかも、こうして開発したコントローラは実際のネットワークでもそのまま動作します！

仮想ネットワーク機能を使うには、その構成を記述した設定ファイルを用意します。たとえば、仮想スイッチ (@<tt>{vswitch}) を 1 台定義するには次のように書きます。

//emlist{
vswitch { datapath_id 0xabc }
//}

@<tt>{vswitch} に続く括弧 (@<tt>{{...}}) 内で指定されている @<tt>{datapath_id} (@<tt>{0xabc}) は、仮想スイッチを識別するための 16 進数の値です。これはちょうどネットワークカードにおける MAC アドレスのような存在で、スイッチを一意に特定する ID として使われます。OpenFlow 規格によると、この値には 64 ビットの一意な整数値を割り振ることになっています。仮想スイッチでは好きな値を設定できるので、もし仮想スイッチを複数台作る場合にはお互いがぶつからないように注意してください。

それでは試しに仮想スイッチ 3 台の構成でスイッチ監視ツールを起動してみます。次の内容の設定ファイルを @<tt>{switch-monitor.conf} として保存してください。なお、それぞれの @<tt>{datapath_id} がかぶらないように @<tt>{0x1}、@<tt>{0x2}、@<tt>{0x3} と連番を振っていることに注意してください。

//emlist{
vswitch { datapath_id 0x1 }
vswitch { datapath_id 0x2 }
vswitch { datapath_id 0x3 }
//}

コントローラを仮想ネットワークで実行するには、この設定ファイルを @<tt>{trema run} の "@<tt>{-c}" オプションに渡します。@<tt>{switch-monitor.rb} を次のように実行してみましょう。

//cmd{
% ./trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
//}

最初の 3 行で、スイッチ 3 台 (@<tt>{0x1}、@<tt>{0x2}、@<tt>{0x3}) が検出されていることがわかります (@<tt>{"Switch 0x? is UP"} の行)。また、一定時間ごとにスイッチ一覧が更新されています (@<tt>{"All switches = 0x1, 0x2, 0x3"} の行)。期待通り、仮想ネットワークの仮想スイッチ 3 台がコントローラによって検出されていることがわかりました。

===[column] 友太郎の質問: datapath ってどういう意味？

Q. 「スイッチに付いている ID を datapath ID って呼ぶのはわかったけど、いったい datapath ってなに？ スイッチのこと？」@<br>{}

A. 実用的には「datapath = OpenFlow スイッチ」と考えて問題ありません。

「データパス」でググると、「CPU は演算処理を行うデータパスと、指示を出すコントローラから構成されます」というハードウェア教科書の記述がみつかります。つまり、ハードウェアの世界では一般に「筋肉にあたる部分＝データパス」「脳にあたる部分＝コントローラ」という分類をするようです。

OpenFlow の世界でも同じ用法が踏襲されています。OpenFlow のデータパスはパケット処理を行うスイッチを示し、その制御を行うソフトウェア部分をコントローラと呼びます。

===[/column]

=== 仮想スイッチを停止/起動してみる

それでは、スイッチの切断がうまく検出されるか確かめてみましょう。スイッチを停止するコマンドは @<tt>{trema kill} です。別ターミナルを開き、次のコマンドでスイッチ @<tt>{0x3} を落としてみてください。

//cmd{
% ./trema kill 0x3
//}

すると、@<tt>{trema run} を動かしたターミナルに次の出力が表示されているはずです。

//cmd{
% ./trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
Switch 0x3 is DOWN
//}

うまくいきました！それでは、逆にさきほど落としたスイッチを起動してみましょう。

//cmd{
% ./trema up 0x3
//}

出力がこうなっていれば成功です。

//cmd{
% ./trema run ./switch-monitor.rb -c ./switch-monitor.conf
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

このように、@<tt>{trema kill} と @<tt>{trema up} は仮想ネットワークのスイッチを制御するためのコマンドです。引数にスイッチの Datapath ID を指定することで、スイッチを停止/起動してコントローラの反応を確かめることができます。次にそれぞれのコマンドをまとめておきます。

 * @<tt>{trema kill [Datapath ID]}: 指定した仮想スイッチを停止する。
 * @<tt>{trema up [Datapath ID]}: 指定した仮想スイッチを起動する。

#@warn(リンクを切ったりつなげたりする例をトポロジの章で紹介 & Hello Trema の章からも引用)

動作イメージがわかったところで、ソースコードの解説に移ります。その前に新しく登場した Ruby の品詞を頭に入れておきましょう。

== 即席 Ruby 入門

@<list>{switch-monitor.rb} のソースコードを眺めると、いくつか見慣れない品詞や構文が登場しています。順に紹介していきますが、やはり最初からすべてを覚える必要はありません。わからなくなったときに見直すようにしてください。

=== シンボル

#@warn(以下の説明に出てくる、文字列とメソッドの引数をまだ説明していない)

シンボルは文字列の軽量版と言える品詞です。メソッド名と同じく英字、数字、アンダースコアを含めることができますが、@<tt>{:a} や @<tt>{:number} や @<tt>{:show_switches} のように必ずコロンで始めることになっています。シンボルは定数のように一度決めると変更できないので、文字列のようにいつの間にか書き変わっている心配がありません。このため、ハッシュのキーによく使われます。また、シンボルはメソッドの引数にメソッドを渡すときにも使われます。

次の例を見てください。メソッド @<tt>{hello} は画面に "Hello" と表示するだけのメソッドです。そして、このようなメソッドを引数に取り 3 回実行してくれる @<tt>{say_3times} というメソッドがあったとします。

//emlist{
# 画面に Hello と表示する
def hello
  puts "Hello"
end


# 引数の method_name に指定されたメソッドを 3 回実行する
def say_3times method_name
  ...
end
//}

ここでもし @<tt>{say_3times} の引数に @<tt>{hello} を直接渡してしまうと、@<tt>{say_3times} の実行に入る前に @<tt>{hello} が実行されてしまうため say_3times は正しく実行できません。詳しく言うと、@<tt>{hello} の返り値は空 (Ruby の世界では @<tt>{nil} と表現されます) なのでこれは @<tt>{say_3times nil} と同じになります。

//emlist{
# まちがい!
@<tt>{say_3times} hello
//}

そこでもしこのように引数にメソッドを渡したいときには @<tt>{:hello} とシンボルにし、誤って先に実行されないようにしてから渡すことになります。

//emlist{
# 正しい
@<tt>{say_3times} :hello

(出力)
Hello
Hello
Hello
//}

=== インスタンス変数

アットマークで始まる語はインスタンス変数です。@<tt>{@x} や @<tt>{@age} や @<tt>{@switches} がそれに当たります。これらの変数はたとえば人間の歳や身長などといった属性を定義するときによく使われます。アットマークはアトリビュート (属性) を意味すると考えれば覚えやすいでしょう。インスタンス変数はクラスの中のメソッド定義内であればどこからでも触れます。次の例を見てください。

//emlist{
class Boy
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

上の Boy クラスは、初期化したときにはそのインスタンス変数 @<tt>{@age} は 0、つまり 0 歳です。@<tt>{happy_birthday} を呼び出すたびに歳をとり @<tt>{@age} が 1 インクリメントされます。

=== 配列

配列は角カッコで囲まれたリストで、カンマで区切られています。

 * @<tt>{[]} は空の配列です。
 * @<tt>{[1, 2, 3]} は数字の配列です。
 * @<tt>{["友太郎", "葵ちゃん", "取間先生"]} は文字列の配列です。
 
Ruby の配列はとても直感的に要素を足したり取り除いたりできます。どのくらい直感的かお見せしましょう。@<br>{}

//noindent
@<tt>{<<} を使うと配列のお尻に指定した要素を加えます。

//emlist{
["友太郎", "葵ちゃん", "取間先生"] << "宮坂主任"
#=> ["友太郎", "葵ちゃん", "取間先生", "宮坂主任"]
//}

//noindent
@<tt>{-=} を使うと左右の配列どうしを見比べ、共通する要素を取り除いてくれます。

//emlist{
["バナナ", "みかん", "テレビ", "りんご", "たわし" ] -= ["テレビ", "たわし"]
#=> ["バナナ", "みかん", "りんご"]
//}

=== メソッド呼び出し

変数や定数が名詞なら、メソッドは動詞です。メソッドは普通、変数や定数の後にドットでつなげられています。

//emlist{
door.open
//}

上の例では @<tt>{open} がメソッドです。open (開ける) は動詞なのでメソッドです。ふつう何らかの動作をすると新しい結果が得られます。

//emlist{
"redrum".reverse
#=> "murder"
//}

文字列 @<tt>{"redrum"} をひっくりかえすと新しい @<tt>{"murder"} 文字列が返ってきました。

質問すると答を返してくれることもあります。

//emlist{
"restaurant".include? "aura"
#=> true
//}

メソッドは引数を取るものもあります。

//emlist{
[ "M", "A", "S", "H" ].join( "★" )
#=> "M★A★S★H"
//}

Ruby にはこのようなメソッドが何千種類もあります。そしてもちろん、クラス定義の中で自分で追加することもできます。

さて、今回は少し長くなりましたがこれで必要な Ruby の勉強は終わりです。わからなくなったらいつでも読み返してください。

== ソースコード

それではスイッチ監視ツールのソースコードを読み解いていきましょう。Trema はスイッチの接続と切断を捕捉するための 2 つのハンドラメソッドを提供しています。

 * @<tt>{switch_ready}: OpenFlow スイッチは、起動すると OpenFlow コントローラへ接続しに行きます。コントーラはスイッチとの接続が確立すると @<tt>{switch_ready} ハンドラが呼ばれます。引数にはスイッチの Datapath ID が渡されます。
 * @<tt>{switch_disconnected}: スイッチが障害など何らかの原因でコントローラとの接続を切った場合、コントローラの @<tt>{switch_disconnected} ハンドラが呼ばれます。引数にはスイッチの Datapath ID が渡されます。

=== スイッチの起動を捕捉する

@<tt>{switch_ready} ハンドラでは、スイッチ一覧リストに新しく接続したスイッチを追加し、接続したスイッチの情報を画面に表示します。

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

@<tt>{@switches} は現在起動しているスイッチのリストを管理するインスタンス変数で、@<tt>{start} ハンドラで空の配列に初期化されます。新しくスイッチが起動すると末尾にその Datapath ID を追加します。また、@<tt>{info} メソッドで Datapath ID を表示します。

@<tt>{to_hex} は @<tt>{datapath_id} に定義されたメソッドで、整数値を十六進の文字列 ("0xabc" など) に変換します。また、文字列中の @<tt>{#{...}} は文字列内に文字列を組込む特殊な文法です。これは次のコードと同じで、文字列を @<tt>{+} でつなげるかわりに見やすくしています。

//emlist{
info "Switch " + datapath_id.to_hex + " is UP"
//}

=== スイッチの切断を捕捉する

@<tt>{switch_disconnected} ハンドラでは、スイッチ一覧リストから切断したスイッチを削除し、切断したスイッチの情報を画面に表示します。

//emlist{
class SwitchMonitor < Controller
  # ...

  def switch_disconnected datapath_id
    @switches -= [datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end

  # ...
end
//}

@<tt>{switch_ready} とは逆に配列の引き算 (@<tt>{-=}) で切断したスイッチの Datapath ID を @<tt>{@switches} から除いています。また、切断したスイッチの Datapath ID を表示しています。

=== スイッチ一覧を一定時間ごとに表示する

#@warn(クラスメソッドの説明)

最後に、スイッチの一覧を一定時間ごとに表示する部分です。このように一定時間ごとに何らかの処理を行いたい場合には、Trema のタイマー機能を使います。次のように @<tt>{periodic_timer_event} に続いて一定間隔ごとに呼び出したいメソッドのシンボル名、間隔を秒数で指定しておくと、指定されたメソッドが呼ばれます。タイマーで呼び出すメソッドは、通常クラスの外からは呼びませんのでプライベートメソッドにしておく必要があります。Ruby では @<tt>{private} と書いた行以降のメソッドはプライベートメソッドとして定義され、クラスの外からは見えなくなります。

//emlist{
class MyController < Controller
  periodic_timer_event :callme_every_15seconds, 15

  # ...


  private


  def call_me_every_15seconds
    # ...
  end
//}

スイッチ監視ツールの場合は、タイマーで 10 秒ごとに @<tt>{show_switches} メソッドを呼んでいます。@<tt>{show_switches} メソッドでは、見やすい出力を得るためにスイッチの Datapath ID のリスト (@<tt>{@switches}) をアルファベット順にソートし (@<tt>{sort})、カンマでつなげて (@<tt>{join( ", " )}) 表示するという工夫をしています。

//list[timer][スイッチの一覧を 10 秒ごとに表示する]{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10

  # ...


  private


  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

== まとめ/参考文献

スイッチの動作状況を監視するスイッチモニタを作りました。学んだことは次の3つです。

 * OpenFlow ネットワークはパケットを処理するスイッチ(datapath)と、ス
   イッチを制御するソフトウェア(コントローラ)から構成される。Trema は、
   このコントローラを書くためのプログラミングフレームワークである
 * Trema は仮想ネットワーク構築機能を持っており、OpenFlow スイッチを持っ
   ていなくてもコントローラの開発やテストが可能。たとえば、仮想ネットワー
   クに仮想スイッチを追加し、任意の datapath ID を設定できる
 * コントローラは OpenFlow の各種イベントに対応するハンドラを定義するこ
   とでスイッチをコントロールできる。たとえば、@<tt>{switch_ready} と
   @<tt>{switch_disconnected} ハンドラでスイッチの起動と切断イベントに
   対応するアクションを書ける

===[column] 友太郎の質問: @<tt>{switch_ready} ってなに？

Q.「OpenFlow の仕様を読んでみたけど、どこにも @<tt>{switch_ready} って
出てこなかったよ？ OpenFlow にそんなイベントが定義されてるの？」

A. @<tt>{switch_ready} は Trema 独自のイベントで、スイッチが Trema に接
続し指示が出せるようになった段階でコントローラに送られます。実は、
@<tt>{switch_ready} の裏では@<img>{switch_ready} の一連の処理が行われて
おり、Trema が OpenFlow プロトコルの詳細をうまくカーペットの裏に隠してく
れているのです。

//image[switch_ready][@<tt>{switch_ready} イベントが起こるまで]

最初に、スイッチとコントローラがしゃべる OpenFlow プロトコルが合ってい
るか確認します。OpenFlow の HELLO メッセージを使ってお互いのプロトコル
バージョンを確認し、うまく会話できそうか確認します。

次は、スイッチを識別するための datapath ID の取得です。datapath ID のよ
うなスイッチ固有の情報は、スイッチに対して OpenFlow の Features
Request メッセージを送ることで取得できます。成功した場合、datapath ID
やポート数などの情報が Features Reply メッセージに乗ってやってきます。

最後にスイッチを初期化します。スイッチに以前の状態が残っていると、コン
トローラが管理する情報と競合が起こるため、初期化することでこれを避けま
す。これら一連の処理が終わると、ようやく @<tt>{switch_ready} がコントロー
ラに通知されます。
